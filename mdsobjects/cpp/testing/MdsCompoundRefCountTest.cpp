
#include <mdsobjects.h>
#include <testing.h>

#include <testutils/unique_ptr.h>


using namespace MDSplus;
using namespace testing;


void test4(){
    
  
    
    Tree *myTree = new Tree("my_tree",-1,"NEW");    
    TreeNode *node1 = myTree->getNode("NODE1");
    TreeNode *node2 = myTree->getNode("NODE2");
    
    std::cout <<std::endl;
    std::cout <<">> TreeNode *node1 = myTree->getNode(""NODE1"");" <<std::endl;
    std::cout <<">> TreeNode *node2 = myTree->getNode(""NODE2"");" <<std::endl<<std::endl;
    std::cout << " Node1: refcount=" << node1->refCount << std::endl;
    std::cout << " Node2: refcount=" << node2->refCount << std::endl<<std::endl;
    
    Data *args[2];
    args[0] = new Int32(2);
    
    std::cout <<">> Data *args[2]; " << std::endl;
    std::cout <<">> args[0] = new Int32(2); "<< std::endl<< std::endl;
    std::cout << " args[0]: refcount=" << args[0]->refCount << std::endl<< std::endl;
    
    args[1] = node1;
    node1->incRefCount();
    
    std::cout <<">> args[1] = node1;" << std::endl;
    std::cout <<">> node1->incRefCount();" << std::endl<< std::endl;
    std::cout << " args[1]: refcount=" << args[1]->refCount << std::endl<< std::endl;
    
    Function *expr = new Function(38, 2, args);
    node2->putData(expr);
    
    std::cout <<">> Function *expr = new Function(38, 2, args);" << std::endl;
    std::cout <<">> node2->putData(expr); " << std::endl<< std::endl;
    std::cout << " args[0]: refcount=" << args[0]->refCount << std::endl;
    std::cout << " args[1]: refcount=" << args[1]->refCount << std::endl<< std::endl;
    
    deleteData(expr);
    std::cout <<">> deleteData(expr);"  << std::endl << std::endl;
    std::cout << " args[0]: no check on refcount (it has been destroyed)" << std::endl;
    std::cout << " args[1]: refcount=" << args[1]->refCount << std::endl << std::endl;
    
    deleteData(node1);
    deleteData(node2);
    delete myTree;
}

void test5() {

#ifdef _WIN32    
    _putenv_s("my_tree_path",".");
#else
    setenv("my_tree_path",".",1);
#endif  
    
    BEGIN_TESTING( test 5 );
    unique_ptr<Tree> myTree = new Tree("my_tree", -1, "NEW");    
    unique_ptr<TreeNode> node1 = myTree->addNode("NODE1","NUMERIC");    
    unique_ptr<TreeNode> node2 = myTree->addNode("NODE2","NUMERIC");
    myTree->write();
    
    myTree = new Tree("my_tree", -1);
    //setActiveTree(myTree);
    
    node1 = myTree->getNode("NODE1");
    node1->putData(new Int32(5552368));
    
    node2 = myTree->getNode("NODE2");        
    unique_ptr<Data> expr = compile("2+NODE1",myTree);
    node2->putData(expr);
    
    TEST1( node2->getData()->getInt() == 5552370 );    
    END_TESTING;
}


void test6() {
#ifdef _WIN32    
    _putenv_s("my_tree_path",".");
#else
    setenv("my_tree_path",".",1);
#endif  
    
    BEGIN_TESTING( test 6 );
    unique_ptr<Tree> myTree = new Tree("my_tree", -1, "NEW");    
//    setActiveTree(myTree);
    unique_ptr<TreeNode> node1 = myTree->addNode("NODE1","NUMERIC");
    unique_ptr<Data> data = compile("SIN(0:2*$pi:0.01)");
    std::cout << data->getString() << "\n";
    node1->putData(data);
    myTree->write();
    END_TESTING;
}

int main(){
    
#ifdef _WIN32    
    _putenv_s("my_tree_path",".");
#else
    setenv("my_tree_path",".",1);
#endif  
    
    // test4();
//    test5();
    test6();
}
