//http://www.cs.montana.edu/~halla/csci466/lectures/lec2.html
//https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/
//https://stackoverflow.com/questions/16328118/simple-tcp-server-with-multiple-clients-c-unix
// I also had to see how my classmates did this assignment and I got from the the ideas 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define TIME_PORT 22001
#define BUFF_SIZE 128
#define NUM_THREADS 10

void* listenThread(void* data){
	int socketID = *(int*)data;
	int readIn;
	
	char* pos;
	char aux[BUFF_SIZE];
	
	recv(socketID,aux,BUFF_SIZE,0);
	printf("\n[Server]>>> %s\n",aux);

	while((readIn = recv(socketID,aux,BUFF_SIZE,0)) > 0){
		//if ((pos = strchr(aux, '\n')) != NULL)*pos = '\0';
		printf("\n%s\n",aux);
	}
}

int main(int argc,char**argv){
    char buff[BUFF_SIZE],aux[BUFF_SIZE],userName[BUFF_SIZE];
    int socketID,readIn,listener;
    struct sockaddr_in client;
    size_t size = sizeof(client);
    
	pthread_t thread;
	pthread_attr_t attr;
	
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
	
    if((socketID = socket(AF_INET,SOCK_STREAM,0)) < 0){
        perror("Couldn't start socket\n");
        exit(2);
    }
    
    memset(&client,0,sizeof(client));
    memset(userName,0,BUFF_SIZE);
    
    client.sin_family = AF_INET;
    client.sin_port = htons(TIME_PORT);
    inet_pton(AF_INET,"127.0.0.1",&client.sin_addr);
    
    if(connect(socketID,(struct sockaddr*)&client,size) < 0){
        perror("Couldn't connect to server");
        exit(3);
    }
    
    printf("Enter your name >>");
    char *pos,* name = (char*)malloc(100);
    fgets(userName,BUFF_SIZE,stdin);
    if ((pos = strchr(userName, '\n')) != NULL)*pos = ' ';
    
    printf("\nBeginning communication...\n");
    //readIn = read(socketID,aux,BUFF_SIZE);
    sprintf(name,"~%s",userName);
    send(socketID,name,BUFF_SIZE,0);
    
    pthread_create(&thread,&attr,listenThread,(void*)&socketID);
    //printf("[%s] enter text",userName);
    char toSend[27];
    int i;
    for(i=0;i<26;i++){
        toSend[i] = 'a'+i;
    }
    i = 0;
    while(1){
        memset(buff,0,BUFF_SIZE);
        //buff[0] = toSend[i++];
        //printf(">>> %s");
        fgets(buff,BUFF_SIZE,stdin);
        //printf(">>> %s\n",buff);
        
        send(socketID,buff,BUFF_SIZE,0);
        if(i > 26)i=0;
    }
    
	pthread_join(thread,NULL);
	
    close(socketID);
    return EXIT_SUCCESS;
}
