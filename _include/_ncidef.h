#ifndef INTERNAL_NCIDEF_H
#define INTERNAL_NCIDEF_H
/**************************************
  _NCIDEF.H - definitions of constants
  used in  to define compression method 
  names and routines.
**************************************/

#define DEFINE_COMPRESSION_METHOD_STRINGS \
    static const char *compression_methods[] = {"standard", "gzip"};

#define DEFINE_COMPRESSION_METHOD_METHODS \
    static const DESCRIPTOR(mdsshr_dsc, "libMdsShr"); \
    static const DESCRIPTOR(gzip_dsc, "gzip"); \
    static const mdsdsc_t *compression_images[] = {NULL, &mdsshr_dsc}; \
    static const mdsdsc_t *compression_routines[] = {NULL, &gzip_dsc}; 

#endif //INTERNAL_NCIDEF_H
