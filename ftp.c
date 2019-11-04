
#include <winsock.h>
#include <stdio.h>
#include <string.h>

#ifndef BUFFER_LENGTH
#define BUFFER_LENGTH 512
#endif

int send_file(int sock, char * filename);
int recv_file(int sock);
int send_data(int sock, void* data, int length);
int recv_data(int sock, void* data, int maxbuflen);
int send_all(int sock, void* buffer, int length);
char *get_filename_extension(const char *filename);

int send_file(int sock, char * filename){

	FILE * file;
	file = fopen(filename, "rb");

	if (file == NULL) {
		printf("\n Nao foi possivel ler o arquivo");
		send_data(sock,"",0); // mensagem de fim de transmissao, precisa ser checada no servidor.c
		return 1;
	}

	char buffer[BUFFER_LENGTH+1];
	memset(buffer, 0, sizeof(buffer));
	int bytesRead;
	int bytesSent;

	//manda o nome do arquivo
	bytesSent = send_data(sock, filename, strlen(filename));
	if (bytesSent == SOCKET_ERROR) {
		return SOCKET_ERROR;
	}

	while(1){
		bytesRead = fread(buffer,1,BUFFER_LENGTH,file);
		bytesSent = send_data(sock, buffer, bytesRead);
		if (bytesSent == SOCKET_ERROR) { //nao temos garantia de que todos os dados foram enviados
			return SOCKET_ERROR;
		}

		if(feof(file)){ //se lemos todo o arquivo, mandamos o sinal de fim de transmissao, com tamanho 0
			send_data(sock,"",0);
			break;
		}

	}

	fclose(file);
	return 0;
}

int recv_file(int sock){
	char buffer[BUFFER_LENGTH+1];
	int bytesReceived;

	//espera pelo nome do arquivo
	bytesReceived = recv_data(sock, buffer, BUFFER_LENGTH);
	if (bytesReceived == SOCKET_ERROR) {
		return SOCKET_ERROR;
	}
	buffer[bytesReceived] = '\0';

	char *ext = get_filename_extension(buffer);

	char outputFilename[] = "download";

	strcat(outputFilename, ext);
	FILE * file = fopen(outputFilename, "wb");
	if (file == NULL) {
		printf("Erro na criacao do arquivo.\n");
		return 1;
	}

	while (1) {
		bytesReceived = recv_data(sock, buffer, BUFFER_LENGTH);
		if (bytesReceived == SOCKET_ERROR) {
			return SOCKET_ERROR;
		}
		if (bytesReceived == 0) { //verifica se eh um sinal de fim de transmissao
			break;
		}

		fwrite(buffer, sizeof(char), bytesReceived, file);

	}
	fclose(file);
	return 0;
}

//retorna o numero de bytes enviados(i.e. o tamanho da mensagem apenas) ou SOCKET_ERROR
int send_data(int sock, void* data, int length){
	int bytes = 0;
	char strLength[10];
	snprintf(strLength, 10, "%d", length);
	// envia tamanho da mensagem
	if (send_all(sock, strLength, sizeof(strLength)) == SOCKET_ERROR) {
		return SOCKET_ERROR;
	}
	bytes = send_all(sock, data, length);
	// envia a mensagem
	if (bytes == SOCKET_ERROR) {
		return SOCKET_ERROR;
	}

	return bytes;
}
//extrai o header da mensagem
//retorna o numero de bytes enviados(i.e. o tamanho da mensagem apenas), SOCKET_ERROR ou 0 caso o header indique
int recv_data(int sock, void* data, int maxbuflen){
	int bytes = 0;
	char strLength[10];
	int length = 0;
	//primeiro pegamos o tamanho dos dados que seguirao
	if(recv(sock,strLength,10,0) == SOCKET_ERROR){
		return SOCKET_ERROR;
	}
	sscanf(strLength,"%d",&length);
	if(length == 0){
		return 0;
	}
	int preferredLength;
	//agora os dados
	if(length > maxbuflen){
		preferredLength = maxbuflen;
	}
	else preferredLength = length;
	bytes = recv(sock,data,preferredLength,0);
	if(bytes == SOCKET_ERROR){
		return SOCKET_ERROR;
	}
	return bytes;
}

//garante o envio da mensagem ate que tudo seja enviado ou ocorra um erro.
int send_all(int sock, void *buff, int length) {
	int total = 0; // quantos bytes mandamos
	int n;

	while (total < length) {
		n = send(sock, buff + total, length, 0);
		if (n == SOCKET_ERROR) {
			return SOCKET_ERROR;
		}
		total += n;
		length -= n;
	}

	return total;
}

char *get_filename_extension(const char *filename) {
    char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot;
}

