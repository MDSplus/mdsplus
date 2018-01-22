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

#include <treeshr.h>


#include "testing.h"
#include "testutils/unique_ptr.h"
#include "testutils/String.h"
#include "mdsplus/AutoPointer.hpp"



using namespace MDSplus;
using namespace testing;

namespace testing {
class TestTreeNodePotected : public MDSplus::TreeNode {
public:
    TestTreeNodePotected(TreeNode *node)  :
        TreeNode(node->getNid(),node->getTree()) {}

    using TreeNode::isImmutable;
    using TreeNode::getFlag;
    using TreeNode::setFlag;
};
} // testing




namespace testing {
class TestNode {
public:
    TreeNode *node;

    TestNode(TreeNode *node) :
        node(node)
    {}

    friend std::ostream &
    operator << (std::ostream &o, const TestNode &n) {
        o << " -- test node -- \n"
          << "name:   " << n.node->getNodeNameStr() << "\n"
          << "usage:  " << n.node->getUsage() << "\n"
          << "parent: " << unique_ptr<TreeNode>(n.node->getParent())->getNodeNameStr() << "\n"
          << "tree:   " << n.node->getTree()->getName() << "\n";
        return o;
    }

    bool operator == (const TestNode &other) {
        return this->node->getNid() == other.node->getNid();
    }

    void operator()(std::string name, std::string usage, std::string parent, std::string tree) {
        TEST1( node->getNodeNameStr() == toupper(name) );
        TEST1( std::string(node->getUsage()) == toupper(usage) );
        TEST1( unique_ptr<TreeNode>(node->getParent())->getNodeNameStr() == toupper(parent) );
        TEST1( node->getTree()->getName() == tree );
    }
};




void print_segment_info(TreeNode *node, int segment = -1)
{
    char dtype,dimct;
    int dims[8], next;
    std::cout << "info> " << node->getPathStr() << "  ";
    node->getSegmentInfo(segment,&dtype, &dimct, dims, &next);
    std::cout << "dtype: " << (int)dtype << " ";
    std::cout << "dims:" << AutoString(unique_ptr<Array>(new Int32Array(dims,dimct))->decompile()).string;
    if(next == dims[dimct-1]) std::cout << " fullfilled\n";
    else std::cout << " next empty element: " << next << "\n";
}
} // testing


#ifdef _WIN32
#include <windows.h>
#endif


int main(int argc UNUSED_ARGUMENT, char *argv[] UNUSED_ARGUMENT)
{
    BEGIN_TESTING(TreeNode);


#ifdef _WIN32
#define setenv(name,val,extra) _putenv_s(name,val)
#endif

    setenv("t_treenode_path",".",1);
    setenv("t_treenode2_path",".",1);


    unique_ptr<Tree> tree = new Tree("T_TREENODE",-1,"NEW");
    unique_ptr<Tree> tree2 = new Tree("T_TREENODE2",-1,"NEW");

    unique_ptr<TreeNode> node2 = tree2->addNode("test_node","ANY");
    unique_ptr<TreeNode> subtree = tree->addNode("T_TREENODE2","SUBTREE");
    unique_ptr<TreeNode> node = tree->addNode("test_node","ANY");
    unique_ptr<TreeNode> node_child = tree->addNode("test_node:node_child","ANY");

    tree2->write();
    tree->write();

    ////////////////////////////////////////////////////////////////////////////////
    //  Constructors  //////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////


    { // CTR
        TreeNode * new_node = new TreeNode(node->getNid(), tree);
        TEST1( new_node->getNodeNameStr() == "TEST_NODE" );
        TEST1( std::string(new_node->getUsage()) == "ANY" );
        TEST_MDS_EXCEPTION( new TreeNode(123,NULL), "%TREE-W-NOT_OPEN, Tree not currently open" );
        deleteData(new_node);
    }

    { // PROTECTED METHODS -  should be checked ?? //
        TEST0( TestTreeNodePotected(node).isImmutable() );

    }


    ////////////////////////////////////////////////////////////////////////////////
    //  Operators  /////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////


    { // OSTREAM OPERATOR //
        unique_ptr<TreeNode> node = tree->getNode("test_node");
        std::stringstream ss;
        ss << (TreeNode*)node;
        TEST1( ss.str() == "\\T_TREENODE::TOP:TEST_NODE");
    }


    ////////////////////////////////////////////////////////////////////////////////
    //  Path  //////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////


    {
        unique_ptr<TreeNode> node = tree->getNode("test_node");

        // getTree()
        TEST1( node->getTree() == tree );

        // setTree()
        node->setTree(tree2);
        TEST1( node->getTree() == tree2 );
        node->setTree(tree);

        // getPath()
        TEST1( AutoString(node->getPath()).string ==  "\\T_TREENODE::TOP:TEST_NODE" );
        TEST1( AutoString(node_child->getPath()).string ==  "\\T_TREENODE::TOP:TEST_NODE:NODE_CHILD" );

        // getPathStr()
        TEST1( node->getPathStr() ==  "\\T_TREENODE::TOP:TEST_NODE" );
        TEST1( node_child->getPathStr() ==  "\\T_TREENODE::TOP:TEST_NODE:NODE_CHILD" );

        // getMinPath()
        TEST1( AutoString(node->getMinPath()).string == "TEST_NODE" );
        TEST1( AutoString(node_child->getMinPath()).string == "TEST_NODE:NODE_CHILD" );

        // getMinPathStr()
        TEST1( node->getMinPathStr() == "TEST_NODE" );
        TEST1( node_child->getMinPathStr() == "TEST_NODE:NODE_CHILD" );

        // getFullPath()
        TEST1( AutoString(node->getFullPath()).string ==  "\\T_TREENODE::TOP:TEST_NODE" );
        TEST1( AutoString(node_child->getFullPath()).string ==  "\\T_TREENODE::TOP:TEST_NODE:NODE_CHILD" );

        // getFullPathStr()
        TEST1( node->getFullPathStr() ==  "\\T_TREENODE::TOP:TEST_NODE" );
        TEST1( node_child->getFullPathStr() ==  "\\T_TREENODE::TOP:TEST_NODE:NODE_CHILD" );

        // getNodeName()
        TEST1( AutoString(node->getNodeName()).string == "TEST_NODE" );
        TEST1( AutoString(node_child->getNodeName()).string == "NODE_CHILD" );

        // getNodeNameStr()
        TEST1( node->getNodeNameStr() == "TEST_NODE" );
        TEST1( node_child->getNodeNameStr() == "NODE_CHILD" );

        //        std::cout << node_child->getOriginalPartName() << "\n";
        //        std::cout << node_child->getOriginalPartNameStr() << "\n";

        unique_ptr<TreeNode> n;

        // getNode()
        n = tree->getNode("test_node:node_child");
        TEST1( node_child->getNid() == n->getNid() );

        // getNode( String )
        n = tree->getNode( unique_ptr<String>(new String("test_node:node_child")) );
        TEST1( node_child->getNid() == n->getNid() );

        // isOn()
        TEST1( node->isOn() );

        // setOn()
        node->setOn(false);
        TEST0( node->isOn() );
        node->setOn(true);
        TEST1( node->isOn() );

    }




    ////////////////////////////////////////////////////////////////////////////////
    //  Data In/Out  ///////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    {

        // putData
        unique_ptr<Data> data = new Int32(5552368);
        node->putData( data );

        // getData
        data = node->getData();
        TEST1( data->getInt() == 5552368 );

        int len UNUSED_ARGUMENT = node->getLength();

        // getLength()  Nci length of Int32 is 12
        TEST1( node->getLength() == 12 );


        data = new String(
                    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras quis dolor non mauris imperdiet dapibus. "
                    "Donec et lorem blandit, scelerisque turpis quis, volutpat sapien. Nam felis ex, commodo vitae turpis sed,"
                    " sodales commodo elit. Vivamus eu vehicula diam. Vivamus vitae vulputate purus. Etiam id pretium urna. "
                    "Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. "
                    "Nulla purus eros, iaculis non sapien ac, tempus luctus nisi. Vestibulum pulvinar lobortis elementum. "
                    "Quisque ultricies sagittis nulla eu aliquet. Curabitur eleifend sollicitudin est. "
                    "Vestibulum fringilla laoreet velit quis hendrerit. Aenean sodales suscipit mattis. "
                    "Curabitur nunc dui, efficitur at elit quis. "
        );

        node->setCompressOnPut(true);
        node->putData( data );

        len = node->getLength();
        len = node->getCompressedLength();

        // SOME BUGS HERE !!!!

        //        std::cout << node->getLength() << "\n";
        //        std::cout << node->getCompressedLength() << "\n";

//        TEST1( node->getLength() == 729 );
//        TEST1( node->getCompressedLength() == 721 );

        // test if data retrieved after compress on put is the same of original data //
        node->setCompressOnPut(true);
        node->putData( data );
        TEST1( AutoString(unique_ptr<Data>(node->getData())->getString()).string ==
               AutoString(data->getString()).string );

        len = node->getLength();
        len = node->getCompressedLength();

        // deleteData()
        node->deleteData();
        TEST_MDS_EXCEPTION( node->getData(),
                            "%TREE-E-NODATA, No data available for this node" );

        //        data = new String("time");
        //        std::cout << node->getTimeInserted() << "\n";
        //        std::cout << get << "\n";

    }

    ////////////////////////////////////////////////////////////////////////////////
    //  Special Nodes  /////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    {
        // TODO:  test
        // - getStatus()
        // - doMethod()
        // - isSetup()
        // - getConglomerateElt()
        // - getNumElts()
        // - getConglomerateNodes()
    }

    ////////////////////////////////////////////////////////////////////////////////
    //  Structure  /////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    {
        tree->edit(true);
        unique_ptr<TreeNode> test_structure   ( tree->addNode("test_struct","STRUCUTURE") );
        unique_ptr<TreeNode> parent     ( tree->addNode("test_struct.parent","STRUCTURE") );
        unique_ptr<TreeNode> child   ( tree->addNode("test_struct.parent:child","STRUCTURE") );
        unique_ptr<TreeNode> brother  ( tree->addNode("test_struct.parent:brother","STRUCTURE") );
        unique_ptr<TreeNode> brother2  ( tree->addNode("test_struct.parent:brother2","STRUCTURE") );
        unique_ptr<TreeNode> parmem1  ( tree->addNode("test_struct.parent:mem1","NUMERIC") );
        unique_ptr<TreeNode> parmem2  ( tree->addNode("test_struct.parent:mem2","TEXT") );


        TEST1( unique_ptr<TreeNode>(parent->getParent())->getNid() == test_structure->getNid() );
        TEST1( unique_ptr<TreeNode>(child->getParent())->getNid() == parent->getNid() );
        TEST1( unique_ptr<TreeNode>(test_structure->getChild())->getNid() == parent->getNid() );

        // nodes are alphabetically ordered //
        TEST1( unique_ptr<TreeNode>(brother->getBrother())->getNid() == brother2->getNid() );
        TEST1( unique_ptr<TreeNode>(brother2->getBrother())->getNid() == child->getNid() );

        // isChild()
        TEST1( child->isChild() );

        // isMember()
        TEST1( parmem1->isMember() );

        // children are only structire elements //
        TEST1( parent->getNumChildren() == 3 );

        // parent has two members
        TEST1( parent->getNumMembers() == 2 );

        // chidren + members
        TEST1( parent->getNumDescendants() == 5 );

        int num_children;
        // FIX: delete array
        TreeNode **children = parent->getChildren(&num_children);
        TEST1( num_children == 3 );

        // nodes are alphabetically ordered //
        TEST1( children[0]->getNodeNameStr() == "BROTHER" );
        TEST1( children[1]->getNodeNameStr() == "BROTHER2" );
        TEST1( children[2]->getNodeNameStr() == "CHILD" );
        for(int i=0; i< num_children; ++i) {
            deleteData(children[i]);
        }
        delete [] children;

        int num_members;
        // FIX: delete array
        TreeNode **members = parent->getMembers(&num_members);
        TEST1( num_members = 2 );

        // nodes are alphabetically ordered //
        TEST1( members[0]->getNodeNameStr() == "MEM1" );
        TEST1( members[1]->getNodeNameStr() == "MEM2" );
        for(int i=0; i< num_members; ++i) {
            deleteData(members[i]);
        }
        delete [] members;

        int num_desc;
        // FIX: delete array
        TreeNode **desc = parent->getDescendants(&num_desc);
        TEST1( num_desc == 5 );
        parmem1->putData( unique_ptr<Data>(new Int32(5552368)) );
        parmem2->putData( unique_ptr<Data>(new String("lorem ipsum")) );
        for(int i=0; i< num_desc; ++i) {
            deleteData(desc[i]);
        }
        delete [] desc;

        TEST_EXCEPTION( child->putData( unique_ptr<Data>(new Int32(5552368)) ), MdsException );

        TEST1( std::string(parmem1->getClass()) == "CLASS_S" );
        TEST1( std::string(parmem2->getClass()) == "CLASS_S" );

        TEST1( std::string(parmem1->getDType()) == "DTYPE_L" );
        TEST1( std::string(parmem2->getDType()) == "DTYPE_T" );

        TEST1( std::string(parmem1->getUsage()) == "NUMERIC" );
        TEST1( std::string(parmem2->getUsage()) == "TEXT" );

        // DEPTHS .. (remember: \TOP has depth 1 ) //
        TEST1( unique_ptr<TreeNode>(tree->getNode("\\TOP"))->getDepth() == 1 );
        TEST1( test_structure->getDepth() == 2 );
        TEST1( parent->getDepth() == 3 );
    }


    ////////////////////////////////////////////////////////////////////////////////
    //  Flags  /////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    {
        tree->edit(true);
        unique_ptr<TreeNode> node = tree->addNode("test_flags","ANY");

        // WRITE ONCE //////////////////////////////////////////////////////////

        TEST0 ( node->isWriteOnce() );
        node->setWriteOnce(true);
        TEST1 ( node->isWriteOnce() );

        node->putData( unique_ptr<Data>( new Int32(5552368)) );
        TEST_EXCEPTION( node->putData( unique_ptr<Data>( new Int32(5552369)) ),
                        MdsException );

        node->setWriteOnce(false);
        TEST0 ( node->isWriteOnce() );
        node->putData( unique_ptr<Data>( new Int32(5552369)) );


        // COMPRESS ON PUT /////////////////////////////////////////////////////

        // compression is enabled by default //
        TEST1 ( node->isCompressOnPut() );

        node->setCompressOnPut(false);
        TEST0 ( node->isCompressOnPut() );

        node->setCompressOnPut(true);
        TEST1 ( node->isCompressOnPut() );



        // NO WRITE ////////////////////////////////////////////////////////////

        tree->write();
        tree->edit(false);
        tree->createPulse(1);
        tree->edit(true);

        unique_ptr<Tree> shot = new Tree("T_TREENODE",1);
        unique_ptr<TreeNode> shot_node = shot->getNode("test_flags");


        // NO WRITE MODEL //

        TEST0 ( node->isNoWriteModel() );
        node->putData( unique_ptr<Data>( new Int32(5552369)) );

        node->setNoWriteModel(true);
        TEST1 ( node->isNoWriteModel() );
        TEST_EXCEPTION( node->putData( unique_ptr<Data>( new Int32(5552369)) ),
                        MdsException );

        // but we can always write it in shot //
        shot_node->putData( unique_ptr<Data>( new Int32(5552369)) );

        node->setNoWriteModel(false);
        TEST0 ( node->isNoWriteModel() );
        node->putData( unique_ptr<Data>( new Int32(5552369)) );


        // NO WRITE SHOT //

        TEST0 ( shot_node->isNoWriteShot() );
        shot_node->putData( unique_ptr<Data>( new Int32(5552369)) );

        shot_node->setNoWriteShot(true);
        TEST1 ( shot_node->isNoWriteShot() );
        TEST_EXCEPTION( shot_node->putData( unique_ptr<Data>( new Int32(5552369)) ),
                        MdsException );

        // but we can always write it in model //
        node->putData( unique_ptr<Data>( new Int32(5552369)) );

        shot_node->setNoWriteShot(false);
        TEST0 ( shot_node->isNoWriteShot() );
        shot_node->putData( unique_ptr<Data>( new Int32(5552369)) );


        // IS ESSENTIAL //
        // TODO: test Essential feature in Action //

        TEST0( node->isEssential() );

        node->setEssential(true);
        TEST1( node->isEssential() );

        node->setEssential(false);
        TEST0( node->isEssential() );



        // IS INCLUDED IN PULSE //

        // TODO: capire ...
        TEST0( node->isIncludedInPulse() );
        shot->edit();
        node = shot->addNode("onlypulse","ANY");

        //        tree->write();
        //        tree->edit(false);
        //        tree->createPulse(2);
        //        tree->edit(true);
        //        shot = new Tree("T_TREENODE",2);
        //        TEST1( node->isIncludedInPulse() );
        //        TEST_EXCEPTION( unique_ptr<Data>(unique_ptr<TreeNode>(shot->getNode("no_in_pulse"))->getData()), MdsException );

        node->setIncludedInPulse(true);
        TEST1( node->isIncludedInPulse() );

    }


    ////////////////////////////////////////////////////////////////////////////////
    //  Versioning  ////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    {
        tree->edit(true);
        tree->setVersionsInModel(true);


        unique_ptr<TreeNode> node = tree->addNode("test_version","ANY");
        TEST0( node->containsVersions() );

        node->putData( unique_ptr<Data>(new Int32(5552368)) );
        TEST0( node->containsVersions() );

        node->putData( unique_ptr<Data>(new String(" yes .. ")) );
        TEST1( node->containsVersions() );

        node->putData( unique_ptr<Data>(new String("we are back!")) );
        TEST1( node->containsVersions() );

    }

    ////////////////////////////////////////////////////////////////////////////////
    //  Segments  //////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    {
        tree->edit(true);
        unique_ptr<TreeNode>(tree->addNode("test_seg","STRUCTURE"));
        unique_ptr<TreeNode> node = tree->addNode("test_seg:any","ANY");

        int array[] = {1,1,2,3,5,8,13,21,34,55};

        unique_ptr<Int32Array> array_data = new Int32Array(array,10);
        unique_ptr<Range> array_time = new Range(new Int32(0), new Int32(9), new Int32(1));
        node->beginSegment( unique_ptr<Data>(array_data->getElementAt(0)), unique_ptr<Data>(array_data->getElementAt(9)), array_time, array_data );

        array[0] = 1;
        array[1] = 1;

        array_data = new Int32Array(array,2);

        // fill all segment of ones //
        node->putSegment(array_data,0);
        node->putSegment(array_data,2);
        node->putSegment(array_data,4);
        node->putSegment(array_data,6);
        node->putSegment(array_data,8);

        { // test ones //
            unique_ptr<Data> data = node->getData();
            int *a, len;
            a = data->getIntArray(&len);
            for(int i=0; i<len; ++i) TEST1( a[i] == 1 );
            delete[] a;
        }

        // creates a overlapping segment //
        array_data = new Int32Array(array,10);
        node->makeSegment( unique_ptr<Data>(array_data->getElementAt(0)), unique_ptr<Data>(array_data->getElementAt(9)), array_time, array_data );


        TEST1 ( node->getNumSegments() == 2 );

        // updateSegment .. moves last segment forward of 10s
        node->updateSegment(unique_ptr<Data>(new Int32(10)),
                            unique_ptr<Data>(new Int32(19)),
                            unique_ptr<Range>(new Range(new Int32(10), new Int32(19), new Int32(1))) );


        // updateSegment .. moves first segment backward of 10s
        node->updateSegment(0,
                            unique_ptr<Data>(new Int32(-19)),
                            unique_ptr<Data>(new Int32(0)),
                            unique_ptr<Range>(new Range(new Int32(-19), new Int32(0), new Int32(1))) );

        { // getSegmentLimits
            Data *start = NULL; // new Float32(0.);
            Data *end = NULL; // new Float32(0.);
            node->getSegmentLimits(0, &start, &end);
            TEST1( start->getInt() == -19 );
            TEST1( end->getInt() == 0 );
            deleteData(start);
            deleteData(end);

            node->getSegmentLimits(1, &start, &end);
            TEST1( start->getInt() == 10 );
            TEST1( end->getInt() == 19 );
            deleteData(start);
            deleteData(end);
        }

        { // getSegment
            unique_ptr<Array> data = node->getSegment(1);
            int num_elements;
            AutoArray<int> elements(data->getIntArray(&num_elements));
            TEST1( num_elements == 10 );
            for(int i=0; i<10; ++i)
                TEST1( elements[i] == array[i] );
        }

        { // getSegmentdimension
            unique_ptr<Data> dim = node->getSegmentDim(0);
            TEST1( std::string( AutoString(dim->decompile()) ) == "-19 : 0 : 1" );
            dim = node->getSegmentDim(1);
            TEST1( std::string( AutoString(dim->decompile()) ) == "10 : 19 : 1" );
        }

        { // getSegmentAndDimension
            unique_ptr<Array> segment;
            unique_ptr<Data> dimension;
            node->getSegmentAndDimension(0,segment.base(),dimension.base());
        }


        { // timestamped
            unique_ptr<TreeNode> ts_node = tree->addNode("test_seg:ts","SIGNAL");

            array_data = new Int32Array(array,10);
            ts_node->beginTimestampedSegment( array_data );
            TEST1( ts_node->getNumSegments() == 1 );
            int64_t times[10];
            for(int i=0; i<10; ++i) {
                times[i] = (int64_t)array[i];
            }

            // putTimestampedSegment of size 1
            ts_node->putTimestampedSegment( unique_ptr<Array>(new Int32Array(array,1)), times);

            // putTimestampedSegment of size 2
            ts_node->putTimestampedSegment( unique_ptr<Array>(new Int32Array(array,2)), times);

            // putrow puts single element into segment
            ts_node->putRow( unique_ptr<Array>(new Int32Array(array,1)), times );

            // putrow puts two elements into segment
            ts_node->putRow( unique_ptr<Array>(new Int32Array(array,2)), times );

            // Now putRow does not throw exception !
            //            TEST_EXCEPTION(
            //                        ts_node->putRow(unique_ptr<Array>(new Int32Array(array,2)), times),
            //                        MdsException );

            // makeTimestampedSegment
            ts_node->makeTimestampedSegment( array_data, times );
            TEST1( ts_node->getNumSegments() == 2 );
            TEST1( AutoString(unique_ptr<Data>(ts_node->getSegmentDim(1))->decompile()).string
                   == "[1Q,1Q,2Q,3Q,5Q,8Q,13Q,21Q,34Q,55Q]" );

        }

        {   // IMAGES in segments //
            unique_ptr<TreeNode> n2 = tree->addNode("test_seg:image","SIGNAL");

            unsigned char array[] = {
                0,0,0,0,9,9,0,0,0,
                0,0,0,9,9,9,0,0,0,
                0,0,0,9,9,9,0,0,0,
                1,1,1,1,9,9,1,1,1,
                1,1,1,1,9,9,1,1,1,
                0,0,9,9,9,9,9,0,0,
                0,0,9,9,9,9,9,0,0
                ,
                0,0,0,9,9,9,0,0,0,
                0,0,9,9,9,9,9,0,0,
                0,0,9,9,0,9,9,0,0,
                1,1,1,1,9,9,9,1,1,
                1,1,1,9,9,1,1,1,1,
                0,0,9,9,9,9,9,0,0,
                0,0,9,9,9,9,9,0,0
                ,
                0,0,9,9,9,9,9,0,0,
                0,0,9,9,9,9,9,0,0,
                0,0,0,0,9,9,0,0,0,
                1,1,1,9,9,9,1,1,1,
                1,1,1,1,1,9,9,1,1,
                0,0,9,9,9,9,9,0,0,
                0,0,9,9,9,9,0,0,0
                ,
                0,0,0,0,9,9,9,0,0,
                0,0,0,9,9,9,0,0,0,
                0,0,9,9,9,0,0,0,0,
                1,1,9,9,9,9,9,1,1,
                1,1,9,9,9,9,9,1,1,
                0,0,0,0,0,9,9,0,0,
                0,0,0,0,0,9,9,0,0
            };

            // we will cover 10 frames with the following time data //
            float time[] = { 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9 };

            // four frames in one segment
            int dims[] = {4,7,9};   // { FRAMES, HEIGHT, WIDTH } //

            n2->makeSegment(unique_ptr<Data>(new Float32(0)),
                            unique_ptr<Data>(new Float32(0.35)),
                            unique_ptr<Data>(new Float32Array(time,4)),
                            unique_ptr<Array>(new Uint8Array(array,3,dims)) );
            TEST1( n2->getNumSegments() == 1 );
            std::cout << AutoString(unique_ptr<Data>(n2->getSegmentDim(0))->decompile()).string << "\n";

            print_segment_info(n2);
            {
                char dtype,dimct;
                int next;
                int dims_test[8];
                n2->getSegmentInfo(-1,&dtype,&dimct,dims_test,&next);
                TEST1( dtype == 2 );
                TEST1( dimct == 3 );
                TEST1( next == 4 );
                TEST1( dims_test[0] == dims[2] );
                TEST1( dims_test[1] == dims[1] );
                TEST1( dims_test[2] == dims[0] );
            }

            // add three frames in one segment
            dims[0] = 3; // 3 FRAMES //
            n2->makeSegment(unique_ptr<Data>(new Float32(0.4)),
                            unique_ptr<Data>(new Float32(0.65)),
                            unique_ptr<Data>(new Float32Array(&time[4],3)),
                            unique_ptr<Array>(new Uint8Array(array,3,dims)) );
            TEST1( n2->getNumSegments() == 2 );
            std::cout << AutoString(unique_ptr<Data>(n2->getSegmentDim(1))->decompile()).string << "\n";
            print_segment_info(n2);



            // put remaining segments ... //
            n2->beginSegment(unique_ptr<Data>(new Float32(0.7)),
                             unique_ptr<Data>(new Float32(0.9)),
                             unique_ptr<Data>(new Float32Array(&time[7],3)),
                             unique_ptr<Array>(new Uint8Array(array,3,dims)) );
            TEST1( n2->getNumSegments() == 3 );
            std::cout << AutoString(unique_ptr<Data>(n2->getSegmentDim(2))->decompile()).string << "\n";

            dims[0] = 1; // 1 FRAME //
            n2->putSegment( unique_ptr<Array>(new Uint8Array(array,3,dims)),0 );
            print_segment_info(n2);
            {
                char dtype,dimct;
                int next;
                int dims_test[8];
                n2->getSegmentInfo(-1,&dtype,&dimct,dims_test,&next);
                TEST1( dtype == 2 );
                TEST1( dimct == 3 );
                TEST1( next == 1 );
                TEST1( dims_test[0] == dims[2] );
                TEST1( dims_test[1] == dims[1] );
                TEST1( dims_test[2] == 3 );
            }

            dims[0] = 2; // 2 FRAMES //
            n2->putSegment( unique_ptr<Array>(new Uint8Array(array,3,dims)),1 );
            print_segment_info(n2);
            TEST1( n2->getNumSegments() == 3 );

            { // TEST ALL DATA WRITTEN IN SIGNAL //
                std::vector<unsigned char> test_array = unique_ptr<Data>(n2->getData())->getByteUnsignedArray();
                // 1 2 3 4 //
                int shift = 0;
                int len = 4 * 7 * 9;
                for(int i=0; i<len; ++i)
                    TEST1( array[i] == test_array[i+shift] );
                // 1 2 3 //
                shift += len;
                len = 3 * 7 * 9;
                for(int i=0; i<len; ++i)
                    TEST1( array[i] == test_array[i+shift] );
                // 1 //
                shift += len;
                len = 1 * 7 * 9;
                for(int i=0; i<len; ++i)
                    TEST1( array[i] == test_array[i+shift] );
                // 1 2 //
                shift += len;
                len = 2 * 7 * 9;
                for(int i=0; i<len; ++i)
                    TEST1( array[i] == test_array[i+shift] );
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    //  Edit Nodes  ////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    {

        unique_ptr<TreeNode> root = (tree->addNode("test_edit","STRUCTURE"));
        unique_ptr<TreeNode> n1 = tree->addNode("test_edit:n1","ANY");

        // addNode with relative path //
        unique_ptr<TreeNode> n2 = n1->addNode("n2","ANY");
        TestNode(n2.base())("n2", "ANY", "n1", "T_TREENODE");


        // addNode with absolute path //
        unique_ptr<TreeNode> n3 = n1->addNode("\\top.test_edit:n3","ANY");
        TestNode(n3.base())("n3", "ANY", "test_edit", "T_TREENODE");

        // remove relative path //
        n1->remove("n2");

        // cannot remove a node that reside in the parent node //
        TEST_EXCEPTION( n1->remove("n3") , MdsException );

        // remove switching to parent node //
        unique_ptr<TreeNode>(n1->getParent())->remove("n3");

        n1->rename("\\top:test_rename");
        TEST1(unique_ptr<TreeNode>(n1->getParent())->getNodeNameStr() == "TOP");

        n1->rename("\\top.test_edit:parent");
        TestNode(n1.base())( "parent", "ANY", "test_edit", "T_TREENODE");

        n2 = n1->addNode("subnode","ANY");
        n3 = n2->addNode("child","ANY");

        n3->move(n1);
        TEST1(unique_ptr<TreeNode>(n3->getParent())->getNodeNameStr() == "PARENT");

        n3->move(root,"new_parent");
        TEST1(unique_ptr<TreeNode>(n1->getParent())->getNodeNameStr() == "TEST_EDIT");

        n2->addTag("n2");

        n3->addTag("n3");
        n3 = tree->getNode("\\n3");

        n3->removeTag("n3");
        TEST_EXCEPTION( unique_ptr<TreeNode>(tree->getNode("\\n3")), MdsException );


        delete n1->addDevice("device","DIO2");

    }

    END_TESTING;

    BEGIN_TESTING(TreeNode-Tree reference);

#   ifdef _WIN32
    _putenv_s("test_node_path",".");
#   else
    setenv("test_node_path",".",1);
#   endif

    Tree *tree = new Tree("test_node", -1, "NEW");
    TreeNode *n = tree->addNode(":DATA", "NUMERIC");
    delete n;
    tree->write();
    delete tree;
    tree = new Tree("test_node", -1);
    tree->createPulse(1);
    tree->createPulse(2);
    delete tree;
    tree = new Tree("test_node", 1);
    Tree *tree1 = new Tree("test_node", 2);
    n = tree->getNode(":DATA");
    Data *d = new Int32(1);
    Data *d1 = new Int32(2);
    n->putData(d);
    TreeNode *n1 = tree1->getNode(":DATA");
    n1->putData(d1);
    deleteData(d);
    deleteData(d1);
    d = n->getData();
    d1 = n1->getData();
    TEST1(d->getInt() == 1);
    TEST1(d1->getInt() == 2);
    deleteData(d);
    deleteData(d1);
    delete n;
    delete n1;
    delete tree;
    delete tree1;
    END_TESTING;

}
