all : "$(OUTDIR)\MdsShr.lib" "$(OUTDIR)\TreeShr.lib" "$(OUTDIR)\TdiShr.lib" "$(OUTDIR)\MdsIpShr.lib" "$(OUTDIR)\MdsMisc.lib" "$(OUTDIR)\MdsServerShr.lib" "$(OUTDIR)\MdsLib.lib" "$(OUTDIR)\Mdsdcl.lib" "$(OUTDIR)\MdsMath.lib" "$(OUTDIR)\RemCamShr.lib"
	echo Done

"$(OUTDIR)\MdsShr.lib" : "$(OUTDIR)\..\mdsshr\MdsShr.def"
	lib /def:$? /out:$@ /machine:$(ARCH)

"$(OUTDIR)\TreeShr.lib" : "$(OUTDIR)\..\treeshr\TreeShr.def"
	lib /def:$? /out:$@ /machine:$(ARCH)

"$(OUTDIR)\TdiShr.lib" : "$(OUTDIR)\..\tdishr\TdiShr.def"
	lib /def:$? /out:$@ /machine:$(ARCH)

"$(OUTDIR)\MdsIpShr.lib" : "$(OUTDIR)\..\mdstcpip\MdsIpShr.def"
	lib /def:$? /out:$@ /machine:$(ARCH)

"$(OUTDIR)\MdsMisc.lib" : "$(OUTDIR)\..\mdsmisc\MdsMisc.def"
	lib /def:$? /out:$@ /machine:$(ARCH)

"$(OUTDIR)\ServerShr.lib" : "$(OUTDIR)\..\servershr\MdsServerShr.def"
	lib /def:$? /out:$@ /machine:$(ARCH)

"$(OUTDIR)\MdsLib.lib" : "$(OUTDIR)\..\mdslib\MdsLib.def"
	lib /def:$? /out:$@ /machine:$(ARCH)

"$(OUTDIR)\Mdsdcl.lib" : "$(OUTDIR)\..\mdsdcl\Mdsdcl.def"
	lib /def:$? /out:$@ /machine:$(ARCH)

"$(OUTDIR)\MdsMath.lib" : "$(OUTDIR)\..\math\MdsMath.def"
	lib /def:$? /out:$@ /machine:$(ARCH)

"$(OUTDIR)\RemCamShr.lib" : "$(OUTDIR)\..\remcam\RemCamShr.def"
	lib /def:$? /out:$@ /machine:$(ARCH)

