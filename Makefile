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
            if (test -f $(DEPEND)) then \
	      make;\
            else\
              make DEPEND=/dev/null $(DEPEND);\
              make;\
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

