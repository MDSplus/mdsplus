#include <config.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <strings.h>
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

typedef enum { LOOP, TTL, MULTICAST_IF, PORT, ADDRESS } setting_types;
#define NUM_SETTINGS 5
static const char *settings[NUM_SETTINGS] = {0,0,0,0,0};
static const char *environ_var[NUM_SETTINGS] = {"mdsevent_loop", "mdsevent_ttl", "mdsevent_interface", "mdsevent_port", "mdsevent_address"};
static const char *xml_setting[NUM_SETTINGS] = {"IP_MULTICAST_LOOP", "IP_MULTICAST_TTL", "IP_MULTICAST_IF", "PORT", "ADDRESS"};
static const char *fname = "eventsConfig.xml";

int UdpEventGetLoop(unsigned char *loop) {
  int status = 0;
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
  return status;
}

EXPORT int UdpEventGetTtl(unsigned char *ttl) {
  int status = 0;
  if (settings[TTL]) {
    char *endptr;
    long int lttl = strtol(settings[TTL],&endptr,0);
    if (*endptr == '\0' && lttl >= 0) {
      *ttl = (unsigned char)lttl;
      status = 1;
    } else
      fprintf(stderr, "Invalid udp_multicast_ttl value specified. Value must be an integer >= 0.\n");
  }
  return status;
}

EXPORT int UdpEventGetPort(unsigned short *port) {
  int status = 0;
  if (settings[PORT]) {
    struct servent *srv = getservbyname(settings[PORT], "UDP");
    if (srv) {
      *port = srv->s_port;
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
    *port = srv ? srv->s_port : 4000;
    status = 1;
  }
  return status;
}

EXPORT int UdpEventGetInterface(struct in_addr **interface_addr) {
  int status = 0;
#ifndef _WIN32
  if (settings[MULTICAST_IF]) {
    struct ifaddrs *ifaddr=0, *ifa=0;
    if (getifaddrs(&ifaddr) == 0) {
      for (ifa=ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
	if (ifa->ifa_addr == NULL)
	  continue;
	if ((strcasecmp(ifa->ifa_name,settings[MULTICAST_IF])==0) &&
	    ((ifa->ifa_addr->sa_family == AF_INET) ||
	     (ifa->ifa_addr->sa_family == AF_INET6))) {
	  struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
	  *interface_addr = memcpy(malloc(sizeof(addr->sin_addr)),&addr->sin_addr,sizeof(addr->sin_addr));
	  status = 1;
	  break;
	}
      }
      freeifaddrs(ifaddr);
    }
  }
#endif
  return status;
}

EXPORT int UdpEventGetAddress(char **address, unsigned char *arange) {
  unsigned int num, p1 = 224, p2 = 0, p3 = 0, p4 = 175, p5 = 255;
  *address = (char *)malloc(50);
  arange[0]=0;
  arange[1]=255;
  if (settings[ADDRESS]) {
    if (strcasecmp(settings[ADDRESS], "compat") == 0) {
      strcpy(*address, "225.0.0.%d");
      arange[0]=0;
      arange[1]=255;
    } else if (((num = sscanf(settings[ADDRESS], "%d.%d.%d.%d-%d", &p1, &p2, &p3, &p4, &p5)) > 3)
	       && (p1 < 256) && (p2 < 256) && (p3 < 256) && (p4 < 256) && (p5 >= p4) && (p5 < 256)){
      sprintf(*address, "%d.%d.%d.%%d", p1, p2, p3);
      arange[0] = (unsigned char)p4;
      arange[1] = num < 5 ? (unsigned char)p4 : (unsigned char)p5;
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
    

void InitializeEventSettings()
{
  int i, missing=0;
  xmlInitParser();
  for (i=0;i<NUM_SETTINGS;i++) {
    settings[i]=getenv(environ_var[i]);
    if (settings[i]==NULL)
      missing=1;
  }

  if (missing) {
    /* If not all setting already set look for settings in HOME/.mdsplus/eventsConfig.xml
       then in $MDSPLUS_DIR/local/eventsConfig.xml */
    char *home_dir;

#ifdef __WIN32
    home_dir = getenv("USERPROFILE");
#else
    home_dir = getenv("HOME");
#endif
    if (home_dir != NULL) {
      xmlDocPtr doc=NULL;
      static const char *home_xml_dir = "/.mdsplus/";
      char *xmlfname = malloc(strlen(home_dir) + strlen(home_xml_dir) + strlen(fname) + 10);
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
	  }
	}
	xmlFreeDoc(doc);
      }
    }
  }
  if (missing) {
    xmlDocPtr doc=NULL;
    char *mdsplus_dir = getenv("MDSPLUS_DIR");
    static const char *local_xml_dir = "/local/";
    if (mdsplus_dir != NULL) {
      char *xmlfname = malloc(strlen(mdsplus_dir) + strlen(local_xml_dir) + strlen(fname) + 10);
      sprintf(xmlfname,"%s%s%s",mdsplus_dir,local_xml_dir,fname);
      if (access(xmlfname,R_OK) == 0)
	doc = xmlParseFile(xmlfname);
      free(xmlfname);
      if (doc) {
	for (i=0;i<NUM_SETTINGS;i++) {
	  if (settings[i] == NULL) {
	    settings[i] = getProperty(doc, "UdpEvents", xml_setting[i]);
	  }
	}
	xmlFreeDoc(doc);
      }
    }
  }
}
