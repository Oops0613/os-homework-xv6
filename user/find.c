#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

//参考文件grep.c
char buf[1024];//用于暂存path
int match(char*, char*);
int matchhere(char*, char*);
int matchstar(int, char*, char*);

int
match(char *re, char *text)
{
  if(re[0] == '^')
    return matchhere(re+1, text);
  do{  // must look at empty string
    if(matchhere(re, text))
      return 1;
  }while(*text++ != '\0');
  return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
  if(re[0] == '\0')
    return 1;
  if(re[1] == '*')
    return matchstar(re[0], re+2, text);
  if(re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if(*text!='\0' && (re[0]=='.' || re[0]==*text))
    return matchhere(re+1, text+1);
  return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
  do{  // a * matches zero or more instances
    if(matchhere(re, text))
      return 1;
  }while(*text!='\0' && (*text++==c || c=='.'));
  return 0;
}
//参考文件grep.c

char* fmtname(char *path);
void find(char *path, char *re);

int 
main(int argc, char** argv){
    if(argc < 2){
      printf("Parameters are not enough\n");
    }
    else{
      //在路径path下递归搜索文件 
      find(argv[1], argv[2]);
    }
    exit(0);
}

// 对ls中的fmtname，去掉了空白字符串
char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  if(strlen(p) >= DIRSIZ)
    return p;
  memset(buf, 0, sizeof(buf));
  memmove(buf, p, strlen(p));
  return buf;
}

void 
find(char *path, char *re){
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  
  if((fd = open(path, 0)) < 0){//判断是否打开成功
      fprintf(2, "find: cannot open %s\n", path);
      return;
  }

  if(fstat(fd, &st) < 0){//确认文件类型
      fprintf(2, "find: cannot stat %s\n", path);
      close(fd);
      return;
  }
  
  switch(st.type)//根据文件类型打印不同信息
  {
  case T_FILE://是文件
      if(match(re, fmtname(path)))//判断名字是否匹配
          printf("%s\n", path);
      break;
  case T_DIR://是目录
      if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
          printf("find: path too long\n");
          break;
      }
      strcpy(buf, path);
      p = buf + strlen(buf);//移动到字符串尾部并加上‘/’
      *p++ = '/';
      while(read(fd, &de, sizeof(de)) == sizeof(de)){
          if(de.inum == 0)
              continue;
          memmove(p, de.name, DIRSIZ);//拼接字符串
          p[DIRSIZ] = 0;
          if(stat(buf, &st) < 0){
              printf("find: cannot stat %s\n", buf);
              continue;
          }
          char* lstname = fmtname(buf);
          if(strcmp(".", lstname) == 0 ||
           strcmp("..", lstname) == 0){
            continue;
          }
          else{
            find(buf, re);//在下一层递归查询
          }
      }
      break;
  }
  close(fd);
}
