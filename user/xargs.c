#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

#define BUFSIZE 256
int getline(char buffer[])
{
    char c;
    int i=0;
    while(read(0,&c,1))
    {
        if(c!='\n')
            buffer[i++]=c;
        else
        break;
    }
    buffer[i]=0;
    return i;
}

int main(int argc, char* argvs[])
{
  char buf[BUFSIZE];
  char* cmd[MAXARG];

  while (getline(buf))
  {
    if (fork() == 0)//子进程
    {
      memcpy(cmd, argvs + 1, sizeof(char*) * (argc - 1));
      cmd[argc - 1] = buf;
      exec(cmd[0], cmd);
    }
    else
    {//清空命令
      memset(cmd, 0, sizeof(char*) * MAXARG);
      wait(0);//等待子进程结束
    }
  }
  exit(0);
}


