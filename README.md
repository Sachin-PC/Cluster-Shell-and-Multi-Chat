# Cluster-Shell-and-Multi-Chat

Shell Design Document 
Custom bash like Shell is created, which will take the command that can have multiple arguments. Fork will create a child process for the command and then execv call is used to execve a new program for that particular command. The new program will completely overwrite the child. 
1. User can enter a command with multiple arguments.
2. shell supports any number of commands in the pipeline. e.g. ls|wc|wc|wc. Prints details 
such as pipe fds, process pids and the steps. Redirection operators can be used in combination with 
pipes.
3. Shell supports # operator. The meaning of this: it carries same semantics as pipe but use 
message queue instead of pipe. The operator ## works in this way: ls ## wc , sort. output of 
ls is replicated to both wc and sort using message queues
4. Shell supports S operator. The meaning of this: it carries same semantics as pipe but use 
shared memory instead of pipe. The operator SS works in this way: Using example, ls SS wc, 
sort. Output of ls is replicated to both wc and sort using shared memory
5. shell supports <, >, and >> redirection operators. Prints details such as fd of the file, 
remapped fd.
6. The shell will support multiple commands in the pipeline by manipulating the file descriptors associated with the pipes. Redirection is also handled by the help of descriptors. 
7. ##: Implementing a command like "ls ## wc", a message queue is used. The output of ls will be stored in the message queue buffer pointed to by the message queue identifier and wc or any other commands (there can be more than one command also ) will receive the input from the message queue into the pipe that will be fed into the command. 
8. SS: Similarly, in this operation, the output of one command will be passed to the subsequent commands with the help of shared memory. 

Cluster Shell

Shell features are extended to a cluster of machines, each identified by a name. Assume that N nodes in the cluster are named as n1, n2 ….. nN.
Cluster shell is run on any one of the nodes. When a command is run e.g. ls it executed on the local system. When n2.ls is run in n1, it is executed on node n2 and the output is listed on n1.
When n*.ls is run on n1, ls is run on all nodes and output is displayed on n1. This applies to other commands as well. By default, all commands on a remote node are executed in the home 
directory of the user logged in n1. 
When n2.cd <path> or n*.cd <path> is executed, directory is changed.
1. When the command n1.cat file|n2.sort|n3.uniq is executed on n5, the commands are executed on different nodes taking input from the previous command but the last output is displayed on the node n5 it is executed on. 
2. The command nodes displays the list of nodes (name, ip) currently active in the cluster.
3. Concurrent server is used to establish communication between various nodes. 
4. If there is a pipelining command, then first of all, the client will send the command to the server, the server will execute it and pass the results to the client, and then the client will send the results to the new server, which will execute the command with the inputs coming from the client, but the results will be displayed in the node on which the command was executed initially. All this is done through sockets, specifically Internet domain IPV4 sockets. 
5. Active connections are checked and printed when the nodes command is invoked, showing the name and the IP which are alive on the cluster.

MultiChat Application

A multi chat application is created where users can subscribe to different course groups and can publish or receive messages in the subscribed groups
Each course is assigned a unique Multicast Class D address.
1. Multiple students can register for a course using course name or
course id and each student can register for multiple courses.
2. We are adding the students to join the course using
IP_ADD_MEMBERSHIP to join a specific multicast group
3. We are dropping a student from a course with
IP_DROP_MEMBERSHIP.
4. The registered students of the course can get the notification
from the student who is attending the class using the following
socket options:
*IP_BLOCK_SOURCE
*IP_UNBLOCK_SOURCE
