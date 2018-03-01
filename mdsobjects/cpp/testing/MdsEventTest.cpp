/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

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

#include <unistd.h>

#include <mdsobjects.h>
#include <mdsplus/Mutex.hpp>

#include "testing.h"
#include "testutils/testutils.h"
#include "testutils/unique_ptr.h"

using namespace MDSplus;
using namespace testing;
namespace mds = MDSplus;

static void* setevent(void* evname) {
  sleep(1);
  Event::setEvent((char*)evname);
  //std::cout << "Event set\n" << std::flush;
  pthread_exit(0);
  return NULL;
}

static void* seteventraw(void* args) {
  sleep(1);
  std::string* str = ((std::string**)args)[1];
  Event::setEventRaw(((char**)args)[0],str->size(),(char*)str->c_str());
  //std::cout << "EventRaw set\n" << std::flush;
  pthread_exit(0);
  return NULL;
}

static void* seteventdata(void* args) {
  sleep(1);
  Event::setEvent(((char**)args)[0],((Data**)args)[1]);
  //std::cout << "EventData set\n" << std::flush;
  pthread_exit(0);
  return NULL;
}

int main(int argc UNUSED_ARGUMENT, char *argv[] UNUSED_ARGUMENT)
{
    BEGIN_TESTING(Event);
    pthread_attr_t attr, *attrp;
    if (pthread_attr_init(&attr))
      attrp = NULL;
    else {
      attrp = &attr;
      pthread_attr_setstacksize(&attr, 0x100000);
    }
    try {
    static char evname[100] = "empty";
    if(strcmp(evname,"empty") == 0)
        sprintf(evname,"event_test_%d",getpid());

    { // NULL EVENT //
	pthread_t thread;
	if (pthread_create(&thread, attrp, setevent, (void*)evname))
	  throw std::runtime_error("ERROR: Could not create thread for setevent");
        Event ev(evname);
	std::cout << "Waiting for wait\n" << std::flush;
        ev.wait();
	std::cout << "Waiting for thread\n" << std::flush;
	pthread_join(thread,NULL);
    }

    { // RAW EVENT //
        static std::string str("test string to be compared");
	void* args[] = {evname,&str};
	pthread_t thread;
	if (pthread_create(&thread, attrp, seteventraw, (void*)args))
	  throw std::runtime_error("ERROR: Could not create thread for seteventraw");
	Event ev(evname);
	size_t buf_len = 0;
	std::cout << "Waiting for waitRaw\n" << std::flush;
	const char *buf = ev.waitRaw(&buf_len);
	std::cout << "Waiting for thread\n" << std::flush;
	pthread_join(thread,NULL);
	TEST1( std::string(str) == std::string(buf) );
    }

    { // DATA EVENT //
	static unique_ptr<String> str = new String("test string to be compared");
	void* args[] = {evname,str};
	pthread_t thread;
	if (pthread_create(&thread, attrp, seteventdata, (void*)args))
	  throw std::runtime_error("ERROR: Could not create thread for seteventdata");
	Event ev(evname);
	std::cout << "Waiting for waitData\n" << std::flush;
	unique_ptr<Data> data = ev.waitData();
	std::cout << "Waiting for thread\n" << std::flush;
	pthread_join(thread,NULL);
	TEST1( AutoString(data->getString()).string == AutoString(str->getString()).string );
    }
    } catch (...) {
      if (attrp) pthread_attr_destroy(attrp);
      throw;
    }
    if (attrp) pthread_attr_destroy(attrp);
    END_TESTING;
}
