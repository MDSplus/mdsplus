#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <treeshr.h>
#include "mit_encoder_gen.h"

extern int TdiExecute();
extern int mit_encoder___set_event();
extern int GenDeviceFree();

static DESCRIPTOR(event_lookup,"EVENT_LOOKUP($)");
static int one=1;
#define return_on_error(func,statret) status = func; if (!(status & 1)) return statret
#define pio(f,a,d) return_on_error(DevCamChk(CamPiow(setup->name, a, f, d, 16, 0), &one, 0), status);

int mit_encoder___init(struct descriptor *niddsc_ptr, InInitStruct *setup)
{
  int       status;
  static short     code[] = {0,0,0,0,0,0,0};
  short     inhibit = 0;
  int       chan;
  for (chan = 0; chan < 7; chan++) {
    static int chan_nid;
    chan_nid = setup->head_nid + MIT_ENCODER_N_CHANNEL_1_EVENT + (MIT_ENCODER_N_CHANNEL_2 - MIT_ENCODER_N_CHANNEL_1) * chan;
    if (TreeIsOn(chan_nid)&1) {
      static struct descriptor code_dsc = {1, DTYPE_BU, CLASS_S, 0};
      static DESCRIPTOR_NID(cnid_dsc,&chan_nid);
      code_dsc.pointer = (char *)&code[chan];
      return_on_error(TdiExecute(&event_lookup, &cnid_dsc, &code_dsc MDS_END_ARG), status);
    }
    else
      inhibit |= 1 << chan;
  }
  for (chan = 0; chan < 7; chan++)
    if (!(inhibit & (1 << chan))) {
      int       a = chan + 1;
      pio(17,a,&code[chan]);
    }
  pio(16,0,&inhibit);
  return status;
}

int mit_encoder___trigger(struct descriptor *niddsc_ptr, InTriggerStruct *setup)
{
  int status;
  pio(25,0,0);
  return status;
}

#undef pio
#define pio(f,a,d) return_on_error(DevCamChk(CamPiow(setup.name, a, f, d, 16, 0), &one, 0), status);

int mit_encoder__set_event(struct descriptor *niddsc_ptr, struct descriptor *method_dsc, struct descriptor *event_dsc)
{
  InSet_eventStruct setup;
  int status = mit_encoder___set_event(niddsc_ptr, &setup);
  if (status & 1)
  {
    static unsigned short event = 0;
    static struct descriptor event_d = {1,DTYPE_BU,CLASS_S,(char *)&event};
    static int nid;
    static DESCRIPTOR_NID(cnid_dsc,&nid);
    if ((status = TdiExecute(&event_lookup, event_dsc, &event_d MDS_END_ARG)) & 1)
    {
      pio(17,7,&event);
      pio(25,0,0);  
      nid = setup.head_nid + MIT_ENCODER_N_SOFT_CHANNEL_EVENT;
      if (TreeIsOn(nid)&1) {
        struct descriptor_d name = {0, DTYPE_T, CLASS_D, 0};
        int lstatus = DevText(&nid, (struct descriptor *)&name);
        if (lstatus && (name.length > 0))
          if (TdiExecute(&event_lookup, &cnid_dsc, &event_d MDS_END_ARG) & 1) {
            pio(17,7,&event);
          }
      }
    }
    GenDeviceFree(&setup);
  }
  return status;
}
