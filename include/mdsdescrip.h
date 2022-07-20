#ifndef MDSDESCRIP_H_DEFINED
#define MDSDESCRIP_H_DEFINED
#include <inttypes.h>
#include <mdsplus/mdsconfig.h>
#include <status.h>
#define MAX_DIMS 8

#ifdef _WIN32
#define __char_align__ char
#define __fill_name__ fill
#define __fill_value__ 0,

#else /* _WINDOWS */
#define __char_align__
#define __fill_name__
#define __fill_value__
#endif /* _WINDOWS */

#ifdef MDSOBJECTSCPPSHRVS_EXPORTS
// visual studio uses int types for typedef
#define TYPEDEF(bytes) enum
#define ENDDEF(type, name) \
  ;                        \
  typedef type name
#else
#define TYPEDEF(bytes) typedef enum __attribute__((__packed__))
#define ENDDEF(type, name) name
#endif

typedef uint16_t length_t;

TYPEDEF(1){
#define DEFINE(NAME, value) DTYPE_##NAME = value,
#include "dtypedef.h"
#undef DEFINE
    DTYPE_UNDEFINED = 0} ENDDEF(uint8_t, dtype_t);

#define DTYPE_Z DTYPE_MISSING
#define DTYPE_NATIVE_FLOAT DTYPE_FS
#define DTYPE_NATIVE_DOUBLE DTYPE_FT
#define DTYPE_FLOAT_COMPLEX DTYPE_FSC
#define DTYPE_DOUBLE_COMPLEX DTYPE_FTC

#ifndef DTYPE_FLOAT
#define DTYPE_FLOAT DTYPE_NATIVE_FLOAT
#endif

#ifndef DTYPE_DOUBLE
#define DTYPE_DOUBLE DTYPE_NATIVE_DOUBLE
#endif

TYPEDEF(1){
#define DEFINE(NAME, value) CLASS_##NAME = value,
#include "classdef.h"
#undef DEFINE
    CLASS_MISSING = 0} ENDDEF(uint8_t, class_t);

TYPEDEF(2){
#define COM
#define OPC(name, NAME, ...) OPC_##NAME,
#include "opcbuiltins.h"
#undef OPC
#undef COM
    TdiFUNCTION_MAX} ENDDEF(uint16_t, opcode_t);

#ifdef __cplusplus
#define MDSDSC_HEAD(ptr_type)                                             \
  length_t length;   /* specific to descriptor class;  typically a 16-bit \
                        (unsigned) length */                              \
  dtype_t dtype;     /* data type code */                                 \
  class_t class_;    /* descriptor class code */                          \
  ptr_type *pointer; /* address of first byte of data element */
#else
#define MDSDSC_HEAD(ptr_type)                                             \
  length_t length;   /* specific to descriptor class;  typically a 16-bit \
                        (unsigned) length */                              \
  dtype_t dtype;     /* data type code */                                 \
  class_t class;     /* descriptor class code */                          \
  ptr_type *pointer; /* address of first byte of data element */
#endif
/*
 *	Descriptor Prototype - each class of descriptor consists of at least the
 *following fields:
 */
typedef struct descriptor
{
  MDSDSC_HEAD(char)
} mdsdsc_t;

/*
 *	Fixed-Length Descriptor:
 */
typedef struct descriptor_s
{
  MDSDSC_HEAD(char)
} mdsdsc_s_t;

/*
 *	Dynamic String Descriptor:
 */
typedef struct descriptor_d
{
  MDSDSC_HEAD(char)
} mdsdsc_d_t;

/*
 *	Array Descriptor:
 */

typedef struct
{
  unsigned __char_align__ : 3; /* reserved;  must be zero */
  unsigned __char_align__
      binscale : 1; /* if set, scale is a power-of-two, otherwise, -ten */
  unsigned __char_align__
      redim : 1; /* if set, indicates the array can be redimensioned */
  unsigned __char_align__
      column : 1; /* if set, indicates column-major order (FORTRAN) */
  unsigned __char_align__
      coeff : 1; /* if set, indicates the multipliers block is present */
  unsigned __char_align__
      bounds : 1; /* if set, indicates the bounds block is present */
} aflags_t;       /* array flag bits */

typedef int8_t scale_t;
typedef uint8_t dimct_t;
typedef uint8_t digits_t;
typedef uint32_t arsize_t;

/*
length	length of an array element in bytes,
        or if dtype is K_DTYPE_V, bits,
        or if dtype is K_DTYPE_P, digits (4 bits each)
dtype	data type code
class	descriptor class code = K_CLASS_A
pointer address of first actual byte of data storage
scale 	signed power-of-two or -ten multiplier, as specified by
        binscale, to convert from internal to external form
digits	if nonzero, number of decimal digits in internal representation
aflags
dimct	number of dimensions
arsize	total size of array in bytes,
        or if dtype is K_DTYPE_P, digits (4 bits each)
*/

#define ARRAY_HEAD(ptr_type)                                                     \
  MDSDSC_HEAD(ptr_type)                                                          \
  scale_t scale;   /* signed power-of-two or -ten multiplier, as specified by */ \
  digits_t digits; /* if nonzero, number of decimal digits in internal           \
                      representation */                                          \
  aflags_t aflags;                                                               \
  dimct_t dimct;                                                                 \
  arsize_t arsize;

#define IS_ARRAY_DSC(p) \
  ((p)->class == CLASS_A || (p)->class == CLASS_CA || (p)->class == CLASS_APD)

typedef struct descriptor_a
{
  ARRAY_HEAD(char)
  /*
   * One or two optional blocks of information may follow contiguously at this
   * point; the first block contains information about the dimension multipliers
   * (if present, aflags.coeff is set), the second block contains information
   * about the dimension bounds (if present, aflags.bounds is set).  If the
   * bounds information is present, the multipliers information must also be
   * present.
   *
   * The multipliers block has the following format:
   *      char    *a0;            Address of the element whose subscripts are
   * all zero int     m [DIMCT];      Addressing coefficients (multipliers)
   *
   * The bounds block has the following format:
   *      struct
   *      {
   *              uint32_t    l;      Lower bound
   *              uint32_t    u;      Upper bound
   *      } bounds [DIMCT];
   *
   * (DIMCT represents the value contained in dimct.)
   */
} mdsdsc_a_t;

/************************************************
  Supplementary definitons for array classes.
  Useful for classes A, CA, and APD.
*************************************************/

#define ARRAY(ptr_type)  \
  struct                 \
  {                      \
    ARRAY_HEAD(ptr_type) \
  }

#define ARRAY_COEFF(ptr_type, dimct) \
  struct                             \
  {                                  \
    ARRAY_HEAD(ptr_type)             \
    ptr_type *a0;                    \
    uint32_t m[dimct];               \
  }
typedef uint32_t dim_t;
typedef struct
{
  int l;
  int u;
} bound_t;
#define ARRAY_BOUNDS(ptr_type, dimct) \
  struct                              \
  {                                   \
    ARRAY_HEAD(ptr_type)              \
    ptr_type *a0;                     \
    dim_t m[dimct];                   \
    bound_t bounds[dimct];            \
  }

#define DESCRIPTOR_A(name, len, type, ptr, arsize) \
  ARRAY(char)                                      \
  name = {(length_t)len, type, CLASS_A, (char *)ptr, 0, 0, {0, 1, 1, 0, 0}, 1, arsize}

#define DESCRIPTOR_A_COEFF(name, len, type, ptr, dimct, arsize) \
  ARRAY_COEFF(char, dimct)                                      \
  name = {(length_t)len, type, CLASS_A, (char *)ptr, 0, 0, {0, 1, 1, 1, 0}, dimct, arsize, 0, {0}}

#define DESCRIPTOR_A_COEFF_2(name, len, type, ptr, arsize, rows, columns) \
  ARRAY_COEFF(char, 2)                                                    \
  name = {(length_t)len,                                                  \
          type,                                                           \
          CLASS_A,                                                        \
          (char *)ptr,                                                    \
          0,                                                              \
          0,                                                              \
          {0, 1, 1, 1, 0},                                                \
          2,                                                              \
          arsize,                                                         \
          (char *)ptr,                                                    \
          {rows, columns}}

#define DESCRIPTOR_A_BOUNDS(name, len, type, ptr, dimct, arsize) \
  ARRAY_BOUNDS(char, dimct)                                      \
  name = {(length_t)len, type, CLASS_A, (char *)ptr, 0, 0, {0, 1, 1, 1, 1}, dimct, arsize, 0, {0}, {{0, 0}}}

/************************************************
  CLASS_XD extended dynamic descriptor definition.
        Dynamic memory pointed to must be freed
        using the pointer and l_length.
        length is not used and 0 by default.
        The descriptor is usually on the stack.
*************************************************/
typedef uint32_t l_length_t;
typedef struct descriptor_xd
{
  MDSDSC_HEAD(mdsdsc_t)
  l_length_t l_length;
} mdsdsc_xd_t;

#define MDSDSC_D_INITIALIZER \
  {                          \
    0, DTYPE_DSC, CLASS_D, 0 \
  }
#define MDSDSC_XD_INITIALIZER    \
  {                              \
    0, DTYPE_DSC, CLASS_XD, 0, 0 \
  }
#define EMPTYXD(name) mdsdsc_xd_t name = MDSDSC_XD_INITIALIZER

/************************************************
  CLASS_XS extended static descriptor definition.
*************************************************/

typedef struct descriptor_xs
{
  MDSDSC_HEAD(mdsdsc_t)
  l_length_t l_length;
} mdsdsc_xs_t;

/************************************************
  CLASS_R Record descriptor definition.
*************************************************/
typedef uint8_t ndesc_t;
typedef uint8_t rec_ptr_t;
#define RECORD_HEAD(ptr_type) \
  MDSDSC_HEAD(ptr_type)       \
  ndesc_t ndesc;              \
  unsigned __fill_name__ : 24;

typedef struct descriptor_r
{
  RECORD_HEAD(uint8_t)
  mdsdsc_t *dscptrs[1];
} mdsdsc_r_t;

#define RECORD(ndesc)         \
  struct                      \
  {                           \
    RECORD_HEAD(uint8_t)      \
    mdsdsc_t *dscptrs[ndesc]; \
  }

#define DESCRIPTOR_R(name, type, ndesc) \
  RECORD(ndesc)                         \
  name = {(length_t)0, type, CLASS_R, 0, ndesc, __fill_value__{0}}

/************************************************
  CLASS_CA Compressed array descriptor definition.
        This describes an array's shape and type.
        The evaluated class is CLASS_A.
        The length, dtype, multipliers, and
        bounds are imposed on the result.

        The data is reconstructed (decompressed) by
        the evaluation of the pointed descriptor.
        The reconstruction must be self-contained.
        It must have all length and type info.
*************************************************/

#define DESCRIPTOR_CA(name, len, type, ptr, arsize) \
  ARRAY(mdsdsc_t)                                   \
  name = {(length_t)len, type, CLASS_CA, (mdsdsc_t *)ptr, 0, 0, {0, 1, 1, 0, 0}, 1, arsize}
#define DESCRIPTOR_CA_COEFF(name, len, type, ptr, dimct, arsize) \
  ARRAY_COEFF(mdsdsc_t, dimct)                                   \
  name = {(length_t)len, type, CLASS_CA, (mdsdsc_t *)ptr, 0, 0, {0, 1, 1, 1, 0}, dimct, arsize}
#define DESCRIPTOR_CA_BOUNDS(name, len, type, ptr, dimct, arsize) \
  ARRAY_BOUNDS(mdsdsc_t, dimct)                                   \
  name = {(length_t)len, type, CLASS_CA, (mdsdsc_t *)ptr, 0, 0, {0, 1, 1, 1, 1}, dimct, arsize}

/************************************************
  CLASS_APD Array of pointers to descriptors.
        This describes an array's shape and type.
        The evaluated class is CLASS_A.
        The multipliers and
        bounds are imposed on the result.
        The final dtype, length, and arsize are
        from evaluation.

        The pointer points to a list of addresses
        of descriptors.
        However, the length and arsize describe
        the list of pointers to descriptors.
*************************************************/

#define DESCRIPTOR_APD(name, type, ptr, ndesc) \
  ARRAY(mdsdsc_t *)                            \
  name = {(length_t)sizeof(mdsdsc_t *),        \
          type,                                \
          CLASS_APD,                           \
          (mdsdsc_t **)ptr,                    \
          0,                                   \
          0,                                   \
          {0, 1, 1, 0, 0},                     \
          1,                                   \
          ndesc * sizeof(mdsdsc_t *)}
#define DESCRIPTOR_APD_COEFF(name, type, ptr, dimct, ndesc) \
  ARRAY_COEFF(mdsdsc_t *, dimct)                            \
  name = {(length_t)sizeof(mdsdsc_t *),                     \
          type,                                             \
          CLASS_APD,                                        \
          (mdsdsc_t **)ptr,                                 \
          0,                                                \
          0,                                                \
          {0, 1, 1, 1, 0},                                  \
          dimct,                                            \
          ndesc * sizeof(mdsdsc_t *)}
#define DESCRIPTOR_APD_BOUNDS(name, type, ptr, dimct, ndesc) \
  ARRAY_BOUNDS(mdsdsc_t *, dimct)                            \
  name = {(length_t)sizeof(mdsdsc_t *),                      \
          type,                                              \
          CLASS_APD,                                         \
          (mdsdsc_t **)ptr,                                  \
          0,                                                 \
          0,                                                 \
          {0, 1, 1, 1, 1},                                   \
          dimct,                                             \
          ndesc * sizeof(mdsdsc_t *)}

/*****************************************************
  MISSING marks omitted argument, converts to default.
  IDENT is text-like variable name for compiler.
  NID is 4-bytes describing a node identifier.
  PATH is text-like tree location.
  Others are supported record data types.
*****************************************************/

typedef struct descriptor_param
{
  RECORD_HEAD(void)
  mdsdsc_t *value;
  mdsdsc_t *help;
  mdsdsc_t *validation;
} mds_param_t;

#define DESCRIPTOR_PARAM(name, value, help, validation) \
  mds_param_t name = {(length_t)0,                      \
                      DTYPE_PARAM,                      \
                      CLASS_R,                          \
                      0,                                \
                      3,                                \
                      __fill_value__(mdsdsc_t *) value, \
                      (mdsdsc_t *)help,                 \
                      (mdsdsc_t *)validation}

typedef struct descriptor_signal
{
  RECORD_HEAD(void)
  mdsdsc_t *data;
  mdsdsc_t *raw;
  mdsdsc_t *dimensions[1];
} mds_signal_t;

#define SIGNAL(ndims)            \
  struct                         \
  {                              \
    RECORD_HEAD(void)            \
    mdsdsc_t *data;              \
    mdsdsc_t *raw;               \
    mdsdsc_t *dimensions[ndims]; \
  }

#define DESCRIPTOR_SIGNAL(name, ndims, data, raw) \
  SIGNAL(ndims)                                   \
  name = {0,                                      \
          DTYPE_SIGNAL,                           \
          CLASS_R,                                \
          0,                                      \
          ndims + 2,                              \
          __fill_value__(mdsdsc_t *) data,        \
          (mdsdsc_t *)raw,                        \
          {0}}
#define DESCRIPTOR_SIGNAL_1(name, data, raw, dimension) \
  SIGNAL(1)                                             \
  name = {0,                                            \
          DTYPE_SIGNAL,                                 \
          CLASS_R,                                      \
          0,                                            \
          3,                                            \
          __fill_value__(mdsdsc_t *) data,              \
          (mdsdsc_t *)raw,                              \
          {(mdsdsc_t *)dimension}}
#define DESCRIPTOR_SIGNAL_2(name, data, raw, dim_1, dim_2) \
  SIGNAL(2)                                                \
  name = {0,                                               \
          DTYPE_SIGNAL,                                    \
          CLASS_R,                                         \
          0,                                               \
          4,                                               \
          __fill_value__(mdsdsc_t *) data,                 \
          (mdsdsc_t *)raw,                                 \
          {(mdsdsc_t *)dim_1, (mdsdsc_t *)dim_2}}

typedef struct descriptor_dimension
{
  RECORD_HEAD(void)
  mdsdsc_t *window;
  mdsdsc_t *axis;
} mds_dimension_t;

#define DESCRIPTOR_DIMENSION(name, window, axis)             \
  mds_dimension_t name = {0,                                 \
                          DTYPE_DIMENSION,                   \
                          CLASS_R,                           \
                          0,                                 \
                          2,                                 \
                          __fill_value__(mdsdsc_t *) window, \
                          (mdsdsc_t *)axis}

typedef struct descriptor_window
{
  RECORD_HEAD(void)
  mdsdsc_t *startidx;
  mdsdsc_t *endingidx;
  mdsdsc_t *value_at_idx0;
} mds_window_t;

#define DESCRIPTOR_WINDOW(name, start, iend, xref)       \
  mds_window_t name = {0,                                \
                       DTYPE_WINDOW,                     \
                       CLASS_R,                          \
                       0,                                \
                       3,                                \
                       __fill_value__(mdsdsc_t *) start, \
                       (mdsdsc_t *)iend,                 \
                       (mdsdsc_t *)xref}

typedef struct descriptor_axis
{
  RECORD_HEAD(void)
  mdsdsc_t *dscptrs[3];
} mds_axis_t;

/* This is to be replaced by RANGEs of scalars or vectors */
typedef struct
{
  mdsdsc_t *slope;
  mdsdsc_t *begin;
  mdsdsc_t *ending;
} slope_segment_t;
typedef struct descriptor_slope
{
  RECORD_HEAD(void)
  slope_segment_t segment[1];
} mds_slope_t;

#define SLOPE(nsegs)                \
  struct                            \
  {                                 \
    RECORD_HEAD(void)               \
    slope_segment_t segment[nsegs]; \
  }

typedef struct descriptor_function
{
  RECORD_HEAD(opcode_t)
  mdsdsc_t *arguments[1];
} mds_function_t;

#define FUNCTION(nargs)         \
  struct                        \
  {                             \
    RECORD_HEAD(opcode_t)       \
    mdsdsc_t *arguments[nargs]; \
  }

#define DESCRIPTOR_FUNCTION(name, opcode_ptr, nargs)           \
  FUNCTION(nargs)                                              \
  name = {(length_t)sizeof(opcode_t), DTYPE_FUNCTION, CLASS_R, \
          (opcode_t *)opcode_ptr, nargs, __fill_value__{0}}

#define DESCRIPTOR_FUNCTION_0(name, opcode_ptr)      \
  mds_function_t name = {(length_t)sizeof(opcode_t), \
                         DTYPE_FUNCTION,             \
                         CLASS_R,                    \
                         (opcode_t *)opcode_ptr,     \
                         0,                          \
                         __fill_value__{0}}

#define DESCRIPTOR_FUNCTION_1(name, opcode_ptr, arg) \
  mds_function_t name = {(length_t)sizeof(opcode_t), \
                         DTYPE_FUNCTION,             \
                         CLASS_R,                    \
                         (opcode_t *)opcode_ptr,     \
                         1,                          \
                         __fill_value__{(mdsdsc_t *)arg}}

#define DESCRIPTOR_FUNCTION_2(name, opcode_ptr, arg_1, arg_2) \
  FUNCTION(2)                                                 \
  name = {(length_t)sizeof(opcode_t),                         \
          DTYPE_FUNCTION,                                     \
          CLASS_R,                                            \
          (opcode_t *)opcode_ptr,                             \
          2,                                                  \
          __fill_value__{(mdsdsc_t *)arg_1, (mdsdsc_t *)arg_2}}

typedef struct descriptor_conglom
{
  RECORD_HEAD(void)
  mdsdsc_t *image;
  mdsdsc_t *model;
  mdsdsc_t *name;
  mdsdsc_t *qualifiers;
} mds_conglom_t;

#define DESCRIPTOR_CONGLOM(sname, image, model, name, qualifiers) \
  mds_conglom_t sname = {0,                                       \
                         DTYPE_CONGLOM,                           \
                         CLASS_R,                                 \
                         0,                                       \
                         4,                                       \
                         __fill_value__(mdsdsc_t *) image,        \
                         (mdsdsc_t *)model,                       \
                         (mdsdsc_t *)name,                        \
                         (mdsdsc_t *)qualifiers}

typedef struct descriptor_range
{
  RECORD_HEAD(void)
  mdsdsc_t *begin;
  mdsdsc_t *ending;
  mdsdsc_t *deltaval;
} mds_range_t;

#define DESCRIPTOR_RANGE(name, begin, ending, delta)    \
  mds_range_t name = {0,                                \
                      DTYPE_RANGE,                      \
                      CLASS_R,                          \
                      0,                                \
                      3,                                \
                      __fill_value__(mdsdsc_t *) begin, \
                      (mdsdsc_t *)ending,               \
                      (mdsdsc_t *)delta}

typedef struct descriptor_action
{
  RECORD_HEAD(void)
  mdsdsc_t *dispatch;
  mdsdsc_t *task;
  mdsdsc_t *errorlogs;
  mdsdsc_t *completion_message;
  mdsdsc_a_t *performance;
} mds_action_t;

#define DESCRIPTOR_ACTION(name, dispatch, task, errorlogs)  \
  mds_action_t name = {0,                                   \
                       DTYPE_ACTION,                        \
                       CLASS_R,                             \
                       0,                                   \
                       5,                                   \
                       __fill_value__(mdsdsc_t *) dispatch, \
                       (mdsdsc_t *)task,                    \
                       (mdsdsc_t *)errorlogs,               \
                       0,                                   \
                       0}

/*****************************************************
  So far three types of scheduling are supported.
*****************************************************/
TYPEDEF(1){TreeSCHED_NONE = 0, TreeSCHED_ASYNC = 1, TreeSCHED_SEQ = 2,
           TreeSCHED_COND = 3} ENDDEF(uint8_t, treesched_t);

typedef struct descriptor_dispatch
{
  RECORD_HEAD(treesched_t)
  mdsdsc_t *ident;
  mdsdsc_t *phase;
  mdsdsc_t *when;
  mdsdsc_t *completion;
} mds_dispatch_t;

#define DESCRIPTOR_DISPATCH(name, type, ident, phase, when, completion) \
  mds_dispatch_t name = {(length_t)sizeof(treesched_t),                 \
                         DTYPE_DISPATCH,                                \
                         CLASS_R,                                       \
                         (treesched_t *)type,                           \
                         4,                                             \
                         __fill_value__(mdsdsc_t *) ident,              \
                         (mdsdsc_t *)phase,                             \
                         (mdsdsc_t *)when,                              \
                         (mdsdsc_t *)completion}

typedef struct descriptor_program
{
  RECORD_HEAD(void)
  mdsdsc_t *time_out;
  mdsdsc_t *program;
} mds_program_t;

#define DESCRIPTOR_PROGRAM(name, program, timeout)          \
  mds_program_t name = {0,                                  \
                        DTYPE_PROGRAM,                      \
                        CLASS_R,                            \
                        0,                                  \
                        2,                                  \
                        __fill_value__(mdsdsc_t *) timeout, \
                        (mdsdsc_t *)program}

typedef struct descriptor_routine
{
  RECORD_HEAD(void)
  mdsdsc_t *time_out;
  mdsdsc_t *image;
  mdsdsc_t *routine;
  mdsdsc_t *arguments[1];
} mds_routine_t;

#define ROUTINE(nargs)          \
  struct                        \
  {                             \
    RECORD_HEAD(void)           \
    mdsdsc_t *time_out;         \
    mdsdsc_t *image;            \
    mdsdsc_t *routine;          \
    mdsdsc_t *arguments[nargs]; \
  }

#define DESCRIPTOR_ROUTINE(name, image, routine, timeout, nargs) \
  ROUTINE(nargs)                                                 \
  name = {0,                                                     \
          DTYPE_ROUTINE,                                         \
          CLASS_R,                                               \
          0,                                                     \
          nargs + 3,                                             \
          __fill_value__(mdsdsc_t *) timeout,                    \
          (mdsdsc_t *)image,                                     \
          (mdsdsc_t *)routine}

typedef struct descriptor_procedure
{
  RECORD_HEAD(void)
  mdsdsc_t *time_out;
  mdsdsc_t *language;
  mdsdsc_t *procedure;
  mdsdsc_t *arguments[1];
} mds_procedure_t;

#define PROCEDURE(nargs)        \
  struct                        \
  {                             \
    RECORD_HEAD(void)           \
    mdsdsc_t *time_out;         \
    mdsdsc_t *language;         \
    mdsdsc_t *procedure;        \
    mdsdsc_t *arguments[nargs]; \
  }

#define DESCRIPTOR_PROCEDURE(name, language, procedure, timeout, nargs) \
  PROCEDURE(nargs)                                                      \
  name = {0,                                                            \
          DTYPE_PROCEDURE,                                              \
          CLASS_R,                                                      \
          0,                                                            \
          nargs + 3,                                                    \
          __fill_value__(mdsdsc_t *) timeout,                           \
          (mdsdsc_t *)language,                                         \
          (mdsdsc_t *)procedure}

typedef struct descriptor_method
{
  RECORD_HEAD(void)
  mdsdsc_t *time_out;
  mdsdsc_t *method;
  mdsdsc_t *object;
  mdsdsc_t *arguments[1];
} mds_method_t;

#define METHOD(nargs)           \
  struct                        \
  {                             \
    RECORD_HEAD(void)           \
    mdsdsc_t *time_out;         \
    mdsdsc_t *method;           \
    mdsdsc_t *object;           \
    mdsdsc_t *arguments[nargs]; \
  }

#define METHOD_0        \
  struct                \
  {                     \
    RECORD_HEAD(void)   \
    mdsdsc_t *time_out; \
    mdsdsc_t *method;   \
    mdsdsc_t *object;   \
  }

#define DESCRIPTOR_METHOD(name, method, object, timeout, nargs) \
  METHOD(nargs)                                                 \
  name = {0,                                                    \
          DTYPE_METHOD,                                         \
          CLASS_R,                                              \
          0,                                                    \
          nargs + 3,                                            \
          __fill_value__(mdsdsc_t *) timeout,                   \
          (mdsdsc_t *)method,                                   \
          (mdsdsc_t *)object}

#define DESCRIPTOR_METHOD_0(name, method, object, timeout) \
  METHOD_0                                                 \
  name = {0,                                               \
          DTYPE_METHOD,                                    \
          CLASS_R,                                         \
          0,                                               \
          3,                                               \
          __fill_value__(mdsdsc_t *) timeout,              \
          (mdsdsc_t *)method,                              \
          (mdsdsc_t *)object}

TYPEDEF(1){TreeDEPENDENCY_AND = 10, TreeDEPENDENCY_OR = 11} ENDDEF(uint8_t,
                                                                   treedep_t);

typedef struct descriptor_dependency
{
  RECORD_HEAD(treedep_t)
  mdsdsc_t *arguments[2];
} mds_dependency_t;

#define DESCRIPTOR_DEPENDENCY(name, mode_ptr, arg_1, arg_2)  \
  mds_dependency_t name = {(length_t)sizeof(treedep_t),      \
                           DTYPE_DEPENDENCY,                 \
                           CLASS_R,                          \
                           (treedep_t *)mode_ptr,            \
                           2,                                \
                           __fill_value__(mdsdsc_t *) arg_1, \
                           (mdsdsc_t *)arg_2}

TYPEDEF(1){
    TreeNEGATE_CONDITION = 7,
    TreeIGNORE_UNDEFINED = 8,
    TreeIGNORE_STATUS = 9,
} ENDDEF(uint8_t, treecond_t);

typedef struct descriptor_condition
{
  RECORD_HEAD(treecond_t)
  mdsdsc_t *condition;
} mds_condition_t;

#define DESCRIPTOR_CONDITION(name, mode_ptr, condition) \
  mds_condition_t name = {(length_t)sizeof(treecond_t), \
                          DTYPE_CONDITION,              \
                          CLASS_R,                      \
                          (treecond_t *)mode_ptr,       \
                          1,                            \
                          __fill_value__(mdsdsc_t *) condition}

typedef struct descriptor_with_units
{
  RECORD_HEAD(void)
  mdsdsc_t *data;
  mdsdsc_t *units;
} mds_with_units_t;

#define DESCRIPTOR_WITH_UNITS(name, value, units)            \
  mds_with_units_t name = {0,                                \
                           DTYPE_WITH_UNITS,                 \
                           CLASS_R,                          \
                           0,                                \
                           2,                                \
                           __fill_value__(mdsdsc_t *) value, \
                           (mdsdsc_t *)units}

/*********************************************
  A CALL invokes a routine in a shared image.
  TDI syntax is image->routine:type(arg0,...).
  The result (R0,R1) is given dtype, def=L.
*********************************************/

typedef struct descriptor_call
{
  RECORD_HEAD(dtype_t)
  mdsdsc_t *image;
  mdsdsc_t *routine;
  mdsdsc_t *arguments[1];
} mds_call_t;

#define CALL(nargs)             \
  struct                        \
  {                             \
    RECORD_HEAD(dtype_t)        \
    mdsdsc_t *image;            \
    mdsdsc_t *routine;          \
    mdsdsc_t *arguments[nargs]; \
  }

#define DESCRIPTOR_CALL(name, dtype_ptr, nargs, image, routine) \
  CALL(nargs)                                                   \
  name = {(length_t)sizeof(dtype_t),                            \
          DTYPE_CALL,                                           \
          CLASS_R,                                              \
          dtype_ptr,                                            \
          nargs + 2,                                            \
          __fill_value__(mdsdsc_t *) image,                     \
          (mdsdsc_t *)routine,                                  \
          {0}}

typedef struct descriptor_with_error
{
  RECORD_HEAD(void)
  mdsdsc_t *data;
  mdsdsc_t *error;
} mds_with_error_t;

#define DESCRIPTOR_WITH_ERROR(name, data, error)            \
  mds_with_error_t name = {0,                               \
                           DTYPE_WITH_ERROR,                \
                           CLASS_R,                         \
                           0,                               \
                           2,                               \
                           __fill_value__(mdsdsc_t *) data, \
                           (mdsdsc_t *)error}

typedef struct descriptor_opaque
{
  RECORD_HEAD(void)
  mdsdsc_t *data;
  mdsdsc_t *opaque_type;
} mds_opaque_t;

#define DESCRIPTOR_OPAQUE(name, data, opaque_type)      \
  mds_opaque_t name = {0,                               \
                       DTYPE_OPAQUE,                    \
                       CLASS_R,                         \
                       0,                               \
                       2,                               \
                       __fill_value__(mdsdsc_t *) data, \
                       (mdsdsc_t *)opaque_type}

#define DESCRIPTOR_FLOAT(name, _float)                                   \
  mdsdsc_t name = {(length_t)sizeof(float), DTYPE_NATIVE_FLOAT, CLASS_S, \
                   (char *)_float}

#define DESCRIPTOR_LONG(name, _int32) \
  mdsdsc_t name = {(length_t)sizeof(int32_t), DTYPE_L, CLASS_S, (char *)_int32}

#define DESCRIPTOR_QUADWORD(name, _int64) \
  mdsdsc_t name = {(length_t)sizeof(int64_t), DTYPE_Q, CLASS_S, (char *)_int64}

#define DESCRIPTOR_NID(name, _nid) \
  mdsdsc_t name = {(length_t)sizeof(int), DTYPE_NID, CLASS_S, (char *)_nid}

/*
 *	A simple macro to construct a 32-bit string descriptor:
 */
#define DESCRIPTOR(name, string)                                     \
  mdsdsc_t name = {(length_t)(sizeof(string) - 1), DTYPE_T, CLASS_S, \
                   (char *)(string)}
#define DESCRIPTOR_FROM_CSTRING(name, cstring)                  \
  mdsdsc_t name = {(length_t)strlen(cstring), DTYPE_T, CLASS_S, \
                   (char *)(cstring)}

/*
 * array typedefs
 */

/* ARRAY_COEFF(a,b*x) acts like ARRAY_BOUNDS(a,b)
 * but bounds and m are merged into m
 */
typedef ARRAY_COEFF(char, MAX_DIMS) array_coeff;
typedef ARRAY_COEFF(char, MAX_DIMS * 3) array_bounds;
typedef ARRAY_COEFF(mdsdsc_t *, MAX_DIMS * 3) array_bounds_desc;
typedef ARRAY(char) array;
typedef ARRAY(int) array_int;
typedef ARRAY(mdsdsc_t *) array_desc;
typedef SIGNAL(MAX_DIMS) signal_maxdim;

typedef union {
  mdsdsc_t dsc;
  // grep '} mdsdsc_' include/mdsdescrip.h | perl -n -e '/mdsdsc_([a-z]+)_t;/ &&
  // print "mdsdsc_$1_t\t$1;\n"'
  mdsdsc_s_t s;
  mdsdsc_d_t d;
  mdsdsc_a_t a;
  mdsdsc_xd_t xd;
  mdsdsc_xs_t xs;
  mdsdsc_r_t r;
  // grep '} mds_' include/mdsdescrip.h | perl -n -e '/mds_([a-z]+)_t;/ && print
  // "mds_$1_t\t$1;\n"'
  mds_param_t param;
  mds_signal_t signal;
  mds_dimension_t dimension;
  mds_window_t window;
  mds_axis_t axis;
  mds_slope_t slope;
  mds_function_t function;
  mds_conglom_t conglom;
  mds_range_t range;
  mds_action_t action;
  mds_dispatch_t dispatch;
  mds_program_t program;
  mds_routine_t routine;
  mds_procedure_t procedure;
  mds_method_t method;
  mds_dependency_t dependency;
  mds_condition_t condition;
  mds_call_t call;
  mds_opaque_t opaque;
} mdsobj_t;

#undef TYPEDEF
#undef ENDDEF
#endif
