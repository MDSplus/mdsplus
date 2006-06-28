extern "C" int PutRecord(int nid, char *data, int size, int policy);


EXPORT int RTreePutRecord(int nid, struct descriptor *descriptor_ptr, int utility_update)
{
	EMPTYXD(ser_xd);

	int status;
	struct descriptor_a *arrPtr;

printf("RTreePutRecord %x %d %s\n", descriptor_ptr, descriptor_ptr->class, RTreeGetPath(nid));
//	printDecompiled(descriptor_ptr);	

	//Manage Empty descriptor
	if(descriptor_ptr->class == CLASS_XD && ((struct descriptor_xd *)descriptor_ptr)->l_length == 0)
		return MdsTreePutData(tree, nid, NULL, 0);

	MdsSerializeDscOut(descriptor_ptr, &ser_xd);
	arrPtr = (struct descriptor_a *)ser_xd.pointer;
	status = MdsTreePutData(tree, nid, arrPtr->pointer, arrPtr->arsize);
	MdsFree1Dx(&ser_xd, 0);
printf("FATTO\n");
	return status;
}


EXPORT extern int RTreeGetRecord(int nid, struct descriptor_xd *dsc_ptr)
{
	void *data;
	int dataLen;
	int status;
printf("RTreeGetRecord\n");


	status = MdsTreeGetData(tree, nid, &data, &dataLen);

printf("STATUS: %d\n", status);

	if((status & 1))
		MdsSerializeDscIn((char *)data, dsc_ptr);
	return status;
}

