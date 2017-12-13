EXPORT int descr_(int *dtype, void *data, int *dim1, int *dim2, int *dim3, int *dim4, int *dim5, int *dim6,
	  int *dim7, int *dim8, int *dim9)
{
  return descr(dtype, data, dim1, dim2, dim3, dim4, dim5, dim6, dim7, dim8, dim9);
}
EXPORT int descr2_(int *dtype, int *dim1, int *dim2, int *dim3, int *dim4, int *dim5, int *dim6, int *dim7,
	   int *dim8, int *dim9)
{
  return descr2(dtype, dim1, dim2, dim3, dim4, dim5, dim6, dim7, dim8, dim9);
}
EXPORT int mdsconnect_(char *host)
{
  return MdsConnect(host);
}
EXPORT int mdsclose_(char *tree, int *shot) 
{
  return MdsClose(tree, shot); 
}
EXPORT void mdsdisconnect_() 
{ 
  MdsDisconnect();
}
EXPORT int mdsopen_(char *tree, int *shot) 
{ 
  return MdsOpen(tree, shot); 
}
EXPORT int mdssetdefault_(char *node) 
{ 
  return MdsSetDefault(node); 
}
EXPORT int mdssetsocket_(int *newsocket) 
{ 
  return MdsSetSocket(newsocket); 
}
EXPORT int mdsput_(char *node, char *expression,
	   int *a00, int *a01, int *a02, int *a03, int *a04, int *a05, int *a06, int *a07, int *a08,
	   int *a09, int *a10, int *a11, int *a12, int *a13, int *a14, int *a15, int *a16, int *a17,
	   int *a18, int *a19, int *a20, int *a21, int *a22, int *a23, int *a24, int *a25, int *a26,
	   int *a27, int *a28, int *a29, int *a30, int *a31, int *a32, int *a33, int *a34, int *a35,
	   int *a36, int *a37, int *a38, int *a39, int *a40, int *a41, int *a42, int *a43, int *a44,
	   int *a45, int *a46, int *a47, int *a48, int *a49, int *a50, int *a51, int *a52, int *a53,
	   int *a54, int *a55, int *a56, int *a57, int *a58, int *a59, int *a60, int *a61, int *a62,
	   int *a63, int *a64, int *a65, int *a66, int *a67, int *a68, int *a69, int *a70, int *a71,
	   int *a72, int *a73, int *a74, int *a75, int *a76, int *a77, int *a78, int *a79, int *a80,
	   int *a81, int *a82, int *a83, int *a84, int *a85, int *a86, int *a87, int *a88, int *a89,
	   int *a90, int *a91, int *a92, int *a93, int *a94, int *a95, int *a96, int *a97, int *a98,
	   int *a99)
{
  return MdsPut(node, expression,
		   a00, a01, a02, a03, a04, a05, a06, a07, a08, a09,
		   a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
		   a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
		   a30, a31, a32, a33, a34, a35, a36, a37, a38, a39,
		   a40, a41, a42, a43, a44, a45, a46, a47, a48, a49,
		   a50, a51, a52, a53, a54, a55, a56, a57, a58, a59,
		   a60, a61, a62, a63, a64, a65, a66, a67, a68, a69,
		   a70, a71, a72, a73, a74, a75, a76, a77, a78, a79,
		   a80, a81, a82, a83, a84, a85, a86, a87, a88, a89,
		   a90, a91, a92, a93, a94, a95, a96, a97, a98, a99);
}
EXPORT int mdsvalue_(char *expression,
	     int *a00, int *a01, int *a02, int *a03, int *a04, int *a05, int *a06, int *a07,
	     int *a08, int *a09, int *a10, int *a11, int *a12, int *a13, int *a14, int *a15,
	     int *a16, int *a17, int *a18, int *a19, int *a20, int *a21, int *a22, int *a23,
	     int *a24, int *a25, int *a26, int *a27, int *a28, int *a29, int *a30, int *a31,
	     int *a32, int *a33, int *a34, int *a35, int *a36, int *a37, int *a38, int *a39,
	     int *a40, int *a41, int *a42, int *a43, int *a44, int *a45, int *a46, int *a47,
	     int *a48, int *a49, int *a50, int *a51, int *a52, int *a53, int *a54, int *a55,
	     int *a56, int *a57, int *a58, int *a59, int *a60, int *a61, int *a62, int *a63,
	     int *a64, int *a65, int *a66, int *a67, int *a68, int *a69, int *a70, int *a71,
	     int *a72, int *a73, int *a74, int *a75, int *a76, int *a77, int *a78, int *a79,
	     int *a80, int *a81, int *a82, int *a83, int *a84, int *a85, int *a86, int *a87,
	     int *a88, int *a89, int *a90, int *a91, int *a92, int *a93, int *a94, int *a95,
	     int *a96, int *a97, int *a98, int *a99)
{
  return MdsValue(expression,
		     a00, a01, a02, a03, a04, a05, a06, a07, a08, a09,
		     a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
		     a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
		     a30, a31, a32, a33, a34, a35, a36, a37, a38, a39,
		     a40, a41, a42, a43, a44, a45, a46, a47, a48, a49,
		     a50, a51, a52, a53, a54, a55, a56, a57, a58, a59,
		     a60, a61, a62, a63, a64, a65, a66, a67, a68, a69,
		     a70, a71, a72, a73, a74, a75, a76, a77, a78, a79,
		     a80, a81, a82, a83, a84, a85, a86, a87, a88, a89,
		     a90, a91, a92, a93, a94, a95, a96, a97, a98, a99);
}

EXPORT int mdsput2_(char *node, char *expression,
	   int *a00, int *a01, int *a02, int *a03, int *a04, int *a05, int *a06, int *a07, int *a08,
	   int *a09, int *a10, int *a11, int *a12, int *a13, int *a14, int *a15, int *a16, int *a17,
	   int *a18, int *a19, int *a20, int *a21, int *a22, int *a23, int *a24, int *a25, int *a26,
	   int *a27, int *a28, int *a29, int *a30, int *a31, int *a32, int *a33, int *a34, int *a35,
	   int *a36, int *a37, int *a38, int *a39, int *a40, int *a41, int *a42, int *a43, int *a44,
	   int *a45, int *a46, int *a47, int *a48, int *a49, int *a50, int *a51, int *a52, int *a53,
	   int *a54, int *a55, int *a56, int *a57, int *a58, int *a59, int *a60, int *a61, int *a62,
	   int *a63, int *a64, int *a65, int *a66, int *a67, int *a68, int *a69, int *a70, int *a71,
	   int *a72, int *a73, int *a74, int *a75, int *a76, int *a77, int *a78, int *a79, int *a80,
	   int *a81, int *a82, int *a83, int *a84, int *a85, int *a86, int *a87, int *a88, int *a89,
	   int *a90, int *a91, int *a92, int *a93, int *a94, int *a95, int *a96, int *a97, int *a98,
	   int *a99)
{
  return MdsPut2(node, expression,
		   a00, a01, a02, a03, a04, a05, a06, a07, a08, a09,
		   a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
		   a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
		   a30, a31, a32, a33, a34, a35, a36, a37, a38, a39,
		   a40, a41, a42, a43, a44, a45, a46, a47, a48, a49,
		   a50, a51, a52, a53, a54, a55, a56, a57, a58, a59,
		   a60, a61, a62, a63, a64, a65, a66, a67, a68, a69,
		   a70, a71, a72, a73, a74, a75, a76, a77, a78, a79,
		   a80, a81, a82, a83, a84, a85, a86, a87, a88, a89,
		   a90, a91, a92, a93, a94, a95, a96, a97, a98, a99);
}

EXPORT int mdsvalue2_(char *expression,
	     int *a00, int *a01, int *a02, int *a03, int *a04, int *a05, int *a06, int *a07,
	     int *a08, int *a09, int *a10, int *a11, int *a12, int *a13, int *a14, int *a15,
	     int *a16, int *a17, int *a18, int *a19, int *a20, int *a21, int *a22, int *a23,
	     int *a24, int *a25, int *a26, int *a27, int *a28, int *a29, int *a30, int *a31,
	     int *a32, int *a33, int *a34, int *a35, int *a36, int *a37, int *a38, int *a39,
	     int *a40, int *a41, int *a42, int *a43, int *a44, int *a45, int *a46, int *a47,
	     int *a48, int *a49, int *a50, int *a51, int *a52, int *a53, int *a54, int *a55,
	     int *a56, int *a57, int *a58, int *a59, int *a60, int *a61, int *a62, int *a63,
	     int *a64, int *a65, int *a66, int *a67, int *a68, int *a69, int *a70, int *a71,
	     int *a72, int *a73, int *a74, int *a75, int *a76, int *a77, int *a78, int *a79,
	     int *a80, int *a81, int *a82, int *a83, int *a84, int *a85, int *a86, int *a87,
	     int *a88, int *a89, int *a90, int *a91, int *a92, int *a93, int *a94, int *a95,
	     int *a96, int *a97, int *a98, int *a99)
{
  return MdsValue2(expression,
		     a00, a01, a02, a03, a04, a05, a06, a07, a08, a09,
		     a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
		     a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
		     a30, a31, a32, a33, a34, a35, a36, a37, a38, a39,
		     a40, a41, a42, a43, a44, a45, a46, a47, a48, a49,
		     a50, a51, a52, a53, a54, a55, a56, a57, a58, a59,
		     a60, a61, a62, a63, a64, a65, a66, a67, a68, a69,
		     a70, a71, a72, a73, a74, a75, a76, a77, a78, a79,
		     a80, a81, a82, a83, a84, a85, a86, a87, a88, a89,
		     a90, a91, a92, a93, a94, a95, a96, a97, a98, a99);
}

