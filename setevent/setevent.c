extern int MDSEvent(char *eventname);

int main(int argc, char **argv)
{
  int status;
  if (argc < 2)
  {
    printf("Usage: %s <event>\n",argv[0]);
    status = 0;
  }
  else
    status = MDSEvent(argv[1]);
  return(status);
}    
