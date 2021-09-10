#define _WINSOCK_DEPRECATED_NO_WARNINGS		// 소켓 버전을 업데이트함에 따라 이전에 사용되던 함수를 금지하는 것을 무시

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>  // 소켓, 윈도우에서 통신을 관장하는 헤더
#include <process.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT		7777
#define MAX_CLIENT	2

void InitServer();
void RunServer();
void CloseServer();
void ErrorHandling(const char*);	// 예외처리 함수

unsigned _stdcall Client1RecvThread(void*);			// 시스템에서 제공해주는 공짜 쓰레드

WSADATA wsaData;
SOCKET servSock, clientSock[2];			// 디스크립터
SOCKADDR_IN servAddr, clientAddr[2];	// 주소에 관련된 구조체

int clientAddrLen = 0;	// 클라이언트 주소 정보를 가지고 있는 데이터의 길이
char client1Cmd[10];	// client 1로부터 받는 명령
char client2Cmd[10];	// client 2로부터 받는 명령

int main(int argc, char* argv[])
{
	system("mode con cols=37 lines=24");		// 콘솔 크기 조절

	InitServer();

	RunServer();

	CloseServer();

	return 0;
}

void RunServer()
{
	printf("START GAME!\n");

	// 1 player 수신 쓰레드
	_beginthreadex(NULL, 0, Client1RecvThread, 0, 0, NULL);

	// 2 player의 요청을 처리하는 로직
	while (1) {

		int res = 0;
		memset(client2Cmd, 0x00, sizeof(client2Cmd));

		res = recv(clientSock[1], client2Cmd, sizeof(client2Cmd) - 1, 0);		// 2번으로부터 오는 데이터를 한없이 대기. 올 때까지.

		if (-1 == res) {
			ErrorHandling("recv() error!");
		}

		if (0 == strcmp(client2Cmd, "exit")) {
			break;
		}

		if (0 == client2Cmd[0] && 0 == client2Cmd[1]) {
			continue;
		}

		send(clientSock[0], client2Cmd, sizeof(client2Cmd) - 1, 0);

		printf("[2P 이동] x : %d, y : %d \n", client2Cmd[0], client2Cmd[1]);

	}
}

unsigned _stdcall Client1RecvThread(void* args)
{
	while (1) {

		int res = 0;
		memset(client1Cmd, 0x00, sizeof(client1Cmd));

		res = recv(clientSock[0], client1Cmd, sizeof(client1Cmd) - 1, 0);		// 1번으로부터 오는 데이터를 한없이 대기. 올 때까지.

		if (-1 == res) {
			ErrorHandling("recv() error!");
		}

		if (0 == strcmp(client1Cmd, "exit")) {
			break;
		}

		if (0 == client1Cmd[0] && 0 == client1Cmd[1]) {
			continue;
		}

		send(clientSock[1], client1Cmd, sizeof(client1Cmd) - 1, 0);

		printf("[1P 이동] x : %d, y : %d \n", client1Cmd[0], client1Cmd[1]);

	}

	printf("Client1RecvThread Close.\n");

	return 0;
}

void InitServer()
{
	// 소켓 라이브러리 초기화 (버전, IP 형식(V4, V6))//////////////////////////////// STEP1
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		ErrorHandling("WSAStartup() error!");
	}

	// 전화기를 설치한다.							/////////////////////////////// STEP 2
	servSock = socket(AF_INET, SOCK_STREAM, 0);

	if (INVALID_SOCKET == servSock) {
		ErrorHandling("socket() error!");
	}

	// 소켓 정보 초기화								////////////////////////////// STEP 3
	memset(&servAddr, 0x00, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);		// 서버의 IP 초기화
	servAddr.sin_port = PORT;							// 포트 초기화

	// 소켓에 IP 주소와 PORT 번호 결합
	if (SOCKET_ERROR == bind(servSock, (SOCKADDR*)&servAddr, sizeof(servAddr))) {	/////////// STEP 4
		ErrorHandling("bind() error");
	}

	// 클라이언트 연결 대기
	if (SOCKET_ERROR == listen(servSock, MAX_CLIENT)) {	/////////////////////////// STEP 5
		ErrorHandling("listen() error");
	}

	fputs("유저 접속 대기중...", stdout);

	clientAddrLen = sizeof(clientAddr[0]);	// 클라이언트의 주소 정보의 길이는 버전마다 상이하기 때문에 중요한 요소

	for (int idx = 0; idx < MAX_CLIENT; idx++) {

		// 클라이언트 연결 수락
		clientSock[idx] = accept(servSock, (SOCKADDR*)&(clientAddr[idx]), &clientAddrLen);		///////////////// STEP 6

		if (INVALID_SOCKET == clientSock[idx]) {
			ErrorHandling("accept() error");
		}
		else {
			printf("%s Connection Complete! \n", inet_ntoa(clientAddr[idx].sin_addr));
		}

	}
}

void CloseServer()
{
	for (int idx = 0; idx < MAX_CLIENT; idx++) {
		closesocket(clientSock[idx]);
	}

	closesocket(servSock);

	WSACleanup();
}

void ErrorHandling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stdout);
	exit(-1);

}