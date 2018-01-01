/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <signal.h>
#include <pthread.h>
#include <mdsplus/mdsconfig.h>

#define MAX_CAMERAS 8
#define FLAG_VALUE 0xDEADBEEF

static unsigned char *NewCamBuf(int size)
{
  unsigned char *tmp;
  int *iptr;

  tmp = malloc(size + 3 * sizeof(int));
  if (tmp == NULL) {
    return 0;
  }
  iptr = (int *)tmp;
  *iptr = FLAG_VALUE;
  iptr = (int *)(tmp + sizeof(int));
  *iptr = size;
  iptr = (int *)(tmp + 2 * sizeof(int) + size);
  *iptr = FLAG_VALUE;
  return tmp + 2 * sizeof(int);
}

static int CheckCamBuf(unsigned char *buf, int size)
{
  int *start_flag_ptr = (int *)(buf - 2 * sizeof(int));
  int *size_ptr = (int *)(buf - sizeof(int));
  int *end_flag_ptr = (int *)(buf + size);
  return ((*start_flag_ptr != FLAG_VALUE) || (*size_ptr != size) || (*end_flag_ptr != FLAG_VALUE));
}

static int first = 1;
typedef struct _DC1394Capture {
  int start_flag;
  int camera_num;		/* just counted from 1 in the order found */
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
  int bpp;			/* bytes per pixel */
  int max_frames;

  unsigned char *frames;
  double *times;

  dc1394_camerainfo camera_info;
  dc1394_miscinfo misc_info;
  dc1394_cameracapture camera;	/* returned by setup_capture */
  pthread_t thread_id;

  int debug;
  int end_flag;
} DC1394Capture;

static DC1394Capture Cameras[MAX_CAMERAS];

static void *AcquireFrames(void *arg);
void dc1394Done(void *arg);

EXPORT int dc1394Init(int cam, int width, int height, int max_frames, int trigger_mode, int shutter,
	       int gain, int trig_on, int mode, int frame_rate, int debug)
{
  char dev_name[32];		/* /dev/video1394/n  where 0 >= n < MAX_CAMERAS */
  dc1394_feature_set features;
  int numNodes;
  int numCameras;
  nodeid_t *camera_nodes;
  int i;
  unsigned char *buf;
  if (first) {
    if (debug > 0)
      fprintf(stderr, "First time zeroing Cameras array\n");
    memset((void *)Cameras, 0, sizeof(Cameras));
    for (i = 0; i < MAX_CAMERAS; i++) {
      Cameras[i].start_flag = FLAG_VALUE;
      Cameras[i].end_flag = FLAG_VALUE;
    }
    first = 0;
  }
  switch (mode) {
  case (MODE_640x480_MONO):
  case (MODE_1024x768_MONO):
    Cameras[cam].bpp = 1;
    break;
  case (MODE_640x480_MONO16):
  case (MODE_1024x768_MONO16):
    Cameras[cam].bpp = 2;
    break;
  }

  /***********************************************/
  /* make room for the answers and write down all of the knobs for later use */
  if ((width != Cameras[cam].width) ||
      (height != Cameras[cam].height) || (max_frames != Cameras[cam].max_frames)) {
    if (Cameras[cam].frames != NULL)
      free(Cameras[cam].frames);
    if (Cameras[cam].times != NULL)
      free(Cameras[cam].times);
    Cameras[cam].frames = NULL;
    Cameras[cam].times = NULL;
    Cameras[cam].width = 0;
    Cameras[cam].height = 0;
    Cameras[cam].max_frames = 0;
    Cameras[cam].debug = debug;
  }
  if (Cameras[cam].frames == NULL) {
    Cameras[cam].frames = NewCamBuf(width * height * max_frames * Cameras[cam].bpp);
    if (Cameras[cam].frames == NULL) {
      fprintf(stderr, "Could not allocate memory for %d frames (%d x %d x %d(bytes per pixel))\n",
	      max_frames, width, height, Cameras[cam].bpp);
      Cameras[cam].max_frames = 0;
      return (0);
    }

    Cameras[cam].times = (double *)malloc(max_frames * sizeof(double));
    if (Cameras[cam].times == NULL) {
      fprintf(stderr, "could not allocate memory for %d times - Aborting \n", max_frames);
      free(Cameras[cam].frames);
      Cameras[cam].frames = NULL;
      return (0);
    }

    Cameras[cam].width = width;
    Cameras[cam].height = height;
    Cameras[cam].max_frames = max_frames;
  }
  Cameras[cam].next_frame = 0;
  Cameras[cam].trigger_mode = trigger_mode;
  Cameras[cam].shutter = shutter;
  Cameras[cam].gain = gain;

  if (CheckCamBuf(Cameras[cam].frames, width * height * max_frames * Cameras[cam].bpp) != 0) {
    fprintf(stderr, "*********************\n Camera buffer trashed \n*************************");
    return 0;
  }

  /* fill the frames with zeros */
  memset((void *)Cameras[cam].frames, 0, width * height * max_frames * Cameras[cam].bpp);

  /***********************************************/

  if (debug > 0)
    fprintf(stderr, "Starting Camera setup \n");

  if ((cam < 0) || (cam >= MAX_CAMERAS)) {
    fprintf(stderr, "camera num must be 0 <= num < %d\n", MAX_CAMERAS);
    return (0);
  }

  if (Cameras[cam].thread_id != 0) {
    dc1394Done((void *)cam);
  }

  /* set the video1394 device name for DMA transfers */
  sprintf(dev_name, "/dev/video1394/%1d", cam);

  /* open ohci and asign handle to it */
  Cameras[cam].handle = dc1394_create_handle(cam);
  if (Cameras[cam].handle == NULL) {
    fprintf(stderr, "Unable to aquire a raw1394 handle\n\n"
	    "Please check \n"
	    "  - if the kernel modules `ieee1394',`raw1394' and `ohci1394' are loaded \n"
	    "  - if you have read/write access to /dev/raw1394\n\n");
    return (0);
  }

  if (debug > 0)
    fprintf(stderr, "have the handle, now get the nodecount\n");
  numNodes = raw1394_get_nodecount(Cameras[cam].handle);
  camera_nodes = dc1394_get_camera_nodes(Cameras[cam].handle, &numCameras, 1);
  fflush(stdout);
  if (numCameras < 1) {
    fprintf(stderr, "no cameras found :(\n");
    dc1394_destroy_handle(Cameras[cam].handle);
    return (0);
  }

  if (debug > 0)
    fprintf(stderr, "have the node list and count, now setup the dma\n");

  if (dc1394_dma_setup_capture(Cameras[cam].handle, camera_nodes[0], 0, FORMAT_VGA_NONCOMPRESSED + ((mode == 101) || (mode == 103)), mode, SPEED_400, frame_rate, 3,	/* frames */
			       0,	/* drop frames */
			       dev_name, &Cameras[cam].camera) != DC1394_SUCCESS) {
    fprintf(stderr, "unable to setup camera-\n"
	    "check line %d of %s to make sure\n"
	    "that the video mode,framerate and format are\n"
	    "supported by your camera\n", __LINE__, __FILE__);
    dc1394_dma_release_camera(Cameras[cam].handle, &Cameras[cam].camera);
    dc1394_destroy_handle(Cameras[cam].handle);
    dc1394_free_camera_nodes(camera_nodes);
    return (0);
  }

  if (debug > 0)
    fprintf(stderr, "DMA setup, now the trigger mode\n");
  /* set trigger mode */
  if (dc1394_set_trigger_mode(Cameras[cam].handle, Cameras[cam].camera.node, trigger_mode) !=
      DC1394_SUCCESS) {
    fprintf(stderr, "unable to set camera trigger mode\n");
  }

  /* eventually the same for the shutter and gain */

  if (dc1394_set_trigger_on_off(Cameras[cam].handle, Cameras[cam].camera.node, trig_on) !=
      DC1394_SUCCESS) {
    fprintf(stderr, "unable to set trigger on to %d\n", trig_on);
  }

  if (debug > 0)
    fprintf(stderr, "trigger mode set, now the shutter\n");
  /* set the shutter */
  if (shutter == 0) {
    if (dc1394_auto_on_off(Cameras[cam].handle, camera_nodes[0], FEATURE_SHUTTER, 1) !=
	DC1394_SUCCESS) {
      fprintf(stderr, "unable to set shutter to auto\n");
    }
  } else {
    if (dc1394_auto_on_off(Cameras[cam].handle, camera_nodes[0], FEATURE_SHUTTER, 0) !=
	DC1394_SUCCESS) {
      fprintf(stderr, "unable to set shutter to manual\n");
    }
    if (dc1394_set_shutter(Cameras[cam].handle, camera_nodes[0], shutter) != DC1394_SUCCESS) {
      fprintf(stderr, "unable to set shutter to %d\n", shutter);
    }
  }
  if (debug > 0)
    fprintf(stderr, "shutter set, now the gain to %d\n", gain);
  /* and the gain */
  if (gain == 0) {
    if (dc1394_auto_on_off(Cameras[cam].handle, camera_nodes[0], FEATURE_GAIN, 1) != DC1394_SUCCESS) {
      fprintf(stderr, "unable to set gain to auto\n");
    }
  } else {
    if (dc1394_auto_on_off(Cameras[cam].handle, camera_nodes[0], FEATURE_GAIN, 0) != DC1394_SUCCESS) {
      fprintf(stderr, "unable to set gain to manual\n");
    }
    if (dc1394_set_gain(Cameras[cam].handle, camera_nodes[0], gain) != DC1394_SUCCESS) {
      fprintf(stderr, "unable to set gain to %d\n", gain);
    }
  }

  if (debug > 0)
    fprintf(stderr, "gain set now allocate the memory\n");

  /* free the camera nodes */
  dc1394_free_camera_nodes(camera_nodes);

/*   /\* make room for the answers and write down all of the knobs for later use *\/ */
/*   if ( (width != Cameras[cam].width) || */
/*        (height != Cameras[cam].height) || */
/*        (max_frames != Cameras[cam].max_frames) ){ */
/*     if (Cameras[cam].frames != NULL) */
/*       free(Cameras[cam].frames); */
/*     if (Cameras[cam].times != NULL) */
/*       free(Cameras[cam].times); */
/*     Cameras[cam].frames = NULL; */
/*     Cameras[cam].times = NULL; */
/*     Cameras[cam].width=0; */
/*     Cameras[cam].height=0; */
/*     Cameras[cam].max_frames=0; */
/*     Cameras[cam].debug = debug; */
/*   } */
/*   if (Cameras[cam].frames == NULL) { */
/*     Cameras[cam].frames = (unsigned char *)malloc(width*height*max_frames); */
/*     if (Cameras[cam].frames == NULL) { */
/*       fprintf(stderr, "Could not allocate memory for %d frames (%d x %d)\n", max_frames, width, height); */
/*       dc1394_dma_release_camera(Cameras[cam].handle,&Cameras[cam].camera); */
/*       Cameras[cam].max_frames = 0; */
/*       return(0); */
/*     } */
/*     Cameras[cam].times = (double *)malloc(max_frames*sizeof(double)); */
/*     if (Cameras[cam].times == NULL) { */
/*       fprintf(stderr, "could not allocate memory for %d times - Aborting \n", max_frames); */
/*       free (Cameras[cam].frames); */
/*       Cameras[cam].frames=NULL; */
/*       dc1394_dma_release_camera(Cameras[cam].handle,&Cameras[cam].camera); */
/*       return(0); */
/*     } */

/*     Cameras[cam].width = width; */
/*     Cameras[cam].height = height; */
/*     Cameras[cam].max_frames = max_frames; */
/*   } */
/*   Cameras[cam].next_frame = 0; */
/*   Cameras[cam].trigger_mode = trigger_mode; */
/*   Cameras[cam].shutter = shutter; */
/*   Cameras[cam].gain = gain; */

/*   /\* fill the frames with zeros *\/ */
/*   memset((void *)Cameras[cam].frames, 0, width*height*max_frames); */

  /* fill the frames with zeros */
  memset((void *)Cameras[cam].frames, 0, width * height * max_frames);

/*    if (dc1394_set_trigger_on_off(Cameras[cam].handle, Cameras[cam].camera.node, */
/* 				0) != DC1394_SUCCESS) */
/*     { */
/*       fprintf(stderr, "unable to set trigger on \n"); */
/*     } */

/*-----------------------------------------------------------------------
   *  have the camera start sending us data
   *-----------------------------------------------------------------------*/
  if (debug > 0)
    fprintf(stderr, "start the ISO transmission\n");
  if (dc1394_start_iso_transmission(Cameras[cam].handle, Cameras[cam].camera.node)
      != DC1394_SUCCESS) {
    fprintf(stderr, "unable to start camera iso transmission\n");
    dc1394_dma_release_camera(Cameras[cam].handle, &Cameras[cam].camera);
    dc1394_destroy_handle(Cameras[cam].handle);
    return (0);
  }

  if (debug > 0)
    fprintf(stderr, "Create the thread\n");
  if (pthread_create(&Cameras[cam].thread_id, NULL, AcquireFrames, (void *)cam) != 0) {
    fprintf(stderr, "Could not create thread to handle camera daq\n");
    dc1394_dma_release_camera(Cameras[cam].handle, &Cameras[cam].camera);
    return (0);
  }
  if (debug > 0)
    fprintf(stderr, "all done with init\n");
  return (1);
}

static void *AcquireFrames(void *arg)
{
  struct timeval tv;
  int start_sec;
  int cam = (int)arg;

  pthread_cleanup_push(dc1394Done, arg);
  gettimeofday(&tv, NULL);
  start_sec = tv.tv_sec;
  //  fprintf(stderr, "Here I am in AcquireFrames - max_frames is %d,  next_frame is %d cam is %d\n", Cameras[cam].max_frames, Cameras[cam].next_frame, cam);
  for (Cameras[cam].next_frame = 0; Cameras[cam].next_frame < Cameras[cam].max_frames;
       Cameras[cam].next_frame++) {
    if (Cameras[cam].debug > 1)
      fprintf(stderr, "trying to read %d \n", Cameras[cam].next_frame);
    if (dc1394_dma_single_capture(&Cameras[cam].camera) != DC1394_SUCCESS) {
      fprintf(stderr, "unable to capture a frame\n");
      break;
    }
    //  fprintf(stderr, "\t got one\n");
    gettimeofday(&tv, NULL);
    Cameras[cam].times[Cameras[cam].next_frame] = tv.tv_sec - start_sec + (double)tv.tv_usec * 1E-6;
    memcpy((void *)Cameras[cam].frames +
	   Cameras[cam].next_frame * Cameras[cam].width * Cameras[cam].height * Cameras[cam].bpp,
	   Cameras[cam].camera.capture_buffer,
	   Cameras[cam].width * Cameras[cam].height * Cameras[cam].bpp);
    dc1394_dma_done_with_buffer(&Cameras[cam].camera);
  }
  /* clean up active daq */
  dc1394_stop_iso_transmission(Cameras[cam].handle, Cameras[cam].camera.node);
  dc1394_dma_unlisten(Cameras[cam].handle, &Cameras[cam].camera);
  dc1394_dma_release_camera(Cameras[cam].handle, &Cameras[cam].camera);

  Cameras[cam].thread_id = 0;
  pthread_cleanup_pop(0);
  if (Cameras[cam].debug > 1)
    fprintf(stderr, "All done with acquire \n");

  return;
}

EXPORT int dc1394ReadFrames(int cam, char *data)
{
  if (first) {
    fprintf(stderr, "***********************\nIn ReadFrames but first is set ! - returning 0\n");
    return 0;
  }
  if (CheckCamBuf
      (Cameras[cam].frames,
       Cameras[cam].width * Cameras[cam].height * Cameras[cam].max_frames * Cameras[cam].bpp) !=
      0) {
    fprintf(stderr,
	    "*******ReadFrames**************\n Camera buffer trashed \n*************************");
    return 0;
  }

  if (Cameras[cam].debug > 0)
    fprintf(stderr, "Reading frames\n");
  if (Cameras[cam].frames != NULL)
    if (Cameras[cam].bpp == 2) {
      swab(Cameras[cam].frames, data,
	   Cameras[cam].width * Cameras[cam].height * Cameras[cam].next_frame * Cameras[cam].bpp);
    } else {
      memmove(data, Cameras[cam].frames,
	      Cameras[cam].width * Cameras[cam].height * Cameras[cam].next_frame *
	      Cameras[cam].bpp);
  } else
    fprintf(stderr, "No memory allocated ReadFrames\n");
  return (1);
}

EXPORT int dc1394ReadTimes(int cam, double *data)
{
  if (first) {
    fprintf(stderr, "***********************\nIn ReadTimes but first is set ! - returning 0\n");
    return 0;
  }
  if (CheckCamBuf
      (Cameras[cam].frames,
       Cameras[cam].width * Cameras[cam].height * Cameras[cam].max_frames * Cameras[cam].bpp) !=
      0) {
    fprintf(stderr,
	    "*******ReadTimes**************\n Camera buffer trashed \n*************************");
    return 0;
  }
  if (Cameras[cam].debug > 0)
    fprintf(stderr, "Reading times\n");
  if (Cameras[cam].times != NULL)
    memmove((char *)data, Cameras[cam].times, Cameras[cam].next_frame * sizeof(double));
  else
    fprintf(stderr, "No memory allocated ReadTimes\n");
  if (Cameras[cam].frames != NULL)
    return 1;
}

EXPORT int dc1394NumFrames(int cam)
{
  if (first) {
    fprintf(stderr, "***********************\nIn NumFrames but first is set ! - returning 0\n");
    return 0;
  }
  if (CheckCamBuf
      (Cameras[cam].frames,
       Cameras[cam].width * Cameras[cam].height * Cameras[cam].max_frames * Cameras[cam].bpp) !=
      0) {
    fprintf(stderr,
	    "*******NumTimes**************\n Camera buffer trashed \n*************************");
    return 0;
  }
  if (Cameras[cam].debug > 0)
    fprintf(stderr, "Reading number of frames\n");
  return (Cameras[cam].next_frame);
}

EXPORT void dc1394Done(void *arg)
{
  int cam = (int)arg;
  /* clean up any active daq */
  if (first)
    fprintf(stderr, "***********************\nIn Done but first is set ! - returning 0\n");

  if (CheckCamBuf
      (Cameras[cam].frames,
       Cameras[cam].width * Cameras[cam].height * Cameras[cam].max_frames * Cameras[cam].bpp) !=
      0) {
    fprintf(stderr,
	    "*******Done**************\n Camera buffer trashed \n*************************");
    return;
  }
  if (Cameras[cam].debug > 0)
    fprintf(stderr, "In done\n");

  if (Cameras[cam].thread_id) {
    void *status;
    if (Cameras[cam].debug > 0)
      fprintf(stderr, "  unlisten\n");
    dc1394_dma_unlisten(Cameras[cam].handle, &Cameras[cam].camera);
    if (Cameras[cam].debug > 0)
      fprintf(stderr, "  stop the ios_transmission\n");
    dc1394_stop_iso_transmission(Cameras[cam].handle, Cameras[cam].camera.node);
    if (Cameras[cam].debug > 0)
      fprintf(stderr, "  release the dma resources\n");
    dc1394_dma_release_camera(Cameras[cam].handle, &Cameras[cam].camera);
    if (Cameras[cam].debug > 0)
      fprintf(stderr, "  Cancel the thread\n");
    /*
       pthread_cancel(Cameras[cam].thread_id);
       pthread_join(Cameras[cam].thread_id, &status);
     */
    Cameras[cam].thread_id = 0;
  }
  if (Cameras[cam].debug > 0)
    fprintf(stderr, "done with Done\n");
  return;
}

EXPORT void dc1394Cleanup(void *arg)
{
  int cam = (int)arg;
  void *status = NULL;
  if (Cameras[cam].thread_id) {
    if (Cameras[cam].debug > 0)
      fprintf(stderr, "  Cancel the thread\n");
    pthread_cancel(Cameras[cam].thread_id);
    pthread_join(Cameras[cam].thread_id, &status);
    Cameras[cam].thread_id = 0;
  }
}

EXPORT int dc1394LoadImage()
{
  fprintf(stderr, "dc1394_support.so now loaded \n");
  return (1);
}
