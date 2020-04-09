#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Windows.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <iostream>
#include<vector>
using namespace std;

enum CMD {
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGINOUT,
	CMD_LOGINOUT_RESULT,
	CMD_ERROR
};

struct DataHeader {
	short dataLength;
	short cmd;
};

//DataPackage
struct Login: public DataHeader
{
	Login() {
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char username[32];
	char password[32];
};
struct LoginResult: public DataHeader
{
	LoginResult() {
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};

struct Loginout: public DataHeader
{
	Loginout() {
		dataLength = sizeof(Loginout);
		cmd = CMD_LOGINOUT;
	}
	char username[32];
};

struct LoginoutResult: public DataHeader
{
	LoginoutResult() {
		dataLength = sizeof(LoginoutResult);
		cmd = CMD_LOGINOUT_RESULT;
		result = 0;
	}
	int result;
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

	switch (header->cmd)
	{
		case CMD_LOGIN:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			Login* login = (Login*)szRecv;
			std::cout << "收到命令：" << login->cmd << " ，数据长度：" << login->dataLength
				<< " , 用户：" << login->username << " ，密码： " << login->password << endl;

			LoginResult ret;
			send(_cSock, (char*)&ret, sizeof(ret), 0);
		}
		break;
		case CMD_LOGINOUT:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			Loginout* loginout = (Loginout*)szRecv;
			std::cout << "收到命令：" << loginout->cmd << " ，数据长度：" << loginout->dataLength
				<< " , 用户：" << loginout->username << endl;

			LoginoutResult ret;
			send(_cSock, (char*)&ret, sizeof(ret), 0);
		}
		break;
		default: {
			DataHeader header = { 0,CMD_ERROR };
			send(_cSock, (char*)&header, sizeof(header), 0);
		}
		break;
	}
}

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
		int ret=select(_sock+1,&fdRead,&fdWrite,&fdExp,nullptr);
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
