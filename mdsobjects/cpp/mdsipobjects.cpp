#include <stdio.h>
#include "mdsobjects.h"
using namespace MDSplus;

extern "C"  void *getManyObj(char *serializedIn);
extern "C"  void *putManyObj(char *serializedIn);
extern "C" void *compileFromExprWithArgs(char *expr, int nArgs, void *args, void *tree);

void *getManyObj(char *serializedIn)
{
	List *inArgs = (List *)deserialize(serializedIn);
	if(inArgs->clazz != CLASS_APD)// || inArgs->dtype != DTYPE_LIST)
	{
		printf("INTERNAL ERROR: Get Multi did not receive a LIST argument\n");
		return 0;
	}

	int nArgs = inArgs->len();
	String *nameKey = new String("name");
	String *exprKey = new String("exp");
	String *argsKey = new String("args");
	String *valueKey = new String("value");
	String *errorKey = new String("error");
	Dictionary *result = new Dictionary();
	for(int idx = 0; idx < nArgs; idx++)
	{
		Dictionary *currArg = (Dictionary* )inArgs->getElementAt(idx);
		if(currArg->clazz != CLASS_APD)// || currArg->dtype != DTYPE_DICTIONARY)
		{
			printf("INTERNAL ERROR: Get Multi Argument is not a DICTIONARY argument\n");
			return 0;
		}
		String *nameData = (String *)currArg->getItem(nameKey);
		String *exprData = (String *)currArg->getItem(exprKey);
		char *expr = exprData->getString();
		List *argsData = (List *)currArg->getItem(argsKey);
		
		Dictionary *answDict = new Dictionary();
		Data *currAnsw;
		try {
			if(argsData && argsData->len() > 0)
				currAnsw = executeWithArgs(expr, argsData->getDscs(), argsData->len());
			else
				currAnsw = execute(expr, (Data *)NULL);

			answDict->setItem(valueKey, currAnsw);
		}catch(MdsException *exc)
		{
			String *errorData = new String((char *)exc->what());
			answDict->setItem(errorKey, errorData);
		}
		result->setItem(nameData, answDict);
		deleteData(exprData);
		deleteData(nameData);
		delete [] expr;
		deleteData(argsData);
		deleteData(currArg);
	}
	void *resDsc = result->convertToDsc();
	deleteData(nameKey);
	deleteData(exprKey);
	deleteData(argsKey);
	//deleteData(valueKey);
	deleteData(errorKey);
	deleteData(result);
	deleteData(inArgs);
	return resDsc;
}

void *putManyObj(char *serializedIn)
{
	List *inArgs = (List *)deserialize(serializedIn);
	if(inArgs->clazz != CLASS_APD)// || inArgs->dtype != DTYPE_LIST)
	{
		printf("INTERNAL ERROR: Get Multi did not receive a LIST argument\n");
		return 0;
	}

	int nArgs = inArgs->len();
	String *nodeKey = new String("node");
	String *exprKey = new String("exp");
	String *argsKey = new String("args");
	Dictionary *result = new Dictionary();
	for(int idx = 0; idx < nArgs; idx++)
	{
		Dictionary *currArg = (Dictionary* )inArgs->getElementAt(idx);
		if(currArg->clazz != CLASS_APD)// || currArg->dtype != DTYPE_DICTIONARY)
		{
			printf("INTERNAL ERROR: Get Multi Argument is not a DICTIONARY argument\n");
			return 0;
		}
		String *nodeNameData = (String *)currArg->getItem(nodeKey);
		String *exprData = (String *)currArg->getItem(exprKey);
		char *expr = exprData->getString();
		List *argsData = (List *)currArg->getItem(argsKey);
		int nPutArgs = 0;
		if(argsData)
			nPutArgs = argsData->len();

		try {
			Tree *tree = getActiveTree();
			Data *compiledData = (Data *)compileFromExprWithArgs(expr, nPutArgs, (argsData)?argsData->getDscs():0, tree);
			TreeNode *node = tree->getNode(nodeNameData);
			node->putData(compiledData);
			delete node;
			deleteData(compiledData);
			String *successData = new String("Success");
			result->setItem(nodeNameData, successData);
		}catch(MdsException *exc)
		{
			String *errorData = new String((char *)exc->what());
			result->setItem(nodeNameData, errorData);
		}
		deleteData(exprData);
		deleteData(argsData);
		deleteData(currArg);
	}
	void *resDsc = result->convertToDsc();
	deleteData(nodeKey);
	deleteData(exprKey);
	deleteData(argsKey);
	deleteData(inArgs);
	return resDsc;
}
