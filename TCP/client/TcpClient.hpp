#pragma once

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#include <Windows.h>
	#include <WinSock2.h>
	#pragma comment(lib,"ws2_32.lib")
	#include <iostream>
	#include <string.h>
	#include <thread>
#else
	#include<unistd.h>
	#include<arpa/inet.h>
	#include<string.h>
#endif //_WIN32



class TcpClient {
public:
	TcpClient() {

	}

	virtual ~TcpClient() {

	}

	//初始化socket
	int initSocket() {
		//启动win sock 2.x环境

	}

	//连接服务器
	int Connect(char* ip, unsigned short port) {

	}

	//关闭socket
	void Close() {
		//关闭win sock 2.x环境

	}

	//发送数据


	//接收数据



};

