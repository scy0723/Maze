#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "maze.h"

// Game ���� �Լ�
void TextColorChange(int);
void GotoXY(int, int);
void DrawMap(int, int, int, int);
void RunGame(int, int);
int Checker(int, int);
int GameOverChecker(int, int);

// Socket ���� �Լ�
void ErrorHandling(const char*);
void InitConn();
void CloseConn();
void SendToServer(int, int);
unsigned _stdcall RecvFromServer(void*);

// Socket ���� ���� ����
WSADATA wsaData;
SOCKET cSocket;
SOCKADDR_IN servAddr;
char message[10];

int playerNo = 0;

int main(int argc, char* argv[])
{
	if (argc < 2) {
		fputs("Input the Player ID", stderr);
		exit(-1);
	}

	InitConn();

	int player1PosX = 4, player1PosY = 3;
	int player2PosX = 32, player2PosY = 16;

	// 1P, 2P �ʿ� �׷��ֱ�
	DrawMap(player1PosX, player1PosY, player2PosX, player2PosY);

	_beginthreadex(NULL, 0, RecvFromServer, 0, 0, NULL);

	if (0 == strncmp(argv[1], "1", sizeof(char))) {			// 1P�� ������ ��
		playerNo = 1;
		RunGame(player1PosX, player1PosY);
	}
	else if (0 == strncmp(argv[1], "2", sizeof(char))) {	// 2P�� ������ ��
		playerNo = 2;
		RunGame(player2PosX, player2PosY);
	}

	CloseConn();

	return 0;
}

unsigned _stdcall RecvFromServer(void* args)
{
	char message[10] = { 0, };

	int otherPlayerPosX = 0, otherPlayerPosY = 0;

	if (1 == playerNo) {	//���� 1 �÷��̾���
		otherPlayerPosX = 32, otherPlayerPosY = 18;		// �ٸ� ������ �ʱ� ��ġ�� 32, 18
	}
	else if (2 == playerNo) {	//���� 2 �÷��̾���
		otherPlayerPosX = 4, otherPlayerPosY = 5;		// �ٸ� ������ �ʱ� ��ġ�� 4, 5
	}

	while (1) {

		int res = 0;
		memset(message, 0x00, sizeof(message));

		res = recv(cSocket, message, sizeof(message) - 1, 0);

		if (-1 == res) {
			ErrorHandling("recv() error");
		}

		if (0 == strncmp(message, "exit", sizeof("exit"))) {
			break;
		}

		GotoXY(otherPlayerPosX, otherPlayerPosY);		// ������ �ִ� ��ġ�� �����.
		printf("  ");

		GotoXY(message[0], message[1]);					// �ٸ� �÷��̾���� ���ŵ� ��ǥ�� �׸���.
		printf("��");

		otherPlayerPosX = message[0], otherPlayerPosY = message[1];

		GotoXY(0, 22);
		printf("x : %d, y : %d", message[0], message[1]);

	}

	return 0;
}

void SendToServer(int cieclePosX, int circlePosY)
{
	message[0] = cieclePosX;
	message[1] = circlePosY;

	send(cSocket, message, sizeof(message), 0);
}

void InitConn()
{
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		ErrorHandling("WSAStartup() error!");
	}

	cSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (INVALID_SOCKET == cSocket) {
		ErrorHandling("socket() error!");
	}

	// ���� ���� ���� �ʱ�ȭ
	memset(&servAddr, 0x00, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servAddr.sin_port = PORT;

	// ����
	if (SOCKET_ERROR == connect(cSocket, (SOCKADDR*)&servAddr, sizeof(servAddr))) {
		ErrorHandling("connect() error!");
	}
}

void CloseConn()
{
	closesocket(cSocket);

	WSACleanup();
}

void ErrorHandling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stdout);
	exit(-1);

}

void TextColorChange(int colorNumber)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorNumber);
}

void GotoXY(int x, int y)
{
	COORD pos = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void DrawMap(int player1PosX, int player1PosY, int player2PosX, int player2PosY)
{
	int idx = 0, idy = 0;

	system("mode con cols=37 lines=24");

	GotoXY(8, 1), TextColorChange(15), printf("�ڡ�  �̷� ã��  �ڡ�");

	GotoXY(0, 2), TextColorChange(12);

	for (idx = 0; idx < HEIGHT; idx++) {

		for (idy = 0; idy < WIDTH; idy++) {

			switch (map[idx][idy]) {

			case 0:
				printf("  ");
				break;
			case 1:
				TextColorChange(8);
				printf("��");
				break;
			case 2:
				TextColorChange(12);
				printf("��");
				break;
			default:
				break;
			}

		}

		printf("\n");

	}

	TextColorChange(14);
	GotoXY(player1PosX, player1PosY + 2);
	printf("��");

	TextColorChange(14);
	GotoXY(player2PosX, player2PosY + 2);
	printf("��");

}

int Checker(int x, int y)
{
	switch (map[y][x]) {

	case 0:
	case 2:
		return 1;
		break;
	default:
		return 0;
		break;

	}
}

void RunGame(int circlePosX, int circlePosY)
{
	int cmd = 0;

	while (1) {

		cmd = _getch();

		TextColorChange(14);

		switch (cmd) {

		case UP:

			GotoXY(circlePosX, circlePosY + 2);
			printf("  ");

			circlePosY = circlePosY - 1;

			if (!Checker(circlePosX, circlePosY)) {
				circlePosY = circlePosY + 1;
			}

			SendToServer(circlePosX, circlePosY + 2);
			break;

		case DOWN:

			GotoXY(circlePosX, circlePosY + 2);
			printf("  ");

			circlePosY = circlePosY + 1;

			if (!Checker(circlePosX, circlePosY)) {
				circlePosY = circlePosY - 1;
			}

			SendToServer(circlePosX, circlePosY + 2);
			break;

		case LEFT:

			GotoXY(circlePosX, circlePosY + 2);
			printf("  ");

			circlePosX = circlePosX - 2;

			if (!Checker(circlePosX, circlePosY)) {
				circlePosX = circlePosX + 2;
			}

			SendToServer(circlePosX, circlePosY + 2);
			break;

		case RIGHT:

			GotoXY(circlePosX, circlePosY + 2);
			printf("  ");

			circlePosX = circlePosX + 2;

			if (!Checker(circlePosX, circlePosY)) {
				circlePosX = circlePosX - 2;
			}

			SendToServer(circlePosX, circlePosY + 2);
			break;

		case EXIT:
			send(cSocket, "exit", sizeof("exit"), 0);
			return;

		}

		GotoXY(circlePosX, circlePosY + 2);
		printf("��");

		if (GameOverChecker(circlePosX, circlePosY)) {
			break;
		}
		else {
			continue;
		}

	}
}

int GameOverChecker(int circlePosX, int circlePosY)
{
	if (32 == circlePosX && 3 == circlePosY || 33 == circlePosX && 3 == circlePosY) {

		system("cls");

		int color = 0;

		for (; color <= 15; color++) {
			GotoXY(10, 11);
			TextColorChange(color);
			printf("�̷� Ż�� ����!");
			Sleep(100);
		}

		return 1;

	}

	return 0;

}