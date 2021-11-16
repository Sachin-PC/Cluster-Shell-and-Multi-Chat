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
#define MAX_READ 8192

struct message {
    long mtype;
    char mtext[MAX_READ];
};




int format_command(char cwa[MAX_READ],char *command[10], int *flag, int *index)
{
    int i=0,j=0,k=0,count=0,len=0;
    len = strlen(cwa);
    char cmds[10][MAX_READ];
    for(i=0;i<len-1;i++)
    {

        if(cwa[i] == '#')
        {

            if(i == len - 2 || i == len -1)
            {
                printf("Invalid input\n");
                exit(0);
            }
            else if(cwa[i+1] == '#')
            {
                *flag = 1;
                //cmds[k][j] = '\0';
                *index = i+3;
                i=len-1;

            }
            else
            {
                cmds[k][j] = '\0';
                k++;
                j=0;
                i = i+1;
            }
        }
        else if(cwa[i] == ' ')
        {
            if(i != len-2)
            {
                if(cwa[i+1] != '|')
                {
                    cmds[k][j] = cwa[i];
                    j++;
                }
                else
                {
                    /*cmds[k][j] = '\0';
                    k++;
                    j=0;*/
                }
            }
        }
        else
        {
            cmds[k][j] = cwa[i];
            j++;
        }
    }
    cmds[k][j]='\0';
    for(i=0;i<=k;i++)
    {
        command[i]=cmds[i];
    }
    command[i]=NULL;
    return k+1;
}



int format_comma_seperated_commands(char cwa[MAX_READ],char *command[10])
{
    int i=0,j=0,k=0,count=0,len=0;
    len = strlen(cwa);
    char cmds[10][MAX_READ];
    for(i=0;i<len-1;i++)
    {

        if(cwa[i] == ',')
        {

            if(i == len - 2 || i == len -1)
            {
                printf("Invalid input\n");
                exit(0);
            }
            else
            {
                cmds[k][j] = '\0';
                k++;
                j=0;
                i = i+1;
            }
        }
        else if(cwa[i] == ' ')
        {
            if(i != len-2)
            {
                if(cwa[i+1] != ',')
                {
                    cmds[k][j] = cwa[i];
                    j++;
                }
                else
                {
                    /*cmds[k][j] = '\0';
                    k++;
                    j=0;*/
                }
            }
        }
        else
        {
            cmds[k][j] = cwa[i];
            j++;
        }
    }
    cmds[k][j]='\0';
    for(i=0;i<=k;i++)
    {
        command[i]=cmds[i];
    }
    command[i]=NULL;
    return k+1;
}



void forward_output(int *commands[10] , int msqid, int no_of_commands, char main_cmd[100])
{
    //printf("Inside forward_output\nmain command = %s\n",main_cmd);
    int i=0;
    /*for(i=0;i<no_of_commands;i++)
    {
        printf("commands[%d] =%sYES\n",i,commands[i]);
    }*/

    struct message input_data;
    if (msgrcv(msqid, &input_data, MAX_READ, 1, 0) == -1)
    {
        perror("msgrcv");
        return EXIT_FAILURE;
    }
    /*else
    {
        //printf("The message from message queue is %s\n",input_data.mtext);
    }*/



    for(i=0;i<no_of_commands;i++)
    {

        int fd_inp=0,fd_oup=0;
        fd_inp = open("input.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        fd_oup = open("output.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        int numWrite=0;
        //char buffer[1024];
        numWrite = write(fd_inp, input_data.mtext, strlen(input_data.mtext));
        //printf("numWrite = %d\n",numWrite);
        if(numWrite == -1)
        {
            printf("Error in writing file\n");
            exit(0);
        }
        pid_t ret=0;
        ret = fork();
        if(ret == 0)
        {
            int no_of_args=0;
            char *args[10];
            char *p = strtok(commands[i], " ");
            while (p != NULL)
            {
                args[no_of_args] = p;
                //printf("args[%d] = %s\n",no_of_args,args[no_of_args]);
                p = strtok(NULL, " ");
                no_of_args++;
            }
            //printf("\n");
            args[no_of_args]=NULL;
            close(0);
            dup2(fd_inp,0);
            lseek(0,0,0);
            close(1);
            dup2(fd_oup,1);
            execvp(args[0],args);
        }
        else
        {
            wait(NULL);
            char recvd_msg[MAX_READ];
            int numRead=0;
            lseek(fd_oup,0,0);
            numRead = read(fd_oup, recvd_msg, MAX_READ);
            //printf("numRead = %d",numRead);
            if (numRead == -1)
            {
                printf("Error in reading file\n");
                exit(0);
            }
            else
            {
                    recvd_msg[numRead] = '\0';

                    printf("The output of command %s%s is :\n%s\n",main_cmd,commands[i],recvd_msg);
            }

        }
    }


}






int main(int argc, char* argv[])
{
    int i=0, no_of_cmds=0,j=0,flag=0, index=0;
    char *commands[10];
    char cwa[MAX_READ];
    printf("Enter the command\n");
    fgets(cwa, MAX_READ, stdin);
    no_of_cmds = format_command(cwa,commands,&flag, &index);
    //printf("flag = %d\t index = %d\n",flag,index);


    int msqid = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (msqid == -1) {
        perror("msgget");

        return EXIT_FAILURE;
    }
    struct message message;
    memset(&(message.mtext), 0, MAX_READ * sizeof(char));

    for(i=0;i<no_of_cmds;i++)
    {
        char cmd[MAX_READ];
        pid_t ret;
        int j=0;
        int fd_oup =0;
        int fd_inp=0;
        //printf("command[%d] = %s\n",i,commands[i]);
        fd_oup = open("output.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        fd_inp = open("input.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

        ret = fork();
        if(ret == 0)
        {
            j=0;
            int no_of_args=0;
            char *args[10];
            char *p = strtok(commands[i], " ");
            while (p != NULL)
            {
                args[no_of_args] = p;
                p = strtok(NULL, " ");
                no_of_args++;
            }
            args[no_of_args]=NULL;


            if(i != 0)
            {
                struct message input_data;
                if (msgrcv(msqid, &input_data, MAX_READ, 1, 0) == -1)
                {
                    perror("msgrcv");
                    return EXIT_FAILURE;
                }
                //printf("message received by child is= %s\n", input_data.mtext);

                int numWrite=0;
                //char buffer[1024];
                numWrite = write(fd_inp, input_data.mtext, strlen(input_data.mtext));
                //printf("numWrite = %d\n",numWrite);
                if(numWrite == -1)
                {
                    printf("Error in writing file\n");
                    exit(0);
                }
                close(0);
                dup2(fd_inp,0);
                lseek(0,0,0);
            }
            close(1);
            dup2(fd_oup,1);
            execvp(args[0],args);
        }
        else
        {
            wait(NULL);
            char recvd_msg[MAX_READ];
            int numRead=0;
            lseek(fd_oup,0,0);
            numRead = read(fd_oup, recvd_msg, MAX_READ);
            //printf("numRead = %d",numRead);
            if (numRead == -1)
            {
                printf("Error in reading file\n");
                exit(0);
            }
            else
            {
                    recvd_msg[numRead] = '\0';
                    //printf("\nThe input data was: %s\n", recvd_msg);
                    message.mtype = 1;
                    strcpy(message.mtext,recvd_msg);
                    if (msgsnd(msqid, &message, sizeof(long) + (strlen(message.mtext) * sizeof(char)) + 1, 0) == -1)
                    {
                        printf("Error in sending message for i = %d",i);
                        perror("msgsnd");
                        return EXIT_FAILURE;
                    }
            }

        }

    }


    if(flag == 0)
    {
        struct message result;
        if (msgrcv(msqid, &result, MAX_READ, 1, 0) == -1)
        {
            perror("msgrcv");
            return EXIT_FAILURE;
        }
        else
        {
            printf("%s\n",result.mtext);
        }
    }
    else
    {

        char main_cmd[100];
        for(i=0;i<index-2;i++)
        {
            main_cmd[i]=cwa[i];
        }
        main_cmd[i]=' ';
        i++;
        main_cmd[i]='\0';
        char *sub_cmds;
        sub_cmds = cwa + index;
        char *scnd_cs[10];
        no_of_cmds = format_comma_seperated_commands(sub_cmds,scnd_cs);
        forward_output(scnd_cs,msqid, no_of_cmds, main_cmd);

    }
    /*if(mq_close(msqid) == -1)
    {
        printf("Error in closing the message queue\n");
        exit(0);
    }*/

    return 0;
}
