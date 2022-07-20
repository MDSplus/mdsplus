#include <camdef.h>
#include <camshr.h>
#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <ncidef.h>
#include <stdlib.h>
#include <string.h>
#include <strroutines.h>
#include <tdishr.h>
#include <treeshr.h>
#include <usagedef.h>

extern int GenDeviceSignal(int, unsigned long, unsigned long);
extern int GenDeviceCallData(int, int, struct descriptor_xd *);
extern int GenDeviceCvtFloatCode();
extern int GenDeviceCvtIntCode();
extern int GenDeviceCvtStringCode();
extern int GenDeviceHeadNid();
extern int GenDeviceFree();
extern int GenDeviceHeadNid();

#ifndef MDSMSG_CONSTANTS

#define MDSMSG_CONSTANTS
#define MSG_FATAL 0
#define MSG_SUCCESS 1
#define MSG_ERROR 2
#define MSG_INFO 3
#define MSG_WARNING 4
#define MSG_MASK 0x8008000
#define MSG_FACNUM_M 0x40000
#define MSG_MSGNUM_M 8

#endif

#define DEV$_BAD_ENDIDX \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 1 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_FILTER \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 2 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_FREQ \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 3 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_GAIN \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 4 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_HEADER \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 5 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_HEADER_IDX \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 6 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_MEMORIES \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 7 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_MODE \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 8 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_NAME \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 9 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_OFFSET \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 10 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_STARTIDX \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 11 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_NOT_TRIGGERED \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 12 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_FREQ_TOO_HIGH \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 13 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_INVALID_NOC \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 14 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_RANGE_MISMATCH \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 15 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAMACERR \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 16 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_VERBS \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 17 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BAD_COMMANDS \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 18 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_ADNR \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 19 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_ERR \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 20 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_LOSYNC \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 21 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_LPE \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 22 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_TMO \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 23 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_TPE \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 24 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_STE \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 25 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_DERR \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 26 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_SQ \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 27 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_NOSQ \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 28 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_SX \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 29 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_CAM_NOSX \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 30 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_INV_SETUP \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 31 * MSG_MSGNUM_M + MSG_ERROR)
#define DEV$_BADPARAM \
  (MSG_MASK + 2015 * MSG_FACNUM_M + 32 * MSG_MSGNUM_M + MSG_ERROR)
#define GEN_DEV$_NODATA \
  (MSG_MASK + 2037 * MSG_FACNUM_M + 1 * MSG_MSGNUM_M + MSG_WARNING)
#define GEN_DEV$_INV_SETUP \
  (MSG_MASK + 2037 * MSG_FACNUM_M + 2 * MSG_MSGNUM_M + MSG_WARNING)
#define GEN_DEV$_X_IO_ERR \
  (MSG_MASK + 2037 * MSG_FACNUM_M + 3 * MSG_MSGNUM_M + MSG_WARNING)

#ifdef MSG_LIBRARY

int getmsg(int sts, char **facnam, char **msgnam, char **msgtext)
{
  static struct msg
  {
    int sts;
    char *facnam;
    char *msgnam;
    char *msgtext;
  } msgs[] = {
      {DEV$_BAD_ENDIDX, "DEV", "BAD_ENDIDX",
       "unable to read end index for channel"},
      {DEV$_BAD_FILTER, "DEV", "BAD_FILTER", "illegal filter selected"},
      {DEV$_BAD_FREQ, "DEV", "BAD_FREQ",
       "illegal digitization frequency selected"},
      {DEV$_BAD_GAIN, "DEV", "BAD_GAIN", "illegal gain selected"},
      {DEV$_BAD_HEADER, "DEV", "BAD_HEADER", "unable to read header selection"},
      {DEV$_BAD_HEADER_IDX, "DEV", "BAD_HEADER_IDX",
       "unknown header configuration index"},
      {DEV$_BAD_MEMORIES, "DEV", "BAD_MEMORIES",
       "unable to read number of memory modules"},
      {DEV$_BAD_MODE, "DEV", "BAD_MODE", "illegal mode selected"},
      {DEV$_BAD_NAME, "DEV", "BAD_NAME", "unable to read module name"},
      {DEV$_BAD_OFFSET, "DEV", "BAD_OFFSET", "illegal offset selected"},
      {DEV$_BAD_STARTIDX, "DEV", "BAD_STARTIDX",
       "unable to read start index for channel"},
      {DEV$_NOT_TRIGGERED, "DEV", "NOT_TRIGGERED",
       "device was not triggered,  check wires and triggering device"},
      {DEV$_FREQ_TOO_HIGH, "DEV", "FREQ_TOO_HIGH",
       "the frequency is set to high for the requested number of channels"},
      {DEV$_INVALID_NOC, "DEV", "INVALID_NOC",
       "the NOC (number of channels) requested is greater than the physical "
       "number of channels"},
      {DEV$_RANGE_MISMATCH, "DEV", "RANGE_MISMATCH",
       "the range specified on the menu doesn't match the range setting on the "
       "device"},
      {DEV$_CAMACERR, "DEV", "CAMACERR", "Error doing CAMAC IO"},
      {DEV$_BAD_VERBS, "DEV", "BAD_VERBS",
       "Error reading interpreter list (:VERBS)"},
      {DEV$_BAD_COMMANDS, "DEV", "BAD_COMMANDS", "Error reading command list"},
      {DEV$_CAM_ADNR, "DEV", "CAM_ADNR",
       "CAMAC: Address not recognized (2160)"},
      {DEV$_CAM_ERR, "DEV", "CAM_ERR",
       "CAMAC: Error reported by crate controler"},
      {DEV$_CAM_LOSYNC, "DEV", "CAM_LOSYNC",
       "CAMAC: Lost Syncronization error"},
      {DEV$_CAM_LPE, "DEV", "CAM_LPE", "CAMAC: Longitudinal Parity error"},
      {DEV$_CAM_TMO, "DEV", "CAM_TMO", "CAMAC: Highway time out error"},
      {DEV$_CAM_TPE, "DEV", "CAM_TPE", "CAMAC: Transverse Parity error"},
      {DEV$_CAM_STE, "DEV", "CAM_STE", "CAMAC: Serial Transmission error"},
      {DEV$_CAM_DERR, "DEV", "CAM_DERR", "CAMAC: Delayed error from SCC"},
      {DEV$_CAM_SQ, "DEV", "CAM_SQ", "CAMAC: I/O completion with Q = 1"},
      {DEV$_CAM_NOSQ, "DEV", "CAM_NOSQ", "CAMAC: I/O completion with Q = 0"},
      {DEV$_CAM_SX, "DEV", "CAM_SX", "CAMAC: I/O completion with X = 1"},
      {DEV$_CAM_NOSX, "DEV", "CAM_NOSX", "CAMAC: I/O completion with X = 0"},
      {DEV$_INV_SETUP, "DEV", "INV_SETUP", "device was not properly set up"},
      {GEN_DEV$_NODATA, "GEN_DEV", "NODATA", "Data not Found"},
      {GEN_DEV$_INV_SETUP, "GEN_DEV", "INV_SETUP",
       "!%T - Device !AS model !AS - invalid setup "},
      {GEN_DEV$_X_IO_ERR, "GEN_DEV", "X_IO_ERR",
       " Hardware Error: X response failed"}};
  size_t i;
  int status = 0;
  for (i = 0; i < sizeof(msgs) / sizeof(struct msg); i++)
  {
    if (msgs[i].sts == sts)
    {
      *facnam = msgs[i].facnam;
      *msgnam = msgs[i].msgnam;
      *msgtext = msgs[i].msgtext;
      status = 1;
      break;
    }
  }
  return status;
}

#endif

#define STRING_LITERAL(string) "" #string ""

#define ADD_NODE(name, usage)                                     \
  {                                                               \
    status = TreeAddNode(STRING_LITERAL(name), &curr_nid, usage); \
    if (STATUS_NOT_OK)                                            \
    {                                                             \
      TreeSetDefaultNid(old_nid);                                 \
      return status;                                              \
    }                                                             \
    flags = 0;                                                    \
  }

#define ADD_NODE_INTEGER(name, value, usage)                               \
  {                                                                        \
    int num = value, curr_usage = usage;                                   \
    struct descriptor num_d = {4, DTYPE_L, CLASS_S, 0};                    \
    num_d.pointer = (char *)&num;                                          \
    if (curr_usage)                                                        \
      status = TreeAddNode(STRING_LITERAL(name), &curr_nid, curr_usage);   \
    else                                                                   \
      status =                                                             \
          TreeAddNode(STRING_LITERAL(name), &curr_nid, TreeUSAGE_NUMERIC); \
    if (STATUS_OK)                                                         \
      status = TreePutRecord(curr_nid, &num_d, 0);                         \
    if (STATUS_NOT_OK)                                                     \
    {                                                                      \
      TreeSetDefaultNid(old_nid);                                          \
      return status;                                                       \
    }                                                                      \
    flags = 0;                                                             \
  }

#define ADD_NODE_FLOAT(name, value, usage)                                 \
  {                                                                        \
    float num = value;                                                     \
    int curr_usage = usage;                                                \
    struct descriptor num_d = {4, DTYPE_F, CLASS_S, 0};                    \
    num_d.pointer = (char *)&num;                                          \
    if (curr_usage)                                                        \
      status = TreeAddNode(STRING_LITERAL(name), &curr_nid, curr_usage);   \
    else                                                                   \
      status =                                                             \
          TreeAddNode(STRING_LITERAL(name), &curr_nid, TreeUSAGE_NUMERIC); \
    if (STATUS_OK)                                                         \
      status = TreePutRecord(curr_nid, &num_d, 0);                         \
    if (STATUS_NOT_OK)                                                     \
    {                                                                      \
      TreeSetDefaultNid(old_nid);                                          \
      return status;                                                       \
    }                                                                      \
    flags = 0;                                                             \
  }

#define ADD_NODE_STRING(name, string, usage)                                 \
  {                                                                          \
    DESCRIPTOR(string_d, string);                                            \
    int curr_usage = usage;                                                  \
    if (curr_usage)                                                          \
      status = TreeAddNode(STRING_LITERAL(name), &curr_nid, curr_usage);     \
    else                                                                     \
      status = TreeAddNode(STRING_LITERAL(name), &curr_nid, TreeUSAGE_TEXT); \
    if (STATUS_OK)                                                           \
      status = TreePutRecord(curr_nid, (struct descriptor *)&string_d, 0);   \
    if (STATUS_NOT_OK)                                                       \
    {                                                                        \
      TreeSetDefaultNid(old_nid);                                            \
      return status;                                                         \
    }                                                                        \
    flags = 0;                                                               \
  }

#define ADD_NODE_EXPR(name, usage)                                           \
  {                                                                          \
    DESCRIPTOR(expr_d, expr);                                                \
    usage_t curr_usage = usage;                                              \
    struct descriptor_xd comp_expr_xd = {0, DTYPE_DSC, CLASS_XD, 0, 0};      \
    status =                                                                 \
        TdiCompile((struct descriptor *)&expr_d, &comp_expr_xd MDS_END_ARG); \
    if (STATUS_NOT_OK)                                                       \
    {                                                                        \
      TreeSetDefaultNid(old_nid);                                            \
      return status;                                                         \
    }                                                                        \
    if (!curr_usage)                                                         \
    {                                                                        \
      switch (comp_expr_xd.pointer->dtype)                                   \
      {                                                                      \
      case DTYPE_DISPATCH:                                                   \
        curr_usage = TreeUSAGE_DISPATCH;                                     \
        break;                                                               \
      case DTYPE_ACTION:                                                     \
        curr_usage = TreeUSAGE_ACTION;                                       \
        break;                                                               \
      case DTYPE_CONGLOM:                                                    \
        curr_usage = TreeUSAGE_DEVICE;                                       \
        break;                                                               \
      case DTYPE_PROGRAM:                                                    \
      case DTYPE_METHOD:                                                     \
      case DTYPE_ROUTINE:                                                    \
        curr_usage = TreeUSAGE_TASK;                                         \
        break;                                                               \
      case DTYPE_SIGNAL:                                                     \
        curr_usage = TreeUSAGE_SIGNAL;                                       \
        break;                                                               \
      case DTYPE_RANGE:                                                      \
        curr_usage = TreeUSAGE_AXIS;                                         \
        break;                                                               \
      case DTYPE_WINDOW:                                                     \
        curr_usage = TreeUSAGE_WINDOW;                                       \
        break;                                                               \
      case DTYPE_T:                                                          \
        curr_usage = TreeUSAGE_TEXT;                                         \
        break;                                                               \
      default:                                                               \
        curr_usage = TreeUSAGE_NUMERIC;                                      \
      }                                                                      \
    }                                                                        \
    status = TreeAddNode(STRING_LITERAL(name), &curr_nid, curr_usage);       \
    if (STATUS_OK)                                                           \
      status = TreePutRecord(curr_nid, comp_expr_xd.pointer, 0);             \
    if (STATUS_NOT_OK)                                                       \
    {                                                                        \
      TreeSetDefaultNid(old_nid);                                            \
      return status;                                                         \
    }                                                                        \
    MdsFree1Dx(&comp_expr_xd, 0);                                            \
    flags = 0;                                                               \
  }

#define ADD_NODE_ACTION(name, method_in, phase_in, sequence, completion_in,    \
                        timout, server, usage)                                 \
  {                                                                            \
    DESCRIPTOR(phase_d, STRING_LITERAL(phase_in));                             \
    DESCRIPTOR(method_d, STRING_LITERAL(method_in));                           \
    DESCRIPTOR(server_d, STRING_LITERAL(server));                              \
    char *compl = completion_in;                                               \
    int tim = timout, seq = sequence;                                          \
    treesched_t disp_mode = TreeSCHED_SEQ;                                     \
    usage_t curr_usage = usage;                                                \
    struct descriptor sequence_d = {4, DTYPE_L, CLASS_S, 0},                   \
                      timout_d = {4, DTYPE_L, CLASS_S, 0},                     \
                      completion_d = {0, DTYPE_T, CLASS_S, 0},                 \
                      object_d = {4, DTYPE_NID, CLASS_S, 0};                   \
    DESCRIPTOR_DISPATCH(dispatch_d, 0, 0, 0, 0, 0);                            \
    DESCRIPTOR_METHOD_0(task_d, 0, 0, 0);                                      \
    DESCRIPTOR_ACTION(action_d, 0, 0, 0);                                      \
    completion_d.pointer = compl;                                              \
    sequence_d.pointer = (char *)&seq;                                         \
    timout_d.pointer = (char *)&tim;                                           \
    object_d.pointer = (char *)&head_nid;                                      \
    dispatch_d.pointer = &disp_mode;                                           \
    dispatch_d.ident = (struct descriptor *)&server_d;                         \
    dispatch_d.phase = (struct descriptor *)&phase_d;                          \
    dispatch_d.when = &sequence_d;                                             \
    task_d.method = (struct descriptor *)&method_d;                            \
    task_d.object = (struct descriptor *)&object_d;                            \
    action_d.dispatch = (struct descriptor *)&dispatch_d;                      \
    action_d.task = (struct descriptor *)&task_d;                              \
    if (tim)                                                                   \
      task_d.time_out = &timout_d;                                             \
    if (compl)                                                                 \
    {                                                                          \
      completion_d.length = strlen(compl);                                     \
      dispatch_d.completion = &completion_d;                                   \
    }                                                                          \
    if (curr_usage)                                                            \
      status = TreeAddNode(STRING_LITERAL(name), &curr_nid, curr_usage);       \
    else                                                                       \
      status = TreeAddNode(STRING_LITERAL(name), &curr_nid, TreeUSAGE_ACTION); \
    if (STATUS_OK)                                                             \
      status = TreePutRecord(curr_nid, (struct descriptor *)&action_d, 0);     \
    if (STATUS_NOT_OK)                                                         \
    {                                                                          \
      TreeSetDefaultNid(old_nid);                                              \
      return status;                                                           \
    }                                                                          \
    flags = 0;                                                                 \
  }

#define COPY_PART_NAME(name)                                      \
  {                                                               \
    DESCRIPTOR(return_d, STRING_LITERAL(name));                   \
    if (out_d->class == CLASS_XD)                                 \
    {                                                             \
      status = MdsCopyDxXd(&return_d, (mdsdsc_xd_t *const)out_d); \
    }                                                             \
    else                                                          \
    {                                                             \
      status = StrCopyDx((struct descriptor *)out_d,              \
                         (struct descriptor *)&return_d);         \
    }                                                             \
  }

#define DevNODATA 2
#define DevMODSTR 1
#define DevMODINT 2
#define DevMODFLO 3
#define DevMODSLO 4
#define DevMODRAN 5
#define DevMODSEAR 6
#define DevMODRANLON 7

#define declare_variables(in_struct_type)                           \
  int error_code __attribute__((unused)) = 1;                       \
  int head_nid = GenDeviceHeadNid(nid_d_ptr), status;               \
  short code __attribute__((unused));                               \
  struct descriptor_xd xd                                           \
      __attribute__((unused)) = {0, DTYPE_DSC, CLASS_XD, 0, 0};     \
  struct descriptor_a *array_d_ptr __attribute__((unused));         \
  in_struct_type in_struct __attribute__((unused));                 \
  static int curr_nid __attribute__((unused));                      \
  static int curr_int __attribute__((unused));                      \
  static float curr_float __attribute__((unused));                  \
  static struct descriptor curr_nid_d __attribute__((unused)) = {   \
      sizeof(int), DTYPE_NID, CLASS_S, (char *)&curr_nid};          \
  static struct descriptor curr_float_d __attribute__((unused)) = { \
      sizeof(float), DTYPE_FS, CLASS_S, (char *)&curr_float};       \
  static struct descriptor curr_int_d __attribute__((unused)) = {   \
      sizeof(int), DTYPE_L, CLASS_S, (char *)&curr_int};

#define initialize_variables(in_struct_type)                \
  memset((char *)&in_struct, '\0', sizeof(in_struct_type)); \
  in_struct.head_nid = head_nid;

#define next_xd                                     \
  memcpy((char *)&work_xd[xd_count++], (char *)&xd, \
         sizeof(struct descriptor_xd));             \
  xd.pointer = 0;                                   \
  xd.l_length = 0;

#define read_descriptor(pos, field)      \
  curr_nid = head_nid + pos;             \
  status = TreeGetRecord(curr_nid, &xd); \
  if (STATUS_OK)                         \
    in_struct.field = xd.pointer;        \
  next_xd
#define read_descriptor_error(pos, field, err)                             \
  curr_nid = head_nid + pos;                                               \
  status = TreeGetRecord(curr_nid, &xd);                                   \
  if (STATUS_NOT_OK)                                                       \
    error(head_nid, GEN_DEV$_INV_SETUP, err) in_struct.field = xd.pointer; \
  next_xd
#define read_integer(pos, field)                          \
  curr_nid = head_nid + pos;                              \
  status = TdiData(&curr_nid_d, &curr_int_d MDS_END_ARG); \
  if (STATUS_OK)                                          \
    in_struct.field = curr_int;
#define read_integer_error(pos, field, err)               \
  curr_nid = head_nid + pos;                              \
  status = TdiData(&curr_nid_d, &curr_int_d MDS_END_ARG); \
  if (STATUS_NOT_OK)                                      \
    error(head_nid, GEN_DEV$_INV_SETUP, err) else in_struct.field = curr_int;
#define check_range(field, min, max, err)                 \
  if ((in_struct.field < min) || (in_struct.field > max)) \
    error(head_nid, GEN_DEV$_INV_SETUP, err);
#define check_integer_set(field, table, max_idx, err)                   \
  status = GenDeviceCvtIntCode(&code, in_struct.field, table, max_idx); \
  if (STATUS_NOT_OK)                                                    \
  error(head_nid, GEN_DEV$_INV_SETUP, err)
#define check_integer_conv_set(field, conv_field, table, max_idx, err)  \
  status = GenDeviceCvtIntCode(&code, in_struct.field, table, max_idx); \
  if (STATUS_NOT_OK)                                                    \
    error(head_nid, GEN_DEV$_INV_SETUP, err) else in_struct.conv_field = code;
#define read_integer_array_error(pos, field, field_count, err)               \
  curr_nid = head_nid + pos;                                                 \
  status = GenDeviceCallData(DevMODINT, curr_nid, &xd);                      \
  if (STATUS_NOT_OK)                                                         \
    error(head_nid, GEN_DEV$_INV_SETUP, err) else                            \
    {                                                                        \
      array_d_ptr = (struct descriptor_a *)xd.pointer;                       \
      if (array_d_ptr->class != CLASS_A)                                     \
        error(head_nid, GEN_DEV$_INV_SETUP, err) else                        \
        {                                                                    \
          in_struct.field = (int *)array_d_ptr->pointer;                     \
          in_struct.field_count = array_d_ptr->arsize / array_d_ptr->length; \
        }                                                                    \
    }                                                                        \
  next_xd
#define read_integer_array(pos, field, field_count)                      \
  curr_nid = head_nid + pos;                                             \
  status = GenDeviceCallData(DevMODINT, curr_nid, &xd);                  \
  if (STATUS_OK)                                                         \
  {                                                                      \
    array_d_ptr = (struct descriptor_a *)xd.pointer;                     \
    if (array_d_ptr->class == CLASS_A)                                   \
    {                                                                    \
      in_struct.field = (int *)array_d_ptr->pointer;                     \
      in_struct.field_count = array_d_ptr->arsize / array_d_ptr->length; \
    }                                                                    \
  }                                                                      \
  next_xd
#define read_float_array_error(pos, field, field_count, err)             \
  curr_nid = head_nid + pos;                                             \
  status = GenDeviceCallData(DevMODFLO, curr_nid, &xd);                  \
  if (STATUS_NOT_OK)                                                     \
    error(head_nid, GEN_DEV$_INV_SETUP, err) array_d_ptr =               \
        (struct descriptor_a *)xd.pointer;                               \
  if (array_d_ptr->class != CLASS_A)                                     \
    error(head_nid, GEN_DEV$_INV_SETUP, err) else                        \
    {                                                                    \
      in_struct.field = (float *)array_d_ptr->pointer;                   \
      in_struct.field_count = array_d_ptr->arsize / array_d_ptr->length; \
    }                                                                    \
  }                                                                      \
  next_xd
#define read_float_array(pos, field, field_count)                        \
  curr_nid = head_nid + pos;                                             \
  status = GenDeviceCallData(DevMODFLO, curr_nid, &xd);                  \
  if (STATUS_OK)                                                         \
  {                                                                      \
    array_d_ptr = (struct descriptor_a *)xd.pointer;                     \
    if (array_d_ptr->class == CLASS_A)                                   \
    {                                                                    \
      in_struct.field = (float *)array_d_ptr->pointer;                   \
      in_struct.field_count = array_d_ptr->arsize / array_d_ptr->length; \
    }                                                                    \
  }                                                                      \
  next_xd
#define read_float(pos, field)                              \
  curr_nid = head_nid + pos;                                \
  status = TdiData(&curr_nid_d, &curr_float_d MDS_END_ARG); \
  if (STATUS_OK)                                            \
    in_struct.field = curr_float;
#define read_float_error(pos, field, err)                   \
  curr_nid = head_nid + pos;                                \
  status = TdiData(&curr_nid_d, &curr_float_d MDS_END_ARG); \
  if (STATUS_NOT_OK)                                        \
    error(head_nid, GEN_DEV$_INV_SETUP, err) else in_struct.field = curr_float;
#define check_float_set(field, table, max_idx, err)                       \
  status = GenDeviceCvtFloatCode(&code, in_struct.field, table, max_idx); \
  if (STATUS_NOT_OK)                                                      \
  error(head_nid, GEN_DEV$_INV_SETUP, err)
#define check_float_conv_set(field, conv_field, table, max_idx, err)      \
  status = GenDeviceCvtFloatCode(&code, in_struct.field, table, max_idx); \
  if (STATUS_NOT_OK)                                                      \
    error(head_nid, GEN_DEV$_INV_SETUP, err) else in_struct.conv_field = code;
#define read_string(pos, field)                         \
  curr_nid = head_nid + pos;                            \
  status = GenDeviceCallData(DevMODSTR, curr_nid, &xd); \
  if (STATUS_OK)                                        \
    in_struct.field = xd.pointer->pointer;              \
  next_xd
#define read_string_error(pos, field, err)                          \
  curr_nid = head_nid + pos;                                        \
  status = GenDeviceCallData(DevMODSTR, curr_nid, &xd);             \
  if (STATUS_NOT_OK)                                                \
    error(head_nid, GEN_DEV$_INV_SETUP, err) else in_struct.field = \
        xd.pointer->pointer;                                        \
  next_xd
#define check_string_set(field, table, max_idx, err)                       \
  status = GenDeviceCvtStringCode(&code, in_struct.field, table, max_idx); \
  if (STATUS_NOT_OK)                                                       \
  error(head_nid, GEN_DEV$_INV_SETUP, err)
#define check_string_conv_set(field, conv_field, table, max_idx, err)      \
  status = GenDeviceCvtStringCode(&code, in_struct.field, table, max_idx); \
  if (STATUS_NOT_OK)                                                       \
    error(head_nid, GEN_DEV$_INV_SETUP, err) else in_struct.conv_field = code;
#define build_results_with_xd_and_return(num_xd)                             \
  in_struct.__xds =                                                          \
      (struct descriptor_xd *)malloc(num_xd * sizeof(struct descriptor_xd)); \
  in_struct.__num_xds = num_xd;                                              \
  memcpy((char *)in_struct.__xds, (char *)work_xd,                           \
         num_xd * sizeof(struct descriptor_xd));                             \
  *in_ptr = in_struct;                                                       \
  return error_code
#define build_results_and_return \
  *in_ptr = in_struct;           \
  return error_code
