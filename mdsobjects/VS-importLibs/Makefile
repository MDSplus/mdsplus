all : "$(OUTDIR)\MdsShr.lib" "$(OUTDIR)\TreeShr.lib" "$(OUTDIR)\TdiShr.lib" "$(OUTDIR)\MdsIpShr.lib" "$(OUTDIR)\MdsMisc.lib" "$(OUTDIR)\ServerShr.lib" "$(OUTDIR)\MdsLib.lib" "$(OUTDIR)\MdsDclShr.lib" "$(OUTDIR)\MdsMath.lib" "$(OUTDIR)\RemCam.lib"
	echo Done

"$(OUTDIR)\MdsShr.lib" : "$(OUTDIR)\..\mdsshr\mdsshr.def"
	lib /def:$? /out:$@ /machine:$(ARCH)

"$(OUTDIR)\TreeShr.lib" : "$(OUTDIR)\..\treeshr\treeshr.def"
	lib /def:$? /out:$@ /machine:$(ARCH)

"$(OUTDIR)\TdiShr.lib" : "$(OUTDIR)\..\tdishr\tdishr.def"
	lib /def:$? /out:$@ /machine:$(ARCH)

"$(OUTDIR)\MdsIpShr.lib" : "$(OUTDIR)\..\mdstcpip\mdsipshr.def"
	lib /def:$? /out:$@ /machine:$(ARCH)

"$(OUTDIR)\MdsMisc.lib" : "$(OUTDIR)\..\mdsmisc\mdsmisc.def"
	lib /def:$? /out:$@ /machine:$(ARCH)

"$(OUTDIR)\ServerShr.lib" : "$(OUTDIR)\..\servershr\servershr.def"
	lib /def:$? /out:$@ /machine:$(ARCH)

"$(OUTDIR)\MdsLib.lib" : "$(OUTDIR)\..\mdslib\MdsLib.def"
	lib /def:$? /out:$@ /machine:$(ARCH)

"$(OUTDIR)\MdsDclShr.lib" : "$(OUTDIR)\..\mdsdcl\MdsDclShr.def"
	lib /def:$? /out:$@ /machine:$(ARCH)

"$(OUTDIR)\MdsMath.lib" : "$(OUTDIR)\..\math\MdsMath.def"
	lib /def:$? /out:$@ /machine:$(ARCH)

"$(OUTDIR)\RemCam.lib" : "$(OUTDIR)\..\remcam\remcam.def"
	lib /def:$? /out:$@ /machine:$(ARCH)

