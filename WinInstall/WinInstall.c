#include <windows.h>
int main()
{
  HKEY key = NULL;
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment",0,KEY_READ | KEY_WRITE,&key) == ERROR_SUCCESS)
  {
    long valtype;
    unsigned long path_len;
 
    if (RegQueryValueEx(key,"PATH",0,&valtype,NULL,&path_len) == ERROR_SUCCESS)
    {
      char *mdsplusdir = "%MDSPLUSDIR%";
      char *path = malloc(path_len + strlen(mdsplusdir) + 1);
      path[0] = 0;
      RegQueryValueEx(key,"PATH",0,&valtype,path,&path_len);
      if (path[0])
        strcat(path,";");
      strcat(path,mdsplusdir);
        RegSetValueEx(key,"PATH",0,REG_EXPAND_SZ,path,(DWORD)strlen(path)); 
      free(path);
    }
  }
  if (key) RegCloseKey(key);
  return 0;
}
