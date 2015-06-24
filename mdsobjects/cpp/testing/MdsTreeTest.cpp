#include <stdlib.h>
#include <mdsobjects.h>

#include "testing.h"
#include "testutils/unique_ptr.h"
#include "mdsplus/AutoPointer.hpp"

using namespace MDSplus;
using namespace testing;

//////////////////Class Tree/////////////////////////////////
//class EXPORT Tree
//{
//    friend void setActiveTree(Tree *tree);
//    friend Tree *getActiveTree();
//    Tree(void *dbid, char const * name, int shot);
//protected:
//    Tree(){}
//    std::string name;
//    int shot;
//    void *ctx;

//public:
//    Tree(char const * name, int shot);
//    Tree(char const * name, int shot, char const * mode);
//    ~Tree();
//    void *operator new(size_t sz);
//    void operator delete(void *p);
    
//    static void lock();
//    static void unlock();
//    static void setCurrent(char const * treeName, int shot);
//    static int getCurrent(char const * treeName);
//    static Tree *create(char const * name, int shot);

//    void *getCtx() {return ctx;}
//    void edit();
//    void write();
//    void quit();
//    TreeNode *getNode(char const *path);
//    TreeNode *getNode(TreePath *path);
//    TreeNode *getNode(String *path);
//    TreeNode *addNode(char const * name, char *usage);
//    TreeNode *addDevice(char const * name, char *type);
//    void remove(char const *name);
//    TreeNodeArray *getNodeWild(char const *path, int usageMask);
//    TreeNodeArray *getNodeWild(char const *path);
//    void setDefault(TreeNode *treeNode);
//    TreeNode *getDefault();
//    bool versionsInModelEnabled();
//    bool versionsInPulseEnabled();
//    bool isModified();
//    bool isOpenForEdit();
//    bool isReadOnly();
//    void setVersionsInModel(bool enable);
//    void setVersionsInPulse(bool enable);
//    void setViewDate(char *date);
//    void setTimeContext(Data *start, Data *end, Data *delta);
//    void createPulse(int shot);
//    void deletePulse(int shot);
//    StringArray *findTags(char *wild);
//    void removeTag(char const * tagName);
//    int64_t getDatafileSize();
//};
///////////////////End Class Tree /////////////////////



int main(int argc, char *argv[])
{
    BEGIN_TESTING(Tree);
    
    setenv("test_tree_path",".",true);
    
        
    { // CTR
        Tree *tree;        
        tree = new Tree("test_tree",-1,"NEW"); delete tree;
//        tree = new Tree("test_tree",-1,"NORMAL"); delete tree;
//        tree = new Tree("test_tree",-1,"READONLY"); delete tree;
//        tree = new Tree("test_tree",-1,"EDIT"); delete tree;
//        tree = new Tree("test_tree",-1); delete tree;
        
//        TEST_MDS_EXCEPTION ( new Tree("test_tree",-1,"dumy error"),
//                             "Invalid Open mode" );                                
    }
        
    
    
    
    
    END_TESTING;
}


