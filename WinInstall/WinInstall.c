#include <windows.h>
#include <stdio.h>

#ifdef _WIN64
const char *mdspath=";%MDSPLUSDIR%\\bin_x86_64;%MDSPLUSDIR%\\bin_x86;%MDSPLUSDIR%";
#else
const char *mdspath=";%MDSPLUSDIR%\\bin_x86;%MDSPLUSDIR%";
#endif
const char *envkey="SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment";

static void AddMdsPath() {
  DWORD_PTR resultptr;
  HKEY key = NULL;
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT(envkey),0,KEY_READ | KEY_WRITE,&key) == ERROR_SUCCESS) {
    long valtype;
    unsigned long path_len;
    if (RegQueryValueEx(key,"Path",0,&valtype,NULL,&path_len) == ERROR_SUCCESS){
      char *path = malloc(path_len + strlen(mdspath) + 1);
      path[0] = 0;
      RegQueryValueEx(key,"Path",0,&valtype,path,&path_len);
      if (path[0]) {
	strcat(path,mdspath);
	if (RegSetValueEx(key,"Path",0,REG_EXPAND_SZ,path,(DWORD)strlen(path)) != ERROR_SUCCESS)
	  printf("Error setting PATH=%s\n",path); 
      } else {
        fprintf(stderr,"Error retrieving original Path value\n");
      }
      free(path);
    } else {
      fprintf(stderr,"Error retrieving original Path value length\n");
    }
    RegCloseKey(key);
    SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
		       (LPARAM) TEXT("Environment"), SMTO_ABORTIFHUNG,
		       5000, &resultptr);
  } else {
    fprintf(stderr,"Error opening Environment registry key\n");
  }
}

static void RemoveMdsPath() {
  DWORD_PTR resultptr;
  int found = 0;
  HKEY key = NULL;
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT(envkey),0,KEY_READ | KEY_WRITE,&key) == ERROR_SUCCESS) {
    long valtype;
    unsigned long path_len;
    if (RegQueryValueEx(key,"PATH",0,&valtype,NULL,&path_len) == ERROR_SUCCESS) {
      char *path;
      char *mpath;
      path = malloc(path_len);
      RegQueryValueEx(key,"PATH",0,&valtype,path,&path_len);
      while ((mpath=strstr(path,mdspath)) != (char *)0) {
	found=1;
	mpath[0]='\0';
	strcat(path,mpath+strlen(mdspath));
      }
      if (found)
	RegSetValueEx(key,"PATH",0,REG_EXPAND_SZ,path,(DWORD)strlen(path));
      free(path);
    }
  }
  RegCloseKey(key);
  SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
		     (LPARAM) "Environment", SMTO_ABORTIFHUNG,
		     5000, &resultptr);
  return;
}

int main(int argc, char **argv) {
  if (argc != 2){
    exit(0);
  }
  if (strcmp(argv[1],"/Install") == 0) {
    RemoveMdsPath();
    AddMdsPath();
    return 0;
  }
  else if (strcmp(argv[1],"/Uninstall") == 0) {
    RemoveMdsPath();
    return 0;
  }
  printf("Invalid command\n");
  return 1;
}



