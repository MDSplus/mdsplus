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
#ifndef WIN32
#define DYNreadline "libreadline.so"
#define DYNcurses   "libcurses.so"
#define DYNTdiShr "libTdiShr.so"
#endif
#include <string.h>
#include <stdio.h>
#include <mdsdescrip.h>
#include <mds_stdarg.h>

#define MAXEXPR 16384

#include <stdlib.h>
#include <sys/stat.h>

#ifdef HAVE_READLINE_READLINE_H
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include <dlfcn.h>
#ifdef DYNTdiShr
static void *TDIhandle = NULL;
int (*BTdiExecute) () = NULL;
#else
extern int TdiExecute();
#define BTdiExecute TdiExecute
#endif

#ifdef DYNreadline
static void *READhandle = NULL;
static void *CURSEShandle = NULL;
#include <dlfcn.h>
#ifndef PARAMS
#define PARAMS(gub) gub
#endif
void (*Busing_history) PARAMS((void)) = NULL;
int (*Bread_history) PARAMS((const char *)) = NULL;
int (*Bwrite_history) PARAMS((const char *)) = NULL;
void (*Badd_history) PARAMS((const char *)) = NULL;
int (*Bhistory_truncate_file) PARAMS((const char *, int)) = NULL;
char *(*Breadline) PARAMS((const char *)) = NULL;;
#else
#define Busing_history using_history
#define Bread_history read_history
#define Bwrite_history write_history
#define Badd_history add_history
#define Bhistory_truncate_file history_truncate_file
#define Breadline readline
#endif

#define HFILE "/.tdic"

char *bfgets(char *s, int size, FILE * stream, char *prompt);	/* fgets replacement */
#define PROMPT "TDI> "
#define MDSPATH "MDS_PATH"
/*
static void tdiputs(char *line);
*/
/* Lookup up entry point in shareable */
void *dlsymget(void *handle, char *sym)
{
  char *error;
  void *ret = NULL;
  ret = dlsym(handle, sym);
  dlerror();			/* Clear any existing error */
  if ((error = dlerror()) != NULL) {
    fprintf(stderr, "%s\n", error);
    exit(1);
  }
  return (ret);
}

int main(int argc, char **argv)
{
  FILE *in = stdin;
  int status;
  char temp[MAXEXPR];		/* temp strings for command */

/*
  static char expr[MAXEXPR] = "WRITE(_OUTPUT_UNIT,`DECOMPILE(`";
*/
  static char expr[MAXEXPR] = "";
  static struct descriptor expr_dsc = { 0, DTYPE_T, CLASS_S, (char *)expr };
  static EMPTYXD(ans);
  static EMPTYXD(output_unit);
  static DESCRIPTOR(out_unit_stdout, "PUBLIC _OUTPUT_UNIT=*");
  static DESCRIPTOR(out_unit_other, "PUBLIC _OUTPUT_UNIT=FOPEN($,'w')");
  static DESCRIPTOR(reset_output_unit, "PUBLIC _OUTPUT_UNIT=$");
  int prefixlen = strlen(expr);
  char line_in[MAXEXPR];
  char last_line[MAXEXPR];
#ifdef _WIN32
  char *hfile = "%HOMEDRIVE%%HOMEPATH%\\AppData\\Local\\tdic";
#else
  char hfile[32];
  strcpy(hfile, getenv("HOME"));
  strcat(hfile, HFILE);
#endif
#ifdef DYNTdiShr
  if (TDIhandle == NULL) {
    TDIhandle = dlopen(DYNTdiShr, RTLD_LAZY);
    if (!TDIhandle) {
      fprintf(stderr, "%s\n", dlerror());
      exit(1);
    }
    *(void **)(&BTdiExecute) = dlsymget(TDIhandle, "TdiExecute");
  }
#endif

#ifdef DYNreadline
  if (READhandle == NULL) {
    CURSEShandle = dlopen(DYNcurses, RTLD_NOW | RTLD_GLOBAL);
    READhandle = dlopen(DYNreadline, RTLD_LAZY);
//      READhandle = dlopen(DYNreadline, RTLD_NOW);
//      READhandle = dlopen(DYNreadline, RTLD_NOW | RTLD_GLOBAL);
    if (!READhandle) {
      fprintf(stderr, "%s\n", dlerror());
      exit(1);
    }
    *(void **)(&Busing_history) = dlsymget(READhandle, "using_history");
    *(void **)(&Bread_history) = dlsymget(READhandle, "read_history");
    *(void **)(&Bwrite_history) = dlsymget(READhandle, "write_history");
    *(void **)(&Badd_history) = dlsymget(READhandle, "add_history");
    *(void **)(&Bhistory_truncate_file) = dlsymget(READhandle, "history_truncate_file");
    *(void **)(&Breadline) = dlsymget(READhandle, "readline");
  }
#endif
  Busing_history();
  *last_line = 0;
/* get input from file */
  if (argc > 1) {
    in = fopen(argv[1], "r");
    if (in == (FILE *) 0) {
      printf("Error opening input file /%s/\n", argv[1]);
      return (0);
    }
  }
/* if specified, output to a file, defaults to stdout */
  if (argc > 2) {
    struct descriptor out_d = { 0, DTYPE_T, CLASS_S, 0 };
    out_d.length = (unsigned short)strlen(argv[2]);
    out_d.pointer = argv[2];
    BTdiExecute(&out_unit_other, &out_d, &output_unit MDS_END_ARG);
  } else {
    BTdiExecute(&out_unit_stdout, &output_unit MDS_END_ARG);
  }
/* get history loaded */
  Bread_history(hfile);
/* main loop to get characters */
  while (bfgets(line_in, MAXEXPR, in, PROMPT) != NULL) {
    int comment = line_in[0] == '%';	/* is a comment */
    int len = strlen(line_in);	/* get first line of command */
    if (!comment) {
      BTdiExecute(&reset_output_unit, &output_unit, &ans MDS_END_ARG);
/*	 tdiputs(line_in);*/
    }
/* if a continuation, keep getting the rest of the command */
    while (line_in[len - 1] == '\\') {
      bfgets(&line_in[len - 1], MAXEXPR - len + 1, in, "> ");
/*	 if (!comment) tdiputs(&line_in[len-1]);*/
      len = strlen(line_in);
    }
    if (!comment) {
      static DESCRIPTOR(error_out, "_MSG=DEBUG(0),DEBUG(4),WRITE($,_MSG)");
      static DESCRIPTOR(clear_errors, "WRITE($,DECOMPILE($)),DEBUG(4)");
#ifdef OLDHIST
      if (!strcmp(line_in, last_line) || (*line_in == ' ')) {
/*		printf("Removing [%d]\n",Bwhere_history());*/
	HIST_ENTRY *entry = Bremove_history(Bwhere_history());
	if (entry) {
	  free(entry->line);
	  free(entry);
	}
      } else
	strcpy(last_line, line_in);
#else
      if (*line_in && (*line_in != ' ') && strcmp(line_in, last_line)) {
	Badd_history(line_in);
	strcpy(last_line, line_in);
      }
#endif
/* Check the special TDIC options */
      if (!strcmp(line_in, "exit") || !strcmp(line_in, "quit")) {
	Bwrite_history(hfile);
	if (getenv("HISTFILESIZE")) {
	  int i;
	  sscanf(getenv("HISTFILESIZE"), "%d", &i);
	  Bhistory_truncate_file(hfile, i);
	}
	exit(1);
      } else if (*line_in == '!') {
	*line_in = ' ';		/* replace by a space */
	system(line_in);
      } else if (!strcmp(line_in, "help") || !strcmp(line_in, "man")) {
	printf("TDI(C) reference (October 1999)\n");
	printf("      help func <help function>\n");
	printf("      help func* <lists 1st line of each help>\n");
	printf("      use func* <lists lines in comment starting with [call:]>\n");
	printf("      type func(*) <type/p all the functions requested>\n");
	printf("      tcl <TCL command>\n      open <shot#>\n");
	printf("      !<shell command>\n      man [TDI intrinsic]\n");
	printf("      exit (guess)\n      ; behaves as in MatLab\n");
      } else if (!strncmp(line_in, "type ", 5)) {
	char *nameStart, *nameEnd, *mdspath, *tpath;
	struct stat statf;
	strcpy(temp, line_in + 5);	/* copy the target */
/* Check the path name for finding file */
	mdspath = getenv(MDSPATH);	/* get pointer */
	if (mdspath == NULL) {
	  printf("Path [%s] not defined !\n", MDSPATH);
	  return (0);
	}
/* Strip the name down before appending it to command */
	nameStart = temp;
	while (*nameStart == ' ')
	  nameStart++;
/* Find end of name */
	if ((nameEnd = strchr(nameStart, '.')) == NULL)
	  nameEnd = nameStart + strlen(nameStart);
	else
	  nameEnd--;
	while (*nameEnd == ' ' && nameEnd >= nameStart)
	  nameEnd--;
/* Zero end of string */
	*++nameEnd = '\0';
/* If there is anything left, scan the path */
	if (strlen(nameStart))
	  do {
	    strcpy(line_in, "more ");
	    if ((tpath = strchr(mdspath, ';')) == NULL) {
	      strcat(line_in, mdspath);
	    } else {
	      strncat(line_in, mdspath, tpath - mdspath);
	      mdspath = tpath + 1;	/* put to next char */
	    }
	    strcat(line_in, "/");
	    strcat(line_in, nameStart);	/* put back the target */
	    strcat(line_in, ".fun");
	    if (!stat(line_in + 5, &statf)) {
	      printf("Found <%s>\n", line_in + 5);
	      system(line_in);
	      tpath = NULL;
	    }
	  } while (tpath != NULL);
      } else if (!strncmp(line_in, "man ", 4)) {
	strcpy(temp, line_in + 4);	/* copy the target */
	strcpy(line_in, "man ");
	strcat(line_in, temp);	/* put back the target */
	system(line_in);
      } else if (!strncmp(line_in, "tcl", 3)) {
	strcpy(line_in, "$MDSPLUS/bin/tcl");	/* move the text to the left */
	system(line_in);
      }
#ifdef NEVER
      else if (!strncmp(line_in, "tcl", 3)) {
	strcpy(line_in, line_in + 4);	/* move the text to the left */
	mdsdcl$do(&Dtcl, &Dcommand);
      } else if (!strncmp(line_in, "open", 4)) {
	strcpy(temp, line_in + 5);	/* copy shot number into temp */
	strcpy(line_in, "set tree tcv_shot/shot=");
	strcat(line_in, temp);	/* concantenate the number */
	mdsdcl$do(&Dtcl, &Dcommand);
      }
#endif
      else {
#ifdef NEVER
	else
      if (!strncmp(line_in, "use ", 4)) {
	strcpy(temp, line_in + 4);	/* copy the target */
	strcpy(line_in, "help('");
	strcat(line_in, temp);
	strcat(line_in, "',,1)");
      } else if (line_in[length] != ';') {
	strcpy(temp, line_in);	/* copy string */
	strcpy(line_in, "write(*,");
	strcat(line_in, temp);
	strcat(line_in, ")");
      }
#endif
      if (!strncmp(line_in, "help ", 5)) {
	strcpy(temp, line_in + 5);	/* copy the target */
	strcpy(line_in, "help('");
	strcat(line_in, temp);
	strcat(line_in, "');");
      } else if (!strncmp(line_in, "open", 4)) {
	strcpy(temp, line_in + 5);	/* copy shot number into temp */
	strcpy(line_in, "TreeOpen('tcv_shot',");
	strcat(line_in, temp);	/* concantenate the number */
	strcat(line_in, ")");	/* concantenate the number */
      }
	len = strlen(line_in);
      expr[prefixlen] = 0;
      strcat(expr, line_in);
      if (!(comment = line_in[len - 1] == ';'))
          strcat(line_in, " = ");
	expr_dsc.length = strlen(expr);
/*
      expr_dsc.length = strlen(expr)-1;    
      expr_dsc.length = strlen(expr)-1;    
      expr[expr_dsc.length++] = ')';
      expr[expr_dsc.length++] = ')';
      expr[expr_dsc.length++] = ')';
      expr[expr_dsc.length++] = ')';
*/
	status = BTdiExecute(&expr_dsc, &ans MDS_END_ARG);
	if (status & 1) {
	  if (!comment)
	    BTdiExecute(&clear_errors, &output_unit, &ans, &ans MDS_END_ARG);
	} else
	  BTdiExecute(&error_out, &output_unit, &ans MDS_END_ARG);
      }
    }
  }
  return (1);
}
/*
static void tdiputs(char *line)
{
  static EMPTYXD(ans);
  static DESCRIPTOR(write_it, "WRITE(_OUTPUT_UNIT,$)");
  struct descriptor line_d = { 0, DTYPE_T, CLASS_S, 0 };
  line_d.length = (unsigned short)strlen(line);
  line_d.pointer = line;
  if (line[line_d.length - 1] == '\n')
    line_d.length--;
  BTdiExecute(&write_it, &line_d, &ans MDS_END_ARG);
}
*/
#ifndef HAVE_READLINE_READLINE_H
/* Routine to replace fgets using readline on stdin */
static char *Breadline(char *prompt)
{
  if (prompt)
    printf("%s ", prompt);
  return fgets(malloc(1024), 1023, stdin);
}

static void Badd_history(char *string)
{
  return;
}
#endif
char *bfgets(char *s, int size, FILE * stream, char *prompt)
{
  char *rep;
/* if not stdin, read from file */
  if (stream == stdin) {
    rep = Breadline(prompt);
    strncpy(s, rep ? rep : "", size - 1);	/* Copy respecting the size limit */
#ifdef OLDHIST
    if (rep && *rep)		/* Add to history if interesting */
      Badd_history(rep);
#endif
    if (rep)
      free(rep);
    s[size - 1] = '\0';		/* null terminate if necessary */
    return (s);
  } else
    return (fgets(s, size, stream));
}
