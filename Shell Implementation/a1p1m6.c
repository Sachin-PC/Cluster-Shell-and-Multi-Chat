#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <sys/mman.h>
#define MAX_READ 8192

int main(int argc, char* argv[])
{
    int i=0,len=0;;
    char command[1024];
    printf("Enter the command\n");
    fgets(command,1024,stdin);
    //rintf("The input is %sAAA\n",command);
    len = strlen(command);
    //printf("len = %d\n",len);
    //command[len-1] = '\0';
    command[len-1] ='\0';
    //printf("The input is %sAAA\n",command);
    int no_of_args=0;
    char *args[10];
    char *p = strtok(command, " ");
    p = strtok(NULL, " ");
    while (p != NULL)
    {
        args[no_of_args] = p;
        printf("args[%d] = AA%sAA\n",no_of_args,args[no_of_args]);
        p = strtok(NULL, " ");
        no_of_args++;
    }
    args[no_of_args]=NULL;
    //printf("YES\n");
    i=0;
    /*while(args[i] != NULL)
    {
        printf("AAAA%sAAA\n",args[i]);
        i++;
    }*/
    /*int x = execvp(args[0],args);
    if(x == -1)
    {
        perror("exec errrr\n");
    }*/
    pid_t ret,sid;
    ret = fork();
    if(ret == -1)
    {
        printf("Error creating a child\n");
    }
    else if(ret > 0)
    {
        exit(0);
    }
    else
    {
        umask(0);
        sid = setsid();
        if(sid < 0)
        {
            printf("Error craeting a new session\n");
            exit(0);
        }
        else
        {
            if(chdir("/") < 0 )
            {
                printf("Error while changing the directory\n");
                exit(0);
            }
            else
            {
                close(STDIN_FILENO);
                close(STDOUT_FILENO);
                close(STDERR_FILENO);
                args[no_of_args]=NULL;
                //execv(args[0],args);
                execvp(args[0],args);
                //execlp ("ls", "ls", "-l", (char *) 0);
            }
        }
    }

}
