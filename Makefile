PARTS = mdsshr \
	treeshr \
	tdishr \
        remcam \
	tditest \
	xmdsshr \
	dwscope \
	mitdevices \
	mdslibidl \
	mdstcpip	\
	cdu	\
	mdsdcl	\
	ccl	\
	tcl \
	traverser

all :
	- cdir=`pwd`; \
        for dir in $(PARTS) ; do\
          echo Making $${dir};\
	  if (test -d $${dir}) then \
	    cd $${dir};\
            if (test -f Makefile.`uname`) then \
              make='make -f Makefile.'`uname`;\
            else\
              make=make;\
            fi;\
            if (test -f $(DEPEND)) then \
	      $$make;\
            else\
              $$make DEPEND=/dev/null $(DEPEND);\
              $$make;\
            fi;\
	    cd $$cdir;\
          fi;\
	done

clean :
	cdir=`pwd`; \
	for dir in $(PARTS) ; do\
	  if (test -d $${dir}) then \
	    cd $${dir};\
	    make DEPEND=/dev/null clean;\
	    cd $$cdir;\
          fi;\
	done

