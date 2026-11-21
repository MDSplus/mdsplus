/*
Copyright (c) 2026, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdlib.h>

#include <mdsdescrip.h>
#include <mdsobjects.h>

#include "testing.h"
#include "testutils/unique_ptr.h"

using namespace MDSplus;
using namespace testing;

// The mdsip server entry point for Connection.getMany() batches
// (tdi/remote/GetManyExecute.fun -> MdsObjectsCppShr->GetManyExecute)
extern "C" struct descriptor_xd *GetManyExecute(char *serializedIn);

////////////////////////////////////////////////////////////////////////////////
//  GetMany results that evaluate to missing (issue #3070)  ////////////////////
////////////////////////////////////////////////////////////////////////////////

int main()
{
  BEGIN_TESTING(GetMany);

  { // A Dictionary must accept a missing (NULL) value: getManyObj() stores
    // one whenever a query succeeds with a nil result. The rest of the Apd
    // machinery (constructor, propagateDeletion, convertToApdDsc) already
    // NULL-guards its elements.
    unique_ptr<Dictionary> dict = new Dictionary();
    String *key = new String("value"); // freed by the Dictionary destructor
    String lookup("value");

    dict->setItem(key, NULL);
    TEST1(dict->len() == 1);
    TEST1(dict->getItem(&lookup) == NULL);

    // replace the missing value with data...
    Int32 *num = new Int32(42);
    dict->setItem(key, num);
    unique_ptr<Data> value = dict->getItem(&lookup);
    TEST1(value->getInt() == 42);

    // ...and the data with a missing value again
    dict->setItem(key, NULL);
    TEST1(dict->len() == 1);
    TEST1(dict->getItem(&lookup) == NULL);
    deleteData(num);
  }

  { // A missing value must survive the serialize / deserialize round trip
    // used to transport the GetManyExecute() result dictionary.
    unique_ptr<Dictionary> dict = new Dictionary();
    dict->setItem(new String("value"), NULL);

    int size = 0;
    AutoArray<char> serialized(dict->serialize(&size));
    TEST1(size > 0);

    unique_ptr<Dictionary> back = (Dictionary *)deserialize(serialized.get());
    String lookup("value");
    TEST1(back->len() == 1);
    TEST1(back->getItem(&lookup) == NULL);
  }

  { // End to end: a GetMany query that SUCCEEDS with a nil result must come
    // back as { 'value': missing }, matching what a plain get() of the same
    // expression returns, instead of crashing the mdsip worker.
    // The expression mirrors GETNCI(<node>, "RECORD") of a clock whose
    // record is a Range with a missing begin: * : * : 5E-6
    Dictionary *nilQuery = new Dictionary(); // freed by the List destructor
    nilQuery->setItem(new String("name"), new String("nil"));
    nilQuery->setItem(new String("exp"),
                      new String("DATA(BEGIN_OF(BUILD_RANGE(*, *, 5E-6)))"));

    Dictionary *numQuery = new Dictionary();
    numQuery->setItem(new String("name"), new String("num"));
    numQuery->setItem(new String("exp"), new String("1 + 1"));

    unique_ptr<List> queries = new List();
    queries->append(nilQuery);
    queries->append(numQuery);

    int serSize = 0;
    AutoArray<char> serialized(queries->serialize(&serSize));

    struct descriptor_xd *outXd = GetManyExecute(serialized.get());
    TEST1(outXd != NULL && outXd->pointer != NULL);

    // GetManyExecute() returns the result dictionary serialized into a
    // byte array, just as it goes over the wire
    struct descriptor_a *bytes = (struct descriptor_a *)outXd->pointer;
    unique_ptr<Dictionary> result =
        (Dictionary *)deserialize((char *)bytes->pointer);

    String nilName("nil"), numName("num"), valueKey("value"), errorKey("error");

    { // sanity: the numeric query evaluated normally
      unique_ptr<Dictionary> answer = (Dictionary *)result->getItem(&numName);
      TEST1((Dictionary *)answer != NULL);
      unique_ptr<Data> value = answer->getItem(&valueKey);
      TEST1(value->getInt() == 2);
    }

    { // the nil query: answered, not an error, and the value is missing
      unique_ptr<Dictionary> answer = (Dictionary *)result->getItem(&nilName);
      TEST1((Dictionary *)answer != NULL);
      TEST1(answer->getItem(&errorKey) == NULL);
      TEST1(answer->len() == 1); // exactly one entry: 'value'
      TEST1(answer->getItem(&valueKey) == NULL); // ...holding missing
    }
  }

  END_TESTING;
}
