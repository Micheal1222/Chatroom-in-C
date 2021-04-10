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
#include <fcntl.h>

#define TIME_PORT 22001
#define BUFF_SIZE 128
#define OFFSET 4
#define MAX_CONNECTS 10
#define ROOM1 r1
#define ROOM2 r2
#define ROOM3 r3

struct user{
	char* info;
	int ifbusy;
	char space;
	int socket;
};
typedef struct user Client;

struct userList{
	Client* users;
	int no_of_user_conn,* sockets;
	char** infos;
	char** spaces;
};
typedef struct userList clientlist;

int main(int argc,char**argv) {
    char buffer[BUFF_SIZE],input[BUFF_SIZE];
    char** infos = (char**)malloc(100);
    char** spaces = (char**)malloc(50);

    struct sockaddr_in server,user;
    clientlist* list = (clientlist*)malloc(sizeof(clientlist));

    fd_set descTable,cpyTable;

    int SocketID,length,userSocket,indexes = 0, no_of_user_conn = 0,indexinfo = 0;
    int errorselect,condition;
    //int* connec  = calloc(MAX_CONNECTS,sizeof(int));
    int connec[10],remainleft[10];

    memset(&remainleft,0,10*sizeof(int));

    time_t moment;
    struct tm time_s;
    
    for(condition=0;condition<10;condition++){
	char* aux = (char*)malloc(100);
	aux = "EMPTY";
    	infos[condition] = aux;
	spaces[condition] = "r1";
    }
    list->infos = infos;
    list->sockets = connec;

    if((SocketID = socket(AF_INET,SOCK_STREAM,0)) < 0){
        perror("Couldnt connect\n");
        exit(2);
    }
    
    memset(&server,0,sizeof(server));
    memset(&connec,0,MAX_CONNECTS*sizeof(int));

    server.sin_family = AF_INET;
    server.sin_port = htons(TIME_PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    
    bind(SocketID,(struct sockaddr*)&server,sizeof(server));
    listen(SocketID,5);
    
    time(&moment);
    localtime_r(&moment,&time_s);
    sprintf(buffer,"%s",asctime(&time_s));
    length = strlen(buffer)+1;
    
    FD_SET(SocketID,&descTable);

    for(int i=0;1;i++){
	int i = 0;
	cpyTable = descTable;

	select((OFFSET+no_of_user_conn),&cpyTable,0,0,NULL);	
	
	memset(&buffer,0,BUFF_SIZE);

	if(FD_ISSET(SocketID,&cpyTable)){
		int id = accept(SocketID,(struct sockaddr*)&server,(socklen_t*)&length);
		no_of_user_conn++;
		printf("adding a connection %d\n",(no_of_user_conn));
		for(i=0;i<no_of_user_conn;i++){
			printf("Checking can connect at indexes %d, value %d\n",i,list->sockets[i]);

			if(list->sockets[i] == 0){
				Client* user = (Client*)malloc(sizeof(Client));
				user->socket = id;				
				
				list->sockets[i] = id;
				FD_SET(id,&descTable);
				printf("added a connection %d\n",(no_of_user_conn));

				time(&moment);
    				localtime_r(&moment,&time_s);
   				sprintf(buffer,"%s",asctime(&time_s));
				write(list->sockets[i],buffer,BUFF_SIZE);
				
				char* getRoom = (char*)malloc(BUFF_SIZE);
				strcat(getRoom,"enter space [`r1][`r2][`r3]\n");
				write(list->sockets[i],getRoom,BUFF_SIZE);
				break;		
			}
		}
	}
	//{ int m; scanf("%d", &m);}
	if(no_of_user_conn == 0)continue;
	for(i=0;i<no_of_user_conn;i++){
		if(FD_ISSET(list->sockets[i],&cpyTable)){
			int size = read(list->sockets[i],input,BUFF_SIZE);
			remainleft[i] = BUFF_SIZE - size;

			if(size > 0){
				if(input[0] == '~'){
					char* info = (char*)malloc(100);
					strcpy(info,(input+1));
					infos[i] = (char*)info;
					printf("User info is %s\n",infos[i]);				
				}
				if(input[0] == '`'){
					char* space = (char*)malloc(50);
					strcpy(space,(input+1));
					spaces[i] = (char*)space;
					printf("User %s space is %s\n",infos[i],spaces[i]);	
					break;			
				}
				if(input[0] == '-'){
					if(input[1] == 'h'){
						char* help = (char*)malloc(BUFF_SIZE);
						strcat(help,"~[userinfo]\n");
						strcat(help,"-r[space number]\n");
						strcat(help,"-h help menu\n");//help menu
						strcat(help,"-l list space users\n");//list users
						strcat(help,"-a list all space\n");//space listing
						strcat(help,"-p [user] send private message\n");//private message
						write(list->sockets[i],help,BUFF_SIZE);
					}
					if(input[1] == 'r'){
						char* space = (char*)malloc(50);
						strcpy(space,(input+2));
						spaces[i] = (char*)space;
						printf("User %s space is %s\n",infos[i],spaces[i]);	
						break;			
					}
					if(input[1] == 'l'){
						int k;
						for(k=0;k<no_of_user_conn;k++){
							if(list->sockets[k] != 0 && strcmp(spaces[i],spaces[k]) == 0){
								write(list->sockets[i],infos[k],BUFF_SIZE);
							}
						}
					}
				}

				int j;
				char* user = (char*)malloc(BUFF_SIZE);
				if(infos[i] == "EMPTY"){
					sprintf(user,"[%d]>>> ",i);
				}else{
					sprintf(user,"[%s]>>> ",infos[i]);
				}
				strcat(user,input);
				for(j=0;j<no_of_user_conn;j++){
					if(list->sockets[j] != 0 && strcmp(spaces[i],spaces[j]) == 0){
						write(list->sockets[j],user,BUFF_SIZE);
					}
				}			
			}else{
				int prev = no_of_user_conn;
				close(list->sockets[i]);
				list->sockets[i] = 0;
				FD_CLR(list->sockets[i],&descTable);
				no_of_user_conn = (no_of_user_conn < 0)?0:no_of_user_conn-1;
			}		
		}	
	}

    }

    close(SocketID);
    return EXIT_SUCCESS;
}
