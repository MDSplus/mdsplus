
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the JAVAMDS_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// JAVAMDS_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef JAVAMDS_EXPORTS
#define JAVAMDS_API __declspec(dllexport)
#else
#define JAVAMDS_API __declspec(dllimport)
#endif

// This class is exported from the javamds.dll
class JAVAMDS_API CJavamds {
public:
	CJavamds(void);
	// TODO: add your methods here.
};

extern JAVAMDS_API int nJavamds;

JAVAMDS_API int fnJavamds(void);

