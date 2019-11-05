#include <windows.h>
#include <stdio.h>

#define ENVIAR 1
#define ABRIR 2

///////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>
#include "ftp.c"
///////////////////////

char arquivo[512];
char ip[16];
char porta[6];

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

void AddMenus(HWND);
void AddControls(HWND);

HMENU hMenu;
HWND hIp;
HWND hPort;
HWND hArquivo;

////////////////////////////////////////
int remote_socket = 0;
int message_length = 0;

unsigned short remote_port = 0;

char remote_ip[32];

struct sockaddr_in remote_address;

WSADATA wsa_data;
////////////////////////////////////////



void msg_err_exit(char *msg) {
	fprintf(stderr, msg);
	return -1;
}

int cliente() {
	if (WSAStartup(MAKEWORD(2, 0), &wsa_data) != 0)
		msg_err_exit("WSAStartup() failed\n");


	remote_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (remote_socket == INVALID_SOCKET) {
		WSACleanup();
		msg_err_exit("socket() failed\n");
	}

	// preenchendo o remote_address (servidor)
	memset(&remote_address, 0, sizeof(remote_address));
	remote_address.sin_family = AF_INET;
	remote_address.sin_addr.s_addr = inet_addr(ip);
	remote_port = atoi(porta);
	remote_address.sin_port = htons(remote_port);

	printf("conectando ao servidor %s:%d\n", ip, remote_port);
	if (connect(remote_socket, (struct sockaddr *) &remote_address,
			sizeof(remote_address)) == SOCKET_ERROR) {
		WSACleanup();
		msg_err_exit("connect() failed\n");
	}

	if(send_file(remote_socket, arquivo) == SOCKET_ERROR){
		WSACleanup();
		closesocket(remote_socket);
		msg_err_exit("Erro na transferencia do arquivo\n");
	}

	printf("encerrando\n");
	WSACleanup();
	closesocket(remote_socket);


	return 0;
}



int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {

	WNDCLASSW wc = {0};

	wc.hbrBackground = (HBRUSH) COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hInst;
	wc.lpszClassName = L"windowClass";
	wc.lpfnWndProc = WindowProcedure;

	if(!RegisterClassW(&wc))
		return -1;

	CreateWindow("windowClass", "File Sender", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 500, 330, NULL, NULL, NULL, NULL);

	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_SHOW);

	MSG msg = {0};

	while(GetMessage(&msg, NULL, (int) NULL, (int) NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

void abrirArquivo(HWND hWnd) {

	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = arquivo;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 512;
	ofn.lpstrFilter = "All files\0*.*\0";
	ofn.nFilterIndex = 1;

	GetOpenFileName(&ofn);
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch(msg) {
		case WM_COMMAND:
			switch(wp) {
				case ENVIAR:
					GetWindowText(hIp, ip, 16);
					GetWindowText(hPort, porta, 6);
					cliente();
					////////////////////////////////////
					// CHAMAR A FUNÇÃO DE ENVIAR AQUI //
					////////////////////////////////////
					
					break;
				case ABRIR:
					abrirArquivo(hWnd);
					SetWindowText(hArquivo, arquivo);
					break;
			}
			break;
		case WM_CREATE:
			AddControls(hWnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProcW(hWnd, msg, wp, lp);
	}
}

void AddControls(HWND hWnd) {

	CreateWindowW(L"Static", L"IP :", WS_VISIBLE | WS_CHILD | SS_CENTER, 110, 60, 25, 20, hWnd, NULL, NULL, NULL);
	hIp = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 145, 60, 115, 20, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"Static", L"Porta :", WS_VISIBLE | WS_CHILD | SS_CENTER, 285, 60, 50, 20, hWnd, NULL, NULL, NULL);
	hPort = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 335, 60, 45, 20, hWnd, NULL, NULL, NULL);
	hArquivo = CreateWindowW(L"Static", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 60, 140, 255, 20, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"Button", L"Abrir Arquivo", WS_VISIBLE | WS_CHILD, 320, 140, 100, 20, hWnd, (HMENU) ABRIR, NULL, NULL);
	CreateWindowW(L"Button", L"Enviar", WS_VISIBLE | WS_CHILD, 220, 220, 60, 20, hWnd, (HMENU) ENVIAR, NULL, NULL);
}

//////////////////////////////
//////////////////////////////

