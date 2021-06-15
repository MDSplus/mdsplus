#include <mdsshr.h>
static int sandbox = 0;
EXPORT void MdsEnableSandbox()
{
  /*** enable sandbox flag to disable potentially harmful functions such as
     spawn and py() which could be executed by tdi expressions providing shell
     access to remote users. ***/
  sandbox = 1;
}

EXPORT int MdsSandboxEnabled() { return sandbox; }
