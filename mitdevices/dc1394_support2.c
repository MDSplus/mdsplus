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

/******************************************************************
   name:        dc1394_support2.c
   purpose:     Callable routines for dc1394a device type
   description: Based on libdc1394 version 2.  Works with RH5 (2.6) 
********************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <dc1394/dc1394.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>
#include <sys/times.h>
#include <pthread.h>
#include <mdsplus/mdsconfig.h>

static unsigned char *buf = NULL;
static double *frame_times = NULL;
static dc1394camera_t *camera = NULL;
static dc1394video_frame_t *frame = NULL;
static dc1394_t *d = NULL;
static dc1394camera_list_t *list = NULL;
static dc1394error_t err;
static unsigned int next_frame = 0;
static int framesize = 0;
static pthread_t thread_id = 0;
static int max_frames = 0;
static int debug_flag;
static unsigned int width;
static unsigned int height;
static int bytes;
static char model[43] = "\0";
static int needSwap = 0;

static void CleanUp(void *arg __attribute__ ((unused)))
{
  if (debug_flag)
    printf("Starting Cleanup\n");
  dc1394_capture_stop(camera);
  dc1394_video_set_transmission(camera, DC1394_OFF);
  dc1394_camera_free(camera);
  dc1394_free(d);
  camera = NULL;
  d = NULL;
  thread_id = 0;
  if (debug_flag)
    printf("Done with Cleanup\n");
}

static void swapbytes(unsigned char *dest, unsigned char *src, unsigned int sz)
{
  register unsigned int i;
  register unsigned char t;
  for (i = 0; i < sz; i += 2) {
    t = src[i];
    dest[i] = src[i + 1];
    dest[i + 1] = t;
  }
}

EXPORT void *CaptureFrames(void *arg __attribute__ ((unused)))
{
  uint64_t start_time;

  int i;
  pthread_cleanup_push(CleanUp, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &i);
  start_time = 0;

    /*-----------------------------------------------------------------------
     *  have the camera start sending us data
     *-----------------------------------------------------------------------*/
  err = dc1394_video_set_transmission(camera, DC1394_ON);
  if (err != DC1394_SUCCESS) {
    dc1394_log_error("unable to start camera iso transmission");
    dc1394_capture_stop(camera);
    dc1394_camera_free(camera);
    return (0);
  }

  printf("Transmission started\n");

  for (i = 0; i < max_frames; i++) {
    if (debug_flag)
      printf(" try to get frame %d\n", i);
    err = dc1394_capture_dequeue(camera, DC1394_CAPTURE_POLICY_WAIT, &frame);
    if (err != DC1394_SUCCESS)
      break;
    if (start_time == 0)
      start_time = frame->timestamp;
    frame_times[i] = (frame->timestamp - start_time) / 1E6;
    needSwap = ((frame->data_depth == 16) && !frame->little_endian);
    memcpy(&buf[i * framesize], frame->image, framesize);
    err = dc1394_capture_enqueue(camera, frame);
    if (err != DC1394_SUCCESS)
      break;
    if (debug_flag)
      printf("got one - time -s %f\n", frame_times[i]);
    next_frame++;
  }
  pthread_cleanup_pop(1);
  return (0);
}

/*
 * dc1394Init - set up capture, allocate buffers and capture the frames 
 *
 * Arguments:
 *     int mode 
 *        DC1394_VIDEO_MODE_160x120_YUV444= 64,
 *        DC1394_VIDEO_MODE_320x240_YUV422,
 *        DC1394_VIDEO_MODE_640x480_YUV411,
 *        DC1394_VIDEO_MODE_640x480_YUV422,
 *        DC1394_VIDEO_MODE_640x480_RGB8,
 *        DC1394_VIDEO_MODE_640x480_MONO8,
 *        DC1394_VIDEO_MODE_640x480_MONO16,
 *        DC1394_VIDEO_MODE_800x600_YUV422,
 *        DC1394_VIDEO_MODE_800x600_RGB8,
 *        DC1394_VIDEO_MODE_800x600_MONO8,
 *        DC1394_VIDEO_MODE_1024x768_YUV422,
 *        DC1394_VIDEO_MODE_1024x768_RGB8,
 *        DC1394_VIDEO_MODE_1024x768_MONO8,
 *        DC1394_VIDEO_MODE_800x600_MONO16,
 *        DC1394_VIDEO_MODE_1024x768_MONO16,
 *        DC1394_VIDEO_MODE_1280x960_YUV422,
 *        DC1394_VIDEO_MODE_1280x960_RGB8,
 *        DC1394_VIDEO_MODE_1280x960_MONO8,
 *        DC1394_VIDEO_MODE_1600x1200_YUV422,
 *        DC1394_VIDEO_MODE_1600x1200_RGB8,
 *        DC1394_VIDEO_MODE_1600x1200_MONO8,
 *        DC1394_VIDEO_MODE_1280x960_MONO16,
 *        DC1394_VIDEO_MODE_1600x1200_MONO16,
 *        DC1394_VIDEO_MODE_EXIF,
 *        DC1394_VIDEO_MODE_FORMAT7_0,
 *        DC1394_VIDEO_MODE_FORMAT7_1,
 *        DC1394_VIDEO_MODE_FORMAT7_2,
 *        DC1394_VIDEO_MODE_FORMAT7_3,
 *        DC1394_VIDEO_MODE_FORMAT7_4,
 *        DC1394_VIDEO_MODE_FORMAT7_5,
 *        DC1394_VIDEO_MODE_FORMAT7_6,
 *        DC1394_VIDEO_MODE_FORMAT7_7
 *
 *      int iso_speed
 *        DC1394_ISO_SPEED_100= 0,
 *        DC1394_ISO_SPEED_200,
 *        DC1394_ISO_SPEED_400,
 *        DC1394_ISO_SPEED_800,
 *        DC1394_ISO_SPEED_1600,
 *        DC1394_ISO_SPEED_3200
 * 
 *     int max_frames - maximum number of frames to capture
 *        NOTE - Memory is allocated for all of them
 *
 *     int trigger_mode
 *        DC1394_TRIGGER_MODE_0= 384,
 *        DC1394_TRIGGER_MODE_1,
 *        DC1394_TRIGGER_MODE_2,
 *        DC1394_TRIGGER_MODE_3,
 *        DC1394_TRIGGER_MODE_4,
 *        DC1394_TRIGGER_MODE_5,
 *        DC1394_TRIGGER_MODE_14,
 *        DC1394_TRIGGER_MODE_15
 *
 *    int shutter - not sure of the range or units (1 .. 2k ?)
 *    int gain    - 
 *    int trig_on - external trigger 1  internal 0
 *    int frame_rate
 *       DC1394_FRAMERATE_1_875= 32,
 *       DC1394_FRAMERATE_3_75,
 *       DC1394_FRAMERATE_7_5,
 *       DC1394_FRAMERATE_15,
 *       DC1394_FRAMERATE_30,
 *       DC1394_FRAMERATE_60,
 *       DC1394_FRAMERATE_120,
 *       DC1394_FRAMERATE_240
 *    int debug - turn on / off debug printouts
 *
 */

EXPORT int dc1394Init(int mode, int iso_speed, int max_frames_in, int trigger_mode,
	       int shutter, int gain, int trig_on, int frame_rate, int width_in,
	       int height_in, int xoffset, int yoffset, int debug)
{
  pthread_t lthread_id;

  dc1394featureset_t features;
  dc1394video_modes_t modes;

  unsigned int bits_per_pixel;
  size_t i;
  debug_flag = debug;
  max_frames = max_frames_in;
  if (thread_id) {
    lthread_id = thread_id;
    if (pthread_cancel(thread_id) == 0) {
      if (pthread_join(lthread_id, NULL) != 0) {
	perror("Unable to join child thread - restarting parent process");
	exit(errno);
      }
    }
    thread_id = 0;
  }
  next_frame = 0;

  if (d == NULL) {
    d = dc1394_new();
    err = dc1394_camera_enumerate(d, &list);
    if (err != DC1394_SUCCESS) {
      fprintf(stderr, "%s: in %s (%s, line %d): Failed to enumerate cameras - restarting server\n",
	      dc1394_error_get_string(err), __FUNCTION__, __FILE__, __LINE__);
      exit(0);
    }
    if (list->num == 0) {
	fprintf(stderr, "no cameras found - restarting server\n");
	exit(0);
    }
  }
  if (camera == NULL) {
    camera = dc1394_camera_new(d, list->ids[0].guid);
    if (!camera) {
      fprintf(stderr, "Failed to initialize camera with guid %" PRIx64, list->ids[0].guid);
      dc1394_camera_free_list(list);
      exit(0);
    }
  }
  dc1394_capture_stop(camera);
  strncpy(model, camera->vendor, 20);
  strcat(model, "   ");
  strncat(model, camera->model, 20);

  if (debug)
    printf("Using camera model %s with GUID %" PRIx64 "\n", model, camera->guid);

  if (iso_speed >= DC1394_ISO_SPEED_800) {
    if (dc1394_video_set_operation_mode(camera, DC1394_OPERATION_MODE_1394B) != DC1394_SUCCESS) {
      fprintf(stderr, "Can't set 1394B mode. Reverting to 400Mbps\n");
      iso_speed = DC1394_ISO_SPEED_400;
    }
  }
  // set ISO speed:
  err = dc1394_video_set_iso_speed(camera, iso_speed);
  if (err != DC1394_SUCCESS) {
    fprintf(stderr, "Failed to set ISO speed. Error code %d\n", err);
    exit(0);
  }

  err = dc1394_video_get_supported_modes(camera, &modes);
  if (err != DC1394_SUCCESS) {
    fprintf(stderr, "Failed to get supported camera modes - restarting server\n");
    exit(0);
  }
  for (i = 0; i < modes.num; i++)
    if (mode == (int)modes.modes[i])
      break;

  if (i == modes.num) {
    fprintf(stderr, "Mode %d is not supported by camera\n", mode);
    return (0);
  }
  err = dc1394_video_set_mode(camera, mode);
  if (err != DC1394_SUCCESS) {
    fprintf(stderr, "Failed to set camera mode - restarting server\n");
    exit(0);
  }
  if ((mode >= DC1394_VIDEO_MODE_FORMAT7_0) && (mode <= DC1394_VIDEO_MODE_FORMAT7_7)) {
    if (debug > 0)
      fprintf(stderr, "format 7 so attempting to set width %d height %d xoffset %d yoffset %d\n",
	      width_in, height_in, xoffset, yoffset);
    err = dc1394_format7_set_roi(camera, DC1394_VIDEO_MODE_FORMAT7_0, DC1394_COLOR_CODING_MONO8, DC1394_USE_MAX_AVAIL,	// use max packet size
				 xoffset, yoffset,	// left, top
				 width_in, height_in);	// width, height
  }
  if (debug > 0)
    printf("mode set, now the frame rate\n");
  /* and the frame_rate */
  if (dc1394_video_set_framerate(camera, frame_rate) != DC1394_SUCCESS) {
    fprintf(stderr, "unable to set frame_rate to %d\n", frame_rate);
  }

  err = dc1394_capture_setup(camera, 4, DC1394_CAPTURE_FLAGS_DEFAULT);
  if (err != DC1394_SUCCESS) {
    fprintf(stderr, "Failed to setup capture - restarting server\n");
    exit(0);
  }

  if (debug > 0)
    fprintf(stderr, "DMA setup, now the trigger mode %d on/off %d\n", trigger_mode, trig_on);
  /* set trigger mode */
  if (dc1394_external_trigger_set_mode(camera, trigger_mode) != DC1394_SUCCESS) {
    fprintf(stderr, "unable to set camera trigger mode\n");
  }

  if (dc1394_external_trigger_set_power(camera, trig_on) != DC1394_SUCCESS) {
    fprintf(stderr, "unable to set trigger on to %d\n", trig_on);
  }

  if (debug > 0)
    printf("trigger mode set, now the shutter\n");
  /* set the shutter */
  if (dc1394_feature_set_mode
      (camera, DC1394_FEATURE_SHUTTER,
       (shutter == 0) ? DC1394_FEATURE_MODE_AUTO : DC1394_FEATURE_MODE_MANUAL) != DC1394_SUCCESS) {
    fprintf(stderr, "unable to set shutter to auto\n");
  }
  if (shutter != 0) {
    if (dc1394_feature_set_value(camera, DC1394_FEATURE_SHUTTER, shutter) != DC1394_SUCCESS) {
      fprintf(stderr, "unable to set shutter to %d\n", shutter);
    }
  }
  if (debug > 0)
    printf("shutter set, now the gain\n");
  /* and the gain */
  if (dc1394_feature_set_mode
      (camera, DC1394_FEATURE_GAIN,
       (gain == 0) ? DC1394_FEATURE_MODE_AUTO : DC1394_FEATURE_MODE_MANUAL) != DC1394_SUCCESS) {
    fprintf(stderr, "unable to set gain to auto\n");
  }
  if (gain != 0) {
    if (dc1394_feature_set_value(camera, DC1394_FEATURE_GAIN, gain) != DC1394_SUCCESS) {
      fprintf(stderr, "unable to set gain to %d\n", gain);
    }
  }

  if (dc1394_get_image_size_from_video_mode(camera, mode, &width, &height) != DC1394_SUCCESS)
    fprintf(stderr, "Could not get width and height from video mode");

  dc1394_video_get_data_depth(camera, &bits_per_pixel);
  bytes =
      (bits_per_pixel <= 8) ? 1 : ((bits_per_pixel <= 16) ? 2 : ((bits_per_pixel <= 24) ? 3 : 4));

  if (buf)
    free(buf);
  framesize = width * height * bytes;
  buf = (unsigned char *)malloc(framesize * max_frames);
  if (buf == NULL) {
    fprintf(stderr, "Failed to allocate frame memory - restarting server\n");
    exit(0);
  }
  if (frame_times)
    free(frame_times);
  frame_times = (double *)malloc(max_frames * sizeof(double));
  if (frame_times == NULL) {
    fprintf(stderr, "Failed to allocate times memory - restarting server\n");
    exit(0);
  }
    /*-----------------------------------------------------------------------
     *  report camera's features
     *-----------------------------------------------------------------------*/
  if (debug) {
    err = dc1394_feature_get_all(camera, &features);
    if (err != DC1394_SUCCESS) {
      dc1394_log_warning("Could not get feature set");
    } else {
      dc1394_feature_print_all(&features, stdout);
    }
  }

  if (debug)
    printf("Starting thread to take the frames\n");
  if (pthread_create(&thread_id, NULL, CaptureFrames, NULL) != 0) {
    perror("could not create thread to grab frames - restarting server\n");
    exit(0);
  }
  return (1);
}

EXPORT int dc1394ReadTimes(double *data)
{
  if (frame_times) {
    memcpy((char *)data, (char *)frame_times, next_frame * sizeof(double));
    return (1);
  } else
    return (0);
}

EXPORT int dc1394ReadFrames(unsigned char *data)
{
  int status;
  if (buf) {
    if (next_frame > 0) {
      if (needSwap)
	swapbytes(data, buf, framesize * next_frame);
      else
	memcpy(data, buf, framesize * next_frame);
      status = 1;
    } else {
      fprintf(stderr, "No frames taken");
      status = 0;
    }
  } else {
    fprintf(stderr, "Camera not initialized");
    status = 0;
  }
  return (status);
}

EXPORT void dc1394GetCameraModel(char *string)
{
  strcpy(string, model);
}

EXPORT void dc1394Done(void *arg __attribute__ ((unused)))
{
}

EXPORT void dc1394GetCaptureParams(int *num_frames, int *frame_width, int *frame_height, int *frame_depth)
{
  *num_frames = next_frame;
  *frame_width = width;
  *frame_height = height;
  *frame_depth = bytes;
}

EXPORT extern int dc1394LoadImage()
{
  fprintf(stderr, "dc1394_support.so now loaded \n");
  return (1);
}
