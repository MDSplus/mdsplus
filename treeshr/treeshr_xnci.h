
#ifndef _TREESHR_XNCI_H
#define _TREESHR_XNCI_H

#include <treeshr.h>
#ifdef __cplusplus
extern "C"
{
#endif
  // These API exports are unofficial and may be changed in future releases
  // without notice.

  extern EXPORT int _TreeXNciBeginSegment(void *dbid, int nid, const char *xnci,
                                          mdsdsc_t *start, mdsdsc_t *end,
                                          mdsdsc_t *dim, mdsdsc_a_t *initialData,
                                          int idx);
  extern EXPORT int _TreeXNciMakeSegment(void *dbid, int nid, const char *xnci,
                                         mdsdsc_t *start, mdsdsc_t *end,
                                         mdsdsc_t *dim, mdsdsc_a_t *initialData,
                                         int idx, int filled);
  extern EXPORT int _TreeXNciPutSegment(void *dbid, int nid, const char *xnci,
                                        const int rowidx, mdsdsc_a_t *data);
  extern EXPORT int _TreeXNciSetRowsFilled(void *dbid, int nid, const char *xnci,
                                           int rows_filled);
  extern EXPORT int _TreeXNciUpdateSegment(void *dbid, int nid, const char *xnci,
                                           mdsdsc_t *start, mdsdsc_t *end,
                                           mdsdsc_t *dim, int idx);
  extern EXPORT int _TreeXNciBeginTimestampedSegment(void *dbid, int nid,
                                                     const char *xnci,
                                                     mdsdsc_a_t *initialValue,
                                                     int idx);
  extern EXPORT int _TreeXNciPutTimestampedSegment(void *dbid, int nid,
                                                   const char *xnci,
                                                   int64_t *timestamp,
                                                   mdsdsc_a_t *rowdata);
  extern EXPORT int _TreeXNciMakeTimestampedSegment(void *dbid, int nid,
                                                    const char *xnci,
                                                    int64_t *timestamp,
                                                    mdsdsc_a_t *rowdata, int idx,
                                                    int filled);
  extern EXPORT int _TreeXNciPutRow(void *dbid, int nid, const char *xnci,
                                    int bufsize, int64_t *timestamp,
                                    mdsdsc_a_t *rowdata);
  extern EXPORT int _TreeXNciGetNumSegments(void *dbid, int nid, const char *xnci,
                                            int *num);
  extern EXPORT int _TreeXNciGetSegmentLimits(void *dbid, int nid,
                                              const char *xnci, int segidx,
                                              mdsdsc_xd_t *start,
                                              mdsdsc_xd_t *end);
  extern EXPORT int _TreeXNciGetSegment(void *dbid, int nid, const char *xnci,
                                        int segidx, mdsdsc_xd_t *data,
                                        mdsdsc_xd_t *dim);
  extern EXPORT int _TreeXNciGetSegments(void *dbid, int nid, const char *xnci,
                                         mdsdsc_t *start, mdsdsc_t *end,
                                         mdsdsc_xd_t *retSegments);
  extern EXPORT int _TreeXNciGetSegmentInfo(void *dbid, int nid, const char *xnci,
                                            int idx, char *dtype, char *dimct,
                                            int *dims, int *next_row);
  extern EXPORT int _TreeXNciGetSegments(void *dbid, int nid, const char *xnci,
                                         mdsdsc_t *start, mdsdsc_t *end,
                                         mdsdsc_xd_t *out);
  extern EXPORT int _TreeXNciGetSegmentTimes(void *dbid, int nid,
                                             const char *xnci, int *numsegs,
                                             int64_t **times);
  extern EXPORT int _TreeXNciGetSegmentTimesXd(void *dbid, int nid,
                                               const char *xnci, int *numsegs,
                                               mdsdsc_xd_t *start_list,
                                               mdsdsc_xd_t *end_list);
  extern EXPORT int _TreeXNciGetSegmentScale(void *dbid, int nid,
                                             const char *xnci,
                                             mdsdsc_xd_t *value);
  extern EXPORT int _TreeXNciSetSegmentScale(void *dbid, int nid,
                                             const char *xnci, mdsdsc_t *value);
#ifdef __cplusplus
}
#endif
#endif // ifndef _TREESHR_XNCI_H
