#include        <stdio.h>
#include        <string.h>
#include		<stdlib.h>
#include		<ctype.h>
#include        "dasutil.h"

#if defined(vms)
#include        <unistd.h>
#include        <smg$routines.h>
#elif defined(_WIN32)
#include <io.h>
#define read _read
#else
#include        <malloc.h>
#ifdef CURSES
#include        <curses.h>				/*  */
#include        <term.h>				/*  */
#endif
#include        <sys/types.h>
#include        <termios.h>
#endif

/***********************************************************************
* FGETS_WITH_EDIT.C --
*
* Read a line of input.  If source is tty, provide command-line
* recall and editing.
* Calling arguments are similar to the standard fgets().
*
* Unix note:  requires "curses" library  ( -lcurses switch ).
*
* History:
*  05-Mar-1998  TRG  Create.
*
***********************************************************************/


#define DEL     0x7F
#define CTRL_D  0x04
#define CTRL_E  0x05
#define CTRL_H  0x08
#define CTRL_U  0x15
#define ESC     0x1B

#ifndef CURSES
#define KEY_DOWN  0402		/* from curses.h			*/
#define KEY_UP    0403
#define KEY_LEFT  0404
#define KEY_RIGHT 0405
static char  *key_up;
static char  *key_down;
static char  *key_right;
static char  *key_left;
static char  *save_cursor;
static char  *restore_cursor;
static char  *cursor_left;
static char  *cursor_right;
static char  *clr_eol;
#endif

static int   insert_mode = 1;	/* flag					*/
static int   icnt;		/* number of lines read			*/
static int   irecall;		/* number of "recall" line		*/
static char  *history[32];	/* history of recent command lines	*/
#define MAX_HISTORY    (sizeof(history)/sizeof(history[0]))

#define HISTORY_NOT_ACTIVE (-99)	/* special value for irecall	*/



#ifndef vms
	/****************************************************************
	 * decode_escape_sequence:
	 ****************************************************************/
static int   decode_escape_sequence(
    int   fd				/* <r> input file descr		*/
   )
   {
    int   i;
    char  c;
    char  escSequence[32];
    char  *p;
    static char  altCSI[3] = {ESC, '[', '\0' };

    p = key_left;

    i = 0;
    escSequence[i++] = ESC;
    for ( ; read(fd,&c,1)==1 ; )
       {
        escSequence[i++] = c;
        if (c=='[' || c=='O')
            continue;
        if (c>=0x30 && c<=0x7E)
            break;
       }
    escSequence[i] = '\0';

		/*=======================================================
		 * To avoid the confusion about the difference between
		 *  "normal" and "application mode" keypad, make 2nd
		 *  character match whatever is in "key_left".
		 *======================================================*/
    if (escSequence[1]=='[' || escSequence[1]=='O')
        escSequence[1] = key_left[1];

    if (!strcmp(escSequence,key_left))
        return(KEY_LEFT);
    if (!strcmp(escSequence,key_right))
        return(KEY_RIGHT);
    if (!strcmp(escSequence,key_up))
        return(KEY_UP);
    if (!strcmp(escSequence,key_down))
        return(KEY_DOWN);
    return(0);
   }



	/*****************************************************************
	 * start_of_line:
	 * Move cursor to start of line ...
	 *****************************************************************/
static char  *start_of_line(	/* Returns:  addr of line[]		*/
    char  line[]		/* <r> current line			*/
   ,char  *p			/* <r> current position in line[]	*/
   )
   {
    int   i,k;
    int   num;
    char  displayLine[256];

    num = p - line;		/* num chars preceding p	*/
    if (num)
       {
        for (i=0,k=0 ; i<num ; i++)
            k += sprintf(displayLine+k,cursor_left);
        write(1,displayLine,k);
       }
    return(line);
   }



	/****************************************************************
	 * get_history_line:
	 * Return a line from the history[] list ...
	 ****************************************************************/
static char  *get_history_line(	/* Returns:  ptr to cmd from history[]	*/
    int   kchar		/* <r> key input from user		*/
   )
   {
    int   idx;
    int   incr;

    incr = (kchar==KEY_UP ? -1 : 1);
    if (irecall == HISTORY_NOT_ACTIVE)
        irecall = icnt;

    irecall += incr;
    if (irecall<0 || irecall>=icnt || (icnt-irecall)>MAX_HISTORY)
       {
        irecall = (incr > 0) ? icnt : (icnt - MAX_HISTORY - 1);
        if (irecall < -1)
            irecall = -1;
        return("");
       }

    idx = irecall % MAX_HISTORY;
    return(history[idx]);
   }



#ifndef CURSES
static int   setupterm(
    char  *term
   ,int   fd
   ,int   *ierr
   )
   {
    key_up =    "\033OA";
    key_down =  "\033OB";
    key_right = "\033OC";
    key_left =  "\033OD";
    save_cursor =    "\0337";
    restore_cursor = "\0338";
    cursor_left =  "\010";
    cursor_right = "\033[C";
    clr_eol =      "\033[K";

    *ierr = 1;
    return(0);
   }
#endif		/* CURSES	*/
#endif   	/* vms		*/



	/***************************************************************
	 * fgets_with_edit:
	 ***************************************************************/
char  *fgets_with_edit(		/* Returns:  addr of usrline, or NULL	*/
    char  usrline[]		/* <w> user's input buffer		*/
   ,int   maxlen		/* <r> length of input buffer		*/
   ,FILE  *fp			/* <r> file pointer			*/
   ,char  *prompt		/* <r:opt> prompt string		*/
   )
   {
    int   i,k;
    int   fd;
    int   sts;
    char  *p;
    static char  clrEol[12];
    static char  cursorRight[12];
#ifdef vms
    static int   pasteboard_id;
    static int   keyboard_id;
    static int   key_table_id;
    int   flags;
    short wlen;
    static DYNAMIC_DESCRIPTOR(dsc_cmdline);
    static struct descriptor dsc_prompt = {0,DSC_K_DTYPE_T,DSC_K_CLASS_S,0};
#else
    int   idx;
    int   ierr;
    int   kchar;
    int   num;
    char  c;
    char  line[256];
    char  displayLine[256];
#if !defined(_WIN32)
    struct termios  tt,ttsave;
#endif
#endif

    fd = fileno(fp);
    if (!isatty(fd))
        return(fgets(usrline,maxlen,fp));

		/*=======================================================
		 * Note: vt100 "terminfo" seems to have extra chars in
		 *  its "cursor_right" escape sequence.  We'll use
		 *  local string "cursorRight" instead ...
		 *  Likewise for "clr_eol" escape sequence.
		 *======================================================*/
    if (!cursorRight[0])
       {			/* first time only ...			*/
        irecall = HISTORY_NOT_ACTIVE;

#ifdef vms
        flags = 1;
        smg$create_pasteboard(&pasteboard_id,0,0,0,&flags);	/*  */
        smg$create_virtual_keyboard(&keyboard_id);
/*        smg$create_key_table(&key_table_id);		/*  */
        cursorRight[0] = 1;		/* indicate no longer first time */
#else
        sts = setupterm(0,1,&ierr);
        if (sts || ierr!=1)
           {
            printf("after setupterm: sts=%d  ierr=%d\n");
            usrline[0] = '\0';
            return(0);
           }

        if (!strncmp(cursor_right,"\033[C",3))
            strncpy(cursorRight,cursor_right,3);
        else
           {
            if (strlen(cursor_right) >= sizeof(cursorRight))
               {
                fprintf(stderr,"fgets_with_edit: cursorRight too short\n");
                exit(0);
               }
            strcpy(cursorRight,cursor_right);
           }

        if (!strncmp(clr_eol,"\033[K",3))
            strncpy(clrEol,clr_eol,3);
        else
           {
            if (strlen(clr_eol) >= sizeof(clrEol))
               {
                fprintf(stderr,"fgets_with_edit: clrEol[] is too short\n");
                exit(0);
               }
            strcpy(clrEol,clr_eol);
           }
#endif
       }

#ifdef vms
		/*=======================================================
		 * VMS only: read using smg routine ...
		 *======================================================*/
    dsc_prompt.dscA_pointer = prompt ? prompt : "Command> ";
    dsc_prompt.dscW_length = strlen(dsc_prompt.dscA_pointer);
    sts = smg$read_composed_line(&keyboard_id,0,
                &dsc_cmdline,&dsc_prompt,&wlen);
    if (~sts & 1)
       {
        dasmsg(sts,"Error from smg$read_composed_string");
        return(0);
       }
    p = dsc_cmdline.dscA_pointer ? dsc_cmdline.dscA_pointer : "";
    k = (wlen<maxlen) ? wlen : maxlen;
    strncpy(usrline,p,k);
    if (wlen < maxlen)
        usrline[wlen] = '\0';
#else

		/*=======================================================
		 * Save tty settings;  reset tty for command-line editing.
		 *======================================================*/
#if !defined(_WIN32)
    sts = tcgetattr(fd,&tt);
    memcpy(&ttsave,&tt,sizeof(ttsave));	/* save copy of tt	*/
    tt.c_lflag &= ~ICANON;
    tt.c_lflag &= ~ECHO;
    tt.c_cc[VTIME] = 0;
    tt.c_cc[VMIN] = 1;
    sts = tcsetattr(fd,TCSANOW,&tt);
    if (sts)
       {
        perror("Error from tcseattr");
        usrline[0] = '\0';
        return(0);
       }
#endif
		/*=======================================================
		 * Edit input line from tty ...
		 *======================================================*/
    line[0] = '\0';
    if (prompt)
        printf("%s",prompt);
    fflush(stdout);
    for (p=line ; ; )
       {
        k = read(fd,&c,1);
        if (k<=0 || c=='\n')
            break;

        if (isprint(c) || isspace(c))
           {
            write(1,&c,1);	/* Write c at current position ...	*/
            if (*p)
               {		/* if not at end of line		*/
                if (insert_mode)
                   {
                    k = strlen(p);
                    p[k+1] = '\0';
                    for ( ; k ; k--)
                        p[k] = p[k-1];
                    k = sprintf(displayLine,"%s%s%s",
                            save_cursor,p+1,restore_cursor);
                    i = write(1,displayLine,k);
                   }
               }
            else
                *(p+1) = '\0';		/* else, set new EOL		*/
            *p++ = c;
            continue;
           }
        if (!iscntrl(c))
            continue;

        if (c == DEL)
           {
            if (p <= line)
                continue;
            for (i=0,p-- ; p[i]=p[i+1] ; i++)
                ;
            k = sprintf(displayLine,"%s%s%s %s",
                cursor_left,save_cursor,(i?p:""),restore_cursor);
            write(1,displayLine,k);
            continue;
           }

        switch(c)
           {
            default:
                sprintf(displayLine," ^%c=0x%02X",c+'@',c);
                break;

            case ESC:
                kchar = decode_escape_sequence(fd);
                if (kchar == KEY_LEFT)
                   {
                    if (p > line)
                       {
                        p--;
                        write(1,cursor_left,strlen(cursor_left));
                       }
                   }
                else if (kchar == KEY_RIGHT)
                   {
                    if (*p)
                       {
                        p++;
                        k = strlen(cursorRight);
                        write(1,cursorRight,k);
                       }
                   }
                else if (kchar==KEY_UP || kchar==KEY_DOWN)
                   {
                    p = start_of_line(line,p);
                    write(1,clrEol,strlen(clrEol));
                    strcpy(line,get_history_line(kchar));
                    k = strlen(line);
                    write(1,line,k);
                    p = line + k;
                   }
                break;

            case CTRL_E:	/* Jump to end of line ...		*/
                num = strlen(p);
                if (num)
                   {
                    k = 0;
                    for (i=0 ; i<num ; i++)
                        k += sprintf(displayLine+k,cursorRight);
                    write(1,displayLine,k);
                    p += num;
                   }
                break;

            case CTRL_H:	/* Jump to beginning of line ...	*/
                p = start_of_line(line,p);
                break;

            case CTRL_U:	/* Delete to beginning of line ...	*/
                num = p - line;		/* num chars preceding p	*/
                if (num)
                   {
                    for (i=0,k=0 ; i<num ; i++)
                        k += sprintf(displayLine+k,cursor_left);
                    k += sprintf(displayLine+k,"%s%s",p,clrEol);
                    write(1,displayLine,k);
                    for (i=0 ; line[i]=p[i] ; i++)
                        ;		/* shift chars within line[]	*/
                    p = start_of_line(line,line+i);
                   }
                break;
           }
       }
    if (!icnt && (k=strlen(line)))
        history[icnt++] = strcpy(malloc(k+1),line);
    else
       {
        idx = (icnt-1) % MAX_HISTORY;
        if (nonblank(line) && strcmp(line,history[idx]))
           {
            idx = icnt++ % MAX_HISTORY;
            if (history[idx])
                free(history[idx]);
            history[idx] = strcpy(malloc(strlen(line)+1),line);
           }
       }
    irecall = HISTORY_NOT_ACTIVE;
#if !defined(_WIN32)
    sts = tcsetattr(fd,TCSANOW,&ttsave);
    if (sts)
        perror("Error from tcseattr");
#endif
    strncpy(usrline,line,maxlen);
    printf("\n");
#endif
    return(usrline);
   }



	/***************************************************************
	 * main:
	 ***************************************************************/
/*main()
/*   {
/*    int   i,k;
/*    char  usrline[256];
/*    FILE  *fp;
/*
/*    fp = stdin;		/* standard input		*/
/*
/*    for ( ; ; )
/*       {
/*        fgets_with_edit(usrline,sizeof(usrline),fp,"Enter> ");
/*        printf("--> Line = '%s'\n\n",usrline);
/*       }
/*   }							/*  */
