prefix=/usr/local
SHELL=/bin/sh
PM=/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker
SF=/Developer/Tools/SetFile
CP=/Developer/Tools/CpMac

JAVA_APS = javamds \
           javascope \
           javatraverser \
           javadispatcher \
           javaclient

HDF5_APS = hdf5

D3D_PACKAGE = d3dshr

PARTS = \
        mdsshr \
        treeshr \
        tdishr \
        tditest \
        xmdsshr \
        mdstcpip \
        dwscope \
        mdslibidl \
        cdu     \
        mdsdcl  \
        gen_device \
         \
        remcam \
        ccl     \
        servershr \
        tcl \
        tdic \
        traverser \
        mdslib \
        actions \
        math \
        mitdevices \
        wfevent \
        setevent \
        mdsmisc \
        scripts \
        rpm \
        manpages \
         \
         \
        mdssql \
        idlsql \
        idlmdsevent \
        idlmdswidgets \
        
BINARIES = bin etc lib uid \
           tdi idl trees envsyms \
           setup.sh setup.csh java LabView \
           include/ipdesc.h include/mdslib.inc

CLIENT_BINARIES = include/ipdesc.h include/mdslib.inc lib/libMdsIpShr.* \
                    lib/libMdsLib*.* idl


# package up motif libraries?  No license for this.

MOTIF_ROOT=/usr/local
MOTIF_LIBS= Mrm Uil Xm
MOTIF_INCLUDES= Mrm uil Xm X11
MOTIF_BIN= uil xmbind mwm  
MOTIF_MAJOR_VERSION=3
MOTIF_VERSION=3.0.2

MOTIF_FILES=$(MOTIF_LIBS:%=$(MOTIF_ROOT)/lib/lib%.a) \
	$(MOTIF_LIBS:%=$(MOTIF_ROOT)/lib/lib%.dylib) \
	$(MOTIF_LIBS:%=$(MOTIF_ROOT)/lib/lib%.$(MOTIF_MAJOR_VERSION).dylib) \
	$(MOTIF_LIBS:%=$(MOTIF_ROOT)/lib/lib%.$(MOTIF_VERSION).dylib) \
	$(MOTIF_LIBS:%=$(MOTIF_ROOT)/lib/lib%.la) \
	$(MOTIF_BIN:%=$(MOTIF_ROOT)/bin/%) \
	$(MOTIF_ROOT)/lib/X11/ \
	$(MOTIF_INCLUDES:%=$(MOTIF_ROOT)/include/%/)


all: dmg local client

dstroot:
	mkdir dstroot
	tar cf - /usr/local/mdsplus | (cd dstroot; tar xf -)
	rm -r dstroot/usr/local/mdsplus/local   # don't distribute local mods...
	find dstroot -name "*~" | xargs rm

localroot: /usr/local/mdsplus/local
	mkdir localroot
	tar cf - /usr/local/mdsplus/local | (cd localroot; tar xf -)
	find localroot -name "*~" | xargs rm

dmgdir:
	mkdir dmgdir
	cp metapkg_resources/*.rtf dmgdir
	$(SF) -a E dmgdir/*.rtf
	cp dmg_dsstore dmgdir/.DS_Store

dmgdir/MDSplus.mpkg: dmgdir MDSplus_files.pkg MDSplus_server.pkg MDSplus_logicals.pkg
	mkdir $@
	/Developer/Tools/SetFile -a EB $@
	mkdir $@/Contents
	mkdir $@/Contents/Resources
	cp metapkg_resources/*.rtf $@/Contents/Resources
	cp metapkg_resources/Info.plist $@/Contents
	echo pmkrpkg1 > $@/Contents/PkgInfo
	cp metapkg_resources/Description.plist $@/Contents/Resources
	cp metapkg_resources/MDSplus.info $@/Contents/Resources
	$(CP) -r MDSplus_files.pkg $@
	$(CP) -r MDSplus_logicals.pkg $@
	$(CP) -r MDSplus_server.pkg $@
	find $@ -name CVS -exec rm -R {} \;

MDSplus_files.pkg: dstroot
	$(PM) -build -p $(PWD)/$@ -f $(PWD)/dstroot -i $(PWD)/files_resources/Info.plist \
	 -d $(PWD)/files_resources/Description.plist 

Local_files.pkg: localroot
	$(PM) -build -p $(PWD)/$@ -f $(PWD)/localroot -i $(PWD)/local_resources/Info.plist \
	 -d $(PWD)/local_resources/Description.plist

OpenMotif.pkg: $(MOTIF_FILES)
	mkdir tmproot
	tar cf - $(MOTIF_FILES) | (cd tmproot; tar xf -)
	$(PM) -build -p $(PWD)/$@ -f $(PWD)/tmproot -r $(PWD)/openmotif_resources \
		-i $(PWD)/openmotif_resources/Info.plist \
	 	-d $(PWD)/openmotif_resources/Description.plist
	rm -r tmproot

dmg: dmgdir/MDSplus.mpkg
	rm MDSplus.dmg
	./make_dmg MDSplus.dmg dmgdir "MDSplus Installer"

local: dmgdir/MDSplus.mpkg Local_files.pkg
	$(CP) -r Local_files.pkg dmgdir/Local_Files.pkg
	rm MDSplus_local.dmg
	./make_dmg MDSplus_local.dmg dmgdir "MDSplus Installer (Local Version)"

client:
	(cd /usr/local/mdsplus; tar cf - $(CLIENT_BINARIES)) | gzip - > MDSplus.`uname`.client.tgz
    

.IGNORE:

clean:
	rm -rf dmgdir
	rm -rf dstroot
	rm -rf MDSplus_files.pkg
	rm -rf Local_files.pkg
	rm -rf localroot
	
