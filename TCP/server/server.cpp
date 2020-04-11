#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Windows.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <iostream>
#include<vector>
using namespace std;

//enum NAME {
//	MSG_CODE,
//	SEND_TIME,
//	BFNO,
//	LEDNO1,
//	DATA1,
//	LEDNO2,
//	DATA2
//};

struct DataHeader {
	short dataLength;
};

//DataPackage
struct DataPackage : public DataHeader
{
	DataPackage() {
		dataLength = sizeof(DataPackage);
	}
	char msgCode[8];
	char sendTime[14];
	char bfno[2];
	char ledNo1[2];
	char data1[400];
	char ledNo2[2];
	char data2[400];
};


std::vector<SOCKET> g_clients;

int processor(SOCKET _cSock) {
	//缓冲区
	char szRecv[1024] = {};
	//5)接收客户端数据
	int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0) {
		std::cout << "客户端已退出，任务结束" << endl;
		return -1;
	}
	

	recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
	DataPackage *dataPackage= (DataPackage*)szRecv;
	std::cout << "收到数据长度：" << dataPackage->dataLength << endl;
	std::cout << "socket = " << _cSock<<",数据如下：---------" << endl;
	std::cout << "--电文号：" << dataPackage->msgCode << endl;
	std::cout << "--发送时间：" << dataPackage->sendTime << endl;
	std::cout << "--高炉号：" << dataPackage->bfno << endl;
	std::cout << "--大屏1：" << dataPackage->ledNo1 << endl;
	std::cout << "--数据1：" << dataPackage->data1 << endl;
	std::cout << "--大屏2：" << dataPackage->ledNo2 << endl;
	std::cout << "--数据2：" << dataPackage->data2 << endl;
	std::cout << "---------单次接收完成" << endl;

	return 0;

}
#if 1
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
	

	
	while (true)
	{
		//select模型，伯克利socket
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExp;
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);

		FD_SET(_sock, &fdRead);
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdExp);

		for (int n = (int)g_clients.size()-1; n >= 0 ; --n) {
			FD_SET(g_clients[n], &fdRead);
		}
		//nfds是一个整数值，是指fd_set集合中所有描述符（socket）的范围，而不是数量
		//既是所有文件描述符最大值+1，在windows中这个参数可以写0
		timeval t = { 0,0 };
		int ret=select(_sock+1,&fdRead,&fdWrite,&fdExp,&t);
		if (ret < 0) {
			std::cout << "select任务结束" << endl;
			break;
		}
		if (FD_ISSET(_sock, &fdRead)) {
			FD_CLR(_sock, &fdRead);
			//4）accept等待接受客户端的连接
			sockaddr_in clientAddr = {};
			int nAddrLen = sizeof(sockaddr_in);
			SOCKET _cSock = INVALID_SOCKET;
			_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
			if (INVALID_SOCKET == _cSock) {
				std::cout << "ERROR,接受到无效客户端SOCKET###########" << endl;
			}
			else {
				g_clients.push_back(_cSock);
				std::cout << "新客户端加入：IP = " << inet_ntoa(clientAddr.sin_addr) << " , socket = " << _cSock << endl;
			}
		}
		for (size_t n = 0; n < fdRead.fd_count ; ++n) {
			if (-1 == processor(fdRead.fd_array[n])) {
				auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
				if (iter != g_clients.end()) {
					g_clients.erase(iter);
				}
			}
			
		}
	}
	for (size_t n = g_clients.size() - 1; n >= 0; --n) {
		closesocket(g_clients[n]);
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
#endif
