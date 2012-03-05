#define IPDESC_H
#include "mdsip.h"

extern void *MDSIP_IO_HANDLES[];

int MdsValue(int sock, char *expression, 
             void *a001, void *a002, void *a003, void *a004, void *a005, void *a006, void *a007, void *a008, void *a009, void *a010,
             void *a011, void *a012, void *a013, void *a014, void *a015, void *a016, void *a017, void *a018, void *a019, void *a020,
             void *a021, void *a022, void *a023, void *a024, void *a025, void *a026, void *a027, void *a028, void *a029, void *a030,
             void *a031, void *a032, void *a033, void *a034, void *a035, void *a036, void *a037, void *a038, void *a039, void *a040,
             void *a041, void *a042, void *a043, void *a044, void *a045, void *a046, void *a047, void *a048, void *a049, void *a050,
             void *a051, void *a052, void *a053, void *a054, void *a055, void *a056, void *a057, void *a058, void *a059, void *a060,
             void *a061, void *a062, void *a063, void *a064, void *a065, void *a066, void *a067, void *a068, void *a069, void *a070,
             void *a071, void *a072, void *a073, void *a074, void *a075, void *a076, void *a077, void *a078, void *a079, void *a080,
             void *a081, void *a082, void *a083, void *a084, void *a085, void *a086, void *a087, void *a088, void *a089, void *a090,
             void *a091, void *a092, void *a093, void *a094, void *a095, void *a096, void *a097, void *a098, void *a099, void *a100,
             void *a101, void *a102, void *a103, void *a104, void *a105, void *a106, void *a107, void *a108, void *a109, void *a110,
             void *a111, void *a112, void *a113, void *a114, void *a115, void *a116, void *a117, void *a118, void *a119, void *a120,
             void *a121, void *a122, void *a123, void *a124, void *a125, void *a126, void *a127, void *a128, void *a129, void *a130,
             void *a131, void *a132, void *a133, void *a134, void *a135, void *a136, void *a137, void *a138, void *a139, void *a140,
             void *a141, void *a142, void *a143, void *a144, void *a145, void *a146, void *a147, void *a148, void *a149, void *a150,
             void *a151, void *a152, void *a153, void *a154, void *a155, void *a156, void *a157, void *a158, void *a159, void *a160,
             void *a161, void *a162, void *a163, void *a164, void *a165, void *a166, void *a167, void *a168, void *a169, void *a170,
             void *a171, void *a172, void *a173, void *a174, void *a175, void *a176, void *a177, void *a178, void *a179, void *a180,
             void *a181, void *a182, void *a183, void *a184, void *a185, void *a186, void *a187, void *a188, void *a189, void *a190,
             void *a191, void *a192, void *a193, void *a194, void *a195, void *a196, void *a197, void *a198, void *a199, void *a200,
             void *a201, void *a202, void *a203, void *a204, void *a205, void *a206, void *a207, void *a208, void *a209, void *a210,
             void *a211, void *a212, void *a213, void *a214, void *a215, void *a216, void *a217, void *a218, void *a219, void *a220,
             void *a221, void *a222, void *a223, void *a224, void *a225, void *a226, void *a227, void *a228, void *a229, void *a230,
             void *a231, void *a232, void *a233, void *a234, void *a235, void *a236, void *a237, void *a238, void *a239, void *a240,
             void *a241, void *a242, void *a243, void *a244, void *a245, void *a246, void *a247, void *a248, void *a249, void *a250,
             void *a251, void *a252, void *a253)
{
  return mdsip_value(MDSIP_IO_HANDLES[sock-1],expression,
		     a001, a002, a003, a004, a005, a006, a007, a008, a009, a010,
		     a011, a012, a013, a014, a015, a016, a017, a018, a019, a020,
		     a021, a022, a023, a024, a025, a026, a027, a028, a029, a030,
		     a031, a032, a033, a034, a035, a036, a037, a038, a039, a040,
		     a041, a042, a043, a044, a045, a046, a047, a048, a049, a050,
		     a051, a052, a053, a054, a055, a056, a057, a058, a059, a060,
		     a061, a062, a063, a064, a065, a066, a067, a068, a069, a070,
		     a071, a072, a073, a074, a075, a076, a077, a078, a079, a080,
		     a081, a082, a083, a084, a085, a086, a087, a088, a089, a090,
		     a091, a092, a093, a094, a095, a096, a097, a098, a099, a100,
		     a101, a102, a103, a104, a105, a106, a107, a108, a109, a110,
		     a111, a112, a113, a114, a115, a116, a117, a118, a119, a120,
		     a121, a122, a123, a124, a125, a126, a127, a128, a129, a130,
		     a131, a132, a133, a134, a135, a136, a137, a138, a139, a140,
		     a141, a142, a143, a144, a145, a146, a147, a148, a149, a150,
		     a151, a152, a153, a154, a155, a156, a157, a158, a159, a160,
		     a161, a162, a163, a164, a165, a166, a167, a168, a169, a170,
		     a171, a172, a173, a174, a175, a176, a177, a178, a179, a180,
		     a181, a182, a183, a184, a185, a186, a187, a188, a189, a190,
		     a191, a192, a193, a194, a195, a196, a197, a198, a199, a200,
		     a201, a202, a203, a204, a205, a206, a207, a208, a209, a210,
		     a211, a212, a213, a214, a215, a216, a217, a218, a219, a220,
		     a221, a222, a223, a224, a225, a226, a227, a228, a229, a230,
		     a231, a232, a233, a234, a235, a236, a237, a238, a239, a240,
		     a241, a242, a243, a244, a245, a246, a247, a248, a249, a250,
		     a251, a252, a253);
}


int MdsPut(int sock, char *node, char *expression, 
	   void *a001, void *a002, void *a003, void *a004, void *a005, void *a006, void *a007, void *a008, void *a009, void *a010,
	   void *a011, void *a012, void *a013, void *a014, void *a015, void *a016, void *a017, void *a018, void *a019, void *a020,
	   void *a021, void *a022, void *a023, void *a024, void *a025, void *a026, void *a027, void *a028, void *a029, void *a030,
	   void *a031, void *a032, void *a033, void *a034, void *a035, void *a036, void *a037, void *a038, void *a039, void *a040,
	   void *a041, void *a042, void *a043, void *a044, void *a045, void *a046, void *a047, void *a048, void *a049, void *a050,
	   void *a051, void *a052, void *a053, void *a054, void *a055, void *a056, void *a057, void *a058, void *a059, void *a060,
	   void *a061, void *a062, void *a063, void *a064, void *a065, void *a066, void *a067, void *a068, void *a069, void *a070,
	   void *a071, void *a072, void *a073, void *a074, void *a075, void *a076, void *a077, void *a078, void *a079, void *a080,
	   void *a081, void *a082, void *a083, void *a084, void *a085, void *a086, void *a087, void *a088, void *a089, void *a090,
	   void *a091, void *a092, void *a093, void *a094, void *a095, void *a096, void *a097, void *a098, void *a099, void *a100,
	   void *a101, void *a102, void *a103, void *a104, void *a105, void *a106, void *a107, void *a108, void *a109, void *a110,
	   void *a111, void *a112, void *a113, void *a114, void *a115, void *a116, void *a117, void *a118, void *a119, void *a120,
	   void *a121, void *a122, void *a123, void *a124, void *a125, void *a126, void *a127, void *a128, void *a129, void *a130,
	   void *a131, void *a132, void *a133, void *a134, void *a135, void *a136, void *a137, void *a138, void *a139, void *a140,
	   void *a141, void *a142, void *a143, void *a144, void *a145, void *a146, void *a147, void *a148, void *a149, void *a150,
	   void *a151, void *a152, void *a153, void *a154, void *a155, void *a156, void *a157, void *a158, void *a159, void *a160,
	   void *a161, void *a162, void *a163, void *a164, void *a165, void *a166, void *a167, void *a168, void *a169, void *a170,
	   void *a171, void *a172, void *a173, void *a174, void *a175, void *a176, void *a177, void *a178, void *a179, void *a180,
	   void *a181, void *a182, void *a183, void *a184, void *a185, void *a186, void *a187, void *a188, void *a189, void *a190,
	   void *a191, void *a192, void *a193, void *a194, void *a195, void *a196, void *a197, void *a198, void *a199, void *a200,
	   void *a201, void *a202, void *a203, void *a204, void *a205, void *a206, void *a207, void *a208, void *a209, void *a210,
	   void *a211, void *a212, void *a213, void *a214, void *a215, void *a216, void *a217, void *a218, void *a219, void *a220,
	   void *a221, void *a222, void *a223, void *a224, void *a225, void *a226, void *a227, void *a228, void *a229, void *a230,
	   void *a231, void *a232, void *a233, void *a234, void *a235, void *a236, void *a237, void *a238, void *a239, void *a240,
	   void *a241, void *a242, void *a243, void *a244, void *a245, void *a246, void *a247, void *a248, void *a249, void *a250,
	   void *a251, void *a252, void *a253)
{
  return mdsip_put(MDSIP_IO_HANDLES[sock-1],node, expression,
		   a001, a002, a003, a004, a005, a006, a007, a008, a009, a010,
		   a011, a012, a013, a014, a015, a016, a017, a018, a019, a020,
		   a021, a022, a023, a024, a025, a026, a027, a028, a029, a030,
		   a031, a032, a033, a034, a035, a036, a037, a038, a039, a040,
		   a041, a042, a043, a044, a045, a046, a047, a048, a049, a050,
		   a051, a052, a053, a054, a055, a056, a057, a058, a059, a060,
		   a061, a062, a063, a064, a065, a066, a067, a068, a069, a070,
		   a071, a072, a073, a074, a075, a076, a077, a078, a079, a080,
		   a081, a082, a083, a084, a085, a086, a087, a088, a089, a090,
		   a091, a092, a093, a094, a095, a096, a097, a098, a099, a100,
		   a101, a102, a103, a104, a105, a106, a107, a108, a109, a110,
		   a111, a112, a113, a114, a115, a116, a117, a118, a119, a120,
		   a121, a122, a123, a124, a125, a126, a127, a128, a129, a130,
		   a131, a132, a133, a134, a135, a136, a137, a138, a139, a140,
		   a141, a142, a143, a144, a145, a146, a147, a148, a149, a150,
		   a151, a152, a153, a154, a155, a156, a157, a158, a159, a160,
		   a161, a162, a163, a164, a165, a166, a167, a168, a169, a170,
		   a171, a172, a173, a174, a175, a176, a177, a178, a179, a180,
		   a181, a182, a183, a184, a185, a186, a187, a188, a189, a190,
		   a191, a192, a193, a194, a195, a196, a197, a198, a199, a200,
		   a201, a202, a203, a204, a205, a206, a207, a208, a209, a210,
		   a211, a212, a213, a214, a215, a216, a217, a218, a219, a220,
		   a221, a222, a223, a224, a225, a226, a227, a228, a229, a230,
		   a231, a232, a233, a234, a235, a236, a237, a238, a239, a240,
		   a241, a242, a243, a244, a245, a246, a247, a248, a249, a250,
		   a251, a252, a253);
}

int MdsOpen(int sock, char *tree, int shot)
{
  return mdsip_open(MDSIP_IO_HANDLES[sock-1],tree,shot);
}

int MdsClose(int sock)
{
  return mdsip_close(MDSIP_IO_HANDLES[sock-1]);
}

int MdsSetDefault(int sock, char *node)
{
  return mdsip_set_default(MDSIP_IO_HANDLES[sock-1],node);
}

