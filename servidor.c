//SERVIDOR

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>

#define BACKLOG_MAX 5
#define BUFFER_SIZE 512
#define EXIT_CALL_STRING "#quit"

int local_socket = 0;
int remote_socket = 0;

int remote_length = 0;
int message_length = 0;

unsigned short local_port = 0;
unsigned short remote_port = 0;

struct sockaddr_in local_address;
struct sockaddr_in remote_address;

WSADATA wsa_data;

// Exibe uma mensagem de erro e termina o programa
void msg_err_exit(char *msg) {
	fprintf(stderr, msg);
	system("PAUSE");
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
	// inicia o Winsock 2.0 (DLL), Only for Windows
	if (WSAStartup(MAKEWORD(2, 0), &wsa_data) != 0)
		msg_err_exit("WSAStartup() failed\n");

	// criando o socket local para o servidor
	local_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (local_socket == INVALID_SOCKET) {
		WSACleanup();
		msg_err_exit("socket() failed\n");
	}

	printf("Porta local: ");
	scanf("%d", &local_port);
	fflush(stdin);

	// zera a estrutura local_address
	memset(&local_address, 0, sizeof(local_address));

	// internet address family
	local_address.sin_family = AF_INET;

	// porta local
	local_address.sin_port = htons(local_port);

	// endereco
	local_address.sin_addr.s_addr = htonl(INADDR_ANY); // inet_addr("127.0.0.1")

	// interligando o socket com o endereço (local)
	if (bind(local_socket, (struct sockaddr *) &local_address,
			sizeof(local_address)) == SOCKET_ERROR) {
		WSACleanup();
		closesocket(local_socket);
		msg_err_exit("bind() failed\n");
	}

	// coloca o socket para escutar as conexoes
	if (listen(local_socket, BACKLOG_MAX) == SOCKET_ERROR) {
		WSACleanup();
		closesocket(local_socket);
		msg_err_exit("listen() failed\n");
	}

	remote_length = sizeof(remote_address);

	printf("aguardando alguma conexao...\n");
	remote_socket = accept(local_socket, (struct sockaddr *) &remote_address,
			&remote_length);
	if (remote_socket == INVALID_SOCKET) {
		WSACleanup();
		closesocket(local_socket);
		msg_err_exit("accept() failed\n");
	}

	printf("conexao estabelecida com %s\n", inet_ntoa(remote_address.sin_addr));

	FILE * file = fopen("download.txt", "w");
	if(file == NULL){
		printf("Erro na criacao do arquivo.\n");
		return 1;
	}
	printf("aguardando mensagens...\n");

	char buffer[BUFFER_SIZE+1];
	while(1){
		if(recv(remote_socket, buffer, BUFFER_SIZE, 0)== SOCKET_ERROR){
			msg_err_exit("Erro no recebimento dos dados.\n");
		}
		if(buffer[0]== '\x04'){ //verifica se eh um sinal de fim de transmissao
			break;
		}
		printf("Buffer: %s\n", buffer);
		fwrite(buffer, sizeof(char), sizeof(buffer), file);

	}
	fclose(file);

	/*
	  do
	 {
		 // limpa o buffer
	 	 memset(&file, 0, BUFFER_SIZE);

		 // recebe a mensagem do cliente
	 	 message_length = recv(remote_socket, file, BUFFER_SIZE, 0);
	 	 if(message_length == SOCKET_ERROR)
	 	 msg_err_exit("recv() failed\n");

		 // exibe a mensagem na tela
	 	 printf("%s: %s\n", inet_ntoa(remote_address.sin_addr), file);
	 }while(strcmp(file, EXIT_CALL_STRING)); // sai quando receber um "#quit" do cliente
	 */


	printf("encerrando\n");
	WSACleanup();
	closesocket(local_socket);
	closesocket(remote_socket);

	system("PAUSE");
	return 0;
}
