#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <signal.h>
#include <regex.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#define TRUE 1

void *printList(void *parameter);
void *serverThread(void *ptr);
//void *myserver(void *parameter);

struct Process{
    int PID;
    char name[20];
    char status[20];
    time_t startTime;
    time_t endTime;
    time_t runTime;
};

struct Client{
    int cpid;
    int csock;
    int piperead;
    int pipewrite;
    //char *state;
};

int processCount=0;
struct Process table[1000];
struct Client clientTable[1000];
int clientCount=0;
int newPid;
pthread_t thread1, thread2;
int msgsock;

void signalHandler(){
    signal(SIGCHLD, signalHandler);//reset handler to catch SIGCHLD next time
    int pid;
    int status;
    pid= wait(&status);//resorces allocated are released
    for (int process = 0; process < processCount; process++){
        if (table[process].PID==pid){
            if(!WIFSIGNALED(status)){//to check if it was not terminated by a signal
            strcpy(table[process].status, "Externally Terminated");
            //update end time here
            table[process].endTime=time(NULL);
            //time_t total= difftime(table[process].endTime, table[process].endTime);
            //table[process].runTime=total;
            return;
            }
        }
    }
    
}

void clientListUpdate(int pid, int sock, int read, int write){
    clientTable[clientCount].cpid=pid;
    clientTable[clientCount].csock=sock;
    clientTable[clientCount].piperead=read;
    clientTable[clientCount].pipewrite=write;
}

//void *myserver(void *parameter){
void myserver(int msgsock, int pipefd){
    signal(SIGCHLD, signalHandler);
    pthread_create( &thread2, NULL, serverThread, (void*)&pipefd);
    char finalSum[25];
    float num;
    float result;
    char *ptr;
    processCount=0;
    char input2[100];
    /*char add[3]="add";
    char sub[3]="sub";
    char mult[4]="mult";
    char div[3]="div";
    char run[3]="run";
    char kil[4]="kill";
    char list[4]="list";
    char print[5]="print";
    char allList[3]="lis";*/
    int readCount=0;
    //struct Process table[100];//maintain list of processes
    while(TRUE){
        read(msgsock, &input2[readCount],1);
        readCount++;
        if(input2[readCount-1]==';'){
            input2[readCount-1]='\0';
        }
        if(input2[readCount-1]=='\0'){
            //write(STDOUT_FILENO,input2, strlen(input2));
            ptr=strtok(input2, " ");
            ptr = strtok(NULL, " ");//skip the operator part
                if((strcasecmp(input2, "add")) == 0){//addition
                    while (ptr != NULL) {
                        if(isdigit(*ptr) != 0) {//if it is a digit
                            sscanf(ptr, "%f", &num);
                            result += num;
                            ptr = strtok(NULL, " ");
                        }
                        else{//if it is a character
                            write(msgsock, "Character encountered\n", 22);
                            ptr = strtok(NULL, " ");//skip character
                        }
                    }
                    int count=sprintf(finalSum, "%f", result);
                    finalSum[count-1]='\0';
                    write(msgsock, finalSum, count);
                    //perror("add error");
                    readCount=0;
                    result=0;
                    continue;
                    //break;
                }
                else if((strcasecmp(input2, "sub")) == 0){//subtraction
                    while(isdigit(*ptr) == 0) { //skipping characters at start
                        sscanf(ptr, "%f", &num);
                        write(msgsock, "Character encountered\n", 22);
                        ptr = strtok(NULL, " ");
                    }
                    sscanf(ptr, "%f", &num);
                    result=num;//initializing result with first number rather than 0 or 1 as it effects the result
                    ptr = strtok(NULL, " ");
                    while(ptr != NULL) {
                        if(isdigit(*ptr) != 0) {
                            sscanf(ptr, "%f", &num);
                            //printf("%f",num);
                            result -= num;
                            ptr = strtok(NULL, " ");
                        }
                        else{
                            write(msgsock, "Character encountered\n", 22);
                            ptr = strtok(NULL, " ");
                        }
                    }
                    int count=sprintf(finalSum, "%f", result);
                    finalSum[count-1]='\0';
                    write(msgsock, finalSum, count);
                    readCount=0;
                    result=0;
                    continue;
                    //break;
                }
                else if((strcasecmp(input2, "mul")) == 0){//Multiplication
                    result=1;
                    while(ptr != NULL) {
                        if(isdigit(*ptr) != 0) {
                            sscanf(ptr, "%f", &num);
                            result *= num;
                            ptr = strtok(NULL, " ");
                        }
                        else{
                            write(msgsock, "Character encountered\n", 22);
                            ptr = strtok(NULL, " ");
                        }
                    }
                    int count=sprintf(finalSum, "%f", result);
                    finalSum[count-1]='\0';
                    write(msgsock, finalSum, count);
                    readCount=0;
                    result=0;
                    continue;
                    //break;
                }
                else if((strcasecmp(input2, "div")) == 0){//Division
                    int divByZero=0;//flag
                    while(isdigit(*ptr) == 0) { //skipping characters at start
                        sscanf(ptr, "%f", &num);
                        write(msgsock, "Character encountered\n", 22);
                        ptr = strtok(NULL, " ");
                    }
                    sscanf(ptr, "%f", &num);
                    result=num;//initializing result with first number rather than 0 or 1 as it effects the result
                    ptr = strtok(NULL, " ");
                    while(ptr != NULL) {
                        if(isdigit(*ptr) != 0) {
                            sscanf(ptr, "%f", &num);
                            if (num != 0) {//check if number is non zero
                                result /= num;
                                ptr = strtok(NULL, " ");
                            } 
                            else {//avoiding dividion by zero by skipping the number
                                //write(STDOUT_FILENO, "Division by Zero Avoided.\n", 26);
                                divByZero=1;
                                ptr = strtok(NULL, " ");
                            }
                        }
                        else{//when character encountered skip it
                            write(msgsock, "Character encountered\n", 22);
                            ptr = strtok(NULL, " ");
                        }
                    }
                    if(divByZero==1){
                        write(msgsock, "Division by Zero Avoided.\n", 26);
                    }
                    else{
                    int count=sprintf(finalSum, "%f", result);
                    finalSum[count-1]='\0';
                    write(msgsock, finalSum, count);
                    }
                    readCount=0;
                    result=0;
                    continue;
                    //break;
                }
                else if(strcasecmp(input2, "run")==0){//RUN
                    if (ptr==NULL){
                        char temp[]="Enter a program to Run!";
                        temp[23]='\0';
                        write(msgsock, temp,24);
                        //continue;
                    }
                    int newPid= fork();
                    if (newPid==0){
                        int status=execlp(ptr, ptr, NULL);
                        exit(0);
                    }
                    else{
                        //write(STDOUT_FILENO,"hello\n",6);
                        table[processCount].PID=newPid;
                        strcpy(table[processCount].name, ptr);
                        strcpy(table[processCount].status, "Running");
                        table[processCount].startTime=time(NULL);
                        char temp1[20];
                        int pid= table[processCount].PID;
                        char *name= table[processCount].name;
                        char *status= table[processCount].status;
                        struct tm *ts;
                        ts=localtime(&table[processCount].startTime);
                        processCount++;
                        char newTemp[100];
                        //int a=sprintf(newTemp, "PID: %d , Name: %s , Status: %s ", pid, name, status);
                        int a=sprintf(newTemp, "\nPID: %d , Name: %s , Status: %s , Start Time: %d:%d:%d \n", pid, name, status,ts->tm_hour, ts->tm_min, ts->tm_sec);
                        newTemp[a-1]='\0';
                        readCount=0;
                        write(msgsock, newTemp,a);//writing on pipe to remove client from block!
                    }
                    readCount=0;
                    continue;
                    //break;
                }
                else if(strcasecmp(input2, "kill")==0){//KILL
                    int notFound=1;
                    if (ptr==NULL){
                        char temp[]="Enter a program to kill!";
                        temp[24]='\0';
                        write(msgsock, temp,25);
                        readCount=0;
                    }
                    int killpid;
                    if (isdigit(*ptr)!=0){
                        write(STDOUT_FILENO, "killing by PID\n", 15);
                        sscanf(ptr, "%d", &killpid);
                        for (int i = 0; i < processCount; i++){
                            if (table[i].PID==atoi(ptr)){
                                kill(killpid,SIGTERM);
                                strcpy(table[processCount].status, "Terminated");
                                table[i].endTime=time(NULL);
                                //time_t total= difftime(table[i].endTime, table[i].endTime);
                                //table[i].runTime=total;
                                //struct tm *ts;
                                //ts=localtime(&table[processCount].endTime);
                                notFound=0;
                                break;
                            }
                            else{
                                /*char temp[]="Program not found!";
                                temp[18]='\0';
                                write(msgsock, temp,19);*/
                                notFound=1;
                            }
                        }
                    }
                    else if(isalpha(*ptr)!=0){
                        write(STDOUT_FILENO, "killing by name\n", 16);
                        for (int i = 0; i < processCount; i++){
                            if (strcmp(table[i].name, ptr)==0){
                                killpid=table[i].PID;
                                kill(killpid,SIGTERM);
                                strcpy(table[processCount].status, "Terminated");
                                table[i].endTime=time(NULL);
                                //time_t total= difftime(table[i].endTime, table[i].endTime);
                                //table[i].runTime=total;
                                notFound=0;
                                break;
                            }
                            else{
                                /*char temp[]="Program not found!";
                                temp[18]='\0';
                                write(msgsock, temp,19);*/
                                notFound=1;
                            }
                        }
                    }
                    else{
                        char temp[]="Enter a valid program to kill!";
                        temp[31]='\0';
                        write(msgsock, temp,32);
                    }
                    readCount=0;
                    char newTemp[50];
                    if(notFound==0){
                        int a=sprintf(newTemp, "PID: %d: Killed ", killpid);
                        newTemp[a-1]='\0';
                        write(msgsock, newTemp,a);
                    }
                    else{
                        int a=sprintf(newTemp, "No Valid program found!\n ");
                        newTemp[a-1]='\0';
                        write(msgsock, newTemp,a);
                    }
                    continue;
                }
                else if(strcasecmp(input2, "list")==0){//LIST
                    int printCount=0;
                    char newTemp[1000];
                    //write(STDOUT_FILENO,"hello\n",6);
                    for (int i = 0; i < processCount; i++){
                        int pid= table[i].PID;
                        char *name= table[i].name;
                        char *status= table[i].status;
                        time_t start=table[i].startTime;
                        struct tm st;
                        localtime_r(&start, &st);
                        time_t end=table[i].endTime;    
                        struct tm en;
                        localtime_r(&end, &en);
                        time_t run=difftime(end, start);
                        table[i].runTime=run;
                        struct tm ttl;
                        localtime_r(&run, &ttl);
                        //printCount+= sprintf(&newTemp[printCount], "PID: %d , Name: %s , Status: %s %ld:%ld:%ld ", pid, name, status, start, end,run);
                        printCount+= sprintf(&newTemp[printCount], "PID: %d , Name: %s , Status: %s ", pid, name, status);
                        printCount+=sprintf(&newTemp[printCount], " Start Time: %d:%d:%d ", st.tm_hour, st.tm_min, st.tm_sec);
                        printCount+=sprintf(&newTemp[printCount], " End Time: %d:%d:%d ", en.tm_hour, en.tm_min, en.tm_sec);
                        printCount+=sprintf(&newTemp[printCount], " Elapsed Time: %d:%d:%d \n", ttl.tm_hour, ttl.tm_min, ttl.tm_sec);
                    }
                    newTemp[printCount-1]='\0';
                    write(msgsock,newTemp,printCount);
                    //write(STDOUT_FILENO,"HELLO\n",6);
                    readCount=0;
                    continue;
                }   
                else if(strcasecmp(input2, "print")==0){//print
                    //ptr = strtok(NULL, " ");
                    int printCount=0;
                    char str[1000];
                    while(ptr!=NULL){
                        printCount+=sprintf(&str[printCount],"%s ", ptr);
                        ptr = strtok(NULL, " ");
                    }
                    str[printCount-1]='\0';
                    write(STDOUT_FILENO, str, printCount);
                    write(msgsock,"print successfull\n",18);
                    readCount=0;
                    continue;
                }
                else{//if no valid operation selescted
                    char temp[]="No valid Operation Selected!\n";
                    temp[29]='\0';
                    write(msgsock, temp, 30);
                    readCount=0;
                    //continue;
                }
            }
        }
        write(STDOUT_FILENO,"Client Disconnected",20);
}


int main(void){
	int sock, length;
	struct sockaddr_in server;
	int msgsoc;
	char buf[1024];
	int rval;
	int i;

	/* Create socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("opening stream socket");
		exit(1);
	}
	/* Name socket using wildcards */
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = ntohs(44444);
	if (bind(sock, (struct sockaddr *) &server, sizeof(server))) {
		perror("binding stream socket");
		exit(1);
	}
	/* Find out assigned port number and print it out */
	length = sizeof(server);
	if (getsockname(sock, (struct sockaddr *) &server, (socklen_t*) &length)) {
		perror("getting socket name");
		exit(1);
	}
	printf("Socket has port #%d\n", ntohs(server.sin_port));
	fflush(stdout);

	/* Start accepting connections */
	listen(sock, 5);
	do {
		msgsoc = accept(sock, 0, 0);
        pthread_create( &thread1, NULL, printList, (void*)NULL);
		if (msgsoc == -1)
			perror("accept");
		else{
			bzero(buf, sizeof(buf));
			i = 0;
            rval=1;
			if (rval == 0)
				printf("Ending connection\n");
			else{
                int inputPipe[2];
                pipe(inputPipe);
				newPid=fork();
                if(newPid==0){
                    //close(sock);
                    close(inputPipe[1]);//only read in server
                    myserver(msgsoc, inputPipe[0]);
                    //pthread_create( &thread2, NULL, myserver, (void*)NULL);
                }
                else{
                    close(inputPipe[0]);//only write in input
                    clientListUpdate(newPid, msgsoc, inputPipe[0], inputPipe[1]);
                    clientCount++;
                }
            }
		} 
	}while (TRUE);
    pthread_join( thread1, NULL);
    //pthread_join( thread2, NULL); 
    exit(EXIT_SUCCESS); 
}

void *printList(void *parameter){
    char print[5]="print";
    //char list[4]="list";
    char send[4]="send";
    char *ptr;
    char input[100];
    //char copyin[100];
    while(TRUE){
        //char input[100];
        write(STDOUT_FILENO,"\nEnter Command: ",15);
        int a =read(STDIN_FILENO,input,1000);
        input[a-1]='\0';
        //strcpy(copyin, input);
        /*char print[5]="print";
        char list[4]="list";
        char send[4]="send";
        char *ptr;*/
        ptr=strtok(input, " ");
        if(strcasecmp(input, "list") == 0){//list of all connections
            int printCount=0;
            char newTemp[100];
            //write(STDOUT_FILENO,"hello\n",6);
            for (int i = 0; i < clientCount; i++){
                int pid= clientTable[i].cpid;
                int sk= clientTable[i].csock;
                printCount+= sprintf(&newTemp[printCount], "PID: %d , socket: %d  \n ", pid, sk);
                //write(sk, newTemp, printCount);
            }
            write(STDOUT_FILENO, newTemp, printCount);
            //sleep(1);
            //pthread_join( thread2, NULL);
        }
        else if(strcasecmp(input, "print")==0){//print to all clients
            //write(STDOUT_FILENO,"hello\n",6);
            int printCount=0;
            char str[100];
            ptr=strtok(NULL, " ");
            printCount+=sprintf(&str[printCount],"\nServer Sent: ");
            while(ptr!=NULL){
                //write(STDOUT_FILENO,"HELLO\n",5);
                printCount+=sprintf(&str[printCount],"%s ", ptr);
                ptr = strtok(NULL, " ");
            }
            for (int i = 0; i < clientCount; i++){
                int sc=clientTable[i].csock;
                write(sc,str, printCount);   
            }
        }
        else if(strcasecmp(input, "send")==0){//print to specific client
            //write(STDOUT_FILENO, "hello\n", 6);
            int printCount=0;
            char str[100];
            ptr=strtok(NULL, " ");//skip send
            int spid=atoi(ptr);
            ptr=strtok(NULL, " ");//skip pid
            printCount+=sprintf(&str[printCount],"\nServer Sent: ");
            while(ptr!=NULL){
                //write(STDOUT_FILENO,"HELLO\n",5);
                printCount+=sprintf(&str[printCount],"%s ", ptr);
                ptr = strtok(NULL, " ");
            }
            for (int i = 0; i < clientCount; i++){
                if(spid==clientTable[i].cpid){
                    int sc=clientTable[i].csock;
                    write(sc,str, printCount);
                }
            }
        }
        else if(strcasecmp(input, "listof")==0){
            ptr=strtok(NULL, " ");
            int spid=atoi(ptr);
            for (int i = 0; i < clientCount; i++){
                if(spid==clientTable[i].cpid){
                    //write(clientTable[i].pipewrite, "printof", 7);
                    write(clientTable[i].pipewrite, "listof", 6);
                }
            }
            //write(STDOUT_FILENO, "Hello\n", 6);
        }
        else{
            write(STDOUT_FILENO, "Invalid Command!\n", 17);
        }
    }
}

void *serverThread(void *parameter){
    int pipefd = *((int *) parameter);
    char input[6];
    //char print[7]="printof";
    //char *ptr;
    while(TRUE){
        int count= read(pipefd, input, 6);
        //char readme[20];
        //write(STDOUT_FILENO, input, 7);
        if(strcmp(input, "listof")==0){//print individual list
            //write(STDOUT_FILENO, "helloworld\n", 11);
            //int count=read(pipefd,readme, 100);
            //write(STDOUT_FILENO, readme, count);
            int printCount=0;
            char newTemp[100];
            //write(STDOUT_FILENO,"hello\n",6);
            for (int i = 0; i < processCount; i++){
                int pid= table[i].PID;
                char *name= table[i].name;
                char *status= table[i].status;
                time_t start=table[i].startTime;
                struct tm st;
                localtime_r(&start, &st);
                time_t end=table[i].endTime;    
                struct tm en;
                localtime_r(&end, &en);
                time_t run=difftime(end, start);
                table[i].runTime=run;
                struct tm ttl;
                localtime_r(&run, &ttl);
                //printCount+= sprintf(&newTemp[printCount], "PID: %d , Name: %s , Status: %s %ld:%ld:%ld ", pid, name, status, start, end,run);
                printCount+= sprintf(&newTemp[printCount], "PID: %d , Name: %s , Status: %s ", pid, name, status);
                printCount+=sprintf(&newTemp[printCount], " Start Time: %d:%d:%d ", st.tm_hour, st.tm_min, st.tm_sec);
                printCount+=sprintf(&newTemp[printCount], " End Time: %d:%d:%d ", en.tm_hour, en.tm_min, en.tm_sec);
                printCount+=sprintf(&newTemp[printCount], " Elapsed Time: %d:%d:%d \n", ttl.tm_hour, ttl.tm_min, ttl.tm_sec);
            }
            newTemp[printCount-1]='\0';
            write(STDOUT_FILENO,newTemp,printCount);
        }
        //ptr=strtok(input," ");

        //write(STDOUT_FILENO, input, count);
    }
}