#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

#define CMDSTYLE 2

void substring(char s[], char *sub, int pos, int len)//从字符串s中取出一段 
{
   int c = 0;   
   while (c < len) {
      sub[c] = s[pos+c];
      c++;
   }
   sub[c] = '\0';
}

/* 截断 '\n' */
char* cutoffinput(char *buf)
{
    if(strlen(buf) > 1 && buf[strlen(buf) - 1] == '\n'){//最后一个字符为‘\n’
        char *subbuf = (char*)malloc(sizeof(char) * (strlen(buf) - 1));
        substring(buf, subbuf, 0, strlen(buf) - 1);//截取前n-1个字符
        return subbuf;
    }
    else
    {
        char *subbuf = (char*)malloc(sizeof(char) * strlen(buf));
        strcpy(subbuf, buf);
        return subbuf;
    }
}

int 
main(int argc, char *argv[])
{
    int pid;
    char buf[MAXPATH];
    char *args[MAXARG];
    char *cmd;
    /* 默认命令为echo */
    if(argc == 1){
        cmd = "echo";
    }
    else{
        cmd = argv[1];
    }
    /* 计数器 */
    int args_num = 0;
    while (1)
    {
        memset(buf, 0, sizeof(buf));
        gets(buf, MAXPATH);
        char *arg = cutoffinput(buf);
        /* press ctrl + D */
        if(strlen(arg) == 0 || args_num >= MAXARG){
            break;
        }
        args[args_num]= arg;
        args_num++;
    }

    int j;
    char *argv2exec[MAXARG];
    argv2exec[0] = cmd;

    for (j=1; j < argc; j++)
    {
        argv2exec[j] = argv[j - 2];
    }
    
    for (int i = 0; i < args_num; i++)
    {
        argv2exec[i + j] = args[i];
    }
    argv2exec[args_num + j] = 0;
    
    /* 运行命令 */
    if((pid = fork()) == 0){   
        exec(cmd, argv2exec);    
    }  
    else
    {
        wait(0);
    }
    exit(0);
}



