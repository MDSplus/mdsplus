#include <windows.h>
#include <stdio.h>
int main(int argc, char **argv)
{
  HKEY key = NULL;
  char *mdsplusdir;
  if (argc != 3)
  {
	exit(0);
  }
  mdsplusdir=strcpy(malloc(strlen(argv[2])+1),argv[2]);
  if (mdsplusdir[strlen(mdsplusdir)-1] == '"')
		mdsplusdir[strlen(mdsplusdir)-1] = 0;
  if (strcmp(argv[1],"/Install") == 0)
  {
	  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment",0,KEY_READ | KEY_WRITE,&key) == ERROR_SUCCESS)
	  {		
		long valtype;
		unsigned long path_len;
	    if (RegQueryValueEx(key,"PATH",0,&valtype,NULL,&path_len) == ERROR_SUCCESS)
	    {
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
  }
  else if (strcmp(argv[1],"/Uninstall") == 0)
  {
	  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment",0,KEY_READ | KEY_WRITE,&key) == ERROR_SUCCESS)
	  {
	    long valtype;
	    unsigned long path_len;
	    if (RegQueryValueEx(key,"PATH",0,&valtype,NULL,&path_len) == ERROR_SUCCESS)
	    {
	      char *path;
	      char *newpath;
	      char *mpath;
	      path = malloc(path_len);
	      newpath = malloc(path_len);
	      newpath[0] = 0;
	      RegQueryValueEx(key,"PATH",0,&valtype,path,&path_len);
	      for (mpath = strtok(path,";");mpath;mpath=strtok(0,";"))
	      {
	        if (strcmp(mpath,mdsplusdir))
	        {
	          if (newpath[0])
	            strcat(newpath,";");
	          strcat(newpath,mpath);
	        }
	      }
	      RegSetValueEx(key,"PATH",0,REG_EXPAND_SZ,newpath,strlen(newpath)); 
	      free(path);
	      free(newpath);
	    }
	  }
  }
  if (key) RegCloseKey(key);
  return 0;
}
