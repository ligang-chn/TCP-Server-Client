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

	//��ʼ��socket
	int initSocket() {
		//����win sock 2.x����

	}

	//���ӷ�����
	int Connect(char* ip, unsigned short port) {

	}

	//�ر�socket
	void Close() {
		//�ر�win sock 2.x����

	}

	//��������


	//��������



};

