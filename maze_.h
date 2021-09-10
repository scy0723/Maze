
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <conio.h>
#include <process.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT	7777

#define UP      72
#define DOWN    80      //키보드 방향키의 아스키코드값을 정의해둔것입니다.
#define LEFT    75      //이런식으로 정의해두면 편하게 사용할 수 있습니다.
#define RIGHT   77 
#define EXIT    113     // 아스키코드 소문자 q

#define HEIGHT  20
#define WIDTH   36

int map[HEIGHT][WIDTH] = {
     {0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9,0,9},
     {1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9},
     {1,9,0,9,0,9,1,9,0,9,0,9,2,9,1,9,0,9,0,9,0,9,0,9,0,9,0,9,1,9,0,9,0,9,1,9},
     {1,9,0,9,0,9,1,9,0,9,1,9,2,9,0,9,0,9,0,9,1,9,1,9,1,9,1,9,1,9,2,9,2,9,1,9},//하트 32,3
     {1,9,0,9,1,9,1,9,1,9,1,9,2,9,1,9,1,9,1,9,1,9,0,9,0,9,0,9,1,9,2,9,0,9,1,9},
     {1,9,0,9,0,9,0,9,0,9,1,9,2,9,0,9,1,9,0,9,0,9,0,9,1,9,0,9,0,9,2,9,0,9,1,9},
     {1,9,0,9,1,9,1,9,0,9,1,9,2,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,0,9,1,9},
     {1,9,0,9,2,9,1,9,0,9,1,9,2,9,2,9,0,9,0,9,0,9,0,9,1,9,0,9,0,9,0,9,0,9,1,9},
     {1,9,1,9,2,9,1,9,0,9,1,9,1,9,2,9,1,9,1,9,1,9,0,9,1,9,0,9,1,9,2,9,0,9,1,9},
     {1,9,0,9,2,9,1,9,0,9,0,9,0,9,0,9,1,9,2,9,0,9,0,9,1,9,0,9,1,9,1,9,1,9,1,9},
     {1,9,0,9,1,9,1,9,1,9,0,9,2,9,1,9,0,9,2,9,1,9,0,9,1,9,2,9,2,9,2,9,0,9,1,9},
     {1,9,1,9,1,9,0,9,1,9,1,9,2,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,0,9,1,9,1,9,1,9},
     {1,9,0,9,0,9,0,9,0,9,0,9,2,9,1,9,2,9,1,9,0,9,0,9,2,9,1,9,0,9,0,9,0,9,1,9},
     {1,9,0,9,1,9,0,9,1,9,1,9,1,9,2,9,2,9,0,9,0,9,1,9,2,9,1,9,1,9,1,9,0,9,1,9},
     {1,9,0,9,1,9,1,9,1,9,0,9,0,9,0,9,1,9,1,9,0,9,1,9,2,9,0,9,0,9,0,9,0,9,1,9},
     {1,9,0,9,2,9,1,9,0,9,0,9,1,9,0,9,0,9,1,9,1,9,1,9,2,9,1,9,0,9,1,9,1,9,1,9},
     {1,9,1,9,2,9,1,9,1,9,0,9,1,9,1,9,0,9,0,9,0,9,1,9,2,9,1,9,0,9,1,9,0,9,1,9},
     {1,9,0,9,2,9,0,9,0,9,0,9,0,9,0,9,0,9,1,9,0,9,0,9,2,9,1,9,0,9,0,9,0,9,1,9},
     {1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9}
};