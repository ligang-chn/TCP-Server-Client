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
	std::cout << "*********Client********" << endl;
	if (INVALID_SOCKET == _sock) {
		std::cout << "ERROR,����socketʧ��###########" << endl;
	}
	else
	{
		std::cout << "����socket�ɹ�*****************" << endl;
	}
	//2)connect������
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);//host to net unsigned short
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//inet_addr("127.0.0.1");
	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (SOCKET_ERROR == ret) {
		std::cout << "ERROR,����serverʧ��###########" << endl;
	}
	else
	{
		std::cout << "����server�ɹ�*****************" << endl;
	}

	while (true)
	{
		//3)������������
		char cmdBuf[128] = {};
		std::cin >> cmdBuf;
		//4��������������
		if (0 == strcmp(cmdBuf, "exit")) {
			std::cout << "�յ�exit����������" << endl;
			break;
		}
		else {//5���������������������
			send(_sock, cmdBuf, strlen(cmdBuf) + 1, 0);
		}

		//6�����շ�������Ϣ
		char recvBuf[128] = {};
		recv(_sock, recvBuf, 128, 0);
		std::cout << "���յ����ݣ�" << recvBuf << endl;
	}


	//7)�ر��׽���
	closesocket(_sock);
	WSACleanup();
	std::cout << "���˳����������*" << endl;
	getchar();

	return 0;
}