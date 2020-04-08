#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Windows.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <iostream>

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

	
	while (true)
	{
		//缓冲区
		char szRecv[1024] = {};
		//5)接收客户端数据
		int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
		DataHeader* header = (DataHeader*)szRecv;
		if (nLen <= 0) {
			std::cout << "客户端已退出，任务结束" << endl;
			break;
		}

		switch (header->cmd)
		{
			case CMD_LOGIN:
				{
					recv(_cSock, szRecv+sizeof(DataHeader), header->dataLength-sizeof(DataHeader), 0);
					Login* login = (Login*)szRecv;
					std::cout << "收到命令：" << login->cmd << " ，数据长度：" << login->dataLength 
						<< " , 用户：" << login->username << " ，密码： " << login->password << endl;

					LoginResult ret ;
					send(_cSock, (char*)&ret, sizeof(ret), 0);
				}
				break;
			case CMD_LOGINOUT:
				{
					recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
					Loginout* loginout = (Loginout*)szRecv;
					std::cout << "收到命令：" << loginout->cmd << " ，数据长度：" << loginout->dataLength
						<< " , 用户：" << loginout->username << endl;

					LoginoutResult ret ;
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

	//8)关闭socket
	closesocket(_sock);
	//-------------------------
	//清除Windows socket环境
	WSACleanup();
	std::cout << "已退出，任务结束*" << endl;
	getchar();

	return 0;
}
