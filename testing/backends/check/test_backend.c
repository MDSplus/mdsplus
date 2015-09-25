
#include <stdio.h>
#include <stdarg.h>
#include <setjmp.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <sys/wait.h>


#include <check.h>
#include <check_list.h>
#include <check_impl.h>
#include <check_msg.h>
#include <check_log.h>



#include "test_backend.h"


// Check for HAVE_CHECK and HAVE_FORK //
#include <config.h>



#define MSG_LEN 100


static int alarm_received = 0;
static pid_t group_pid = 0;
extern jmp_buf error_jmp_buffer;

static struct sigaction sigint_old_action;
static struct sigaction sigterm_old_action;
static struct sigaction sigalarm_old_action;

static struct sigaction sigalarm_new_action;
static struct sigaction sigint_new_action;
static struct sigaction sigterm_new_action;

static Suite   *suite  = NULL;
static SRunner *runner = NULL;
static TCase   *tcase  = NULL;

extern void eprintf(const char *fmt, const char *file, int line, 
                    ...) CK_ATTRIBUTE_NORETURN;
extern void *emalloc(size_t n);
extern void *erealloc(void *, size_t n);


////////////////////////////////////////////////////////////////////////////////
//  SIG HANDLER  ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///  Signal handler for the proces created by test fork
///

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
    send_failure_info(to_send);
    if( cur_fork_status() == CK_FORK )
    {
#     if defined(HAVE_FORK) && HAVE_FORK==1
       _exit(1);
#     endif /* HAVE_FORK */
    }
    else
    {
        longjmp(error_jmp_buffer, 1);
    }
}


////////////////////////////////////////////////////////////////////////////////
/// Mark point 
/// 

void __mark_point(const char *__assertion, const char *__file, 
                  unsigned int __line, const char *__function)
{
    send_loc_info(__file, __line);
}



////////////////////////////////////////////////////////////////////////////////
/// Assert fail
/// 

void __assert_fail (const char *__assertion, const char *__file,
                    unsigned int __line, const char *__function)
{    
    __test_assert_fail(__file,__line,__assertion,NULL);    
    _exit(1);
}




////////////////////////////////////////////////////////////////////////////////
//  fork  ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static TestResult *receive_result_info_fork(const char *tcname,
                                            const char *tname,
                                            int iter,
                                            int status, int expected_signal,
                                            signed char allowed_exit_value);


//#if defined(HAVE_FORK) && HAVE_FORK==1

static void set_fork_info(TestResult * tr, int status, int expected_signal,
                          signed char allowed_exit_value);
static char *signal_msg(int sig);
static char *signal_error_msg(int signal_received, int signal_expected);
static char *exit_msg(int exitstatus);
static int waserror(int status, int expected_signal);

static char *pass_msg(void);


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
    int was_sig = WIFSIGNALED(status);
    int was_exit = WIFEXITED(status);
    signed char exit_status = WEXITSTATUS(status);
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
            if(tr->msg == NULL)
            {                   /* early exit */
                tr->rtype = CK_ERROR;
                tr->msg = exit_msg(exit_status);
            }
            else
            {
                tr->rtype = CK_FAILURE;
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
//#     endif /* HAVE_FORK */




////////////////////////////////////////////////////////////////////////////////
//  nofork  ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////




static char *pass_msg(void)
{
    return strdup("Passed");
}

static void set_nofork_info(TestResult * tr)
{
    if(tr->msg == NULL)
    {
        tr->rtype = CK_PASS;
        tr->msg = pass_msg();
    }
    else
    {
        tr->rtype = CK_FAILURE;
    }
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


void __test_init(const char *test_name, const char *file, const int line) {
            
//    if(group_pid) {
//        killpg(group_pid,SIGKILL);
//        group_pid = 0;
//    }
    
    if(!suite) {
        suite = suite_create(file);            
        runner = srunner_create(suite);

        // init logger normal for now //
        srunner_init_logging(runner, CK_NORMAL);
        
        // send runner start event //
        log_srunner_start(runner);    
        log_suite_start(runner,suite);    
        
        // set fork //
        //        set_fork_status(srunner_fork_status(runner));
        
        // set up message pipe in check_msg //
        setup_messaging(); 
        
        // set exit function //
        atexit(__test_exit);
    }
    
    tcase  = tcase_create(test_name);
    suite_add_tcase(suite,tcase);

    
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
    
    // mark point in case signal is received before any test //
    send_loc_info(file,line);
    
    
}




////////////////////////////////////////////////////////////////////////////////
//  START TEST  ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int __setup_parent() {
        
    // FORK //
    if( cur_fork_status() == CK_FORK )                        
    {
        pid_t pid = fork();        
        if(pid == -1) 
            // error forking //
            eprintf("Error forking to a new process:", __FILE__, __LINE__);     
        else if(pid == 0) {
            // child process //
            return 0;
        }        
        group_pid = pid;
    }
    else {
        return 0;
    }
    
    TestResult *tr = NULL;
    
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
    send_ctx_info(CK_CTX_SETUP); // FIXX ///
    tr = receive_result_info_fork(tcase->name, "test_main", 0, status, 0, 0);              
    if(tr) srunner_add_failure(runner, tr);  
    srunner_send_evt(runner, tcase, CLEND_T);        
    
    return 1;
}



int __setup_child() {
    
    if(cur_fork_status() == CK_FORK ) {
        setpgid(0, 0);
        group_pid = getpgrp();       
        return 1;
    }
    else {
        srunner_send_evt(runner, tcase, CLSTART_T);                    
        return 0 == setjmp(error_jmp_buffer);
    }
}



////////////////////////////////////////////////////////////////////////////////
//  END TEST  //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void __test_end()
{
    // if forked //
    if(cur_fork_status() == CK_FORK ) {
        if(group_pid) _exit(0);
    }
    // if not forked //
    else {
        TestResult *tr;
        send_ctx_info(CK_CTX_SETUP); // FIXX ///
        tr = receive_result_info_nofork(tcase->name, "test_main", 0, 0);
        if(tr) srunner_add_failure(runner, tr); 
        srunner_send_evt(runner, tcase, CLEND_T);                        
    }
}






////////////////////////////////////////////////////////////////////////////////
//  EXIT FUNCTION  /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void __test_exit()
{   
    // if we are on child yet silently exit //
    if(group_pid) _exit(0);
        
    log_suite_end(runner, suite);     
    srunner_run_end(runner,CK_VERBOSE);        
    int _nerr = srunner_ntests_failed(runner);    
    srunner_free(runner);
        
    _exit(_nerr);
}







void __test_setfork(int value)
{    
    if(value) {
        set_fork_status(CK_FORK);
    }
    else { 
        set_fork_status(CK_NOFORK);
    }
}








