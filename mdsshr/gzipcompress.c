#include <string.h>

#include <mdsdescrip.h>
#include <strroutines.h>

#include <stdio.h>
#include <stdarg.h>
#include <zlib.h>

//#define DEBUG

#define UNUSED(x) (void)(x)

EXPORT int gzip(
    const int *const nitems_ptr,
    const mdsdsc_a_t *const items_dsc_ptr,
    mdsdsc_a_t *const pack_dsc_ptr,
    int *const bit_ptr,
    mdsdsc_d_t * const pdximage,
    mdsdsc_d_t * const pdxentry
)
{
    int ret;
    UNUSED(nitems_ptr);

    // unsigned long maxDestinationSize = compressBound(items_dsc_ptr->length);

    unsigned long pack_length = pack_dsc_ptr->arsize; 
    static const DESCRIPTOR(image, "libMdsShr");
    static const DESCRIPTOR(routine, "gunzip");
    ret = compress(
        (Bytef *)pack_dsc_ptr->pointer,
        &pack_length,
        (Bytef *)items_dsc_ptr->pointer,
        items_dsc_ptr->arsize
    );

    if (ret != Z_OK) {
        return LibSTRTRU;
    }

#ifdef DEBUG
    printf("gzip() %u => %lu\n", pack_dsc_ptr->arsize, pack_length);
#endif
    // The new compressed length, in bits (for some reason)
    *bit_ptr = pack_length * 8;

    if (pdximage) {
        StrCopyDx((mdsdsc_t * const)pdximage, &image);
    }

    if (pdxentry) {
        StrCopyDx((mdsdsc_t * const)pdxentry, &routine);
    }
#ifdef DEBUG
    printf("gzip() called successfully\n");
#endif
    return 1;
}
EXPORT int gunzip(
    int *const nitems_ptr,
    const mdsdsc_a_t *const pack_dsc_ptr,
    mdsdsc_a_t *const items_dsc_ptr,
    int *const bit_ptr
)
{
    int ret;
    UNUSED(nitems_ptr);
    UNUSED(bit_ptr);
    unsigned long items_length = items_dsc_ptr->arsize; 

    ret = uncompress(
        (Bytef *)items_dsc_ptr->pointer,
        &items_length,
        (Bytef *)pack_dsc_ptr->pointer,
        pack_dsc_ptr->arsize
    );

    if (ret != Z_OK) {
        return LibINVSTRDES;
    }
#ifdef DEBUG
    printf("gunzip() called successfully\n");
#endif
    return 1;
}
