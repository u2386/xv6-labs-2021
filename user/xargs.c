#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

char* readline(char *buf, char *from);

int
main(int argc, char *argv[])
{
  char input[2048], *p;
  char buf[32];
  int status;
  char *argv_copy[32];
  int n;

  p = input;
  while((n = read(0, p, sizeof input)) > 0) {
    p += n;
  }

  p = &input[0];
  while(*p!='\0') {
    readline(buf, p);

    if (fork() == 0) {
      int i = 0;
      for(i=0; i+1<argc; i++) {
        argv_copy[i] = argv[i+1];
      }
      argv_copy[i] = buf;
      argv_copy[i+1] = 0;

      if (exec(argv_copy[0], argv_copy) != 0) {
        fprintf(2, "exec failed\n");
        exit(1);
      }

    } else {
      wait(&status);
      if(status != 0) {
        fprintf(2, "child exits %s\n", status);
        exit(1);
      }
    }

    p += strlen(buf)+1;
  }
  exit(0);
}

char*
readline(char *buf, char *from)
{
  int i;
  char c;

  for(i=0; i < strlen(from); ){
    c = from[i];
    if(c == '\n' || c == '\r')
      break;
    buf[i++] = c;
  }
  buf[i] = '\0';
  return buf;
}