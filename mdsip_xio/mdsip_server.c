#include "mdsip.h"

int main(
    int                                     argc,
    char **                                 argv)
{
  mdsip_options_t options;
  if (!mdsip_parse_command(argc, argv, &options))
    return 1;
  mdsip_initialize_io(&options);
  while (1) 
    mdsip_io_wait();
  return 0;
}
