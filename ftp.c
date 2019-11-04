
#include <winsock.h>
#include <stdio.h>
#include <string.h>

int send_data(int sockfd, void* data, int length);
int recv_data(int sockfd, void* data, int maxbuflen);
int send_all(int sock, void* buffer, int length);

//encapsula a mensagem com um "header" que diz o tamanho da mensagem que segue
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

