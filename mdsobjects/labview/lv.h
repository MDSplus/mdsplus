extcode.h : TH_REENTRANT EXTERNC void _FUNCC MoveBlock(const void *src,
                                                       void *dest, size_t);
/* accepts zero bytes */
extcode.h : TH_REENTRANT EXTERNC UHandle _FUNCC DSNewHandle(size_t);
and extcode.h : TH_REENTRANT EXTERNC MgErr _FUNCC NumericArrayResize(int32,
                                                                     int32,
                                                                     UHandle *,
                                                                     size_t);
