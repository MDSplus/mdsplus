#ifdef __cplusplus
extern "C" {
#endif

int camOpenTree(char *treeName, int shot, void **treePtr);
int camCloseTree(void *treePtr);
//int camSaveFrame(void *frame, int width, int height, float frameTime, int pixelSize, void *treePtr, int dataNid, int timebaseNid, int frameIdx, void *frameMetadata, int metaSize, int metaNid);
void camSaveFrame(void *frame, int width, int height, float frameTime, int pixelSize, void *treePtr, int dataNid, int timebaseNid, int frameIdx, void *frameMetadata, int metaSize, int metaNid, void *saveListPtr);

void *handleSave(void *listPtr);
void startSave(void **retList);
void stopSave(void *listPtr);

#ifdef __cplusplus
}
#endif

