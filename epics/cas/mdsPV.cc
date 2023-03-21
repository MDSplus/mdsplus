#include "mdsServer.h"
#include "gddApps.h"
#include "epicsTime.h"
#include "aitTypes.h"

#include <inttypes.h>


#define aitInt8Type 1
#define aitUint8Type 2
#define aitInt16Type 3
#define aitUint16Type 4
#define aitInt32Type 5
#define aitUint32Type 6
#define aitFloat32Type 7
#define aitFloat64Type 8



// static data for mdsPV
char mdsPV::hasBeenInitialized = 0;
gddAppFuncTable<mdsPV> mdsPV::ft;
epicsTime mdsPV::currentTime;


// special gddDestructor to destroy fixed strings

class mdsFixedStringDestructor: public gddDestructor{
    virtual void run (void *);
};


void mdsFixedStringDestructor::run(void * pUntyped)
{
    aitFixedString *ps = (aitFixedString *) pUntyped;
    delete [] ps;
}

class mdsStringDestructor: public gddDestructor{
    virtual void run (void *);
};


void mdsStringDestructor::run(void * pUntyped)
{
    aitString *ps = (aitString *) pUntyped;
    delete ps;
}


// special gddDestructor to destroy the array which represents the PV value

class mdsVecDestructor: public gddDestructor{
    int type;
public: 
    mdsVecDestructor(int type)
    {
	this->type = type;
    }
    virtual void run (void *);
};


void mdsVecDestructor::run(void *pUntyped)
{
    switch(type) {
	case aitInt8Type: 
	{
	    aitInt8 *pf = reinterpret_cast < aitInt8 * > (pUntyped);
     	    delete [] pf;
	    break;
	}
	case aitUint8Type: 
	{
	    aitUint8 *pf = reinterpret_cast < aitUint8 * > (pUntyped);
     	    delete [] pf;
	    break;
	}
	case aitInt16Type: 
	{
	    aitInt16 *pf = reinterpret_cast < aitInt16 * > (pUntyped);
     	    delete [] pf;
	    break;
	}
	case aitUint16Type: 
	{
	    aitUint16 *pf = reinterpret_cast < aitUint16 * > (pUntyped);
     	    delete [] pf;
	    break;
	}
	case aitInt32Type: 
	{
	    aitInt32 *pf = reinterpret_cast < aitInt32 * > (pUntyped);
     	    delete [] pf;
	    break;
	}
	case aitUint32Type: 
	{
	    aitUint32 *pf = reinterpret_cast < aitUint32 * > (pUntyped);
     	    delete [] pf;
	    break;
	}
	case aitFloat32Type:
	{ 
	    aitFloat32 *pf = reinterpret_cast < aitFloat32 * > (pUntyped);
     	    delete [] pf;
	    break;
	}
	case aitFloat64Type:
	{ 
	    aitFloat64 *pf = reinterpret_cast < aitFloat64 * > (pUntyped);
     	    delete [] pf;
	    break;
	}
    }
}

void mdsPV::setGddFromData(Data *data, gdd &outGdd)
{
    char clazz, dtype, nDims;
    short length;
    int *dims;
    void *ptr;

    data->getInfo(&clazz, &dtype, &length, &nDims, &dims, &ptr);
    if(clazz == CLASS_S) //Scalar ata
    {
	switch(data->dtype) {
	    case DTYPE_B: 
		outGdd = (aitInt8)data->getByte();
		break;
	    case DTYPE_BU: 
		outGdd = (aitUint8)data->getByte();
		break;
	    case DTYPE_W: 
		outGdd = (aitInt16)data->getShort();
		break;
	    case DTYPE_WU: 
		outGdd = (aitUint16)data->getShort();
		break;
	    case DTYPE_L: 
		outGdd = (aitInt32)data->getLong();
		break;
	    case DTYPE_LU: 
		outGdd = (aitUint32)data->getLong();
		break;
	    case DTYPE_FLOAT: 
		outGdd = (aitFloat32)data->getFloat();
		break;
	    case DTYPE_DOUBLE: 
		outGdd = (aitFloat64)data->getDouble();
		break;
	    case DTYPE_T:
		char *str = data->getString();
		aitString *aitStr = new aitString(str, aitStrCopy);
		outGdd = *aitStr;
		delete aitStr;
		delete [] str;
	}
    }
    else if (clazz == CLASS_A)
    {
	Array *arr = (Array *)data;
	char *byteArr;
	short *shortArr;
	int *intArr;
	float *floatArr;
	double *doubleArr;
	int size;

	outGdd.setDimension(nDims);
	for(int i = 0; i < nDims; i++)
	    outGdd.setBound(i, 0, dims[i]);
	switch(dtype) {
	    case DTYPE_B: 
		byteArr = arr->getByteArray(&size);
		outGdd.putRef((aitInt8 *)byteArr, new mdsVecDestructor(aitInt8Type));
		break;
	    case DTYPE_BU: 
		byteArr = arr->getByteArray(&size);
		outGdd.putRef((aitUint8 *)byteArr, new mdsVecDestructor(aitUint8Type));
		break;
	    case DTYPE_W: 
		shortArr = arr->getShortArray(&size);
		outGdd.putRef((aitInt16 *)shortArr, new mdsVecDestructor(aitInt16Type));
		break;
	    case DTYPE_WU: 
		shortArr = arr->getShortArray(&size);
		outGdd.putRef((aitUint16 *)shortArr, new mdsVecDestructor(aitUint16Type));
		break;
	    case DTYPE_L: 
		intArr = arr->getIntArray(&size);
		outGdd.putRef((aitInt32 *)intArr, new mdsVecDestructor(aitInt32Type));
		break;
	    case DTYPE_LU: 
		intArr = arr->getIntArray(&size);
		outGdd.putRef((aitUint32 *)intArr, new mdsVecDestructor(aitUint32Type));
		break;
	    case DTYPE_FLOAT: 
		floatArr = arr->getFloatArray(&size);
		outGdd.putRef((aitFloat32 *)floatArr, new mdsVecDestructor(aitFloat32Type));
		break;
	    case DTYPE_DOUBLE: 
		doubleArr = arr->getDoubleArray(&size);
		outGdd.putRef((aitFloat64 *)doubleArr, new mdsVecDestructor(aitFloat64Type));
		break;
	}
    }
    if(nDims > 0)
	delete[] dims;
}	
	
		


mdsPV::mdsPV(mdsServer & casIn, char *name, Tree *treeIn, TreeNode *topNode, bool appendIn) :
    cas(casIn),
    interest(false)
{
    append = appendIn;
    tree = treeIn;
    refCount = 1;
    nDims = 0;
    numEnums = 0;
    valNode = NULL;
    this->name = new char[strlen(name) + 1];
    strcpy(this->name, name);
    TreeNode *prevDef = 0;
//Default vales */
    highAlarm = 1000, lowAlarm = -1000, highWarning = 100, lowWarning = -100, highCtrl = 10, lowCtrl = -10, highGraphic = 1, lowGraphic = -1;

//Retrieve all the PV-related nodes
    try {
	prevDef = tree->getDefault();
	tree->setDefault(topNode);
	valNode = tree->getNode(":VAL");
	const char *usage = valNode->getUsage();
	if(!strcmp(usage, "NUMERIC"))
	    isText = false;
	else
	    isText = true;

	char clazz, dtype, currDims;
	void *ptr;
	short length; 
	if(append && !isText) //if segmented
	{
	    try {
	        Array *arr = valNode->getSegment(0);
//		arr->getInfo(&clazz, &dtype, &length, &currDims, &dims, &ptr);
//		nDims = currDims - 1;
		deleteData(arr);
	    }catch(const MdsException &exc)
	    {
		std::cout << "Error prevDef reading value for " << name << ": " << exc.what() << "\n";
		delete exc;
	    }
	}
	else //not segmented
	{
	    try {
	        Data *data = valNode->getData();
//		data->getInfo(&clazz, &dtype, &length, &currDims, &dims, &ptr);
//		nDims = currDims;
		deleteData(data);
	    }catch(const MdsException &exc)
	    {
		std::cout << "Error reading value for " << name << ": " << exc.what() << "\n";
		delete exc;
	    }
	}

//PATCH per far funzionare l'mdscas
	if(true)
	{
		tree->setDefault(prevDef);
		delete prevDef;
	        valid = true;
		return;
	}


std::cout << "mdsPV  tree "<< tree <<"\n";	
	TreeNode *currNode;
	try {
std::cout << "mdsPV  Data \n";	
	    currNode = tree->getNode(":DIMS");
std::cout << "mdsPV  Data "<< currNode << "\n";	
//	    dims = currNode->getIntArray(&nDims);
//	    delete currNode;
	    return;
	}
	catch(const MdsException &exc)
	{
	    nDims = 0;
std::cout << "mdsPV  nDims exc"<< nDims <<"\n";
	    //delete exc;
	}
std::cout << "mdsPV  nDims "<< nDims <<"\n";

	try {
	    currNode = tree->getNode(":LOPR");
	    lopr = currNode->getDouble();
	    delete currNode;
	    currNode = tree->getNode(":HOPR");
	    hopr = currNode->getDouble();
	    delete currNode;
	    hasOpr = true;
	} catch(const MdsException &exc)
	{
	    std::cout << "Cannot get LOPR or HOPR for " << name << "\n";
	    hasOpr = false;
	    delete exc;
	}
	try {
	    currNode = tree->getNode(":UNITS");
	    units = currNode->getString();
	    delete currNode;
	} catch(const MdsException &exc)
	{
	    std::cout << "Cannot get UNITS for " << name << "\n";
	    units = NULL;
	    delete exc;
	}
	try {
	    currNode = tree->getNode(":HIGH_ALARM");
	    highAlarm = currNode->getDouble();
	    delete currNode;
	    currNode = tree->getNode(":LOW_ALARM");
	    lowAlarm = currNode->getDouble();
	    delete currNode;
	    hasAlarm = true;
	} catch(const MdsException &exc)
	{
//	    std::cout << "Cannot get HIGH_ALARM or LOW_ALARM for " << name << "\n";
	    hasAlarm = false;
	    delete exc;
	}
	try {
	    currNode = tree->getNode(":HIGH_WARN");
	    highWarning = currNode->getDouble();
	    delete currNode;
	    currNode = tree->getNode(":LOW_WARN");
	    lowWarning = currNode->getDouble();
	    delete currNode;
	    hasWarning = true;
	} catch(const MdsException &exc)
	{
//	    std::cout << "Cannot get HIGH_WARN or LOW_WARN for " << name << "\n";
	    hasWarning = false;
	    delete exc;
	}
	try {
	    currNode = tree->getNode(":HIGH_CTRL");
	    highCtrl = currNode->getDouble();
	    delete currNode;
	    currNode = tree->getNode(":LOW_CTRL");
	    lowCtrl = currNode->getDouble();
	    delete currNode;
	    hasCtrl = true;
	} catch(const MdsException &exc)
	{
//	    std::cout << "Cannot get HIGH_CTRL or LOW_CTRL for " << name << "\n";
	    hasCtrl = false;
	    delete exc;
	}
	try {
	    currNode = tree->getNode(":HIGH_GRAPH");
	    highGraphic = currNode->getDouble();
	    delete currNode;
	    currNode = tree->getNode(":LOW_GRAPH");
	    lowGraphic = currNode->getDouble();
	    delete currNode;
	    hasGraphic = true;
	} catch(const MdsException &exc)
	{
//	    std::cout << "Cannot get HIGH_GRAPH or LOW_GRAPH for " << name << "\n";
	    hasGraphic = false;
	    delete exc;
	}
	try  {
	    currNode = tree->getNode(":PREC");
	    precision = currNode->getInt();
	    delete currNode;
	} catch(const MdsException &exc)
	{
//	    std::cout << "Cannot get PREC for " << name << "\n";
	    precision = 0;
	    delete exc;
	}
	try {
	    currNode = tree->getNode(":ENUMS");
	    Data *enumData = currNode->getData();
	    enums = enumData->getStringArray(&numEnums);
	    deleteData(enumData);
	    delete currNode;
	} catch(const MdsException &exc)
	{
	    enums = NULL;
	    numEnums = 0;
	    delete exc;
	}
	tree->setDefault(prevDef);
	delete prevDef;
        valid = true;
   }catch(const MdsException &exc)
    {
	std::cout << "Error initilizing MDSplus nodes: " << exc.what() << "\n";
	valid = false;
	tree->setDefault(prevDef);
	delete prevDef;
 	delete exc;
   }
}


mdsPV::~mdsPV() 
{
    delete [] name;
    if (numEnums > 0)
    {
	for(int i = 0; i < numEnums; i++)
	    delete [] enums[i];
	delete []enums;
    }
    if(nDims > 0)
	delete[]dims;
    if(valNode)
	delete valNode;
}


void mdsPV::destroy()
{
    refCount--;
    if(refCount == 0)
    	delete this;
}





// Called whenever a client starts monitoring the PV

caStatus mdsPV::interestRegister ()
{
    if(!this->getCAS()){
        return S_casApp_success;
    }

    this->interest = true;
    return S_casApp_success;
}


Data *mdsPV::dataFromGdd(const gdd &inGdd)
{
    if(inGdd.isAtomic()) 
    {
	int numDims = inGdd.dimension();
	const gddBounds* pb = inGdd.getBounds ();
	int *dims = new int[numDims];
	for(int i = 0; i < numDims; i++)
	{
	    dims[i] = pb[i].size();
	}
	Data *retData;
	switch(inGdd.primitiveType()) {
	    case aitEnumInt8: 
		retData = new Int8Array((const char *)inGdd.dataPointer(), numDims, dims);
		break;
	    case aitEnumUint8: 
		retData = new Uint8Array((const unsigned char *)inGdd.dataPointer(), numDims, dims);
		break;
	    case aitEnumInt16: 
		retData = new Int16Array((const short  int*)inGdd.dataPointer(), numDims, dims);
		break;
	    case aitEnumUint16: 
		retData = new Uint16Array((const short unsigned int*)inGdd.dataPointer(), numDims, dims);
		break;
	    case aitEnumInt32: 
		retData = new Int32Array((const int *)inGdd.dataPointer(), numDims, dims);
		break;
	    case aitEnumUint32: 
		retData = new Uint32Array((const unsigned int *)inGdd.dataPointer(), numDims, dims);
		break;
	    case aitEnumFloat32: 
		retData = new Float32Array((float *)inGdd.dataPointer(), numDims, dims);
		break;
	    case aitEnumFloat64: 
		retData = new Float64Array((double *)inGdd.dataPointer(), numDims, dims);
		break;
	    default: 
		std::cout << "Fixed String array not supported in put\n"; 
		retData = NULL;
	}
	delete [] dims;	
	return retData;
    }
    else if(inGdd.isScalar())
    {
	aitInt8 int8;
	aitUint8 uint8;
	aitInt16 int16;
	aitUint16 uint16;
	aitInt32 int32;
	aitUint32 uint32;
	aitFloat32 float32;
	aitFloat32 float64;
	aitString str;

	switch(inGdd.primitiveType()) {
		case aitEnumInt8: 
		    inGdd.get(int8);
		    return new Int8(int8);
		case aitEnumUint8: 
		    inGdd.get(uint8);
		    return new Uint8(uint8);
		case aitEnumInt16: 
		    inGdd.get(int16);
		    return new Int16(int16);
		case aitEnumUint16: 
		    inGdd.get(uint16);
		    return new Uint16(uint16);
		case aitEnumInt32: 
		    inGdd.get(int32);
		    return new Int32(int32);
		case aitEnumUint32: 
		    inGdd.get(uint32);
		    return new Uint32(uint32);
		case aitEnumFloat32: 
		    inGdd.get(float32);
		    return new Float32(float32);
		case aitEnumFloat64: 
		    inGdd.get(float64);
		    return new Float64(float64);
		case aitEnumString: 
		    inGdd.get(str);
		    return new String(str.string());	
		default: std::cout << "Unexpected type for scalar in put\n"; return NULL;
	}
    }
    else
    {
	std::cout << "Only Scalars and Arrays can be put\n";
	return NULL;
    } 	

}



caStatus mdsPV::updateValue(const gdd & valueIn)
{
    Data *dataIn = dataFromGdd(valueIn);
    if(!dataIn)
    {
	return S_cas_noConvert;
    }
    try {
    	if(append && !isText)
	{
	    epicsTimeStamp currTime;
	    epicsTimeGetCurrent(&currTime);
	    unsigned long timestamp = ((unsigned long)currTime.secPastEpoch)*1000000000 + currTime.nsec;
	    valNode->putRow(dataIn, (int64_t *)&timestamp, 10); 
	}
	else
	    valNode->putData(dataIn);
    }catch(const MdsException &exc)
    {
	std::cout << "Error writing data: " << exc.what() << "\n";
	deleteData(dataIn);
	return S_casApp_undefined;
    } 
    deleteData(dataIn);
    currValue = &valueIn;
    currValue->reference();
    return S_casApp_success;
}


// Called when no more clients are monitoring this PV

void mdsPV::interestDelete()
{
    this->interest = false;
}


// Called by CAS when a client writes to a PV

caStatus mdsPV::write(const casCtx &, const gdd & valueIn)
{
    int status = updateValue(valueIn);
    if(status != S_casApp_success) return status;

    gdd *currVal = new gdd(valueIn);

    bool alarm = false;
    if(valueIn.isScalar())
    {
	aitFloat64 data;
	currVal->getConvert(data);
	if(hasAlarm && !isText)
	{
	    if(data >= highAlarm)
	    {
		currVal->setStat(epicsAlarmHiHi);
		currVal->setSevr(epicsSevMajor);
		alarm = true;
	    }
	    if(data <= lowAlarm)
	    {
		currVal->setStat(epicsAlarmLoLo);
		currVal->setSevr(epicsSevMajor);
		alarm = true;
	    }
	}
	if(!alarm && hasWarning && !isText)
	{
	    if(data >= highWarning)
	    {
		currVal->setStat(epicsAlarmHigh);
		currVal->setSevr(epicsSevMinor);
	    }
	    if(data <= lowWarning)
	    {
		currVal->setStat(epicsAlarmLow);
		currVal->setSevr(epicsSevMinor);
	    }
	}
    }
    // post a value change event
    caServer * pCAS = this->getCAS();
    if(this->interest == true && pCAS != NULL) {
        casEventMask select(pCAS->valueEventMask() | pCAS->logEventMask());
        this->postEvent(select, *currVal);
    }

    return S_casApp_success;
}
 

// Called by CAS when a client reads the PV value

caStatus mdsPV::read(const casCtx &, gdd & protoIn)
{
    return this->ft.read(*this, protoIn);
}


// Here access control can be implemented

casChannel *mdsPV::createChannel(const casCtx &ctx, const char * const /* pUserName */, const char * const /* pHostName */)
{
    return new mdsChannel(ctx);
}


// Returns the natural type for the value of the PV

aitEnum mdsPV::bestExternalType() const
{
    if(isText)
	return aitEnumString;
    else
	return aitEnumFloat64;
}


const char * mdsPV::getName () const
{
    return name;
}



void mdsPV::initFT ()
{
    if ( mdsPV::hasBeenInitialized ) {
            return;
    }

    // time stamp, status, and severity are extracted from the
    // GDD associated with the "value" application type.


    mdsPV::ft.installReadFunc ("value", &mdsPV::getValue);
    mdsPV::ft.installReadFunc ("precision", &mdsPV::getPrecision);
    mdsPV::ft.installReadFunc ("graphicHigh", &mdsPV::getHighGraphic);
    mdsPV::ft.installReadFunc ("graphicLow", &mdsPV::getLowGraphic);
    mdsPV::ft.installReadFunc ("controlHigh", &mdsPV::getHighCtrl);
    mdsPV::ft.installReadFunc ("controlLow", &mdsPV::getLowCtrl);
    mdsPV::ft.installReadFunc ("alarmHigh", &mdsPV::getHighAlarm);
    mdsPV::ft.installReadFunc ("alarmLow", &mdsPV::getLowAlarm);
    mdsPV::ft.installReadFunc ("alarmHighWarning", &mdsPV::getHighWarning);
    mdsPV::ft.installReadFunc ("alarmLowWarning", &mdsPV::getLowWarning);
    mdsPV::ft.installReadFunc ("units", &mdsPV::getUnits);
    mdsPV::ft.installReadFunc ("enums", &mdsPV::getEnums);


    mdsPV::hasBeenInitialized = 1;
}


caStatus mdsPV::getPrecision ( gdd & inPrec )
{
    if(precision == 0)
    	inPrec.put(4u);
    else
	inPrec.put(precision);
    return S_cas_success;
}


caStatus mdsPV::getHighAlarm ( gdd & value )
{
    if(hasAlarm)
    {
	value.put(highAlarm);
    	return S_cas_success;
    }
    value.put(defHighAlarm);
    return S_cas_success;
//    return S_cas_noConvert;
}


caStatus mdsPV::getLowAlarm ( gdd & value )
{
    if(hasAlarm)
    {
	value.put(lowAlarm);
    	return S_cas_success;
    }
    value.put(defLowAlarm);
    return S_cas_success;
//    return S_cas_noConvert;
}

caStatus mdsPV::getHighWarning ( gdd & value )
{
    if(hasWarning)
    {
	value.put(highWarning);
    	return S_cas_success;
    }
    value.put(defHighWarning);
    return S_cas_success;
//    return S_cas_noConvert;
}


caStatus mdsPV::getLowWarning ( gdd & value )
{
    if(hasAlarm)
    {
	value.put(lowWarning);
    	return S_cas_success;
    }
    value.put(defLowWarning);
    return S_cas_success;
//    return S_cas_noConvert;
}

caStatus mdsPV::getHighCtrl ( gdd & value )
{
    if(hasCtrl)
    {
	value.put(highCtrl);
    	return S_cas_success;
    }
    value.put(defHighCtrl);
    return S_cas_success;
//    return S_cas_noConvert;
}


caStatus mdsPV::getLowCtrl ( gdd & value )
{
    if(hasCtrl)
    {
	value.put(lowCtrl);
    	return S_cas_success;
    }
    value.put(defLowCtrl);
    return S_cas_success;
//    return S_cas_noConvert;
}

caStatus mdsPV::getHighGraphic ( gdd & value )
{
    if(hasGraphic)
    {
	value.put(highGraphic);
    	return S_cas_success;
    }
    value.put(defHighGraphic);
    return S_cas_success;
//    return S_cas_noConvert;
}


caStatus mdsPV::getLowGraphic ( gdd & value )
{
    if(hasGraphic)
    {
	value.put(lowGraphic);
    	return S_cas_success;
    }
    value.put(defLowGraphic);
    return S_cas_success;
//    return S_cas_noConvert;
}


caStatus mdsPV::getUnits( gdd & inUnits )
{
    if(units != NULL)
    {
	aitString str(units, aitStrRefConstImortal);
        inUnits.put(str);
    }
    else
    {
    	aitString str("au", aitStrRefConstImortal);
    	inUnits.put(str);
    }
    return S_cas_success;
}


// returns the eneumerated state strings for a discrete channel(not really relevant here)

caStatus mdsPV::getEnums ( gdd & enumsIn )
{
    if(numEnums > 0)
    {
        aitFixedString *str;
        mdsFixedStringDestructor *pDes;

        str = new aitFixedString[numEnums];
        if (!str) {
            return S_casApp_noMemory;
        }

        pDes = new mdsFixedStringDestructor;
        if (!pDes) {
            delete [] str;
            return S_casApp_noMemory;
        }
	for(int i = 0; i < numEnums; i++)
	    strncpy (str[i].fixed_string, enums[i], sizeof(str[i].fixed_string));
        enumsIn.setDimension(1);
        enumsIn.setBound (0,0,numEnums);
        enumsIn.putRef (str, pDes);
        return S_cas_success;
    }

    return S_cas_success;

}

 
// Called when reading from the GDD

caStatus mdsPV::getValue(gdd & value)
{
    Data *data = getData();
    if(data == NULL)
	return S_casApp_undefined;
    setGddFromData(data, value);
    deleteData(data);
    return S_cas_success;
}


// Returns the number of dimensions for the data handled by this PV

unsigned mdsPV::maxDimension() const
{
    return nDims;
}


// Returns the number of elements for a given dimension of the data

aitIndex mdsPV::maxBound(unsigned dimension) const 
{
    if(dimension >= 0 && dimension < nDims)
	return dims[dimension];
    return 0;
}

Data *mdsPV::getData()
{
    char clazz, dtype, nDims;
    short length;
    int *dims;
    void *ptr;
    if(append && !isText) //if segmented
    {
        try {
	    Array *arr = valNode->getSegment(-1);
	    arr->getInfo(&clazz, &dtype, &length, &nDims, &dims, &ptr);
	    int lastIdx = dims[0] - 1;
	    Data *retData = arr->getElementAt(&lastIdx, 1);
	    deleteData(arr);
	    delete [] dims;
	    return retData;
	}catch(const MdsException &exc)
	{
	    return NULL;
	    delete exc;
	}
    }
    else //not segmented
    {
	try {
	    return valNode->getData();
        }catch(const MdsException &exc)
	{
	    return NULL;
	    delete exc;
	}
    }
}





