#include <stdio.h>
#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <signal.h>
#include <pthread.h>

#define MAX_BUX  4
#define MAX_CAMERAS 8

typedef struct _DC1394Bus
 {
  int bus_num; /* just counted from 0 in the order found */
  raw1394handle_t handle;
  char dev_filename[32];  /* /dev/video1394/num */
  int num_cameras;
  nodeid_t *camera_nodes;  
  struct _DC1394Bus *next;
} DC1394Bus;
  
typedef struct _DC1394Capture {
  int camera_num; /* just counted from 1 in the order found */
  int bus;
  raw1394handle_t handle;
  char *dev_filename;
  //  nodeid_t *camera_node;  
  int next_frame;
  int trigger_mode;
  int shutter;
  int gain;

  int width;
  int height;
  int max_frames;

  unsigned char *frames;
  double  *times;

  dc1394_camerainfo camera_info;  
  dc1394_miscinfo misc_info;
  dc1394_cameracapture camera;  /* returned by setup_capture */ 
  pthread_t thread_id;

  struct _DC1394Capture *next;
} DC1394Capture;

static DC1394Bus *Busses = NULL;
static DC1394Capture *Cameras = NULL;

static void *AcquireFrames(void *arg); 
void  dc1394Done(void *arg);

static DC1394Bus *Find1394Busses()
{
  raw1394handle_t tmp_handle;
  int port;
  int port_num;

  DC1394Bus *Bus, *B;
  if (Busses != NULL) {
    for (Bus=Busses; Bus;) {
      DC1394Bus *this = Bus;
      dc1394_free_camera_nodes(this->camera_nodes);
      dc1394_destroy_handle(this->handle);
      Bus=this->next;
      free(this);
    }
  }

  tmp_handle=raw1394_new_handle();
  port_num=raw1394_get_port_info(tmp_handle, NULL, 0);
  raw1394_destroy_handle(tmp_handle);

  for (port=0; port < port_num; port++) {
    Bus = (DC1394Bus *)malloc(sizeof(DC1394Bus));
    if (Bus == NULL) {
      fprintf(stderr, "error allocating Bus structure - Aborting");
      return NULL;
    }
    Bus->next = NULL;
    Bus->bus_num = port;
    sprintf(Bus->dev_filename, "/dev/video1394/%1.1d", port);
    Bus->handle=dc1394_create_handle(port);
    if (Bus->handle == NULL)
      free(Bus);
    else {
      Bus->camera_nodes = dc1394_get_camera_nodes(Bus->handle,&Bus->num_cameras,0);
      if (Busses) {
	for (B=Busses; B->next; B=B->next);
	B->next=Bus;
      } else
	Busses = Bus;
    }
  }
  return (Busses);
}

static DC1394Capture *NewCamera() {
  DC1394Capture *this=(DC1394Capture *)malloc(sizeof(DC1394Capture));
  if (this) {
    this->frames=NULL;
    this->times=NULL;
    this->next=NULL;
  }
  return(this);
}

static DC1394Capture *Find1394Cameras(DC1394Bus *busses) 
{
  DC1394Bus *Bus;
  DC1394Capture *Cam, *this;
  int i;

  if (Cameras) {
    for (Cam=Cameras; Cam;) {
      this=Cam;
      free(this->frames);
      free(this->times);
      Cam = this->next;
      free(this);
    }
    Cameras = NULL;
  }
  for (Bus=Busses; Bus; Bus=Bus->next) {
    for(i=0; i < Bus->num_cameras; i++) {
      this = NewCamera();
      this->handle=dc1394_create_handle(Bus->bus_num);
      if(dc1394_get_camera_info(this->handle, Bus->camera_nodes[i], &this->camera_info) != DC1394_SUCCESS) {
	fprintf(stderr, "Could not get basic camera information for camera %d - aborting\n", i);
	return(0);
      }
      if(dc1394_get_camera_misc_info(this->handle, Bus->camera_nodes[i], &this->misc_info) != DC1394_SUCCESS) {
	fprintf(stderr, "Could not get misc camera information for camera %d - aborting\n", i);
	return(0);
      }
      this->dev_filename = Bus->dev_filename;
      //      this->camera_node = Bus->camera_nodes[i];
      if (Cameras==NULL)
	Cameras=this;
      else {
	DC1394Capture *p=Cameras;
	for (p=Cameras; p->next; p=p->next);
	p->next=this;
      }
    }
  }           
  return(Cameras);
} 

int dc1394Init(int cam, int width, int height, int max_frames, int trigger_mode, int shutter, int gain, int trig_on, int iso_speed, int frame_rate)
{
  DC1394Bus *Bus;
  DC1394Capture *Cam;
  int i;

  if (Busses == NULL)
    Busses = Find1394Busses();

  if (Busses == NULL) {
    fprintf(stderr, "Could not find any 1394 Busses - Aborting \n");
    return 0;
  }

  if (Cameras == NULL) {
    Cameras = Find1394Cameras(Busses);
  }

  if (Cameras == NULL) {
    fprintf(stderr, "Could not find any DC1394 Cameras on the Busses - Aborting \n");
    return 0;
  }

  for (Cam = Cameras, i = 0; Cam && (i < cam); Cam = Cam->next, i++);

  if (Cam == NULL) {
    fprintf(stderr, "Camera %d requested on %d cameras found  - Aborting \n", cam, i);
    return 0;
  }
  
  /* if we already have a thread clean up any active daq and kill the thread */
  if (Cam->thread_id) {
    void *status;
    pthread_cancel(Cam->thread_id);
    pthread_join(Cam->thread_id, &status);
    Cam->thread_id = 0;
  }

  /*-----------------------------------------------------------------------
   *  setup capture
   *-----------------------------------------------------------------------*/
   if (dc1394_dma_setup_capture(Cam->handle,Cam->camera_info.id,
                           Cam->misc_info.iso_channel, /* channel */ 
                           FORMAT_VGA_NONCOMPRESSED,
                           MODE_640x480_MONO,
                           SPEED_400,
                           FRAMERATE_30,
			   10, /* frames */
			   1,
                           Cam->dev_filename, 
                           &Cam->camera)!=DC1394_SUCCESS) 
     {
       fprintf( stderr,"unable to setup camera-\n"
		"check line %d of %s to make sure\n"
		"that the video mode,framerate and format are\n"
		"supported by your camera\n",
		__LINE__,__FILE__);
       dc1394_dma_release_camera(Cam->handle,&Cam->camera);
       return 0;
     }


   /* set trigger mode */
   if( dc1394_set_trigger_mode(Cam->handle, Cam->camera.node, trigger_mode) != DC1394_SUCCESS) {
    fprintf( stderr, "unable to set camera trigger mode\n");
   }

   /* eventually the same for the shutter and gain */

   if (dc1394_set_trigger_on_off(Cam->handle, Cam->camera.node, trig_on) != DC1394_SUCCESS) {
     fprintf(stderr, "unable to set trigger on to %d\n", trig_on);
   }


  /* make room for the answers and write down all of the knobs for later use */
  if ( (width != Cam->width) ||
       (height != Cam->height) ||
       (max_frames != Cam->max_frames) ){
    if (Cam->frames != NULL)
      free(Cam->frames);
    if (Cam->times != NULL)
      free(Cam->times);
    Cam->frames = NULL;
    Cam->times = NULL;
    Cam->width=0;
    Cam->height=0;
    Cam->max_frames=0;
  }
  if (Cam->frames == NULL) {
    Cam->frames = (unsigned char *)malloc(width*height*max_frames);
    if (Cam->frames == NULL) {
      fprintf(stderr, "Could not allocate memory for %d frames (%d x %d)\n", max_frames, width, height);
      dc1394_dma_release_camera(Cam->handle,&Cam->camera);
      Cam->max_frames = 0;
      return(0);
    }
    Cam->times = (double *)malloc(max_frames*sizeof(double));
    if (Cam->times == NULL) {
      fprintf(stderr, "could not allocate memory for %d times - Aborting \n", max_frames);
      free (Cam->frames);
      Cam->frames=NULL;
      dc1394_dma_release_camera(Cam->handle,&Cam->camera);
      return(0);
    }


    Cam->width = width;
    Cam->height = height;
    Cam->max_frames = max_frames;
  }
  Cam->next_frame = 0;
  Cam->trigger_mode = trigger_mode;
  Cam->shutter = shutter;
  Cam->gain = gain;

  if (pthread_create(&Cam->thread_id,  NULL, AcquireFrames, (void *)Cam) != 0) {
      fprintf(stderr, "Could not create thread to handle camera daq\n");
      dc1394_dma_release_camera(Cam->handle,&Cam->camera);
      return(0);
  }

  /* now have the camera start sending us frames */

  if (dc1394_start_iso_transmission(Cam->handle,Cam->camera.node) !=DC1394_SUCCESS) 
  {
    fprintf( stderr, "unable to start camera iso transmission\n");
    dc1394_dma_release_camera(Cam->handle,&Cam->camera);
    return(0);
  }
  return(1);
}

static void *AcquireFrames(void *arg) 
{
  struct timeval tv;
  int    start_sec;
  DC1394Capture *Cam = (DC1394Capture *)arg;
  pthread_cleanup_push(dc1394Done, arg);
  gettimeofday(&tv, NULL);
  start_sec = tv.tv_sec;
  for (Cam->next_frame=0; Cam->next_frame < Cam->max_frames; Cam->next_frame++) {
    if (dc1394_dma_single_capture(&Cam->camera)!=DC1394_SUCCESS) {
      fprintf( stderr, "unable to capture a frame\n");
      break;
    }
    gettimeofday(&tv, NULL);
    Cam->times[Cam->next_frame] = tv.tv_sec-start_sec+(double)tv.tv_usec*1E-6;
    memcpy((void *)Cam->frames+Cam->next_frame*Cam->width*Cam->height, Cam->camera.capture_buffer, Cam->width*Cam->height);
    dc1394_dma_done_with_buffer(&Cam->camera);
  }
  /* clean up active daq */
  dc1394_stop_iso_transmission(Cam->handle,Cam->camera.node);
  dc1394_dma_unlisten(Cam->handle, &Cam->camera);
  dc1394_dma_release_camera(Cam->handle,&Cam->camera);

  Cam->thread_id =0;
  pthread_cleanup_pop(0);
  return;
}

int dc1394ReadFrames(int cam, char *data) {

  DC1394Capture *Cam;
  int i;
  for (Cam = Cameras, i = 0; Cam && (i < cam); Cam = Cam->next, i++);
  if (Cam == NULL) {
    fprintf(stderr, "Camera %d requested on %d cameras found  - Aborting \n", cam, i);
    return 0;
  }

  memmove(data, Cam->frames, Cam->width*Cam->height*Cam->next_frame);
  return(1);
}

int dc1394ReadTimes(int cam, double *data) {
  DC1394Capture *Cam;
  int i;
  for (Cam = Cameras, i = 0; Cam && (i < cam); Cam = Cam->next, i++);
  if (Cam == NULL) {
    fprintf(stderr, "Camera %d requested on %d cameras found  - Aborting \n", cam, i);
    return 0;
  }
  memmove((char *)data, Cam->times, Cam->next_frame*sizeof(double));
  return 1;
}

int dc1394NumFrames(int cam) {
  DC1394Capture *Cam;
  int i;
  for (Cam = Cameras, i = 0; Cam && (i < cam); Cam = Cam->next, i++);
  if (Cam == NULL) {
    fprintf(stderr, "Camera %d requested on %d cameras found  - Aborting \n", cam, i);
    return 0;
  }
  return(Cam->next_frame);
}

void  dc1394Done(void *arg)
{
  int cam = (int)arg;
  DC1394Capture *Cam;
  int i;
  for (Cam = Cameras, i = 0; Cam && (i < cam); Cam = Cam->next, i++);
  if (Cam == NULL) {
    fprintf(stderr, "Camera %d requested on %d cameras found  - Aborting \n", cam, i);
    return;
  }

  /* clean up any active daq */
  if (Cam->thread_id) {
    void *status;
    dc1394_dma_unlisten(Cam->handle, &Cam->camera);
    dc1394_stop_iso_transmission(Cam->handle,Cam->camera.node);
    dc1394_dma_release_camera(Cam->handle,&Cam->camera);
    pthread_cancel(Cam->thread_id);
    pthread_join(Cam->thread_id, &status);
    Cam->thread_id = 0;
  }
  return;
}
