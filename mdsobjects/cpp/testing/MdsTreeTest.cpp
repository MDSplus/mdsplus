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

//static int convertUsage(std::string const & usage)
//{
//	if (usage == "ACTION")
//		return TreeUSAGE_ACTION;
//	else if (usage == "ANY")
//		return TreeUSAGE_ANY;
//	else if (usage == "AXIS")
//		return TreeUSAGE_AXIS;
//	else if (usage == "COMPOUND_DATA")
//		return TreeUSAGE_COMPOUND_DATA;
//	else if (usage == "DEVICE")
//		return TreeUSAGE_DEVICE;
//	else if (usage == "DISPATCH")
//		return TreeUSAGE_DISPATCH;
//	else if (usage == "STRUCTURE")
//		return TreeUSAGE_STRUCTURE;
//	else if (usage == "NUMERIC")
//		return TreeUSAGE_NUMERIC;
//	else if (usage == "SIGNAL")
//		return TreeUSAGE_SIGNAL;
//	else if (usage == "SUBTREE")
//		return TreeUSAGE_SUBTREE;
//	else if (usage == "TASK")
//		return TreeUSAGE_TASK;
//	else if (usage == "TEXT")
//		return TreeUSAGE_TEXT;
//	else if (usage == "WINDOW")
//		return TreeUSAGE_WINDOW;
//	else
//		return TreeUSAGE_ANY;
//}


//#ifndef USAGEDEF_H

//#define TreeUSAGE_ANY       0
//#define TreeUSAGE_NONE      1 /******* USAGE_NONE is now USAGE_STRUCTURE *******/
//#define TreeUSAGE_STRUCTURE 1
//#define TreeUSAGE_ACTION    2
//#define TreeUSAGE_DEVICE    3
//#define TreeUSAGE_DISPATCH  4
//#define TreeUSAGE_NUMERIC   5
//#define TreeUSAGE_SIGNAL    6
//#define TreeUSAGE_TASK      7
//#define TreeUSAGE_TEXT      8
//#define TreeUSAGE_WINDOW    9
//#define TreeUSAGE_AXIS     10
//#define TreeUSAGE_SUBTREE  11
//#define TreeUSAGE_COMPOUND_DATA 12
//#define TreeUSAGE_MAXIMUM  TreeUSAGE_COMPOUND_DATA
//#define TreeUSAGE_SUBTREE_REF 14 /* Runtime only special usage */
//#define TreeUSAGE_SUBTREE_TOP 15 /* Runtime only special usage */

//#endif




int main(int argc, char *argv[])
{
    BEGIN_TESTING(Tree);
    
    setenv("test_tree_path",".",true);
    setenv("test_tree2_path",".",true);
    
        
    { // CTR
        Tree *tree;        
        tree = new Tree("test_tree",-1,"NEW");    delete tree;
        tree = new Tree("test_tree",-1,"NORMAL"); delete tree;
        tree = new Tree("test_tree",-1,"READONLY"); delete tree;
        tree = new Tree("test_tree",-1,"EDIT");     delete tree;
        tree = new Tree("test_tree",-1);          delete tree;
        
        TEST_MDS_EXCEPTION ( new Tree("test_tree",-1,"dumy error"),
                             "Invalid Open mode" );                                
    }
        
    { // write to parse file node usages //
        Tree *tree = new Tree("test_tree",-1,"NEW");
        
        // write // 
        tree->write();
        delete tree;
                       
        // edit  //
        tree = new Tree("test_tree",-1,"NORMAL");
        tree->edit();
        
        AutoData<TreeNode> node = tree->addNode("test_usage","STRUCTURE");
        tree->write();                        
       
        // fill all kinds of nodes into the test_usage structure //
        AutoData<TreeNode>(tree->addNode("\\test_tree::top.test_usage:ANY","ANY"));
        AutoData<TreeNode>(tree->addNode("\\test_tree::top.test_usage:STRUCTURE","STRUCTURE"));
        AutoData<TreeNode>(tree->addNode("\\test_tree::top.test_usage:AXIS","AXIS"));
        AutoData<TreeNode>(tree->addNode("\\test_tree::top.test_usage:COMPOUND","COMPOUND_DATA"));
        AutoData<TreeNode>(tree->addNode("\\test_tree::top.test_usage:DEVICE","DEVICE"));
        AutoData<TreeNode>(tree->addNode("\\test_tree::top.test_usage:DISPATCH","DISPATCH"));
        AutoData<TreeNode>(tree->addNode("\\test_tree::top.test_usage:NUMERIC","NUMERIC"));
        AutoData<TreeNode>(tree->addNode("\\test_tree::top.test_usage:SIGNAL","SIGNAL"));
        AutoData<TreeNode>(tree->addNode("\\test_tree::top.test_usage:SUBTREE","SUBTREE"));
        AutoData<TreeNode>(tree->addNode("\\test_tree::top.test_usage:TASK","TASK"));
        AutoData<TreeNode>(tree->addNode("\\test_tree::top.test_usage:TEXT","TEXT"));
        AutoData<TreeNode>(tree->addNode("\\test_tree::top.test_usage:WINDOW","WINDOW"));
        tree->write();
                                    
        delete tree;
    }

    { // get Node 
        unique_ptr<Tree> tree = new Tree("test_tree",-1,"READONLY");        
        TEST1( AutoData<TreeNode>(tree->getNode("\\test_tree::top.test_usage:ANY")).get() != NULL );                
        TEST1( AutoData<TreeNode>(tree->getNode(AutoData<String>(new String("\\test_tree::top.test_usage:ANY")))).get() != NULL );
        TEST1( AutoData<TreeNode>(tree->getNode(AutoData<TreePath>(new TreePath(std::string("\\test_tree::top.test_usage:ANY"), tree)))).get() != NULL );
        
        TEST1( AutoData<TreeNode>(tree->getNode("\\top.test_usage:ANY")).get() != NULL );
        TEST1( AutoData<TreeNode>(tree->getNode(AutoData<String>(new String("\\top.test_usage:ANY")))).get() != NULL );
        TEST1( AutoData<TreeNode>(tree->getNode(AutoData<TreePath>(new TreePath(std::string("\\top.test_usage:ANY"), tree)))).get() != NULL );
        
        // test relative paths //        
        TEST1( AutoData<TreeNode>(tree->getNode(".test_usage:ANY")).get() != NULL );
        TEST1( AutoData<TreeNode>(tree->getNode(AutoData<String>(new String(".test_usage:ANY")))).get() != NULL );
        TEST1( AutoData<TreeNode>(tree->getNode(AutoData<TreePath>(new TreePath(std::string(".test_usage:ANY"), tree)))).get() != NULL );
        
        TEST1( AutoData<TreeNode>(tree->getNode("test_usage:ANY")).get() != NULL );
        TEST1( AutoData<TreeNode>(tree->getNode(AutoData<String>(new String("test_usage:ANY")))).get() != NULL );
        TEST1( AutoData<TreeNode>(tree->getNode(AutoData<TreePath>(new TreePath(std::string("test_usage:ANY"), tree)))).get() != NULL );
        
        // set a default sub node //
        tree->setDefault( AutoData<TreeNode>(tree->getNode("test_usage")) );
        
        // test if absolute path is always available //
        TEST1( AutoData<TreeNode>(tree->getNode("\\top.test_usage:ANY")).get() != NULL );
        TEST1( AutoData<TreeNode>(tree->getNode(AutoData<String>(new String("\\top.test_usage:ANY")))).get() != NULL );
        TEST1( AutoData<TreeNode>(tree->getNode(AutoData<TreePath>(new TreePath(std::string("\\top.test_usage:ANY"), tree)))).get() != NULL );
        
        
        TEST1( AutoData<TreeNode>(tree->getNode(".ANY")).get() != NULL );
        TEST1( AutoData<TreeNode>(tree->getNode(AutoData<String>(new String(".ANY")))).get() != NULL );
        TEST1( AutoData<TreeNode>(tree->getNode(AutoData<TreePath>(new TreePath(std::string(".ANY"), tree)))).get() != NULL );
        
        TEST1( AutoData<TreeNode>(tree->getNode("ANY")).get() != NULL );
        TEST1( AutoData<TreeNode>(tree->getNode(AutoData<String>(new String("ANY")))).get() != NULL );
        TEST1( AutoData<TreeNode>(tree->getNode(AutoData<TreePath>(new TreePath(std::string("ANY"), tree)))).get() != NULL );                       
    }

    
    { // test usage and find by usage
        unique_ptr<Tree> tree = new Tree("test_tree",-1,"NORMAL");
        unique_ptr<TreeNodeArray> array;
        
        array = tree->getNodeWild("test_usage:*", 1 << TreeUSAGE_ANY);        
        TEST1(array->getNumNodes() == 1);
        TEST1(array->operator [](0)->getNodeNameStr() == "ANY");
                
                array = tree->getNodeWild("test_usage:*", 1 << TreeUSAGE_AXIS);        
        TEST1(array->getNumNodes() == 1);
        TEST1(array->operator [](0)->getNodeNameStr() == "AXIS");
    
                array = tree->getNodeWild("test_usage:*", 1 << TreeUSAGE_COMPOUND_DATA);        
        TEST1(array->getNumNodes() == 1);
        TEST1(array->operator [](0)->getNodeNameStr() == "COMPOUND");
                
                array = tree->getNodeWild("test_usage:*", 1 << TreeUSAGE_DEVICE);        
        TEST1(array->getNumNodes() == 1);
        TEST1(array->operator [](0)->getNodeNameStr() == "DEVICE");
                
                array = tree->getNodeWild("test_usage:*", 1 << TreeUSAGE_DISPATCH);        
        TEST1(array->getNumNodes() == 1);
        TEST1(array->operator [](0)->getNodeNameStr() == "DISPATCH");
                
                array = tree->getNodeWild("test_usage:*", 1 << TreeUSAGE_NUMERIC);        
        TEST1(array->getNumNodes() == 1);
        TEST1(array->operator [](0)->getNodeNameStr() == "NUMERIC");
                
                array = tree->getNodeWild("test_usage:*", 1 << TreeUSAGE_SIGNAL);        
        TEST1(array->getNumNodes() == 1);
        TEST1(array->operator [](0)->getNodeNameStr() == "SIGNAL");
                
                array = tree->getNodeWild("test_usage.*", 1 << TreeUSAGE_SUBTREE);        
        TEST1(array->getNumNodes() == 1);
        TEST1(array->operator [](0)->getNodeNameStr() == "SUBTREE");
                
                array = tree->getNodeWild("test_usage:*", 1 << TreeUSAGE_TASK);        
        TEST1(array->getNumNodes() == 1);
        TEST1(array->operator [](0)->getNodeNameStr() == "TASK");
                
                array = tree->getNodeWild("test_usage:*", 1 << TreeUSAGE_TEXT);        
        TEST1(array->getNumNodes() == 1);
        TEST1(array->operator [](0)->getNodeNameStr() == "TEXT");
                
                array = tree->getNodeWild("test_usage:*", 1 << TreeUSAGE_WINDOW);        
        TEST1(array->getNumNodes() == 1);
        TEST1(array->operator [](0)->getNodeNameStr() == "WINDOW");                                
    }
    
    { // test quit
        unique_ptr<Tree> tree = new Tree("test_tree",-1,"EDIT");
        tree->addNode("save_me_not","ANY");
        tree->quit();
        
        // we hould not be able to write tree as it should has been closed
        TEST_EXCEPTION(tree->write(), MdsException );
        
        // tests that the node has not been written
        tree = new Tree("test_tree",-1,"NORMAL");
        TEST_EXCEPTION(tree->getNode("save_me_not"), MdsException );
    }    
    
     // add device
    {
        unique_ptr<Tree> tree = new Tree("test_tree",-1,"EDIT");
        tree->addDevice("device","DIO2");
        tree->write();
    }
    {
        unique_ptr<Tree> tree = new Tree("test_tree",-1,"EDIT");
        unique_ptr<TreeNode> node = tree->getNode("device");
        std::cout << node->getUsage() << "\n";
        TEST1( std::string(node->getUsage()) == "DEVICE" );
        
        tree->remove("device");
        TEST_EXCEPTION( tree->getNode("device"), MdsException );
    }        
    
    
    
    
    
    
    END_TESTING;
}


