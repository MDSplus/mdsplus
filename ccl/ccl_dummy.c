
#include        <stdio.h>

#define dummy(a) \
    int a(){printf("\n==>  %s : Dummy routine\n\n",#a);return 0;}

dummy(ccl_dclst)
dummy(ccl_execute)
dummy(ccl_finish)
dummy(ccl_lamwait)
dummy(ccl_lpio)
dummy(ccl_lqscan)
dummy(ccl_lstop)
dummy(ccl_pio)
dummy(ccl_plot_data)
dummy(ccl_qrep)
dummy(ccl_qscan)
dummy(ccl_qstop)
dummy(ccl_rdata)
dummy(ccl_set_memory)
dummy(ccl_set_module)
dummy(ccl_set_xandq)
dummy(ccl_show_data)
dummy(ccl_show_module)
dummy(ccl_show_status)
dummy(ccl_stop)
dummy(ccl_wdata)
