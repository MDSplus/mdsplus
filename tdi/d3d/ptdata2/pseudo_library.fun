/**************************************************************************************************************************
        PUBLIC FUN PSEUDO_LIBRARY()
        
        This TDI function retrieves the location of the PSUEDO Library from the PSEUDO_LIBRARY environment variable

        Author:         Sean Flanagan (flanagan@fusion.gat.com) 

**************************************************************************************************************************/

PUBLIC FUN PSEUDO_LIBRARY() {
	_lib = TranslateLogical("PSEUDO_LIBRARY");
	RETURN(_lib);
}
