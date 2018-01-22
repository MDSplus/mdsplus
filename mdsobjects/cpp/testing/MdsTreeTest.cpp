/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdlib.h>
#include <mdsobjects.h>

#include "testing.h"
#include "testutils/unique_ptr.h"
#include "mdsplus/AutoPointer.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

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



int main(int argc UNUSED_ARGUMENT, char *argv[] UNUSED_ARGUMENT)
{
    BEGIN_TESTING(Tree);

#ifdef _WIN32
#define setenv(name,val,extra) _putenv_s(name,val)
#endif

    setenv("t_tree_path",".",1);
    setenv("t_tree2_path",".",1);

    ////////////////////////////////////////////////////////////////////////////////
    //  Constructors  //////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////


    { // CTR
        Tree *tree;
        tree = new Tree("t_tree",-1,"NEW");    delete tree;
        tree = new Tree("t_tree",-1,"NORMAL"); delete tree;

        tree = new Tree("t_tree",-1,"READONLY");
        TEST1( tree->isReadOnly() );
        delete tree;

        tree = new Tree("t_tree",-1,"EDIT");
        TEST1( tree->isOpenForEdit() );
        delete tree;

        tree = new Tree("t_tree",-1);          delete tree;

        TEST_MDS_EXCEPTION ( new Tree("t_tree",-1,"dumy error"),
                             "Invalid Open mode" );
    }

    ////////////////////////////////////////////////////////////////////////////////
    //  Node Usages  ///////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    { // write to parse file node usages //
        Tree *tree = new Tree("t_tree",-1,"NEW");

        // write //
        tree->write();
        delete tree;

        // edit  //
        tree = new Tree("t_tree",-1,"NORMAL");
        tree->edit();

        AutoData<TreeNode> node = tree->addNode("test_usage","STRUCTURE");
        tree->write();

        // fill all kinds of nodes into the test_usage structure //
        AutoData<TreeNode>(tree->addNode("\\t_tree::top.test_usage:ANY","ANY"));
        AutoData<TreeNode>(tree->addNode("\\t_tree::top.test_usage:STRUCTURE","STRUCTURE"));
        AutoData<TreeNode>(tree->addNode("\\t_tree::top.test_usage:AXIS","AXIS"));
        AutoData<TreeNode>(tree->addNode("\\t_tree::top.test_usage:COMPOUND","COMPOUND_DATA"));
        AutoData<TreeNode>(tree->addNode("\\t_tree::top.test_usage:DEVICE","DEVICE"));
        AutoData<TreeNode>(tree->addNode("\\t_tree::top.test_usage:DISPATCH","DISPATCH"));
        AutoData<TreeNode>(tree->addNode("\\t_tree::top.test_usage:NUMERIC","NUMERIC"));
        AutoData<TreeNode>(tree->addNode("\\t_tree::top.test_usage:SIGNAL","SIGNAL"));
        AutoData<TreeNode>(tree->addNode("\\t_tree::top.test_usage:SUBTREE","SUBTREE"));
        AutoData<TreeNode>(tree->addNode("\\t_tree::top.test_usage:TASK","TASK"));
        AutoData<TreeNode>(tree->addNode("\\t_tree::top.test_usage:TEXT","TEXT"));
        AutoData<TreeNode>(tree->addNode("\\t_tree::top.test_usage:WINDOW","WINDOW"));
        tree->write();

        delete tree;
    }

    { // get Node
        unique_ptr<Tree> tree = new Tree("t_tree",-1,"READONLY");
        TEST1( AutoData<TreeNode>(tree->getNode("\\t_tree::top.test_usage:ANY")).get() != NULL );
        TEST1( AutoData<TreeNode>(tree->getNode(AutoData<String>(new String("\\t_tree::top.test_usage:ANY")))).get() != NULL );
        TEST1( AutoData<TreeNode>(tree->getNode(AutoData<TreePath>(new TreePath(std::string("\\t_tree::top.test_usage:ANY"), tree)))).get() != NULL );

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
        TEST1(AutoData<TreeNode>(tree->getDefault())->getNodeNameStr() == "TEST_USAGE");

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
        unique_ptr<Tree> tree = new Tree("t_tree",-1,"NORMAL");
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


    ////////////////////////////////////////////////////////////////////////////////
    //  Edit open  /////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////


    { // test write
        unique_ptr<Tree> tree = new Tree("t_tree",-1,"READONLY");
        TEST_EXCEPTION( tree->edit(), MdsException );

        tree = new Tree("t_tree",-1,"NORMAL");

        // reopen in normal mode //
        tree->edit(false);
        TEST_EXCEPTION ( unique_ptr<TreeNode>( tree->addNode("save_me_not","ANY")), MdsException );

        // reopen in edit mode //
        tree->edit();
        TEST0( tree->isModified() );
        unique_ptr<TreeNode>( tree->addNode("save_me_not","ANY"));

        // isModified()
        TEST1( tree->isModified() );
        tree->write();
        TEST0( tree->isModified() );

        // remove() node
        tree->remove("save_me_not");
        TEST1( tree->isModified() );
        tree->write();
        TEST0( tree->isModified() );

        unique_ptr<TreeNode>(tree->addNode("save_me_not","ANY"));
        // it does not writes here //

        // tests that the node has not been written
        tree = new Tree("t_tree",-1,"NORMAL");
        TEST_EXCEPTION( unique_ptr<TreeNode>(tree->getNode("save_me_not")), MdsException );
    }

    ////////////////////////////////////////////////////////////////////////////////
    //  Devices  ///////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////


     // add device
    {
        unique_ptr<Tree> tree = new Tree("t_tree",-1,"EDIT");
        unique_ptr<TreeNode>(tree->addDevice("device","DIO2"));
        tree->write();

        unique_ptr<TreeNode> node = tree->getNode("device");
        TEST1( std::string(node->getUsage()) == "DEVICE" );

        tree->remove("device");
        tree->write(); // tree open in edit mode so must call write to avoid memory leak //
        TEST_EXCEPTION( tree->getNode("device"), MdsException );
    }


    ////////////////////////////////////////////////////////////////////////////////
    //  Pulses  ////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    {   // create and delete
        unique_ptr<Tree> tree = new Tree("t_tree",-1);
        tree->createPulse(1);
        tree = new Tree("t_tree",1);
        unique_ptr<TreeNode> node = tree->getNode("test_usage:ANY");

        // test that node has been filled in //
        TEST1( (bool)node );

        // test that the returned instance is the actual node requested //
        TEST1( node->getNodeNameStr() == "ANY" );

        tree->createPulse(2);
        tree = new Tree("t_tree",2);

        tree->deletePulse(2);

        // try to write data into removed pulse //
        // WARNING: this creates a conditional jump into the tree structure so this test has been removed //
        // TEST_EXCEPTION( unique_ptr<TreeNode>(tree->getNode("test_usage:NUMERIC"))->putData(new Int32(5552368)), MdsException );

        // create a pulse without copying from model structure //
        tree = new Tree("t_tree",2,"NEW");

        // test that the new pulse has not the model nodes //
        TEST_EXCEPTION( unique_ptr<TreeNode>(tree->getNode("test_usage:ANY")), MdsException );

        tree->deletePulse(2);
    }


    ////////////////////////////////////////////////////////////////////////////////
    //  Versioning  ////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////


    {   //  VERSIONS  IN PULSE //
        unique_ptr<Tree> tree = new Tree("t_tree",-1,"EDIT");
        tree->setVersionsInModel(false);
        tree->write(); // tree open in edit mode so must call write to avoid memory leak //

        tree = new Tree("t_tree",1,"EDIT");
        unique_ptr<TreeNode> node = tree->addNode("versioned", "NUMERIC");
        tree->setVersionsInPulse(true);
        tree->write();

        tree = new Tree("t_tree",1);

        // first version
        node = tree->getNode("versioned");
        node->putData(AutoPointer<Int32>(new Int32(5552368)));

        // after first put node does not contain any further version //
        TEST0( node->containsVersions() );

        // second version
        node = tree->getNode("versioned");
        node->putData(unique_ptr<Float64>(new Float64(555.2368)));


        // third version
        node = tree->getNode("versioned");
        node->putData(unique_ptr<Int16>(new Int16(555)));

        TEST1( node->containsVersions() );

        // TODO: version in model
    }


    ////////////////////////////////////////////////////////////////////////////////
    //  Tags  //////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    {   // TAGS //
        Tree * tree = new Tree("t_tree",1,"EDIT");
        unique_ptr<TreeNode> node = tree->getNode("test_usage:TEXT");
        node->addTag("test_tag");

        node = tree->getNode("\\test_tag");
        node = tree->getNode("\\t_tree::test_tag");
        tree->removeTag("test_tag");

        TEST_EXCEPTION( unique_ptr<TreeNode>(tree->getNode("\\test_tag")), MdsException );
        TEST_EXCEPTION( unique_ptr<TreeNode>(tree->getNode("\\t_tree::test_tag")), MdsException );

        tree->write(); // tree open in edit mode so must call write to avoid memory leak //
        delete tree;
    }

    END_TESTING;
}


