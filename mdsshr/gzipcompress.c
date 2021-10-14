#include <zlib.h>
#include <string.h>

#include <mdsdescrip.h>
#include <strroutines.h>

#include <stdio.h>

EXPORT int gzip(
    const mdsdsc_a_t *const items_dsc_ptr,
    mdsdsc_a_t *const pack_dsc_ptr,
    int *const bit_ptr,
    mdsdsc_d_t * pdximage,
    mdsdsc_d_t * pdxentry
)
{
    int ret;

    // unsigned long maxDestinationSize = compressBound(items_dsc_ptr->length);

    unsigned long pack_length = pack_dsc_ptr->arsize; 

    ret = compress(
        (Bytef *)pack_dsc_ptr->pointer,
        &pack_length,
        (Bytef *)items_dsc_ptr->pointer,
        items_dsc_ptr->arsize
    );

    if (ret != Z_OK) {
        return LibSTRTRU;
    }

    printf("gzip() %u => %lu\n", pack_dsc_ptr->arsize, pack_length);

    // The new compressed length, in bits (for some reason)
    *bit_ptr = pack_length * 8;

    if (pdximage) {
        const char * IMAGE = "libMdsShr";
        unsigned short IMAGE_LEN = strlen(IMAGE);
        StrCopyR((mdsdsc_t * const)pdximage, &IMAGE_LEN, (char *)IMAGE);
    }

    if (pdxentry) {
        const char * ENTRY = "gunzip";
        unsigned short ENTRY_LEN = strlen(ENTRY);
        StrCopyR((mdsdsc_t * const)pdxentry, &ENTRY_LEN, (char *)ENTRY);
    }

    printf("gzip() called successfully\n");
    return 1;
}
#define UNUSED(x) (void)(x)
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

    printf("gunzip() called successfully\n");
    return 1;
}
