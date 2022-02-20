#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int secs;

  if(argc <= 1){
    fprintf(2, "usage: sleep [seconds]\n");
    exit(1);
  }

  secs = atoi(argv[1]);
  if (sleep(secs*10) != 0) {
      fprintf(2, "sleep failed\n");
      exit(1);
  }

  exit(0);
}
