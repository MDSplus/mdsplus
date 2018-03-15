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
#include <mdsplus/mdsconfig.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <strings.h>
#define LOAD_INITIALIZESOCKETS
#include "mdsshrthreadsafe.h"
#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

//#define DEBUG

typedef enum { LOOP, TTL, MULTICAST_IF, PORT, ADDRESS } setting_types;
#define NUM_SETTINGS 5
static const char *settings[NUM_SETTINGS] = {0,0,0,0,0};
static const char *environ_var[NUM_SETTINGS] = {"mdsevent_loop", "mdsevent_ttl", "mdsevent_interface", "mdsevent_port", "mdsevent_address"};
static const char *xml_setting[NUM_SETTINGS] = {"IP_MULTICAST_LOOP", "IP_MULTICAST_TTL", "IP_MULTICAST_IF", "PORT", "ADDRESS"};
static const char *fname = "eventsConfig.xml";

EXPORT void InitializeEventSettings();

pthread_mutex_t init_lock = PTHREAD_MUTEX_INITIALIZER;
static void initialize(){
  static pthread_once_t once = PTHREAD_ONCE_INIT;
  pthread_once(&once,InitializeEventSettings);
}

EXPORT int UdpEventGetLoop(unsigned char *loop) {
  int status = 0;
  initialize();
  pthread_mutex_lock(&init_lock);
  if (settings[LOOP]) {
    if (strcmp("0", settings[LOOP]) == 0) {
      *loop = 0;
      status = 1;
    } else if (strcmp("1", settings[LOOP]) == 0) {
      *loop = 1;
      status = 1;
    } else
      fprintf(stderr, "Invalid udp_multicast_loop value specified. Value must be 0 or 1.\n"
	      "Using system default\n");
  }
  pthread_mutex_unlock(&init_lock);
  return status;
}

EXPORT int UdpEventGetTtl(unsigned char *ttl) {
  int status = 0;
  initialize();
  pthread_mutex_lock(&init_lock);
  if (settings[TTL]) {
    char *endptr;
    long int lttl = strtol(settings[TTL],&endptr,0);
    if (*endptr == '\0' && lttl >= 0) {
      *ttl = (unsigned char)lttl;
      status = 1;
    } else
      fprintf(stderr, "Invalid udp_multicast_ttl value specified. Value must be an integer >= 0.\n");
  }
  pthread_mutex_unlock(&init_lock);
  return status;
}

EXPORT int UdpEventGetPort(unsigned short *port) {
  int status = 0;
  initialize();
  pthread_mutex_lock(&init_lock);
  if (settings[PORT]) {
    struct servent *srv = getservbyname(settings[PORT], "UDP");
    if (srv) {
      *port = (unsigned short)srv->s_port;
      status = 1;
    } else {
      char *endptr;
      long int lport = strtol(settings[PORT],&endptr,0);
      if (*endptr == '\0' && lport > 0) {
	*port = (unsigned short)lport;
	status = 1;
      } else {
	fprintf(stderr,"Invalid port specified. Specify known port name or integer > 0.\n");
      }
    }
  }
  if (status == 0) {
    struct servent *srv = getservbyname("mdsevent_port","udp");
    *port = srv ? (unsigned short)srv->s_port : 4000u;
    status = 1;
  }
  pthread_mutex_unlock(&init_lock);
  return status;
}
#ifdef DEBUG
inline static void print_addr(FILE * file, struct sockaddr_in *addr) {
  size_t i;
  uint8_t* c = (uint8_t*)&addr->sin_addr;
  fprintf(stderr,"%d",c[0]);
  for (i=1;i< sizeof(addr->sin_addr);i++)
    fprintf(file,".%u",c[i]);
}
#endif
EXPORT int UdpEventGetInterface(struct in_addr **interface_addr) {
  /*******************************************
  struct in_addr is ipv4 so dont look for ipv6
  ********************************************/
  int status = 0;
  initialize();
  pthread_mutex_lock(&init_lock);
  if (settings[MULTICAST_IF]) {
    int err;
#ifdef _WIN32
 #include <iphlpapi.h>
 #define GAA_FLAGS GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER
 #define HAS_NO_ADDRESS(ifa) (ifa->FirstUnicastAddress == NULL)
 #define CHECK_NAME(ifa)     (wcsicmp(ifa->FriendlyName,lookup_name)==0)
 #define freeifaddrs(ifaddr) free(ifaddr);free(lookup_name)
 #define ifa_name	FriendlyName
 #define ifa_addr	FirstUnicastAddress->Address.lpSockaddr
 #define ifa_next	Next
    /*convert interface name to wchar for later compare*/
    int len = strlen(settings[MULTICAST_IF]);
    PWCHAR lookup_name = malloc(len*sizeof(WCHAR));
    mbstowcs(lookup_name, settings[MULTICAST_IF], len);

    /*windows substitute for getifaddrs*/
    ULONG memlen = sizeof (IP_ADAPTER_ADDRESSES);
    IP_ADAPTER_ADDRESSES *ifa,*ifaddr = malloc(memlen);
    while ((err = GetAdaptersAddresses(AF_INET,GAA_FLAGS,NULL,ifaddr,&memlen))==ERROR_BUFFER_OVERFLOW) {
      memlen*=2;
      ifaddr = realloc(ifaddr,memlen);
    }
#else
 #define HAS_NO_ADDRESS(ifa)    (ifa->ifa_addr == NULL)
 #define CHECK_NAME(ifa)     (strcasecmp(ifa->ifa_name,settings[MULTICAST_IF])==0)
    struct ifaddrs *ifaddr=0, *ifa;
    err = getifaddrs(&ifaddr);
#endif
    if (!err) {
      for (ifa=ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
	if HAS_NO_ADDRESS(ifa)
	  continue;
	struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
	if (CHECK_NAME(ifa) && (addr->sin_family == AF_INET)) {
	    *interface_addr = memcpy(malloc(sizeof(addr->sin_addr)),&addr->sin_addr,sizeof(addr->sin_addr));
#ifdef DEBUG
	  fprintf(stderr,"using address: ");print_addr(stderr,addr);fprintf(stderr,"\n");
#endif
	  status = 1;
	  break;
	}
      }
      freeifaddrs(ifaddr);
    }
  }
  pthread_mutex_unlock(&init_lock);
  return status;
}

EXPORT int UdpEventGetAddress(char **address, unsigned char *arange) {
  int num;
  unsigned int p1 = 224, p2 = 0, p3 = 0, p4 = 175, p5 = 255;
  *address = (char *)malloc(50);
  arange[0]=0;
  arange[1]=255;
  initialize();
  pthread_mutex_lock(&init_lock);
  if (settings[ADDRESS]) {
    if (strcasecmp(settings[ADDRESS], "compat") == 0) {
      strcpy(*address, "225.0.0.%d");
      arange[0]=0;
      arange[1]=255;
    } else if (((num = sscanf(settings[ADDRESS], "%d.%d.%d.%d-%d", &p1, &p2, &p3, &p4, &p5)) > 3)
	       && (p1 < 256) && (p2 < 256) && (p3 < 256) && (p4 < 256) && (p5 >= p4) && (p5 < 256)){
      sprintf(*address, "%d.%d.%d.%%d", p1, p2, p3);
      arange[0] = (unsigned char)p4;
      arange[1] = (unsigned char)(num < 5 ? p4 : p5);
    } else {
      fprintf(stderr,
	      "Invalid address format specified. Specify either n.n.n.n or n.n.n.n-n format.\nDefaulting to 224.0.0.175");
      strcpy(*address, "224.0.0.%d");
      arange[0] =  arange[1] = 175;
    }
  } else {
    strcpy(*address, "224.0.0.%d");
    arange[0] = arange[1] = 175;
  }
  pthread_mutex_unlock(&init_lock);
  return 1;
}

static const char *getProperty(xmlDocPtr doc, const char *settings, const char *setting) {
  const char *ans=NULL;
  if (doc != NULL) {
    xmlNodePtr node;
    for (node = doc->children; node && (ans == NULL); node = node->next) {
      if (strcasecmp(settings, (const char *)node->name) == 0) {
	xmlAttrPtr pnode;
	for (pnode = node->properties; pnode && (ans == NULL); pnode = pnode->next) {
	  if (pnode->children && pnode->children->content && strcasecmp(setting, (const char *)pnode->name) == 0)
	    ans = strdup((const char *)pnode->children->content);
	}
      }
    }
  }
  return ans;
}

EXPORT void InitializeEventSettings()
{
  pthread_mutex_lock(&init_lock);
  pthread_cleanup_push((void*)pthread_mutex_unlock,&init_lock);
  INITIALIZESOCKETS;
  int i, missing=0;
  xmlInitParser();
  for (i=0;i<NUM_SETTINGS;i++) {
    settings[i]=getenv(environ_var[i]);
    if (settings[i]==NULL)
      missing=1;
  }
#ifdef DEBUG
  fprintf(stderr,"UdpEventSettings by\nenvironment:");
  for (i=0;i<NUM_SETTINGS;i++)
    if (settings[i])
      fprintf(stderr," %s=\"%s\"",environ_var[i],settings[i]);
  fprintf(stderr,"\n");
#endif
  if (missing) {
#ifdef DEBUG
  fprintf(stderr,"user xml:   ");
#endif
    /* If not all setting already set look for settings in HOME/.mdsplus/eventsConfig.xml
       then in $MDSPLUS_DIR/local/eventsConfig.xml */
    char *home_dir;

#ifdef __WIN32
    home_dir = getenv("USERPROFILE");
#else
    home_dir = getenv("HOME");
#endif
    if (home_dir) {
      xmlDocPtr doc=NULL;
      static const char *home_xml_dir = "/.mdsplus.conf/";
      char* xmlfname = malloc(strlen(home_dir) + strlen(home_xml_dir) + strlen(fname) + 1);
      if (xmlfname) {
	sprintf(xmlfname, "%s%s%s", home_dir, home_xml_dir, fname);
	if (access(xmlfname,R_OK) == 0)
	  doc = xmlParseFile(xmlfname);
	free(xmlfname);
	if (doc) {
	  missing = 0;
	  for (i=0;i<NUM_SETTINGS;i++) {
	    if (settings[i] == NULL) {
	      settings[i] = getProperty(doc, "UdpEvents", xml_setting[i]);
	      if (settings[i] == NULL)
	        missing = 1;
#ifdef DEBUG
	      else
		fprintf(stderr," %s=\"%s\"",environ_var[i],settings[i]);
#endif
	    }
	  }
	  xmlFreeDoc(doc);
	}
      }
    }
#ifdef DEBUG
    fprintf(stderr,"\n");
#endif
  }
  if (missing) {
#ifdef DEBUG
  fprintf(stderr,"local xml:  ");
#endif
    xmlDocPtr doc=NULL;
    char *mdsplus_dir = getenv("MDSPLUS_DIR");
    static const char *local_xml_dir = "/local/";
    if (mdsplus_dir) {
      char *xmlfname = malloc(strlen(mdsplus_dir) + strlen(local_xml_dir) + strlen(fname) + 1);
      if (xmlfname) {
	sprintf(xmlfname,"%s%s%s",mdsplus_dir,local_xml_dir,fname);
	if (access(xmlfname,R_OK) == 0)
	  doc = xmlParseFile(xmlfname);
	free(xmlfname);
	if (doc) {
	  for (i=0;i<NUM_SETTINGS;i++) {
	    if (settings[i] == NULL)
	      settings[i] = getProperty(doc, "UdpEvents", xml_setting[i]);
#ifdef DEBUG
	    else
	      fprintf(stderr," %s=\"%s\"",environ_var[i],settings[i]);
#endif
	  }
	  xmlFreeDoc(doc);
	}
      }
    }
#ifdef DEBUG
    fprintf(stderr,"\n");
#endif
  }
  pthread_cleanup_pop(1);
}
