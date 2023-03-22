#include <string.h>
#include "envDefs.h"
#include "errlog.h"

#include "mdsServer.h"
#include "fdManager.h"

// single threaded ca server tool main loop
extern int main(int argc, const char **argv)
{
    mdsServer    *pCAS;
    unsigned    scanOn = true;
    Tree *tree;
    bool append;

    if(argc > 4 || argc < 3)
    {
	std::cout << "Usage: casmds <experiment> <shot> [append]\n";
	return -1;
    }
    try {
	int shot;
	sscanf(argv[2], "%d", &shot);
	tree = new Tree((char *)argv[1], shot);
	if(argc == 4 && !strcmp(argv[3], "append"))
	    append = true;
	else
	    append = false; 
    }catch(const MdsException &exc)
    {
	std::cout << "Cannot open experiment " << argv[1] << ": " << exc.what() << "\n";
	return -1;
    }
    try {
        pCAS = new mdsServer(tree, append);
    }
    catch ( ... ){
        printf("Server initialization error\n");
        return (-1);
    }

    while (true) {
        fileDescriptorManager.process(1000.0);
    }

    delete pCAS;
    errlogFlush();
    return (0);
}
