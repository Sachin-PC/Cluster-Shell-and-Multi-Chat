#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#define MAX_READ 8192


int format_command(char cwa[MAX_READ],char *command[10])
{
    int i=0,j=0,k=0,count=0,len=0;
    len = strlen(cwa);
    char cmds[10][MAX_READ];
    for(i=0;i<len-1;i++)
    {

        if(cwa[i] == '|')
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




void create_childs(int i, int count, char *commands[10], int p[2])
{
      int p_ch[2];
      int cmd_no=0;
      pipe(p_ch);
      pid_t ret;
      ret = fork();
      if(ret == 0)
      {
          i++;
          cmd_no = count - i;
          if(i < count)
          {
              create_childs(i,count,commands,p_ch);
          }
          else
          {
              //printf("the command to be executed by child %d is %s\n",i,commands[cmd_no]);

              int no_of_args=0;
              char *args[10];
              char *a_ptr = strtok(commands[cmd_no], " ");
              while (a_ptr != NULL)
              {
                args[no_of_args] = a_ptr;
                //printf("args[%d] = %s\n",no_of_args,args[no_of_args]);
                a_ptr = strtok(NULL, " ");
                no_of_args++;
              }
              args[no_of_args]=NULL;



              close(p_ch[0]);
              close(1);
              dup2(p_ch[1],1);
              close(p_ch[1]);
              //execlp ("ls", "ls", "-l", (char *) 0);
              execvp(args[0],args);
          }
      }
      else
      {
          wait(NULL);
          cmd_no = count - i;
          //printf("the command to be executed by child %d is %s\n",i,commands[cmd_no]);

            int no_of_args=0;
            char *args[10];
            char *a_ptr = strtok(commands[cmd_no], " ");
            while (a_ptr != NULL)
            {
                args[no_of_args] = a_ptr;
                //printf("args[%d] = %s\n",no_of_args,args[no_of_args]);
                a_ptr = strtok(NULL, " ");
                no_of_args++;
            }
            args[no_of_args]=NULL;



          close(p_ch[1]);
          close(0);
          dup2(p_ch[0],0);
          close(p_ch[0]);
          close(p[0]);
          close(1);
          dup2(p[1],1);
          close(p[1]);
          //execlp ("grep", "grep", "^d", (char *) 0);
          execvp(args[0],args);
      }
}

int main (int argc, char *argv[])
{
    int i=0, no_of_cmds=0,j=0,flag=0, index=0;
    char *commands[10];
    char cwa[MAX_READ];
    printf("Enter the command\n");
    fgets(cwa, MAX_READ, stdin);
    no_of_cmds = format_command(cwa,commands);
    /*for(i=0;i<no_of_cmds;i++)
    {
        printf("commands[%d] = %s\n",i,commands[i]);
    }*/

    i=1;
    int p[2];
    pid_t ret;
    pipe(p);
    ret = fork();
    if(ret == 0)
    {
        if(no_of_cmds > 1)
        {
            i++;
            create_childs(i,no_of_cmds,commands,p);
        }
    }
    else
    {
        wait(NULL);
        int cmd_no = no_of_cmds - i;
        //printf("the command to be executed by child %d is %s\n",i,commands[cmd_no]);

        int no_of_args=0;
        char *args[10];
        char *a_ptr = strtok(commands[cmd_no], " ");
        while (a_ptr != NULL)
        {
            args[no_of_args] = a_ptr;
            //printf("args[%d] = %s\n",no_of_args,args[no_of_args]);
            a_ptr = strtok(NULL, " ");
            no_of_args++;
        }
        args[no_of_args]=NULL;



        close(p[1]);
        close(0);
        dup2(p[0],0);
        close(p[0]);
        //execlp ("grep", "grep","^d", (char *) 0);
        execvp(args[0],args);

    }
}
