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
	//������
	char szRecv[1024] = {};
	//5)���տͻ�������
	int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0) {
		std::cout << "�ͻ������˳����������" << endl;
		return -1;
	}

	switch (header->cmd)
	{
		case CMD_LOGIN:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			Login* login = (Login*)szRecv;
			std::cout << "�յ����" << login->cmd << " �����ݳ��ȣ�" << login->dataLength
				<< " , �û���" << login->username << " �����룺 " << login->password << endl;

			LoginResult ret;
			send(_cSock, (char*)&ret, sizeof(ret), 0);
		}
		break;
		case CMD_LOGINOUT:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			Loginout* loginout = (Loginout*)szRecv;
			std::cout << "�յ����" << loginout->cmd << " �����ݳ��ȣ�" << loginout->dataLength
				<< " , �û���" << loginout->username << endl;

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
	

	
	while (true)
	{
		//selectģ�ͣ�������socket
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
		//nfds��һ������ֵ����ָfd_set������������������socket���ķ�Χ������������
		//���������ļ����������ֵ+1����windows�������������д0
		int ret=select(_sock+1,&fdRead,&fdWrite,&fdExp,nullptr);
		if (ret < 0) {
			std::cout << "select�������" << endl;
			break;
		}
		if (FD_ISSET(_sock, &fdRead)) {
			FD_CLR(_sock, &fdRead);
			//4��accept�ȴ����ܿͻ��˵�����
			sockaddr_in clientAddr = {};
			int nAddrLen = sizeof(sockaddr_in);
			SOCKET _cSock = INVALID_SOCKET;
			_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
			if (INVALID_SOCKET == _cSock) {
				std::cout << "ERROR,���ܵ���Ч�ͻ���SOCKET###########" << endl;
			}
			else {
				g_clients.push_back(_cSock);
				std::cout << "�¿ͻ��˼��룺IP = " << inet_ntoa(clientAddr.sin_addr) << " , socket = " << _cSock << endl;
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
	//8)�ر�socket
	closesocket(_sock);
	//-------------------------
	//���Windows socket����
	WSACleanup();
	std::cout << "���˳����������*" << endl;
	getchar();

	return 0;
}
