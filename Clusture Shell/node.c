#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <string.h>
#include<sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/msg.h>
#include <mqueue.h>
#include<errno.h>

#define MAX_READ 819200

struct message {
    long mtype;
    char mtext[MAX_READ];
};

void create_server( char *ip, long port_num)
{
    char buffer[MAX_READ + 1];
    int fd;
    ssize_t numRead,numWrite;
    char server_message[MAX_READ + 1];

    int server_socket;
    server_socket = socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in server_address;
    server_address.sin_family= AF_INET;
    server_address.sin_port = htons(port_num);
    if(inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr.s_addr) <= 0)
    {
        printf("error\n");
        exit(0);
    }

    bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));


    //listen(server_socket, 5);


    int msqid = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    int flags=0;
    int client_socket;
    while(1)
    {
        struct message recvd_msg;
        if (msgrcv(msqid, &recvd_msg, MAX_READ, 0,IPC_NOWAIT) == -1)
        {
            if(errno == 42)
            {
                //printf("Path has not yet changed\n");
            }
            else
            {
                perror("msgrcv");
                return EXIT_FAILURE;
            }
        }
        else
        {
            char *pt;
            strcpy(pt,recvd_msg.mtext);
            if (chdir(pt) != 0)
            {
                printf("Error while changing Node 1 Path in Parent\n");
            }
            else
            {
                    printf("Server directory is changed to %s\n",recvd_msg.mtext);
                    char temp[1024];
                    //printf("Current working directory = %s\n", getcwd(temp, 100));
            }
        }

        listen(server_socket, 5);

        client_socket = accept(server_socket, NULL, NULL);
        if(client_socket == -1)
        {
            printf("Error in Connecting with the client\n");
        }
        else
        {
            if(fork() == 0)
            {
                char command[4][1024];
                char reply[1024];
                recv(client_socket, command, MAX_READ+1, 0);
                //printf("The received message is %sNN\nis_path = %d",command[0],command[1][0]);
                char s[100];
                if(command[1][0] == 1)
                {
                    //char *path;
                    //strcpy(path,command[0])
                    //char *pts;
                    //strcpy(pts,command[0]);
                    char *trsp;
                    trsp = command[0] + 3;
                    //strcpy(trsp,command[0]);
                    //printf("pts = BB%sAAA\n",trsp);
                    if (chdir(trsp) != 0)
                    {
                        strcpy(reply,"Error while changing Node 1 Path\n");
                    }
                    else
                    {
                        strcpy(reply,"Node 1 Path changed Succesfully\n");
                        struct message msg;
                        strcpy(msg.mtext,trsp);
                        msg.mtype=1;
                        if (msgsnd(msqid, &msg, sizeof(long) + (strlen(msg.mtext) * sizeof(char)) + 1, 0) == -1)
                        {
                            printf("Error in sending message\n");
                            perror("msgsnd");
                            //return EXIT_FAILURE;
                        }



                    }
                    send(client_socket, reply, sizeof(reply),0);
                    //printf("The current Directory is %s\n", getcwd(s, 100));

                }
                else if(command[1][0] == 2)
                {
                        //printf("YESSSSSSSSS\n");
                }
                else
                {
                    //char ttt[1024];
                    //printf("The current working directory in child = %s\n",getcwd(ttt, 100));
                    int no_of_args=0;
                    char *args[10];
                    char *a_ptr = strtok(command[0], " ");
                    while (a_ptr != NULL)
                    {
                        args[no_of_args] = a_ptr;
                        //printf("args[%d] = %sLL\n",no_of_args,args[no_of_args]);
                        a_ptr = strtok(NULL, " ");
                        no_of_args++;
                    }
                    args[no_of_args]=NULL;

                    int pr_p[2];
                    pipe(pr_p);
                    pid_t ret=0;
                    ret =fork();
                    if(ret < 0)
                    {
                        printf("Error in Forking\n");
                        exit(0);
                    }
                    else if(ret == 0)
                    {
                        close(pr_p[0]);
                        close(1);
                        dup2(pr_p[1],1);
                        close(pr_p[1]);
                        if(command[2][0] == 1)
                        {
                            //printf("Input needed\n");
                            //printf("Input is %s\n",command[3]);
                            int ch_p[2];
                            pipe(ch_p);

                            numWrite=0;
                            //char buffer[1024];
                            numWrite = write(ch_p[1], command[3], strlen(command[3]));
                            //printf("numWrite = %d\n",numWrite);
                            if(numWrite == -1)
                            {
                                printf("Error in writing file\n");
                                exit(0);
                            }
                            close(ch_p[1]);
                            close(0);
                            dup2(ch_p[0],0);
                            close(ch_p[0]);
                        }
                        //execlp ("ls", "ls", "-l", (char *) 0);
                        execvp(args[0],args);
                    }
                    else
                    {
                        wait(NULL);
                        close(pr_p[1]);
                        //printf("YESS\n");
                        numRead=0;
                        char cmd_output[MAX_READ];
                        numRead = read(pr_p[0], cmd_output, MAX_READ);
                        if(numRead == -1)
                        {
                            printf("Error in Reading the file\n");
                            exit(0);
                        }
                        else
                        {
                            printf("%s\n",cmd_output);
                        }
                        send(client_socket, cmd_output, sizeof(cmd_output),0);
                    }
                }
                close(client_socket);
                exit(0);
            }
            else
            {
                //wait()
                sleep(1);
                if (msgrcv(msqid, &recvd_msg, MAX_READ, 0,IPC_NOWAIT) == -1)
                {
                    if(errno == 42)
                    {
                        //printf("Path has not yet changed\n");
                    }
                    else
                    {
                        perror("msgrcv");
                        return EXIT_FAILURE;
                    }
                }
                else
                {

                    if (chdir(recvd_msg.mtext) != 0)
                    {
                        printf("Error while changing Node 1 Path in Parent\n");
                    }
                    else
                    {
                            printf("Server Directory is changed to %s\n",recvd_msg.mtext);
                    }
                }
            }
        }
    }
    close(server_socket);

}

int create_client(char *ip,long port_num, char *server_response, char *command, int is_path, char *inp, int is_inp)
{
    int network_socket;
    network_socket = socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in server_address;
    server_address.sin_family= AF_INET;
    server_address.sin_port = htons(port_num);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");     // htonl(server_address.sin_addr.s_addr);

    int connection_status = connect(network_socket, (struct sockaddr*) &server_address, sizeof(server_address));
    if(connection_status == -1 )
    {
        //printf("There was an error making a connection to the remote socket\n");
        return connection_status;
        //exit(0);
    }

    /*struct message msg;
    strcpy(msg.command,command);
    msg.is_path = is_pth;*/

    //printf("is_path to be sent is %d\n",is_path);
    char msg[4][1024];
    char reply[1024];
    strcpy(msg[0],command);
    msg[1][0] = is_path;
    //printf("msg[1][0] = %d\n",msg[1][0]);
    msg[2][0] = is_inp;
    strcpy(msg[3],inp);
    send(network_socket, msg, MAX_READ,0);
    if(is_path == 2)
    {
            close(network_socket);
    }
    else
    {
    recv(network_socket, server_response, MAX_READ+1, 0);

    close(network_socket);
    }
}


void set_nodes_ip(char *ip[3], struct in_addr *n_ip, int num_nodes)
{
    struct in_addr ip_inbin;
    int c_ip=0,i=0;
    for(i=0;i<num_nodes;i++)
    {
        c_ip = inet_pton(AF_INET, ip[i], &ip_inbin);
        if(c_ip == 0)
        {
            printf("%s is not in presentation form\n",ip[i]);
            exit(0);
        }
        else
        {
            n_ip[i].s_addr = ip_inbin.s_addr;
        }
    }
}

void execute_localcommand(char *cmd,int is_path, char *server_response, int i, char *inp)
{
        //printf("Inside local\n");
        //printf("i = %d and inp = %s\n",i,inp);
        int op_p[2];
        pipe(op_p);
        pid_t ret;
        ret = fork();
        if(ret == 0)
        {
            char *c;
            int no_of_args=0;
            char *args[10];
            char *a_ptr = strtok(cmd, " ");
            while (a_ptr != NULL)
            {
                args[no_of_args] = a_ptr;
                a_ptr = strtok(NULL, " ");
                no_of_args++;
            }
            args[no_of_args]=NULL;

            if(i > 0)
            {
                int ch_p[2];
                pipe(ch_p);
                int numWrite=0;
                char buffer[1024];
                strcpy(buffer,inp);
                numWrite = write(ch_p[1], buffer, strlen(buffer));
                //printf("numWrite = %d\n",numWrite);
                if(numWrite == -1)
                {
                    printf("Error in writing file\n");
                    exit(0);
                }
                close(ch_p[1]);
                close(0);
                dup2(ch_p[0],0);
                close(ch_p[0]);
            }

            close(op_p[0]);
            //lseek(1,0,0);
            close(1);
            dup2(op_p[1],1);
            close(op_p[1]);
            execvp(args[0],args);
            exit(0);
        }
        else
        {
            wait(NULL);
            close(op_p[1]);
            int numRead=0;
            //char cmd_output[MAX_READ];
            numRead = read(op_p[0], server_response, MAX_READ);
            if(numRead == -1)
            {
                printf("Error in Reading the file\n");
                exit(0);
            }
        }
}


int main(int argc, char* argv[])
{

    int num_nodes=3,con_st;
    struct in_addr n_ip[3];
    char *ip[3] = {"172.17.25.39","172.18.16.116","172.18.16.120"};
    char *local_ip = "172.17.25.39";
    long port_nums[3] = {9000,9001,9002};
    long local_portnum = 9000;
    char server_response[MAX_READ+1],inp[MAX_READ + 1];
    struct in_addr ip_inbin;
    struct in_addr local_s1_addr;
    int i=0,j=0,k=0,c_ip=0,is_server=0,is_client=0,is_local=0,execute_onallnodes=0,is_path=0,is_inp=0,is_check=0;
    int total_arguments=0, start_arguments=0;
    char command[MAX_READ+1];

    set_nodes_ip(ip,n_ip,num_nodes);   //Setting Clusture Nodes IP's

    c_ip = inet_pton(AF_INET, local_ip, &ip_inbin); //Converting IP in presentation form to network(binary) form.
    if(c_ip == 0)
    {
        printf("%s is not in presentation form\n",local_ip);
        exit(0);
    }
    local_s1_addr.s_addr = ip_inbin.s_addr;

    if(argc == 1)
    {
        printf("This is a Server\n");
        create_server(local_ip,local_portnum);
    }
    else
    {
        while(1)
        {
            char *cmd;
            //char *cmd_pts[10];
            printf("\n>");
            fgets(command, MAX_READ, stdin);
            //printf("The command is %s\n",command);
            int cmd_len=0,is_np=0,no_of_cmds=0;
            cmd_len = strlen(command);
            command[cmd_len - 1]='\0';
            cmd_len--;
            //printf("command length = %d\n",cmd_len);
            int cmd_no=0;
            char *cmd_pts[10];
            char *c_ptr = strtok(command, "|");
            while (c_ptr != NULL)
            {
                cmd_pts[cmd_no] = c_ptr;
                //printf("cmds[%d] = %s\n",cmd_no,cmd_pts[cmd_no]);
                c_ptr = strtok(NULL, "|");
                cmd_no++;
            }
            cmd_pts[cmd_no]=NULL;
            cmd_len=0;
            char *temp;
            for(i=0;i<cmd_no;i++)
            {
                if(cmd_pts[i][0] == ' ')
                {
                    temp = cmd_pts[i] + 1;
                    cmd_pts[i] = temp;
                }
                cmd_len = strlen(cmd_pts[i]);
                if(cmd_pts[i][cmd_len-1] == ' ')
                {
                    cmd_pts[i][cmd_len-1] = '\0';
                }
                //printf("Command %d = AAA%sBBB\n",i,cmd_pts[i]);

            }

            no_of_cmds = cmd_no;
            for(i =0;i<no_of_cmds;i++)
            {

            //printf("Command %d is %s\n",i,cmd_pts[i]);

            strcpy(command,cmd_pts[i]);
            is_path=0;
            is_client=0;
            is_local=0;
            is_np=0;
            is_inp=0;
            is_check=0;
            execute_onallnodes = 0;
            if(i == 0)
            {
                is_inp =0;
            }
            else
            {
                is_inp = 1;
                strcpy(inp,server_response);
                //printf("The input is %s\n",inp);
            }
            //server_response[0]='\0';
            memset(server_response,0,MAX_READ);
            //execute_onallnodes = 0;
            if(cmd_len <= 3)
            {
                is_local = 1;
            }
            else
            {
                if(cmd_len >= 6)
                {
                    //printf("NNNNNNNN\ncommand = %s\n",command);
                    if(command[0] =='n' && command[1] == 'o' && command[2] == 'd' && command[3] == 'e' && command[4] =='s')
                    {
                        is_check = 1;
                    }
                }
                if(command[0] == 'n' && is_check == 0)
                {
                    if(command[1] >= '1' && command[1] <= '3')
                    {
                            is_np = 1;
                            if(local_s1_addr.s_addr == n_ip[command[1]-48-1].s_addr)
                            {
                                //printf("This command has to be executed in local node\n");
                                is_local = 1;
                            }
                            else
                            {
                                is_client = 1;
                            }
                    }
                    if(command[1] == '*')
                    {
                        execute_onallnodes = 1;
                        is_np = 1;
                    }
                }
            }

            //printf("is_check = %d\n",is_check);

            if(cmd_len < 6)
            {
                is_path = 0;
            }
            else if(is_np == 1)
            {
                if(command[3] == 'c' && command[4] == 'd' && command[5] ==' ')
                {
                    is_path = 1;
                }
                else
                {
                    is_path = 0;
                }
            }

            printf("is_client = %d\n",is_client);

            if(is_check == 1)
            {
                int count=0;

                for(i=0;i<3;i++)
                {

                    if(local_portnum == port_nums[i])
                    {

                    }
                    else
                    {

                        con_st = create_client(ip[i],port_nums[i],server_response,"dummy",2,"dummy",0);
                        if(con_st == -1)
                        {
                            //printf("Error making connection with the remote server %d\n",i+1);
                            count++;
                        }
                        else
                        {
                            printf("Node %d with IP %s is active\n",i+1,ip[i]);
                        }
                    }
                    //printf("i = %d\tcount = %d\n",i,count);
                }
                if(count == 2)
                {
                    printf("There is no active server\n");
                }
                printf("\n\n");
            }
            else if(execute_onallnodes == 1)
            {
                cmd = command + 3;
                for(i=0;i<3;i++)
                {
                    //char resp[1024];
                    if(local_portnum == port_nums[i])
                    {
                        //printf("No\n");
                        printf("Node %d output:\n",i+1);
                        if(is_path == 1)
                        {
                            char *tmpall;
                            tmpall = cmd+3;
                            //printf("the path is %s\n",tmpall);
                            if (chdir(tmpall) != 0)
                            {
                                printf("Error while changing Node 1 Path in Parent\n");
                            }
                            else
                            {
                                    printf("Server Directory is changed to %s\n",tmpall);
                            }
                        }
                        else
                        {
                                execute_localcommand(cmd,is_path,server_response,0,inp);
                                printf("%s\n",server_response);
                        }
                        //execute_command(response);
                    }
                    else
                    {
                        con_st = create_client(ip[i],port_nums[i],server_response,cmd,is_path,inp,0);
                        if(con_st == -1)
                        {
                            printf("Error making connection with the remote server %d\n",i+1);
                        }
                        else
                        {
                            printf("Node %d output :\n%s\n",i+1,server_response);
                        }
                    }
                    printf("\n\n");
                }

            }
            else if(is_client == 1)
            {
                cmd = command + 3;
                printf("The command to be sent is %sAAA and is_path = %d\n",cmd,is_path);
                con_st = create_client(ip[command[1]-48-1],port_nums[command[1]-48-1],server_response,cmd,is_path,inp,is_inp);
                if(con_st == -1)
                {
                    printf("There was an error making a connection to the remote socket\n");
                    if(no_of_cmds > 1)
                    {
                        break;
                    }
                }
                //printf("THE SERVER RESPONSE IS %s\n",server_response);
            }
            else
            {
                //printf("This is local \nis_path = %d\n",is_path);
                if(is_np == 0)
                {
                    //printf("is_path =%d\n",is_path);
                        if(command[0] == 'c' && command[1] == 'd' && command[2] == ' ')
                        {
                            char *tmp;
                            tmp = command+3;
                            if (chdir(tmp) != 0)
                            {
                                printf("Error while changing Node 1 Path in Parent\n");
                            }
                            else
                            {
                                    printf("Server Directory is changed to %s\n",tmp);
                            }
                        }
                        else
                        {
                            execute_localcommand(command,is_path,server_response,i,inp);
                        }
                }
                else
                {
                    cmd = command+3;
                    if(is_path == 1)
                    {
                            char *tmp;
                            tmp = cmd+3;
                            //printf("the path is %s\n",tmp);
                            if (chdir(tmp) != 0)
                            {
                                printf("Error while changing Node 1 Path in Parent\n");
                            }
                            else
                            {
                                    printf("Server Directory is changed to %s\n",tmp);
                            }
                    }
                    else
                    {
                            execute_localcommand(cmd,is_path,server_response,i,inp);
                    }
                }
                //printf("The server response for local output is %sAAAAAAAAAAAAAAAAAAA\n",server_response);
            }
            }
            if(con_st != -1 && execute_onallnodes != 1)
            {
                    printf("%s\n",server_response);
            }
        }
    }
}
