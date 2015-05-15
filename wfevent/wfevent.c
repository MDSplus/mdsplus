#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <mds_stdarg.h>
#ifndef _WIN32
#include <getopt.h>
#endif

#define MAXDATA 4096

/*
Program to wait for MDSPlus event from the command line.
*/

static void printhelp(char *cmd)
{
#ifdef _WIN32
  printf("usage: %s  [/d] [/D] [/t:n] [/?] event-name\n", cmd);
  printf("\n  event-name is the event that you want to wait for."
	 "\n  /d indicates print event data."
	 "\n  /D indicates interpret data as serialized and print it."
	 "\n  /t:nnn or is used to specify a timeout in seconds.\n\n");
  printf("Note: Include spaces before and after the options.\n\n");
#else
  printf("usage: %s event-name [-d|--data] [-D|--serialized] [-t n|--timeout=n] [-h|--help]\n",
	 cmd);
  printf("\n  event-name is the event that you want to wait for."
	 "\n  -d or --data indicates print event data."
	 "\n  -D or --serialized indicates interpret data as serialized and print it."
	 "\n  -t or --timeout is used to specify a timeout in seconds.\n\n");
#endif
}

extern int TdiDecompile();

int main(int argc, char **argv)
{
  int len;
  char data[MAXDATA];
  int opt;
  int showdata = 0;
  int timeout = 0;
  char *event;
  int status;
  int serialized = 0;
#ifdef _WIN32
  int optind = argc;
  int i;
  for (i = 1; i < argc; i++) {
    if (argv[i][0] == '/') {
      switch (argv[i][1]) {
      case '?':
	printhelp(argv[0]);
	return 0;
	break;
      case 'd':
	showdata = 1;
	break;
      case 'D':
	showdata = 1;
	serialized = 1;
	break;
      case 't':
	if (strlen(argv[i]) > 3 && argv[i][2] == ':')
	  timeout = atoi(&argv[i][3]);
	break;
      default:
	printhelp(argv[0]);
	return 1;
	break;
      }
    } else {
      if (optind == argc)
	optind = i;
      else {
	printhelp(argv[0]);
	return 1;
      }
    }
  }
#else
  struct option longopts[] = { {"data", 0, 0, 'd'},
  {"timeout", 1, 0, 't'},
  {"help", 0, 0, 'h'},
  {"serialized", 0, 0, 'D'},
  {0, 0, 0, 0}
  };
  while ((opt = getopt_long(argc, argv, "dDht:", longopts, 0)) > -1) {
    switch (opt) {
    case 'h':
      printhelp(argv[0]);
      return 0;
    case 'd':
      showdata = 1;
      break;
    case 't':
      timeout = atoi(optarg);
      break;
    case 'D':
      showdata = 1;
      serialized = 1;
      break;
    default:
      printhelp(argv[0]);
      return 1;
    }
  }
#endif
  if (optind == argc) {
    printf("Missing event-name\n");
    printhelp(argv[0]);
    return 1;
  }
  event = argv[optind];
  status = MDSWfeventTimed(event, MAXDATA, data, &len, timeout);
  if (status & 1) {
    if (showdata && len) {
      int i, istext = 1;
      if (serialized) {
	EMPTYXD(xd);
	struct descriptor ans = { 0, DTYPE_T, CLASS_D, 0 };
	status = MdsSerializeDscIn(data, &xd);
	if (status & 1) {
	  TdiDecompile(&xd, &ans MDS_END_ARG);
	  if (ans.pointer) {
	    printf("Event %s occurred with data = %.*s\n", event, ans.length, ans.pointer);
	    return 0;
	  }
	} else {
	  printf("Event %s occurred with invalid serialized data\n", event);
	  return 1;
	}
      }
      if (len > MAXDATA)
	len = MAXDATA;
      for (; data[len - 1] == 0 && len > 0; len--) {;
      }
      for (i = 0; i < len; i++) {
	if (data[i] < 32 || data[i] >= 127) {
	  istext = 0;
	  break;
	}
      }
      if (istext)
	printf("Event %s occurred with data = \\%.*s\\\n", event, len, data);
      else {
	printf("Event %s occured with data = [", event);
	for (i = 0; i < len - 1; i++) {
	  printf("%0dB,", (int)data[i]);
	}
	printf("%dB]\n", (int)data[i]);
      }
    }
    return (0);
  } else if (timeout > 0) {
    printf("Event %s timed out.\n", event);
    return 1;
  } else {
    printf("Unknown error occurred\n");
    return 1;
  }
}
