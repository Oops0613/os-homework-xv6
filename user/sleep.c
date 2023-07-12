#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc==1)
  {
    printf("请输入参数！");
  }
  else
  {
    int t=atoi(argv[1]);
    sleep(t);
  }
  exit(0);
}
