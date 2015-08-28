// gcc -ggdb -Wall -std=c99 -m64 -fno-stack-protector -z execstack -o vulnerable vulnerable.c
/* Xeon - 2013*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFSIZE 1024
#define PORT (unsigned short) 0x697A /* 31337 */

void hidden_function()
{
        printf("Found the tresure!");
        exit(0);
}

void string_copy(char *s)
{
	char buffer[32];
	strcpy(buffer, s); //vulnerable function
	printf("%s\n", buffer);
}

int main(int argc, char *argv[])
{
	if(argv[1])
	{
		string_copy(argv[1]);
	}

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	memset(&serv_addr, 0, sizeof serv_addr);
	memset(&clnt_addr, 0, sizeof clnt_addr);

	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = 0x697A;
	unsigned int s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	unsigned int c, clntlen;
	char recvbuf[BUFSIZE];

	FILE *file = stdin;
	fread(&recvbuf, 1, BUFSIZE, file);

	if(!s) return 1;

	if(bind(s, (struct sockaddr *) &serv_addr, sizeof(serv_addr))){
		perror("bind() failed: ");
		return 1;
	}

	if(listen(s, 1)){
		perror("listen() failed: ");
		return 1;
	}
	clntlen = sizeof(clnt_addr);

	while(1){
		c = accept(s, (struct sockaddr *) &clnt_addr, &clntlen);
		if(!c) return 1;

		printf("Incoming connection from %s\n", inet_ntoa(clnt_addr.sin_addr));
		recv(c, recvbuf, BUFSIZE, 0);
		printf("Received %zu bytes...\n", strlen(recvbuf));
		string_copy(recvbuf);
	}

	return 0;
}
