extern int DevLong(int *nid, int *ans);
extern int DevFloat(int *nid, float *ans);
extern int DevCamChk(int status, int *expect_x, int *expect_q);
extern int DevNids(struct descriptor *niddsc, int size, int *buffer);
extern int DevText(int *nid, struct descriptor_d *out);
extern int DevNid(int *nid_in, int *nid_out);
extern int DevWait(float time_in);
