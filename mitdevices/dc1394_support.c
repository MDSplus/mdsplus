#include <stdio.h>
#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <signal.h>
#include <pthread.h>

#define MAX_CAMERAS 8

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

} DC1394Capture;

static DC1394Capture Cameras[MAX_CAMERAS];

static void *AcquireFrames(void *arg); 
void  dc1394Done(void *arg);

int dc1394Init(int cam, int width, int height, int max_frames, int trigger_mode, int shutter, int gain, int trig_on, int iso_speed, int frame_rate)
{
  char dev_name[32];  /* /dev/video1394/n  where 0 >= n < MAX_CAMERAS */
  dc1394_feature_set features;
  int numNodes;
  int numCameras;
  nodeid_t * camera_nodes;

  if ((cam < 0) || (cam >= MAX_CAMERAS)) {
    fprintf(stderr, "camera num must be 0 <= num < %d\n",  MAX_CAMERAS);
    return(0);
  }

  if (Cameras[cam].thread_id != 0) {
    dc1394Done((void *)cam);
  }

  /* set the video1394 device name for DMA transfers */
  sprintf(dev_name, "/dev/video1394/%1d", cam);

  /* open ohci and asign handle to it */
  Cameras[cam].handle = dc1394_create_handle(cam);
  if (Cameras[cam].handle==NULL)
  {
    fprintf( stderr, "Unable to aquire a raw1394 handle\n\n"
             "Please check \n"
             "  - if the kernel modules `ieee1394',`raw1394' and `ohci1394' are loaded \n"
             "  - if you have read/write access to /dev/raw1394\n\n");
    return(0);
  }

  numNodes = raw1394_get_nodecount(Cameras[cam].handle);
  camera_nodes = dc1394_get_camera_nodes(Cameras[cam].handle,&numCameras,1);
  fflush(stdout);
  if (numCameras<1)
  {
    fprintf( stderr, "no cameras found :(\n");
    dc1394_destroy_handle(Cameras[cam].handle);
    return(0);
  }

					
  if (dc1394_dma_setup_capture(Cameras[cam].handle,camera_nodes[0],
			       0,
				FORMAT_VGA_NONCOMPRESSED,
				MODE_640x480_MONO,
				iso_speed,
				frame_rate,
				3, /* frames */
				0,  /* drop frames */
				dev_name,
				&Cameras[cam].camera)!=DC1394_SUCCESS)
  {
    fprintf( stderr,"unable to setup camera-\n"
             "check line %d of %s to make sure\n"
             "that the video mode,framerate and format are\n"
             "supported by your camera\n",
             __LINE__,__FILE__);
    dc1394_dma_release_camera(Cameras[cam].handle,&Cameras[cam].camera);
    dc1394_destroy_handle(Cameras[cam].handle);
    dc1394_free_camera_nodes(camera_nodes);
    return(0);
  }

   /* set trigger mode */
   if( dc1394_set_trigger_mode(Cameras[cam].handle, Cameras[cam].camera.node, trigger_mode) != DC1394_SUCCESS) {
    fprintf( stderr, "unable to set camera trigger mode\n");
   }

   /* eventually the same for the shutter and gain */

   if (dc1394_set_trigger_on_off(Cameras[cam].handle, Cameras[cam].camera.node, trig_on) != DC1394_SUCCESS) {
     fprintf(stderr, "unable to set trigger on to %d\n", trig_on);
   }

   /* set the shutter */
   if (shutter == 0) {
     if (dc1394_auto_on_off(Cameras[cam].handle, camera_nodes[0], FEATURE_SHUTTER, 1) != DC1394_SUCCESS) {
       fprintf(stderr, "unable to set shutter to auto\n");
     }
   }else {
     if (dc1394_auto_on_off(Cameras[cam].handle, camera_nodes[0], FEATURE_SHUTTER, 0) != DC1394_SUCCESS) {
       fprintf(stderr, "unable to set shutter to manual\n");
     }
     if(dc1394_set_shutter(Cameras[cam].handle, camera_nodes[0], shutter) != DC1394_SUCCESS) {
       fprintf(stderr, "unable to set shutter to %d\n", shutter);
     }
   }
   /* and the gain */
   if (gain == 0) {
     if (dc1394_auto_on_off(Cameras[cam].handle, camera_nodes[0], FEATURE_GAIN, 1) != DC1394_SUCCESS) {
       fprintf(stderr, "unable to set gain to auto\n");
     }
   }else {
     if (dc1394_auto_on_off(Cameras[cam].handle, camera_nodes[0], FEATURE_GAIN, 0) != DC1394_SUCCESS) {
       fprintf(stderr, "unable to set gain to manual\n");
     }
     if(dc1394_set_gain(Cameras[cam].handle, camera_nodes[0], gain) != DC1394_SUCCESS) {
       fprintf(stderr, "unable to set gain to %d\n", gain);
     }
   }

   /* free the camera nodes */
  dc1394_free_camera_nodes(camera_nodes);

  /* make room for the answers and write down all of the knobs for later use */
  if ( (width != Cameras[cam].width) ||
       (height != Cameras[cam].height) ||
       (max_frames != Cameras[cam].max_frames) ){
    if (Cameras[cam].frames != NULL)
      free(Cameras[cam].frames);
    if (Cameras[cam].times != NULL)
      free(Cameras[cam].times);
    Cameras[cam].frames = NULL;
    Cameras[cam].times = NULL;
    Cameras[cam].width=0;
    Cameras[cam].height=0;
    Cameras[cam].max_frames=0;
  }
  if (Cameras[cam].frames == NULL) {
    Cameras[cam].frames = (unsigned char *)malloc(width*height*max_frames);
    if (Cameras[cam].frames == NULL) {
      fprintf(stderr, "Could not allocate memory for %d frames (%d x %d)\n", max_frames, width, height);
      dc1394_dma_release_camera(Cameras[cam].handle,&Cameras[cam].camera);
      Cameras[cam].max_frames = 0;
      return(0);
    }
    Cameras[cam].times = (double *)malloc(max_frames*sizeof(double));
    if (Cameras[cam].times == NULL) {
      fprintf(stderr, "could not allocate memory for %d times - Aborting \n", max_frames);
      free (Cameras[cam].frames);
      Cameras[cam].frames=NULL;
      dc1394_dma_release_camera(Cameras[cam].handle,&Cameras[cam].camera);
      return(0);
    }


    Cameras[cam].width = width;
    Cameras[cam].height = height;
    Cameras[cam].max_frames = max_frames;
  }
  Cameras[cam].next_frame = 0;
  Cameras[cam].trigger_mode = trigger_mode;
  Cameras[cam].shutter = shutter;
  Cameras[cam].gain = gain;

/*    if (dc1394_set_trigger_on_off(Cameras[cam].handle, Cameras[cam].camera.node, */
/* 				0) != DC1394_SUCCESS) */
/*     { */
/*       fprintf(stderr, "unable to set trigger on \n"); */
/*     } */

/*-----------------------------------------------------------------------
   *  have the camera start sending us data
   *-----------------------------------------------------------------------*/
  if (dc1394_start_iso_transmission(Cameras[cam].handle,Cameras[cam].camera.node)
      !=DC1394_SUCCESS) 
  {
    fprintf( stderr, "unable to start camera iso transmission\n");
    dc1394_dma_release_camera(Cameras[cam].handle,&Cameras[cam].camera);
    dc1394_destroy_handle(Cameras[cam].handle);
    return(0);
  }

    if (pthread_create(&Cameras[cam].thread_id,  NULL, AcquireFrames, (void *)cam) != 0) {
      fprintf(stderr, "Could not create thread to handle camera daq\n");
      dc1394_dma_release_camera(Cameras[cam].handle,&Cameras[cam].camera);
      return(0);
  }

  return(1);
}

static void *AcquireFrames(void *arg) 
{
  struct timeval tv;
  int    start_sec;
  int cam = (int)arg;

  pthread_cleanup_push(dc1394Done, arg);
  gettimeofday(&tv, NULL);
  start_sec = tv.tv_sec;
  //  fprintf(stderr, "Here I am in AcquireFrames - max_frames is %d,  next_frame is %d cam is %d\n", Cameras[cam].max_frames, Cameras[cam].next_frame, cam);
  for (Cameras[cam].next_frame=0; Cameras[cam].next_frame < Cameras[cam].max_frames; Cameras[cam].next_frame++) {
    //    fprintf(stderr, "trying to read %d \n", Cameras[cam].next_frame);
    if (dc1394_dma_single_capture(&Cameras[cam].camera)!=DC1394_SUCCESS) {
      fprintf( stderr, "unable to capture a frame\n");
      break;
    }
    //  fprintf(stderr, "\t got one\n");
    gettimeofday(&tv, NULL);
    Cameras[cam].times[Cameras[cam].next_frame] = tv.tv_sec-start_sec+(double)tv.tv_usec*1E-6;
    memcpy((void *)Cameras[cam].frames+Cameras[cam].next_frame*Cameras[cam].width*Cameras[cam].height, Cameras[cam].camera.capture_buffer, Cameras[cam].width*Cameras[cam].height);
    dc1394_dma_done_with_buffer(&Cameras[cam].camera);
  }
  /* clean up active daq */
  dc1394_stop_iso_transmission(Cameras[cam].handle,Cameras[cam].camera.node);
  dc1394_dma_unlisten(Cameras[cam].handle, &Cameras[cam].camera);
  dc1394_dma_release_camera(Cameras[cam].handle,&Cameras[cam].camera);

  Cameras[cam].thread_id =0;
  pthread_cleanup_pop(0);
  return;
}

int dc1394ReadFrames(int cam, char *data) {


  memmove(data, Cameras[cam].frames, Cameras[cam].width*Cameras[cam].height*Cameras[cam].next_frame);
  return(1);
}

int dc1394ReadTimes(int cam, double *data) {
  memmove((char *)data, Cameras[cam].times, Cameras[cam].next_frame*sizeof(double));
  return 1;
}

int dc1394NumFrames(int cam) {
  return(Cameras[cam].next_frame);
}

void  dc1394Done(void *arg)
{
  int cam = (int)arg;
  /* clean up any active daq */
  if (Cameras[cam].thread_id) {
    void *status;
    dc1394_dma_unlisten(Cameras[cam].handle, &Cameras[cam].camera);
    dc1394_stop_iso_transmission(Cameras[cam].handle,Cameras[cam].camera.node);
    dc1394_dma_release_camera(Cameras[cam].handle,&Cameras[cam].camera);
    pthread_cancel(Cameras[cam].thread_id);
    pthread_join(Cameras[cam].thread_id, &status);
    Cameras[cam].thread_id = 0;
  }
  return;
}

extern int dc1394LoadImage() {
  fprintf(stderr, "dc1394_support.so now loaded \n");
  return(1);
}
