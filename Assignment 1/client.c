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

	//print user, group permissions alongside filename

}

void moi_fput(char* path){
	char file_input[SIZE];
	char permissions[SIZE];
	FILE *fp;
	FILE *fp_;

	fp = fopen(path, "w+");
	fp_ = fopen("file_permissions.txt", "a");
	printf("Enter contents of file: ");
	scanf(" %[^\n]s", file_input);
	fputs(file_input, fp);
	fputs(path, fp_);
	fputs("|", fp_);
	printf("Enter file permissions for users and groups.\nFORMAT: creator|user permissions|group permissions\n");
	scanf(" %[^\n]s", permissions);
	fputs(permissions, fp_);
	fputs("\n", fp_);
	fclose(fp);
	fclose(fp_);

	//check whether user is permitted to write to existing file
	//check for whether file already exists. If it does then rewrite the permissions, else append to file
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

    //check whether user is permitted to read that file
}

void moi_create_dir(char* path){
	/*refered from: http://nion.modprobe.de/blog/archives/357-Recursive-directory-creation.html*/
	char tmp[SIZE];
	char *p = NULL;
	size_t len;

	char dir_permissions[SIZE];
	FILE *fp;

	fp = fopen("directory_permissions.txt", "a");

	snprintf(tmp, sizeof(tmp),"%s",path);
    len = strlen(tmp);
    if(tmp[len - 1] == '/')
            tmp[len - 1] = 0;
    for(p = tmp + 1; *p; p++)
            if(*p == '/') {
                    *p = 0;
                    mkdir(tmp, S_IRWXU);
                    *p = '/';
                    resolve_path(path);
                    fputs(path, fp);
                    fputs("|", fp);
                    printf("Enter directory permissions for users and groups.\nFORMAT: creator|user permissions|group permissions\n");
                    scanf(" %[^\n]s", dir_permissions);
					fputs(dir_permissions, fp);
					fputs("\n", fp);
            }
    mkdir(tmp, S_IRWXU);
    fclose(fp);
}

void resolve_path(char* path)
	{
		char resolved_path[SIZE];
		realpath(path, resolved_path);
		strcpy(path, resolved_path);
	}

void authenticate_user(char* username){
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	int flag = 0;

	fp = fopen("users.txt", "r");
	if (fp == NULL)
		exit(EXIT_FAILURE);

	while ((read = getline(&line, &len, fp)) != -1) {
		int i;
		remove_newline_char(line);
		i = strcmp(username, line);
		if(i == 0){
			flag = 1;
			break;
		}
	}

	if(flag == 0){
		printf("You are not authenticated!\n");
		exit(EXIT_SUCCESS);
	}
	else
		printf("You have been successfully authenticated!\n");

	fclose(fp);
	if (line)
		free(line);
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
	scanf(" %[^\n]s", username);

	puts("");
	puts("          ***********************************          ");
	puts("***************** Authenticating User *****************");
	puts("          ***********************************          ");
	puts("");

	authenticate_user(username);

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

		if (child_pid < 0){
			perror("Fork Failed");
			exit(1);
		}
		if(child_pid == 0){
			char command[SIZE], path[SIZE];

			printf("Command: ");
			scanf(" %[^\n]s", command);
			remove_newline_char(command);
			printf("Path: ");
			scanf(" %[^\n]s", path);
			remove_newline_char(path);
			// resolve_path(path);

			if(strcmp(command, "ls") == 0){
				resolve_path(path);
				moi_ls(path);
			}

			else if(strcmp(command, "fput") == 0){
				resolve_path(path);
				moi_fput(path);
			}

			else if(strcmp(command, "fget") == 0){
				resolve_path(path);
				moi_fget(path);
			}

			else if(strcmp(command, "create_dir") == 0){
				moi_create_dir(path);
			}

			else
				printf("Command does not exist\n");

			exit(1);	
		}

		else if (child_pid >0){
			wait(NULL);
		}
	} 
	 
	close(sock);
	return 0;
}