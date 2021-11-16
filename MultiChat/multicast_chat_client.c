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

#define MAX_READ 1024

/*struct ip_mreq_source {
struct in_addr imr_multiaddr;
/* IPv4 class D multicast addr 
struct in_addr imr_sourceaddr; /* IPv4 source addr 
struct in_addr imr_interface; /* IPv4 addr of local interface 
};*/



int main(int argc, char* argv[])
{
    char buffer[MAX_READ + 1];
    int fd=0,i=0;
    int id=0;
    int len=0;
    struct ip_mreq mreq;
    ssize_t numRead,numWrite;
    struct ip_mreq_source mreq_source;
    char class_goer[1024]="172.17.25.39";
    char *course_ip[5]={"239.0.0.1","239.0.0.2","239.0.0.3","239.0.0.4","239.0.0.5"};
    char noti[MAX_READ];
    int is_registered[5];
    for(i=0;i<5;i++)
    {
        is_registered[i]=0;
    }

    //char server_message[MAX_READ + 1];
    //char buffer[1024] = " You have succesfully sent a multicast message\n";


    /*mreq_source.imr_multiaddr.s_addr = inet_addr("239.0.0.1");
    mreq_source.imr_sourceaddr.s_addr = htonl();
*/
    int sockfd;
    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0)
    {
        printf("Error creating the socket\n");
        exit(1);
    }

    struct sockaddr_in sock_address;

    bzero((char *)&sock_address, sizeof(sock_address));
    sock_address.sin_family= AF_INET;
    sock_address.sin_port = htons(9009);
    sock_address.sin_addr.s_addr = htonl(INADDR_ANY);
    len = sizeof(sock_address);
    /*if(inet_pton(AF_INET, "172.17.25.39", &sock_address.sin_addr.s_addr) <= 0)
    {
        printf("error\n");
        exit(0);
    }*/
    //server_address.sin_addr.s_addr =htonl(655953580);        //INADDR_ANY;       //local_s1_addr.s_addr;
    //printf("server address is %d",server_address.sin_addr.s_addr);

    //printf("yes\nserver address is %d",local_s1_addr.s_addr);

    //mreq.imr_multiaddr.s_addr = inet_addr("239.0.0.1");   

    mreq_source.imr_sourceaddr.s_addr=inet_addr("172.17.25.39");
    mreq_source.imr_multiaddr.s_addr=inet_addr("239.0.0.1");
    mreq_source.imr_interface.s_addr=inet_addr("172.17.25.40");

    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    bind(sockfd, (struct sockaddr*) &sock_address, sizeof(sock_address));

    while(1)
    {
        printf("Enter the id of the course you want to register or press 0 to exit\n");
        {
            scanf("%d",&id);
            if(id == 0)
            {
                break;
            }
            else if(id > 5)
            {
                printf("There is no course with the given id\n");
            }
            else if(is_registered[id-1] == 1)
            {
                printf("You are already registered to this course\n");
            }
            else
            {
                mreq.imr_multiaddr.s_addr = inet_addr(course_ip[id-1]);
                printf("The group ip is %s\n",course_ip[id-1]);
                if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP,&mreq, sizeof(mreq)) < 0)
                {
                    printf("Error setting the socket\n");
                    exit(1);
                }
                is_registered[id-1] = 1;
                printf("Do you wat to receive notification from the class goer\n");
                scanf("%s",noti);
                if((strcmp(noti,"no") == 0) || (strcmp(noti,"NO") == 0) || (strcmp(noti,"No") == 0))
                {
                    printf("YESS\n");
                    if (setsockopt(sockfd, IPPROTO_IP, IP_BLOCK_SOURCE,&mreq_source, sizeof(mreq_source)) < 0)
                    {
                        printf("Error setting the socket\n");
                        exit(1);
                    }
                }
		
            }
        }
    }  
    printf("argc == %d\n",argc);
    if(argc > 1)
    {
        int pid=0,cid=0,xlen=0;
        pid = fork();
        if(pid == 0)
        {
            printf("YESS\n");
            ssize_t numRead,numWrite;
            //char server_message[MAX_READ + 1];
            char send[1024] = " There is a surprise test for course 3\n";
            char crseid[1024];
            int send_sockfd;
            send_sockfd = socket(AF_INET,SOCK_DGRAM,0);
            if(send_sockfd < 0)
            {
                printf("Error creating the socket\n");
                exit(1);
            }

            struct sockaddr_in send_sock_address;

            bzero((char *)&send_sock_address, sizeof(send_sock_address));
            send_sock_address.sin_family= AF_INET;
            send_sock_address.sin_port = htons(9009);
            //sock_address.sin_addr.s_addr = htonl(INADDR_ANY);
            xlen = sizeof(send_sock_address);

            while(1)
            {
                printf("Enter the course id to which you want to send the messaeg to\n");
                scanf("%d",&cid);
                if(is_registered[cid-1] == 0)
                {
                    printf("You are not registered to this course\n");
                }
                else
                {
                    //printf("Ysss\n");
                    //printf("Enter the message\n");
                    //scanf("%s",send);
                    strcpy(send,"Surprise test for course ");
                    sprintf(crseid,"%d",cid);
                    strcat(send,crseid);
                    send_sock_address.sin_addr.s_addr = inet_addr(course_ip[cid-1]);
                    printf("The course ip = %s\n",course_ip[cid-1]);
                    numWrite = sendto(send_sockfd, send, sizeof(buffer), 0,(struct sockaddr *) &send_sock_address, xlen);
                    if(numWrite <= 0)
                    {
                        printf("error sending a message to multicast group\n");
                        exit(1);
                    }
                }
            }

            exit(0);
        }
    }
    while(1)
    {
        numRead = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &sock_address, &len);
        if(numRead <= 0)
        {
            printf("Error reading message\n");
            exit(1);
        }
        else
        {
            printf("Msg received from %s\nThe message read = %sXXX\n",inet_ntoa(sock_address.sin_addr),buffer);
        }
        i++;
    }
}

