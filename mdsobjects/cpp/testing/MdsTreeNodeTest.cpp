#include <stdlib.h>
#include <mdsobjects.h>

#include <treeshr.h>


#include "testing.h"
#include "testutils/unique_ptr.h"
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
}

int main(int argc, char *argv[])
{
    BEGIN_TESTING(TreeNode);
    
    setenv("test_tree_path",".",1);
    setenv("test_tree2_path",".",1);
    unique_ptr<Tree> tree = new Tree("test_tree",-1,"NEW");
    unique_ptr<Tree> tree2 = new Tree("test_tree2",-1,"NEW");
    
    unique_ptr<TreeNode> node2 = tree2->addNode("test_node","ANY");    
    unique_ptr<TreeNode> subtree = tree->addNode("tree2","SUBTREE");
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
        TEST1( ss.str() == "\\TEST_TREE::TOP:TEST_NODE");
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
        TEST1( AutoString(node->getPath()).string ==  "\\TEST_TREE::TOP:TEST_NODE" );
        TEST1( AutoString(node_child->getPath()).string ==  "\\TEST_TREE::TOP:TEST_NODE:NODE_CHILD" );
        
        // getPathStr()
        TEST1( node->getPathStr() ==  "\\TEST_TREE::TOP:TEST_NODE" );
        TEST1( node_child->getPathStr() ==  "\\TEST_TREE::TOP:TEST_NODE:NODE_CHILD" );
        
        // getMinPath()
        TEST1( AutoString(node->getMinPath()).string == "TEST_NODE" );
        TEST1( AutoString(node_child->getMinPath()).string == "TEST_NODE:NODE_CHILD" );
        
        // getMinPathStr()
        TEST1( node->getMinPathStr() == "TEST_NODE" );
        TEST1( node_child->getMinPathStr() == "TEST_NODE:NODE_CHILD" );
                               
        // getFullPath()
        TEST1( AutoString(node->getFullPath()).string ==  "\\TEST_TREE::TOP:TEST_NODE" );
        TEST1( AutoString(node_child->getFullPath()).string ==  "\\TEST_TREE::TOP:TEST_NODE:NODE_CHILD" );
        
        // getFullPathStr()
        TEST1( node->getFullPathStr() ==  "\\TEST_TREE::TOP:TEST_NODE" );
        TEST1( node_child->getFullPathStr() ==  "\\TEST_TREE::TOP:TEST_NODE:NODE_CHILD" );
        
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
                    "Curabitur nunc dui, efficitur at elit quis. ");
        node->putData( data );        
        //        std::cout << node->getLength() << "\n";
        //        std::cout << node->getCompressedLength() << "\n";
        TEST1( node->getLength() == 729 );
        TEST1( node->getCompressedLength() == 721 );
                
        // test if data retrieved after compress on put is the same of original data //
        node->setCompressOnPut(true);
        node->putData( data );        
        TEST1( AutoString(unique_ptr<Data>(node->getData())->getString()).string == 
               AutoString(data->getString()).string );                
        
        
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
        unique_ptr<TreeNodeArray> children = ( new TreeNodeArray(parent->getChildren(&num_children), num_children) );
        TEST1( num_children == 3 );
        TEST1( children->getNumNodes() == 3 );
                        
        // nodes are alphabetically ordered //
        TEST1( children->operator [](0)->getNodeNameStr() == "BROTHER" );
        TEST1( children->operator [](1)->getNodeNameStr() == "BROTHER2" );
        TEST1( children->operator [](2)->getNodeNameStr() == "CHILD" );
        
        int num_members;
        // FIX: delete array
        unique_ptr<TreeNodeArray> members = ( new TreeNodeArray(parent->getMembers(&num_members), num_members) );
        TEST1( num_members = 2 );
        TEST1( members->getNumNodes() == 2 );        
                        
        // nodes are alphabetically ordered //
        TEST1( members->operator [](0)->getNodeNameStr() == "MEM1" );
        TEST1( members->operator [](1)->getNodeNameStr() == "MEM2" );
        
        int num_desc;
        // FIX: delete array
        unique_ptr<TreeNodeArray> desc = ( new TreeNodeArray( parent->getDescendants(&num_desc), num_desc) );
        TEST1( num_desc == 5 );
        TEST1( desc->getNumNodes() == 5 );
                
        parmem1->putData( unique_ptr<Data>(new Int32(5552368)) );
        parmem2->putData( unique_ptr<Data>(new String("lorem ipsum")) );
        
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
        
        // comression is enabled by default //
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

        unique_ptr<Tree> shot = new Tree("test_tree",1);
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
        
        //        shot = new Tree("test_tree",2);
        
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
        
        int array[] = {1,1,2,4,8,12,20,32,52,84};
        
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
        
        
        tree->write();
        
        
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    //  Edit Nodes  ////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    
    {
        
    }
    
    
    
    END_TESTING;            
}










































