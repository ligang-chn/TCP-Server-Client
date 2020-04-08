#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Windows.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <iostream>

using namespace std;


int main() {
	//����Windows socket 2.x����
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//-------------------------
	//1)����һ��socket�׽���
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//2��������˿ڣ�bind
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);//host to net unsigned short
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//inet_addr("127.0.0.1");
	std::cout << "*********Server********" << endl;
	if (bind(_sock, (sockaddr *)&_sin, sizeof(_sin)) == SOCKET_ERROR) {
		std::cout << "ERROR,������˿�ʧ��###########" << endl;
	}
	else {
		std::cout << "������˿ڳɹ�*****************" << endl;
	}
	//3)listen��������˿�
	if (SOCKET_ERROR == listen(_sock, 5)) {
		std::cout << "ERROR,��������˿�ʧ��###########" << endl;
	}
	else {
		std::cout << "��������˿ڳɹ�*****************" << endl;
	}
	//4��accept�ȴ����ܿͻ��˵�����
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;


	_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (INVALID_SOCKET == _cSock) {
		std::cout << "ERROR,���ܵ���Ч�ͻ���SOCKET###########" << endl;
	}
	else {
		std::cout << "�¿ͻ��˼��룺IP = " << inet_ntoa(clientAddr.sin_addr) << " , socket = " << _cSock << endl;
	}

	char _recvBuf[128] = {};

	while (true)
	{
		//5)���տͻ�������
		int nLen = recv(_cSock, _recvBuf, 128, 0);
		if (nLen <= 0) {
			std::cout << "�ͻ������˳����������" << endl;
			break;
		}
		std::cout << "�յ����" << _recvBuf << endl;
		//6����������
		if (0 == strcmp(_recvBuf, "getName")) {
			//7)send��ͻ��˷���һ������
			char msgBuf[] = "LG";
			send(_cSock, msgBuf, strlen(msgBuf) + 1, 0);
		}
		else if (0 == strcmp(_recvBuf, "getAge")) {
			//7)send��ͻ��˷���һ������
			char msgBuf[] = "24";
			send(_cSock, msgBuf, strlen(msgBuf) + 1, 0);
		}
		else {
			//7)send��ͻ��˷���һ������
			char msgBuf[] = "I am server.";
			send(_cSock, msgBuf, strlen(msgBuf) + 1, 0);
		}
	}

	//8)�ر�socket
	closesocket(_sock);
	//-------------------------
	//���Windows socket����
	WSACleanup();
	std::cout << "���˳����������*" << endl;
	getchar();

	return 0;
}
