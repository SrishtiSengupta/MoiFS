#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <libgen.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>		//socket
#include <arpa/inet.h> 		//inet_addr
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PORT 8888
#define SIZE 20000

void remove_newline_char(char* input){
	input[strcspn(input, "\r\n")] = 0;
}

void moi_ls(char* path){
	
	DIR *dir;
	struct dirent *dp;
	
	if((dir = opendir(path)) == NULL){
		perror("Cannot open this -.- ");
		exit(1);
	}

	dir = opendir(path);
	while((dp = readdir(dir)) != NULL){
		printf("%s\n", dp -> d_name);
	}
}

void moi_fput(char* path){
	char file_input[SIZE];
	FILE *fp;

	fp = fopen(path, "w+");
	printf("Enter contents of file: ");
	scanf(" %[^\n]s", file_input);
	fputs(file_input, fp);
	fclose(fp);
}

void moi_fget(char *path){
	FILE *fp;
	char c;

    fp = fopen(path, "r");
    if (fp == NULL){
        printf("Cannot open file! \n");
        exit(0);
    }

    c = fgetc(fp);
    while (c != EOF){
        printf ("%c", c);
        c = fgetc(fp);
    }
    printf("\n");
 
    fclose(fp);
}

void moi_create_dir(char* path){
}
 
int main(int argc , char *argv[])
{
	int sock;
	struct sockaddr_in server;
	char username[SIZE] , server_reply[SIZE];
	 
	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	 
	server.sin_addr.s_addr = inet_addr("127.0.0.1"); //localhost
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
 
	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	 
	puts("");
	puts("          ***********************************          ");
	puts("****************** Welcome to MoiFS! ******************");
	puts("          ***********************************          ");
	puts("");
	 
	printf("Username : ");
	scanf("%s" , username);

	puts("");
	puts("          ***********************************          ");
	puts("***************** Authenticating User *****************");
	puts("          ***********************************          ");
	puts("");

	//Send some data
	send(sock , username , SIZE , 0);  
 
	//Receive a reply from the server
	recv(sock , server_reply , SIZE , 0);

	puts("Server reply :");
	puts(server_reply);

	printf("Welcome to your shell!\n");

	while(1){
		pid_t child_pid;
		child_pid = fork ();

		if (child_pid <0 ){
			perror("Fork Failed");
			exit(1);
		}
		if(child_pid == 0){
			char command[SIZE], path[SIZE];

			printf("Command: ");
			scanf("%s", command);
			remove_newline_char(command);
			printf("Path: ");
			scanf("%s", path);
			remove_newline_char(path);

			if(strcmp(command, "ls") == 0){
				moi_ls(path);
			}

			if(strcmp(command, "fput") == 0){
				moi_fput(path);
			}

			if(strcmp(command, "fget") == 0){
				moi_fget(path);
			}

			if(strcmp(command, "create_dir") == 0){
				moi_create_dir(path);
			}

			exit(1);	
		}

		else if (child_pid >0){
			wait(NULL);
		}
	} 
	 
	close(sock);
	return 0;
}