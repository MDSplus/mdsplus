PARTS = mdsshr \
	treeshr \
	tdishr \
	tditest \
	xmdsshr \
	dwscope \
	mitdevices \
	traverser \
	mdslibidl \
	mdstcpip

all :
	cdir=`pwd`; \
        for dir in $(PARTS) ; do\
	  if (test -d $${dir}) then \
	    cd $${dir};\
	    make;\
	    cd $$cdir;\
          fi;\
	done

clean :
	cdir=`pwd`; \
	for dir in $(PARTS) ; do\
	  if (test -d $${dir}) then \
	    cd $${dir};\
	    make clean;\
	    cd $$cdir;\
          fi;\
	done

