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
	//������
	char szRecv[1024] = {};
	//5)���տͻ�������
	int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0) {
		std::cout << "�ͻ������˳����������" << endl;
		return -1;
	}
	

	recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
	DataPackage *dataPackage= (DataPackage*)szRecv;
	std::cout << "�յ����ݳ��ȣ�" << dataPackage->dataLength << endl;
	std::cout << "socket = " << _cSock<<",�������£�---------" << endl;
	std::cout << "--���ĺţ�" << dataPackage->msgCode << endl;
	std::cout << "--����ʱ�䣺" << dataPackage->sendTime << endl;
	std::cout << "--��¯�ţ�" << dataPackage->bfno << endl;
	std::cout << "--����1��" << dataPackage->ledNo1 << endl;
	std::cout << "--����1��" << dataPackage->data1 << endl;
	std::cout << "--����2��" << dataPackage->ledNo2 << endl;
	std::cout << "--����2��" << dataPackage->data2 << endl;
	std::cout << "---------���ν������" << endl;

	return 0;

}
#if 1
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
		timeval t = { 0,0 };
		int ret=select(_sock+1,&fdRead,&fdWrite,&fdExp,&t);
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
#endif
