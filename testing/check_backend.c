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

#include <stdio.h>
#include <stdarg.h>
#include <setjmp.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>
extern char *strsignal(int);
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/wait.h>
#endif

#include <check.h>
#include <check_list.h>
#include <check_impl.h>
#include <check_msg.h>
#include <check_log.h>
#include <check_print.h>


// Check for HAVE_CHECK and HAVE_FORK //
#include <mdsplus/mdsconfig.h>

#include "testing.h"

#ifdef _WIN32
// TODO: should we use Fibers here
// (see https://msdn.microsoft.com/en-us/library/windows/desktop/ms686919%28v=vs.85%29.aspx )
// without context switching will be not possible to run all tests if one fails.
#else
#include <ucontext.h>
//static ucontext_t error_jmp_context;
volatile int error_jmp_state;
#endif

#define MSG_LEN 100

static double default_timeout = TEST_DEFAULT_TIMEOUT;
static pid_t group_pid = 0;
extern jmp_buf error_jmp_buffer;

static char *custom_pass_msg = NULL;
static int error_code = 0;

#ifdef HAVE_FORK
static int alarm_received = 0;
static struct sigaction sigint_old_action;
static struct sigaction sigterm_old_action;
static struct sigaction sigalarm_old_action;

static struct sigaction sigalarm_new_action;
static struct sigaction sigint_new_action;
static struct sigaction sigterm_new_action;
#endif

static Suite   *suite  = NULL;
static SRunner *runner = NULL;
static TCase   *tcase  = NULL;

extern void eprintf(const char *fmt, const char *file, int line,
                    ...) CK_ATTRIBUTE_NORETURN;
extern void *emalloc(size_t n);
extern void *erealloc(void *, size_t n);




static int out_fd = -1;
static fpos_t out_pos;

static FILE *switchStdout(const char *newStream)
{
    fflush(stdout);
    fgetpos(stdout, &out_pos);
    out_fd = dup(fileno(stdout));
    if(newStream)
        freopen(newStream, "w", stdout);
    else
        fclose(stdout);
    return fdopen(out_fd, "w");
}


static void revertStdout()
{
    if(out_fd < 0) return;
    fflush(stdout);
    dup2(out_fd, fileno(stdout));
    close(out_fd);
    clearerr(stdout);
    fsetpos(stdout, &out_pos);
}


////////////////////////////////////////////////////////////////////////////////
//  SIG HANDLER  ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///  Signal handler for the proces created by test fork
///
#ifdef HAVE_FORK
static void CK_ATTRIBUTE_UNUSED sig_handler(int sig_nr)
{
    switch (sig_nr)
    {
    case SIGALRM:
        alarm_received = 1;
        killpg(group_pid, SIGKILL);
        break;
    case SIGTERM:
    case SIGINT:
    {
        pid_t own_group_pid;
        int child_sig = SIGTERM;
        if (sig_nr == SIGINT)
        {
            child_sig = SIGKILL;
            sigaction(SIGINT, &sigint_old_action, NULL);
        }
        else
        {
            sigaction(SIGTERM, &sigterm_old_action, NULL);
        }
        killpg(group_pid, child_sig);
        /* POSIX says that calling killpg(0)
             * does not necessarily mean to call it on the callers
             * group pid! */
        own_group_pid = getpgrp();
        killpg(own_group_pid, sig_nr);
        break;
    }
    default:
        eprintf("Unhandrled signal: %d", __FILE__, __LINE__, sig_nr);
        break;
    }
}
#endif




////////////////////////////////////////////////////////////////////////////////
//  Assert  ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


///
/// \brief __ck_assert_fail
///

void __test_assert_fail(const char *file, int line, const char *expr, ...)
{
    const char *msg;
    va_list ap;
    char buf[BUFSIZ];
    const char *to_send;

    send_loc_info(file, line);
    va_start(ap, expr);
    msg = (const char *)va_arg(ap, char *);
    if(msg != NULL)
    {
        vsnprintf(buf, BUFSIZ, msg, ap);
        to_send = buf;
    }
    else
    {
        to_send = expr;
    }

    va_end(ap);
    send_failure_info(to_send, CK_FAILURE);
    if( cur_fork_status() == CK_FORK && group_pid)
    {
#     ifdef HAVE_FORK
        _exit(1);
#     endif /* HAVE_FORK */
    }
    else
    {
        __test_end();
        _exit(1);
    }
}


////////////////////////////////////////////////////////////////////////////////
/// Mark point
///

void __mark_point(const char *__assertion, const char *__file,
                  ASSERT_LINE_TYPE __line, const char *__function
		  __attribute__ ((unused)))
{
    if(!suite) __test_init(__assertion,__file,__line);
    send_loc_info(__file, __line);
}


////////////////////////////////////////////////////////////////////////////////
/// Assert fail
///

void __assert_fail (const char *__assertion, const char *__file,
                    ASSERT_LINE_TYPE __line, const char *__function
		    __attribute__ ((unused)))
{
    if(!suite) __test_init(__assertion,__file,__line);
    __test_assert_fail(__file,__line,__assertion,NULL);

    // FIX
    __test_end();
    abort();
}


static char *pass_msg(void)
{
    if(custom_pass_msg)
        return custom_pass_msg;
    else
        return strdup("Passed");
}

////////////////////////////////////////////////////////////////////////////////
//  fork  ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifdef HAVE_FORK
static TestResult *receive_result_info_fork(const char *tcname,
                                            const char *tname,
                                            int iter,
                                            int status, int expected_signal,
                                            signed char allowed_exit_value);
static void set_fork_info(TestResult * tr, int status, int expected_signal,
                          signed char allowed_exit_value);
static char *signal_msg(int sig);
static char *signal_error_msg(int signal_received, int signal_expected);
static char *exit_msg(int exitstatus);
static int waserror(int status, int expected_signal);
static TestResult *receive_result_info_fork(const char *tcname,
                                            const char *tname,
                                            int iter,
                                            int status, int expected_signal,
                                            signed char allowed_exit_value)
{
    TestResult *tr;

    tr = receive_test_result(waserror(status, expected_signal));
    if(tr == NULL)
    {
        eprintf("Failed to receive test result", __FILE__, __LINE__);
    }
    else
    {
        tr->tcname = tcname;
        tr->tname = tname;
        tr->iter = iter;
        set_fork_info(tr, status, expected_signal, allowed_exit_value);
    }

    return tr;
}

static void set_fork_info(TestResult * tr, int status, int signal_expected,
                          signed char allowed_exit_value)
{
    // nonzero value if the child process terminated because it received a
    // signal that was not handled
    int was_sig = WIFSIGNALED(status);

    // nonzero value if the child process terminated normally with exit, _exit
    int was_exit = WIFEXITED(status);

    // returns the exit status of the child. This consists of the least
    // significant 8 bits of the status argument that the child specified in a
    // call to exit(3) or _exit(2) or as the argument for a return statement in
    // main(). This macro should be employed only if WIFEXITED returned true.
    signed char exit_status = WEXITSTATUS(status);

    // returns the number of the signal that caused the child process to
    // terminate. This macro should be employed only if WIFSIGNALED returned
    // true.
    int signal_received = WTERMSIG(status);

    if(was_sig)
    {
        if(signal_expected == signal_received)
        {
            if(alarm_received)
            {
                /* Got alarm instead of signal */
                tr->rtype = CK_ERROR;
                if(tr->msg != NULL)
                {
                    free(tr->msg);
                }
                tr->msg = signal_error_msg(signal_received, signal_expected);
            }
            else
            {
                tr->rtype = CK_PASS;
                if(tr->msg != NULL)
                {
                    free(tr->msg);
                }
                tr->msg = pass_msg();
            }
        }
        else if(signal_expected != 0)
        {
            /* signal received, but not the expected one */
            tr->rtype = CK_ERROR;
            if(tr->msg != NULL)
            {
                free(tr->msg);
            }
            tr->msg = signal_error_msg(signal_received, signal_expected);
        }
        else
        {
            /* signal received and none expected */
            tr->rtype = CK_ERROR;
            if(tr->msg != NULL)
            {
                free(tr->msg);
            }
            tr->msg = signal_msg(signal_received);
        }
    }

    // no signal .. (child exited by itself)
    else if(signal_expected == 0)
    {
        if(was_exit && exit_status == allowed_exit_value)
        {
            tr->rtype = CK_PASS;
            if(tr->msg != NULL)
            {
                free(tr->msg);
            }
            tr->msg = pass_msg();
        }
        else if(was_exit && exit_status != allowed_exit_value)
        {
            if(tr->rtype == CK_TEST_RESULT_INVALID)
            {                   /* early exit */
                tr->rtype = CK_ERROR;
                tr->msg = exit_msg(exit_status);
                error_code = exit_status;
            }
            else
            {
                //  tr->rtype = CK_FAILURE;
                error_code = exit_status;
            }
        }
    }

    else
    {                           /* a signal was expected and none raised */
        if(was_exit)
        {
            if(tr->msg != NULL)
            {
                free(tr->msg);
            }
            tr->msg = exit_msg(exit_status);
            if(exit_status == allowed_exit_value)
            {
                tr->rtype = CK_FAILURE; /* normal exit status */
            }
            else
            {
                tr->rtype = CK_FAILURE; /* early exit */
            }
        }
    }
}

static char *signal_msg(int signal)
{
    char *msg = (char *)emalloc(MSG_LEN);       /* free'd by caller */

    if(alarm_received)
    {
        snprintf(msg, MSG_LEN, "Test timeout expired");
    }
    else
    {
        snprintf(msg, MSG_LEN, "Received signal %d (%s)",
                 signal, strsignal(signal));
    }
    return msg;
}

static char *signal_error_msg(int signal_received, int signal_expected)
{
    char *sig_r_str;
    char *sig_e_str;
    char *msg = (char *)emalloc(MSG_LEN);       /* free'd by caller */

    sig_r_str = strdup(strsignal(signal_received));
    sig_e_str = strdup(strsignal(signal_expected));
    if(alarm_received)
    {
        snprintf(msg, MSG_LEN,
                 "Test timeout expired, expected signal %d (%s)",
                 signal_expected, sig_e_str);
    }
    else
    {
        snprintf(msg, MSG_LEN, "Received signal %d (%s), expected %d (%s)",
                 signal_received, sig_r_str, signal_expected, sig_e_str);
    }
    free(sig_r_str);
    free(sig_e_str);
    return msg;
}

static char *exit_msg(int exitval)
{
    char *msg = (char *)emalloc(MSG_LEN);       /* free'd by caller */

    snprintf(msg, MSG_LEN, "Early exit with return value %d", exitval);
    return msg;
}

static int waserror(int status, int signal_expected)
{
    int was_sig = WIFSIGNALED(status);
    int was_exit = WIFEXITED(status);
    int exit_status = WEXITSTATUS(status);
    int signal_received = WTERMSIG(status);

    return ((was_sig && (signal_received != signal_expected)) ||
            (was_exit && exit_status != 0));
}
#     endif /* HAVE_FORK */




////////////////////////////////////////////////////////////////////////////////
//  nofork  ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////






static void set_nofork_info(TestResult * tr)
{
    if(tr->msg == NULL && tr->rtype == CK_PASS )
        tr->msg = pass_msg();
}

static TestResult *receive_result_info_nofork(const char *tcname,
                                              const char *tname,
                                              int iter, int duration)
{
    TestResult *tr;

    tr = receive_test_result(0);
    if(tr == NULL)
    {
        eprintf("Failed to receive test result", __FILE__, __LINE__);
    }
    else
    {
        tr->tcname = tcname;
        tr->tname = tname;
        tr->iter = iter;
        tr->duration = duration;
        set_nofork_info(tr);
    }

    return tr;
}




static void srunner_add_failure(SRunner * sr, TestResult * tr)
{
    check_list_add_end(sr->resultlst, tr);
    sr->stats->n_checked++;     /* count checks during setup, test, and teardown */
    if(tr->rtype == CK_FAILURE)
        sr->stats->n_failed++;
    else if(tr->rtype == CK_ERROR)
        sr->stats->n_errors++;

}


static void srunner_run_end(SRunner * sr,
                            enum print_output CK_ATTRIBUTE_UNUSED print_mode)
{
    log_srunner_end(sr);
    srunner_end_logging(sr);
    teardown_messaging();
    set_fork_status(CK_FORK);
}


static void srunner_send_evt(SRunner * sr, void *obj, enum cl_event evt)
{
    List *l;
    Log *lg;

    l = sr->loglst;
    for(check_list_front(l); !check_list_at_end(l); check_list_advance(l))
    {
        lg = (Log *)check_list_val(l);
        fflush(lg->lfile);
        lg->lfun(sr, lg->lfile, lg->mode, obj, evt);
        fflush(lg->lfile);
    }
}





////////////////////////////////////////////////////////////////////////////////
//  test_init  /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


///
/// Test initialization for the check infrastructure .. this do the following:
/// 1. creates the suite and runner instances, 2. set up the form message pipe,
/// 3. initializes the logger, 4. send the event of starting default runner.
///



static enum print_output print_mode = CK_NORMAL;

static int find_first(const char *str, const char *pat) {
    return strstr(str,pat) == str;
}

void __test_init(const char *test_name, const char *file, const int line) {

    if(!suite) {
        suite = suite_create(file);
        runner = srunner_create(suite);

        // print mode normal by default //
        char *env = getenv("TEST_MODE");
        if(env) {
            if(strcmp(env, "silent") == 0)
                print_mode = CK_SILENT;
            if(strcmp(env, "minimal") == 0)
                print_mode = CK_MINIMAL;
            if(strcmp(env, "verbose") == 0)
                print_mode = CK_VERBOSE;
        }

        // init logger NORMAL by default //
        srunner_init_logging(runner, print_mode);

        // format of the tests output //
        char *format = getenv("TEST_FORMAT");

        // to preserve the output format the stdout must be redirected. If no
        // env is defined the result will be null and stdout will be
        // suppressed. The log format preserves the normal stdout as no special
        // formatting is needed.

        char *out_file = getenv("TEST_STDOUT_FILE");

        // the standard output format is taken from the frist item in
        // TEST_FORMAT string

        if(format) {
            if( find_first(format,"log") || find_first(format,"LOG"))
                srunner_register_lfun(runner, stdout, 0, stdout_lfun, print_mode);
            else if( find_first(format,"tap") || find_first(format,"TAP"))
                srunner_register_lfun(runner, switchStdout(out_file), 0, tap_lfun, print_mode);
            else if( find_first(format,"xml") || find_first(format,"XML"))
                srunner_register_lfun(runner, switchStdout(out_file), 0, xml_lfun, print_mode);
        }
        else {
            // default to plain log format //
            srunner_register_lfun(runner, stdout, 0, lfile_lfun, print_mode);
        }

        // if any of TEST_TAPFILE or TEST_XMLFILE is given in env further
        // logger are instanced pointing to specified files.

        if( (out_file = getenv("TEST_TAPFILE")) != NULL ) {
            FILE *f = fopen(out_file,"w");
            if(f) srunner_register_lfun(runner, f, 0, tap_lfun, print_mode);
        }
        if( (out_file = getenv("TEST_XMLFILE")) != NULL ) {
            FILE *f = fopen(out_file,"w");
            if(f) srunner_register_lfun(runner, f, 0, xml_lfun, print_mode);
        }


        // set fork status //
        srunner_set_fork_status(runner, cur_fork_status());

        // send runner start event //
        log_srunner_start(runner);
        log_suite_start(runner,suite);

        // set up message pipe in check_msg //
        setup_messaging();

        // set exit function //
        atexit(__test_exit);
    }

    // create tcase //
    tcase  = tcase_create(test_name);

    // SET tcase TIMEOUT //
    tcase_set_timeout(tcase,default_timeout);
    char *time_unit = getenv("TEST_TIMEUNIT");
    if(time_unit != NULL) {
        char *endptr = NULL;
        double tu = strtod(time_unit, &endptr);
        if(tu >= 0 && endptr != time_unit && (*endptr) == '\0') {
            tcase_set_timeout(tcase,default_timeout*tu);
        }
    }

    // ADD TCASE //
    suite_add_tcase(suite,tcase);

    #ifdef HAVE_FORK
    if(cur_fork_status() == CK_FORK ) {

        // SIGALRM //
        memset(&sigalarm_new_action, 0, sizeof(sigalarm_new_action));
        sigalarm_new_action.sa_handler = sig_handler;
        sigaction(SIGALRM, &sigalarm_new_action, &sigalarm_old_action);

        // SIGINT //
        memset(&sigint_new_action, 0, sizeof(sigint_new_action));
        sigint_new_action.sa_handler = sig_handler;
        sigaction(SIGINT, &sigint_new_action, &sigint_old_action);

        // SIGTERM //
        memset(&sigterm_new_action, 0, sizeof(sigterm_new_action));
        sigterm_new_action.sa_handler = sig_handler;
        sigaction(SIGTERM, &sigterm_new_action, &sigterm_old_action);
    }
    #endif

    // mark point in case signal is received before any test //
    send_ctx_info(CK_CTX_TEST);
    send_loc_info(file,line);

}

////////////////////////////////////////////////////////////////////////////////
//  SET TIMEOUT  ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void __test_timeout(double seconds) {    
    default_timeout = seconds;
}

////////////////////////////////////////////////////////////////////////////////
//  START TEST  ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



int __setup_parent() {


    // FORK //
#ifdef HAVE_FORK
    if( cur_fork_status() == CK_FORK )
    {
        pid_t pid = fork();
        if(pid == -1)
            // error forking //
            eprintf("Error forking to a new process:", __FILE__, __LINE__);
        else if(pid == 0) {
            return 0;
            // continue on child //
        }
        else {
            group_pid = pid;



            timer_t timerid;
            struct itimerspec timer_spec;
            int status = 0;
            alarm_received = 0;

            if(timer_create(check_get_clockid(), NULL, &timerid) == 0)
            {
                /* Set the timer to fire once */
                timer_spec.it_value = tcase->timeout;
                timer_spec.it_interval.tv_sec = 0;
                timer_spec.it_interval.tv_nsec = 0;
                if(timer_settime(timerid, 0, &timer_spec, NULL) == 0)
                {
                    pid_t   pid_w;
                    do pid_w = waitpid(group_pid, &status, 0);
                    while (pid_w == -1 );
                }
                else
                    // settime failed //
                    eprintf("Error in call to timer_settime:", __FILE__, __LINE__);

                /* If the timer has not fired, disable it */
                timer_delete(timerid);
            }
            else
                // create timer failed //
                eprintf("Error in call to timer_create:", __FILE__, __LINE__);

            // kill child group and reset parent status to group_pid = 0 //
            killpg(group_pid, SIGKILL);   /* Kill remaining processes. */
            group_pid = 0;


            srunner_send_evt(runner, tcase, CLSTART_T);
//            send_ctx_info(CK_CTX_SETUP); // FIXX ///
            TestResult *tr;
            tr = receive_result_info_fork(tcase->name, "test_main", 0, status, 0, 0);
            if(tr) srunner_add_failure(runner, tr);
            srunner_send_evt(runner, tr, CLEND_T);
            return 1;
        }
    }
    else
#endif
    // child here or no fork available
    return 0;
}


int __setup_child() {
#ifdef HAVE_FORK
    if(cur_fork_status() == CK_FORK ) {
        setpgid(0, 0);
        group_pid = getpgrp();
        return 1;
    }
#endif
    srunner_send_evt(runner, tcase, CLSTART_T);
    return 1;
}



////////////////////////////////////////////////////////////////////////////////
//  END TEST  //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void __test_end()
{
    // if forked //
#   ifdef HAVE_FORK
    if(cur_fork_status() == CK_FORK) {
        if(group_pid) {
            // child
            _exit(0);
        }
        else
            // parent
            return;
    }
#   endif
    TestResult *tr;
    tr = receive_result_info_nofork(tcase->name, "test_main", 0, 0);
    if(tr) srunner_add_failure(runner, tr);
    srunner_send_evt(runner, tr, CLEND_T);
}



////////////////////////////////////////////////////////////////////////////////
//  EXIT FUNCTION  /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



void __test_exit()
{
    // if we are on child silently exit //
#   ifdef HAVE_FORK
    if(cur_fork_status() == CK_FORK && group_pid) {
        _exit(0);
    }
#   endif
    int _nerr = 0;
    if(runner && suite) {
        log_suite_end(runner, suite);
        srunner_run_end(runner,CK_VERBOSE);
        _nerr = srunner_ntests_failed(runner);
        srunner_free(runner);
        revertStdout();
    }

    if(error_code)
        _exit(error_code);
    else
        _exit(_nerr > 0);
}

// Force exit with custo signal //
void __test_abort(int code, const char *__msg, const char *__file,
                  unsigned int __line, const char *__function)
{
    // TODO: fix this with proper enum related to test_driver
    error_code = code;
    if(tcase) __mark_point(tcase->name,__file,__line,__function);
    else __mark_point("test aborted",__file,__line,__function);

    switch(code) {
    case 77:
        custom_pass_msg = strdup(__msg);
        send_failure_info(__msg, CK_SKIP);
        if( cur_fork_status() == CK_FORK && group_pid)
        {
    #     ifdef HAVE_FORK
            _exit(code);
    #     endif /* HAVE_FORK */
        }
        else
        {
            TestResult *tr;
            tr = receive_result_info_nofork(tcase->name, "test_main", 0, 0);
            if(tr) srunner_add_failure(runner, tr);
            srunner_send_evt(runner, tr, CLEND_T);
        }
        break;
    default:
    case 99:
        __test_assert_fail(__file,__line,__function);
    }
    //    __test_exit();
    exit(code);
}





////////////////////////////////////////////////////////////////////////////////
//  FORK SET FUNCTION  /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void __test_setfork(int value)
{
    if(value) {
        set_fork_status(CK_FORK);
    }
    else {
        set_fork_status(CK_NOFORK);
    }
}
