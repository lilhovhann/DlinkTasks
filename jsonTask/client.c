#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> 

#define MAXVAL 100

void converttoJson(char input[], char json[]){
   sprintf(json, "{'key':'%s'}", input);
}

void converttoString(char json[], char str[]){
    int j = 0;
    
    for(int i = 0; i<MAXVAL; i++){
        if(json[i] == ':'){
            while( json[++i] != '}'){
                if(json[i] != '\'')
                    str[j++] = json[i];
            }
            break;
        }
    }
}


int main(){

	int myclient;
	char jm[MAXVAL];
        char *pjsonmessage = jm;
	char pmessage[MAXVAL];
	char *message = pmessage;
	char server_reply[MAXVAL];
	char reply[MAXVAL];

	myclient = socket(AF_INET, SOCK_STREAM, 0);

	if(myclient<0){
		perror("socket");
        exit(1);
	}
	struct sockaddr_in myclient_address;
		myclient_address.sin_family = AF_INET;
		myclient_address.sin_port = htons(9008);
		myclient_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

		int con = connect(myclient, (struct sockaddr*) &myclient_address, sizeof(myclient_address));

		if(con < 0){
			perror("connect");
        	exit(2);
		}

		while(1){
			printf("Enter message: ");
			scanf("%s", message);
			converttoJson(pmessage, jm);

			if((send(myclient, jm, strlen(jm), 0))<0){
				printf("send failed\n");
				break;
			}

			if((recv(myclient, server_reply, MAXVAL, 0))<0){
				printf("recv failed\n");
				break;
			}
			converttoString(server_reply, reply);

			printf("Server reply: ");
			puts(reply);
		
		}



	close(myclient);
	return 0;
}
