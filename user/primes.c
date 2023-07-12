#include "kernel/types.h"
#include "user.h"

void getPrime(int pd[])//递归地产生质数
{
    int p,q,i=0,j,pid;
    int nums[34];
    int pd_child[2];
    pipe(pd_child);
    
    close(pd[1]);//关闭写端
    int len = read(pd[0], &p, sizeof(p));//len用于判断是否读到数据
    printf("prime %d\n", p);//p是第一个数  

    while(len)
    {
        len=read(pd[0],&q,sizeof(q));//继续从管道中读
        if(q%p!=0)//q不是p的倍数，可以保留
        {
            nums[i++]=q;
        }
    }
    if(i==0)//所有元素均被消去
    {
        exit(0);
    }
    
    close(pd[0]);//关闭读端
    
    if((pid=fork())==0)
    {
        getPrime(pd_child);
    }
    else
    {
        for(j=0;j<i;j++)//将数据写入管道
        {
            write(pd_child[1],&nums[j],sizeof(nums[i]));
        }
        exit(0);
    }
}

int
main(int argc, char** argv)
{
    int pid;
    int p[2];
    pipe(p);

    if((pid = fork()) == 0)//子进程
    {
        getPrime(p);
        exit(0);
    }
    else//父进程（需要初始化）
    {
        int nums[34];
        int i = 0;
        for(i=0; i <= 33; i++)//生成数据
        {
            nums[i] = i+2;
        } 
        close(p[0]);//关闭读端
        for(i=0;i<34;i++)//将数据写入管道
        {
            write(p[1],&nums[i],sizeof(nums[i]));
        }
        exit(0);
    }
}