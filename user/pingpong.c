#include "kernel/types.h"
#include "user/user.h"

#define READ_END 0
#define WRITE_END 1

int
main(int argc, char** argv)
{
    int pid;
    int parent_fd[2];
    int child_fd[2];
    char buffer[16];

    pipe(parent_fd);
    pipe(child_fd);

    if((pid = fork()) == 0)//pid=0为子进程
    {
        close(parent_fd[WRITE_END]);
        read(parent_fd[READ_END],buffer,4);
        printf("%d: received %s\n",getpid(), buffer);
        close(child_fd[READ_END]);
        write(child_fd[WRITE_END], "pong", sizeof(buffer));
        exit(0);
    }
    else
    {
        close(parent_fd[READ_END]);
        write(parent_fd[WRITE_END],"ping",sizeof(buffer));
        close(child_fd[WRITE_END]);
        read(child_fd[READ_END],buffer,4);
        printf("%d: received %s\n",getpid(), buffer);
        exit(0);
    }
}