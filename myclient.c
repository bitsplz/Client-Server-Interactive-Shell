#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define TRUE 1
void *cread(void *parameter);
void client();
void *cwrite(void *parameter);

pthread_t thread1, thread2;
//int sock;
//int count=-1;
char resultBuffer[10];
int resultCount=0;
char input[1000];
void client(int sock){
    write(STDOUT_FILENO, "Operators: \n1. add\n2. sub\n3. mult\n4. div\n5. kill\n6. run\n7. list\n8. print\nAll extra characters will be ignored.\n",112);
    write(STDOUT_FILENO, "kill works with pid\n",20);
    //while(count!=0){//will exit when only enter is pressed
    
    int  iret1, iret2;
    /* Create independent threads each of which will execute function */
    iret1 = pthread_create( &thread1, NULL, cread, (void*)&sock);
    if(iret1)
    {
        fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
        exit(EXIT_FAILURE);
    }
	/*Creating a new thread*/
    iret2 = pthread_create( &thread2, NULL, cwrite, (void*)&sock);
    if(iret2)
    {
        fprintf(stderr,"Error - pthread_create() return code: %d\n",iret2);
        exit(EXIT_FAILURE);
    }
	pthread_join( thread1, NULL);
    pthread_join( thread2, NULL); 
    exit(EXIT_SUCCESS);            
    //}
}

void *cread(void *parameter){
    int sock = *((int *) parameter);
	//int *sock = (char *) ptr;
	while(true){
        write(STDOUT_FILENO,"\nEnter Command: ",15);
        int a =read(STDIN_FILENO,input,1000);
        input[a-1]='\0';
        int n = write(sock,input,a);
        if (n <=1) {
           write(STDOUT_FILENO, "Exiting in 3 seconds\n", 21);
		   sleep(3);
		   exit(0);
        }
		sleep(1);
    }
}
void *cwrite(void *parameter){
    int sock = *((int *) parameter);
	while(true){
        int n = read(sock,resultBuffer,1000);
        write(STDOUT_FILENO,resultBuffer, n);
    }
}

int main(int argc, char *argv[]){
	int sock;
	struct sockaddr_in server;
	struct hostent *hp;
	char buf[1024];

	/* Create socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("opening stream socket");
		exit(1);
	}
	/* Connect socket using name specified by command line. */
	server.sin_family = AF_INET;
	hp = gethostbyname(argv[1]);
	if (hp == 0) {
		fprintf(stderr, "%s: unknown host\n", argv[1]);
		exit(2);
	}
	bcopy(hp->h_addr, &server.sin_addr, hp->h_length);
	server.sin_port = htons(atoi(argv[2]));

	if (connect(sock,(struct sockaddr *) &server,sizeof(server)) < 0) {
		perror("connecting stream socket");
		exit(1);
	}
    client(sock);
	close(sock);
}