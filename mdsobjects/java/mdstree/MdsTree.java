package mdstree;
import mdsdata.*;
public class MdsTree 
{

    public static native String getPath(int nid) throws TreeException;
    public static native String getFullPath(int nid) throws TreeException;
    public static native Data getData(int nid) throws TreeException;
    public static native void deleteData(int nid) throws TreeException;
    public static native void putData(int nid, Data data) throws TreeException;
    public static native void turnOn(int nid) throws TreeException;
    public static native void turnOff(int nid) throws TreeException;
    public static native boolean isOn(int nid) throws TreeException;
    public static native String getInsertionDate(int nid) throws TreeException;
    public static native void open(String experiment, int shot) throws TreeException;
    public static native void close(String experiment, int shot) throws TreeException;
    public static native int find(String path) throws TreeException;
    public static native int[] findWild(String path, int usage) throws TreeException;
    public static native int[] getMembersOf(int nid) throws TreeException;
    public static native int[] getChildrenOf(int nid) throws TreeException;
    public static native int getParent(int nid) throws TreeException;
    public static native int getDefault() throws TreeException;
    public static native void setDefault(int nid) throws TreeException;

    public static native void beginSegment(int nid, ArrayData data, Data start, Data end, Data dimension) throws TreeException;
    public static native void putRow(int nid, Data data, long time, boolean isLast) throws TreeException;
    public static native void beginCachedSegment(int nid, ArrayData data, Data start, Data end, Data dimension) throws TreeException;
    public static native void putCachedRow(int nid, Data data, long time, boolean isLast) throws TreeException;
    public static native void openCached(String experiment, int shot) throws TreeException;
    public static native void closeCached(String experiment, int shot) throws TreeException;
    public static native void configureCache(boolean isShared, int cacheSize);
    public static native void synchCache();

    public static native void putCachedData(int nid, Data data) throws TreeException;
    public static native Data getCachedData(int nid) throws TreeException;
    public static native void registerCallback(int nid, ActiveTreeNode listener) throws TreeException;
    public static native void unregisterCallback(int nid, ActiveTreeNode listener) throws TreeException;
}
