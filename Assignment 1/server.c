#include <stdio.h>
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <string.h>    //strlen
#include <unistd.h>    //write
#include <pthread.h> //for multithreaded processes
#define PORT 8888
#define SIZE 20000

//the thread function
void *connection_handler(void *);
 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket!");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("Bind failed!");
        return 1;
    }
    puts("Bind completed");
     
    //Listen
    listen(socket_desc , 50);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted!");
         
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
         
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("Could not create thread!");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }
     
    if (client_sock < 0)
    {
        perror("Failed to accept!");
        return 1;
    }
     
    return 0;
}
 
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    // int read_size;
    // char *message;
    char client_message[SIZE];
     
    //Send some messages to the client
    // message = "You have been successfully authenticated!";
    // write(sock , message , strlen(message));
     
    // message = "Now type something and i shall repeat what you type \n";
    // write(sock , message , strlen(message));
     
    //Receive a message from client
   /* while( (read_size = recv(sock , client_message , SIZE , 0)) > 0 )
    {
        //Send the message back to client
        write(sock , client_message , strlen(client_message));
    }
     */

    recv(sock , client_message , SIZE, 0);
    printf("User %s connected\n", client_message);

    char tmp[SIZE];

    strcpy(tmp,"Connected to server.");
    send(sock, tmp, SIZE,0);

/*
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }*/
         
    //Free the socket pointer
    free(socket_desc);
     
    return 0;
}