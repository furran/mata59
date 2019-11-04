/*________________________________________________________________________________________________________________*/

/* CLIENTE */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>
#include "ftp.c"

#define BUFFER_SIZE 512
#define EXIT_CALL_STRING "#quit"

int remote_socket = 0;
int message_length = 0;

unsigned short remote_port = 0;

char remote_ip[32];

struct sockaddr_in remote_address;

WSADATA wsa_data;

/* Exibe uma mensagem de erro e termina o programa */
void msg_err_exit(char *msg) {
	fprintf(stderr, msg);
	system("PAUSE");
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
	if (WSAStartup(MAKEWORD(2, 0), &wsa_data) != 0)
		msg_err_exit("WSAStartup() failed\n");

	printf("IP do servidor: ");
	scanf("%s", remote_ip);
	fflush(stdin);

	printf("Porta do servidor: ");
	scanf("%d", &remote_port);
	fflush(stdin);

	remote_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (remote_socket == INVALID_SOCKET) {
		WSACleanup();
		msg_err_exit("socket() failed\n");
	}

	// preenchendo o remote_address (servidor)
	memset(&remote_address, 0, sizeof(remote_address));
	remote_address.sin_family = AF_INET;
	remote_address.sin_addr.s_addr = inet_addr(remote_ip);
	remote_address.sin_port = htons(remote_port);

	printf("conectando ao servidor %s...\n", remote_ip);
	if (connect(remote_socket, (struct sockaddr *) &remote_address,
			sizeof(remote_address)) == SOCKET_ERROR) {
		WSACleanup();
		msg_err_exit("connect() failed\n");
	}

	printf("nome do arquivo:\n");
	char filename[BUFFER_SIZE];
	scanf("%s", filename);

	if(send_file(remote_socket, filename) == SOCKET_ERROR){
		WSACleanup();
		closesocket(remote_socket);
		msg_err_exit("Erro na transferencia do arquivo\n");
	}

	printf("encerrando\n");
	system("PAUSE");
	WSACleanup();
	closesocket(remote_socket);


	return 0;
}
