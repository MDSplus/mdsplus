#include <stdio.h>
#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <signal.h>
#include <pthread.h>

#define MAX_CAMERAS 8

static unsigned int first = 1;
static struct capture1394 {
  dc1394_cameracapture camera;
  raw1394handle_t handle;
  int running;
  int dma_active;
  unsigned char *frames;
  double *times;
  pthread_t thread_id;
  int next_frame;
  int width;
  int height;
  int max_frames;
  int trigger_mode;
  int shutter;
  int gain;
} Cams[8];

//static struct capture1394 Cams[8];

static void *AcquireFrames(void *arg);

void  dc1394Done(void *arg) ;

int dc1394Init(int cam, int width, int height, int max_frames, int trigger_mode, int shutter, int gain, int trig_on, int iso_speed, int frame_rate)
{
  int numNodes;
  nodeid_t * camera_nodes;
  int numCameras;

  if (first) {
    bzero(Cams, sizeof(Cams));
    first = 0;
  }
    printf("now is the time\n");
  if ((cam < 0) || (cam >= MAX_CAMERAS)) {
    fprintf(stderr, "Camera out of range, cam = %d must be between 0 and %d\n", cam, MAX_CAMERAS);
    return 0;
  }

  /* clean up any active daq */
  if (Cams[cam].thread_id != 0) {
    void *status;
    pthread_cancel(Cams[cam].thread_id);
    pthread_join(Cams[cam].thread_id, &status);
    Cams[cam].thread_id = 0;
  }

  /* now find the camera choke if not there or not correct */
  Cams[cam].handle = dc1394_create_handle(0);
  if (Cams[cam].handle==NULL)
  {
    fprintf( stderr, "Unable to aquire a raw1394 handle\n\n"
             "Please check \n"
	     "  - if the kernel modules `ieee1394',`raw1394' and `ohci1394' are loaded \n"
	     "  - if you have read/write access to /dev/raw1394\n\n");
    return(0);
  }

  /*-----------------------------------------------------------------------
   *  get the camera nodes and describe them as we find them
   *-----------------------------------------------------------------------*/
  numNodes = raw1394_get_nodecount(Cams[cam].handle);
  camera_nodes = dc1394_get_camera_nodes(Cams[cam].handle,&numCameras,1);
  if (numCameras<1)
  {
    fprintf( stderr, "no cameras found :(\n");
    dc1394_destroy_handle(Cams[cam].handle);
    Cams[cam].handle = 0;
    return(0);
  }

  /*-----------------------------------------------------------------------
   *  to prevent the iso-transfer bug from raw1394 system, check if
   *  camera is highest node. For details see 
   *  http://linux1394.sourceforge.net/faq.html#DCbusmgmt
   *  and
   *  http://sourceforge.net/tracker/index.php?func=detail&aid=435107&group_id=8157&atid=108157
   *-----------------------------------------------------------------------*/
  if( camera_nodes[0] == numNodes-1)
  {
    fprintf( stderr, "\n"
             "Sorry, your camera is the highest numbered node\n"
             "of the bus, and has therefore become the root node.\n"
             "The root node is responsible for maintaining \n"
             "the timing of isochronous transactions on the IEEE \n"
             "1394 bus.  However, if the root node is not cycle master \n"
             "capable (it doesn't have to be), then isochronous \n"
             "transactions will not work.  The host controller card is \n"
             "cycle master capable, however, most cameras are not.\n"
             "\n"
             "The quick solution is to add the parameter \n"
             "attempt_root=1 when loading the OHCI driver as a \n"
             "module.  So please do (as root):\n"
             "\n"
             "   rmmod ohci1394\n"
             "   insmod ohci1394 attempt_root=1\n"
             "\n"
             "for more information see the FAQ at \n"
             "http://linux1394.sourceforge.net/faq.html#DCbusmgmt\n"
             "\n");
    dc1394_destroy_handle(Cams[cam].handle);
    dc1394_free_camera_nodes(camera_nodes);
    return(0);
  }
  
  /*-----------------------------------------------------------------------
   *  setup capture
   *-----------------------------------------------------------------------*/
   if (dc1394_dma_setup_capture(Cams[cam].handle,camera_nodes[0],
                           0, /* channel */ 
                           FORMAT_VGA_NONCOMPRESSED,
                           MODE_640x480_MONO,
                           SPEED_400,
                           FRAMERATE_30,
				3, /* frames */
			    1,
			    "/dev/video1394/0", 
                           &Cams[cam].camera)!=DC1394_SUCCESS) 
 {
    fprintf( stderr,"unable to setup camera-\n"
             "check line %d of %s to make sure\n"
             "that the video mode,framerate and format are\n"
             "supported by your camera\n",
             __LINE__,__FILE__);
    dc1394_dma_release_camera(Cams[cam].handle,&Cams[cam].camera);
    Cams[cam].dma_active = 0;
    dc1394_destroy_handle(Cams[cam].handle);
    Cams[cam].handle = 0;
    dc1394_free_camera_nodes(camera_nodes);
    return(0);
  }
  dc1394_free_camera_nodes(camera_nodes);
  
  /* set trigger mode */
  if( dc1394_set_trigger_mode(Cams[cam].handle, Cams[cam].camera.node, trigger_mode)
      != DC1394_SUCCESS)
  {
    fprintf( stderr, "unable to set camera trigger mode\n");
    /*    
    dc1394_dma_release_camera(Cams[cam].handle,&Cams[cam].camera);
    Cams[cam].dma_active = 0;
    dc1394_destroy_handle(Cams[cam].handle);
    Cams[cam].handle = 0;
    return(0);
    */
  }

  /* eventually the same for the shutter and gain */

  if (dc1394_set_trigger_on_off(Cams[cam].handle, Cams[cam].camera.node,
				trig_on) != DC1394_SUCCESS)
    {
      fprintf(stderr, "unable to set trigger on to %d\n", trig_on);
    }


  /*-----------------------------------------------------------------------
   *  have the camera start sending us data
   *-----------------------------------------------------------------------*/
  if (dc1394_start_iso_transmission(Cams[cam].handle,Cams[cam].camera.node) !=DC1394_SUCCESS) 
  {
    fprintf( stderr, "unable to start camera iso transmission\n");
    dc1394_dma_release_camera(Cams[cam].handle,&Cams[cam].camera);
    Cams[cam].dma_active = 0;
    dc1394_destroy_handle(Cams[cam].handle);
    Cams[cam].handle = 0;
    return(0);
  }
  Cams[cam].running = 1;

  /* make room for the answers and write down all of the knobs for later use */
  if ( (width != Cams[cam].width) ||
       (height != Cams[cam].height) ||
       (max_frames != Cams[cam].max_frames) ){
    if (Cams[cam].frames != NULL)
      free(Cams[cam].frames);
    Cams[cam].frames = (unsigned char *)malloc(width*height*max_frames);
    if (Cams[cam].frames == NULL) {
      fprintf(stderr, "Could not allocate memory for %d frames (%d x %d)\n", max_frames, width, height);
      dc1394_dma_release_camera(Cams[cam].handle,&Cams[cam].camera);
      Cams[cam].dma_active = 0;
      dc1394_destroy_handle(Cams[cam].handle);
      Cams[cam].handle = 0;
      Cams[cam].max_frames = 0;
      return(0);
    }
    Cams[cam].times = (double *)malloc(max_frames * sizeof(double));
    if (Cams[cam].times == NULL) {
      fprintf(stderr, "Could not allocate memory for %d times\n", max_frames);
      dc1394_dma_release_camera(Cams[cam].handle,&Cams[cam].camera);
      free( Cams[cam].frames );
      Cams[cam].dma_active = 0;
      dc1394_destroy_handle(Cams[cam].handle);
      Cams[cam].handle = 0;
      Cams[cam].max_frames = 0;
     return(0);
    }

    Cams[cam].width = width;
    Cams[cam].height = height;
    Cams[cam].max_frames = max_frames;
  }
  Cams[cam].next_frame = 0;
  Cams[cam].trigger_mode = trigger_mode;
  Cams[cam].shutter = shutter;
  Cams[cam].gain = gain;
  if (pthread_create(&Cams[cam].thread_id,  NULL, AcquireFrames, (void *)cam) != 0) {
      fprintf(stderr, "Could not create thread to handle camera daq\n");
      dc1394_stop_iso_transmission(Cams[cam].handle,Cams[cam].camera.node);
      Cams[cam].running = 0;
      dc1394_dma_release_camera(Cams[cam].handle,&Cams[cam].camera);
      Cams[cam].dma_active = 0;
      dc1394_destroy_handle(Cams[cam].handle);
      Cams[cam].handle = 0;
      return(0);
  }
  return(1);
}

static void *AcquireFrames(void *arg) {
  struct timeval tv;
  int    start_sec;
  int cam = (int)arg;
  pthread_cleanup_push(dc1394Done, (void *)cam);
  printf("in acquire\n");
  gettimeofday(&tv, NULL);
  start_sec = tv.tv_sec;
  printf("start sec is %d\n", start_sec);
  for (Cams[cam].next_frame=0; Cams[cam].next_frame < Cams[cam].max_frames; Cams[cam].next_frame++) {
    if (dc1394_dma_single_capture(&Cams[cam].camera)!=DC1394_SUCCESS) {
      fprintf( stderr, "unable to capture a frame\n");
      dc1394_stop_iso_transmission(Cams[cam].handle,Cams[cam].camera.node);
      Cams[cam].running = 0;
      dc1394_dma_release_camera(Cams[cam].handle,&Cams[cam].camera);
      Cams[cam].dma_active = 0;
      dc1394_destroy_handle(Cams[cam].handle);
      Cams[cam].handle = 0;
      Cams[cam].thread_id =0;
      return;
    }
    gettimeofday(&tv, NULL);
    Cams[cam].times[Cams[cam].next_frame] = tv.tv_sec-start_sec+(double)tv.tv_usec*1E-6;
    printf("The time for frame %d is %g\n", Cams[cam].next_frame, Cams[cam].times[Cams[cam].next_frame]);
    memcpy((void *)Cams[cam].frames+Cams[cam].next_frame*Cams[cam].width*Cams[cam].height, Cams[cam].camera.capture_buffer, Cams[cam].width*Cams[cam].height);
    dc1394_dma_done_with_buffer(&Cams[cam].camera);
    printf("got frame %d\n", Cams[cam].next_frame);
  }
  /* clean up active daq */
  dc1394_stop_iso_transmission(Cams[cam].handle,Cams[cam].camera.node);
  Cams[cam].running = 0;
  dc1394_dma_unlisten(Cams[cam].handle, &Cams[cam].camera);
  dc1394_dma_release_camera(Cams[cam].handle,&Cams[cam].camera);
  Cams[cam].dma_active = 0;
  dc1394_destroy_handle(Cams[cam].handle);
  Cams[cam].handle = 0;

  Cams[cam].thread_id =0;
  pthread_cleanup_pop(0);
  return;
}

int dc1394ReadFrames(int cam, char *data) {
  memmove(data, Cams[cam].frames, Cams[cam].width*Cams[cam].height*Cams[cam].next_frame);
  return(1);
}

int dc1394ReadTimes(int cam, double *data) {
  int i;
  for (i=0; i<Cams[cam].next_frame; i++)
    data[i] = Cams[cam].times[i] -  Cams[cam].times[0];
  return 1;
}

int dc1394NumFrames(int cam) {
  return(Cams[cam].next_frame);
}

void  dc1394Done(void *arg) {
  int cam = (int)arg;
  printf("here I am in done - %d %d\n", cam, Cams[cam].running);
  /* clean up any active daq */
  if (Cams[cam].running) {
    printf("stop the dma\n\n");
    dc1394_dma_unlisten(Cams[cam].handle, &Cams[cam].camera);
    dc1394_stop_iso_transmission(Cams[cam].handle,Cams[cam].camera.node);
    Cams[cam].running = 0;
  }
  if (Cams[cam].dma_active) {
    dc1394_dma_release_camera(Cams[cam].handle,&Cams[cam].camera);
    Cams[cam].dma_active = 0;
  }
  if (Cams[cam].handle != NULL) {
    dc1394_destroy_handle(Cams[cam].handle);
    Cams[cam].handle = 0;
  }

  printf("all done with Don\n");
}
