#include <ctype.h>
#include <mdsdcl_messages.h>
#include <mdsplus/mdsconfig.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#else
#include <time.h>
#endif
#include <dcl.h>
#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <pthread_port.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <tdishr.h>
#include <unistd.h>

EXPORT int tcl_help_device(void *ctx, char **error __attribute__((unused)),
                           char **output __attribute__((unused)))
{
  char *device = NULL;
  char *expr = NULL;
  cli_get_value(ctx, "devtype", &device);
  if (device != NULL)
  {
    int full = cli_present(ctx, "FULL") & 1;
    size_t len = strlen("DevHelp('','')") + strlen(device) + 20;
    expr = malloc(len);
    snprintf(expr, len, "DevHelp('%s',%d)", device, full);
    free(device);
    struct descriptor expr_d = {strlen(expr), DTYPE_T, CLASS_S, expr};
    EMPTYXD(ans_d);
    char *ans;
    int status = TdiExecute(&expr_d, &ans_d MDS_END_ARG);
    free(expr);
    if ((STATUS_OK) && (ans_d.pointer != NULL))
    {
      if ((ans_d.pointer->dtype == DTYPE_T) && (ans_d.pointer->length > 0))
      {
        ans = MdsDescrToCstring(ans_d.pointer);
        if ((*output) == NULL)
          *output = strdup("");
        *output =
            strcat(realloc(*output, strlen(*output) + strlen(ans) + 2), ans);
        strcat(*output, "\n");
        free(ans);
        return MdsdclSUCCESS;
      }
      else
      {
        char *errorstr = "Error obtaining information on that type of device\n";
        if ((*error) == NULL)
          *error = strdup("");
        *error = strcat(realloc(*error, strlen(*error) + strlen(errorstr) + 1),
                        errorstr);
      }
    }
    MdsFree1Dx(&ans_d, 0);
    return status;
  }
  return 0;
}
