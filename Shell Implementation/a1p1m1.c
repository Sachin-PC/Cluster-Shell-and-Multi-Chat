#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/wait.h>
#define MAX_READ 8192

int main(int argc, char *argv[])
{

    int num=0;
    while(1)
    {
        int len=0;
        char command[MAX_READ];
        printf("Enter the command\n");
        fgets(command, MAX_READ, stdin);
        len = strlen(command);
        command[len-1] ='\0';
        char path[100]="/bin/";
        //strcat(path,command);
        printf("The command is %s\n",path);

        pid_t ret;
        ret = fork();
        if(ret == 0)
        {
            printf("YES\n");
            int no_of_args=0,i=0;
            char *args[10];
            char *p = strtok(command, " ");
            while (p != NULL)
            {
                args[no_of_args] = p;
                p = strtok(NULL, " ");
                no_of_args++;
            }
            args[no_of_args]=NULL;
            char pt[1024] = "/bin/";
            char temp[1024];
            strcpy(temp,args[0]);
            //printf("temp =%s\n",temp);
            strcat(pt,temp);
            //printf("The pt is %s\n",pt);
            args[0]=pt;

            //printf("argVec[0] = %sAA\n",args[0]);
            //printf("argVec[1] = %sAA\n",args[1]);
            execv(args[0],args);
        }
        else
        {
            wait(NULL);

        }


    }

}
