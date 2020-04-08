#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Windows.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <iostream>

using namespace std;


int main() {
	//启动Windows socket 2.x环境
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//-------------------------
	//1)建立一个socket套接字
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//2）绑定网络端口，bind
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);//host to net unsigned short
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//inet_addr("127.0.0.1");
	std::cout << "*********Server********" << endl;
	if (bind(_sock, (sockaddr *)&_sin, sizeof(_sin)) == SOCKET_ERROR) {
		std::cout << "ERROR,绑定网络端口失败###########" << endl;
	}
	else {
		std::cout << "绑定网络端口成功*****************" << endl;
	}
	//3)listen监听网络端口
	if (SOCKET_ERROR == listen(_sock, 5)) {
		std::cout << "ERROR,监听网络端口失败###########" << endl;
	}
	else {
		std::cout << "监听网络端口成功*****************" << endl;
	}
	//4）accept等待接受客户端的连接
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;


	_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (INVALID_SOCKET == _cSock) {
		std::cout << "ERROR,接受到无效客户端SOCKET###########" << endl;
	}
	else {
		std::cout << "新客户端加入：IP = " << inet_ntoa(clientAddr.sin_addr) << " , socket = " << _cSock << endl;
	}

	char _recvBuf[128] = {};

	while (true)
	{
		//5)接收客户端数据
		int nLen = recv(_cSock, _recvBuf, 128, 0);
		if (nLen <= 0) {
			std::cout << "客户端已退出，任务结束" << endl;
			break;
		}
		std::cout << "收到命令：" << _recvBuf << endl;
		//6）处理请求
		if (0 == strcmp(_recvBuf, "getName")) {
			//7)send向客户端发送一条数据
			char msgBuf[] = "LG";
			send(_cSock, msgBuf, strlen(msgBuf) + 1, 0);
		}
		else if (0 == strcmp(_recvBuf, "getAge")) {
			//7)send向客户端发送一条数据
			char msgBuf[] = "24";
			send(_cSock, msgBuf, strlen(msgBuf) + 1, 0);
		}
		else {
			//7)send向客户端发送一条数据
			char msgBuf[] = "I am server.";
			send(_cSock, msgBuf, strlen(msgBuf) + 1, 0);
		}
	}

	//8)关闭socket
	closesocket(_sock);
	//-------------------------
	//清除Windows socket环境
	WSACleanup();
	std::cout << "已退出，任务结束*" << endl;
	getchar();

	return 0;
}
