#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> 

#define MAXVAL 100

char opstack[MAXVAL];
char numstack[MAXVAL];
int top = -1;
void push(char item, char dest[]);
void pushint(int item, char dest[]);
char pop(char dest[]);
int is_operator(char symbol);
int prec(char symbol);
void scanner(char infixExp[], char postfixExp[]);
void postfixscanner(char postfixExp[]);

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

void converttoJson(char input[], char json[]){
   sprintf(json, "{'key':'%s'}", input);
}

int main(){


	char infix[MAXVAL];
	char postfix[MAXVAL];

	char jinfix[MAXVAL];
        char result[MAXVAL];
	char jresult[MAXVAL];
	int bytes_read;
	int mysocket, accpt;
	struct sockaddr_in myserver_address;

	mysocket = socket(AF_INET, SOCK_STREAM, 0);

	if(mysocket<0){
		perror("socket");
        exit(1);
	}


	myserver_address.sin_family = AF_INET;
	myserver_address.sin_port = htons(9008);
	myserver_address.sin_addr.s_addr = htonl(INADDR_ANY);


	if(bind(mysocket, (struct sockaddr *)&myserver_address, sizeof(myserver_address))<0){
		perror("bind");
        exit(2);

	}
		
	listen(mysocket, 5);

	while(1){
		accpt = accept(mysocket, NULL, NULL);
		if(accpt<0){
			perror("accept");
			exit(3);
		}

		while(1){
			bytes_read = recv(accpt, jinfix, MAXVAL, 0);
			if(bytes_read <= 0) break;
			converttoString(jinfix,infix);
                        scanner(infix, postfix);
                        postfixscanner(postfix);
                        sprintf(result,"%d",pop(numstack));
			converttoJson(result,jresult);
                        send(accpt, jresult, MAXVAL, 0);
		}

		close(accpt); 	
	}

	return 0;

}



void scanner(char infixExp[], char postfixExp[]){
	int i, j;
		char item;
		char x;

		push('(', opstack);  
		strcat(infixExp,")");                  

		i=0;
		j=0;
		item=infixExp[i];

	while (item != '\0'){
		if(item == '(')
			push(item, opstack);
		else if(isdigit(item))
			postfixExp[j++] = item;
		else if(is_operator(item) == 1){
					x = pop(opstack);
					while(is_operator(x) ==1 && prec(item)<=prec(x)){
						postfixExp[j++] = x;
						x = pop(opstack);
					}
					push(x, opstack);
					push(item, opstack);

				}
		else if(item == ')'){
			x = pop(opstack);
			while(x != '('){
				postfixExp[j] = x;
				j++;
				x = pop(opstack);
			}
		}
		else{
			printf("\n Invalid infix Exp.\n");
			getchar();
			exit(1);
		}
		i++;
		item = infixExp[i];
	}

	if(top>0){
			printf("\nInvalid infix Expression.\n");
			getchar();
			exit(1);
		}
	postfixExp[j] = '\0';
}


void push(char item, char dest[]){
	if(top >= MAXVAL-1)
		printf("\nStack overflow\n");
	else{
		dest[++top] = item;
	}
}
void pushint(int item, char dest[]){
	if(top >= MAXVAL-1)
		printf("\nStack overflow\n");
	else{
		dest[++top] = item;
	}
}

char pop(char dest[]){
	char topitem;

	if(top<0){
		printf("stack is empty");
		getchar();
		exit(1);
	}
	else{
		topitem = dest[top--];
		return topitem;
	}
}

int is_operator(char symbol){
	if(symbol == '*' || symbol == '/' || symbol == '+' || symbol =='-')
		return 1;
	else
		return 0;
}

int prec(char symbol){
	if(symbol == '*' || symbol == '/')
		return 2;
	else if(symbol == '+' || symbol == '-')
		return 1;
	else
		return 0;
}



void postfixscanner(char postfixExp[]){
	int i = 0;
	int second;

	int item = postfixExp[i];

	while(item != '\0'){
		if(isdigit(item)){
			int p = item - '0';
			pushint(p, numstack);
		}
		else if(is_operator(item)){
			switch(item){
			case '+':
				push(pop(numstack)+pop(numstack),numstack);
				break;
			case '*':
				push(pop(numstack)*pop(numstack), numstack);
				break;
			case '-':
				second = pop(numstack);
				push(pop(numstack)-second, numstack);
				break;
			case '/':
				second = pop(numstack);
				if(second != 0){
					push(pop(numstack)/second, numstack);
				}
				else
					printf("zero divisor");
				break;
			default:
				printf("error: unknown command");
			}

		}
		i++;
		item = postfixExp[i];

	}
	numstack[i] = '\0';
}
