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

    setenv("t_treeseg_path",".",1);
      MDSplus::Tree *t = new MDSplus::Tree("t_treeseg",1,"NEW");

      std::cout << "APERTO 1\n";
      MDSplus::TreeNode *n1 = t->addNode("SEG2","SIGNAL");
      MDSplus::TreeNode *n2 = t->addNode("SEG3","SIGNAL");
      float times[1000];
      float data[1000];
      for(int i = 0; i < 1000; i++){
        times[i] = i*100;
        data[i] = i*10;
      }
      MDSplus::Float32Array *dim = new MDSplus::Float32Array(times, 1000);
      MDSplus::Float32Array *vals = new MDSplus::Float32Array(data, 1000);
      MDSplus::Float32 *start = new MDSplus::Float32(0);
      MDSplus::Float32 *end = new MDSplus::Float32(999);

      n1->beginSegment(start, end, dim, vals);
      n2->beginSegment(start, end, dim, vals);
      MDSplus::Float32Array *chunk = new MDSplus::Float32Array(data, 100);
      //Write in chunks of 100
      for(int i = 0; i < 10; i++){
        std::cout << "Writing node1 100 samples chunk " << i << std::endl;
        n1->putSegment(chunk, -1);
        std::cout << "Writing node2 100 samples chunk " << i << std::endl;
        n2->putSegment(chunk, -1);
      }

    END_TESTING;

}
