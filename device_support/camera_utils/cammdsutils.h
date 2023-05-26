#ifdef __cplusplus
extern "C" {
#endif

int camOpenTree(char *treeName, int shot, void **treePtr);
int camCloseTree(void *treePtr);
//int camSaveFrame(void *frame, int width, int height, float frameTime, int pixelSize, void *treePtr, int dataNid, int timebaseNid, int frameIdx, void *frameMetadata, int metaSize, int metaNid);
void camSaveFrame(void *frame, int width, int height, float frameTime, int pixelSize, void *treePtr, int dataNid, int timebaseNid, int frameIdx, void *frameMetadata, int metaSize, int metaNid, void *saveListPtr);
void camSaveFrameDirect(void *frame, int width, int height, float frameTime, int pixelSize, void *treePtr, int dataNid, int timebaseNid,
	int frameIdx, void *saveListPtr);

static void *handleSave(void *listPtr);
void camStartSaveDeferred(void **retList);
void camStartSave(void **retList);
void camStopSave(void *listPtr);

int camSavedFrame(void *listPtr);
int camBlackFrame(void *listPtr);

#ifdef __cplusplus
}
#endif

