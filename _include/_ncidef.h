#ifndef INTERNAL_NCIDEF_H
#define INTERNAL_NCIDEF_H
/**************************************
  _NCIDEF.H - definitions of constants
  used in  to define compression method 
  names and routines.
**************************************/


typedef struct compression_method {
    char *name;
    char *method;
    char *image;
} COMPRESSION_METHOD;

#define DEFINE_COMPRESSION_METHODS \
  static const COMPRESSION_METHOD compression_methods[] = {{"standard", NULL, NULL}, {"gzip", "gzip", "libMdsShr"}}; \
  static const size_t NUM_COMPRESSION_METHODS = sizeof(compression_methods)/sizeof(COMPRESSION_METHOD);
#endif //INTERNAL_NCIDEF_H
