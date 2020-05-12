#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Windows.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <iostream>
#include<vector>
#include<string>
#include<algorithm>
#include<sstream>
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
struct DataPackage
{
	char msgCode[8];//���ĺ�
	char sendTime[14];//����ʱ��
	char bfno;//��¯��
	char ledNo1;//������1
	char data1[400];
	char ledNo2;//������2
	char data2[400];
	char ledNo3;//������3
	char data3[400];
};

struct DataStr
{
	string msgCode;//���ĺ�
	string sendTime;//����ʱ��
	char bfno;//��¯��
	char ledNo1;//������1
	string data1;
	char ledNo2;//������2
	string data2;
	char ledNo3;//������3
	string data3;
	
};

vector<string> split3(const string &str, const char pattern)
{
	vector<string> res;
	stringstream ss(str);   //��ȡstr���ַ�������
	string temp;
	//ʹ��getline�������ַ������ж�ȡ,�����ָ���ʱֹͣ,�ʹ�cin�ж�ȡ����
	//ע��,getlineĬ���ǿ��Զ�ȡ�ո��
	while (getline(ss, temp, pattern))
	{
		res.push_back(temp);
	}
	return res;
}

std::vector<SOCKET> g_clients;

int processor(SOCKET _cSock) {
	//������
	char szRecv[1500] = {};
	//5)���տͻ�������,���Ƚ������ݳ���
	//int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	//DataHeader* header = (DataHeader*)szRecv;
	int nLen = recv(_cSock, szRecv, sizeof(DataPackage), 0);
	if (nLen <= 0) {
		std::cout << "�ͻ������˳����������" << endl;
		return -1;
	}
	//char����ת����string�����㴦��
	string recvStr = szRecv;
	DataStr recvData;
	recvData.msgCode = recvStr.substr(0, 8);
	recvData.msgCode.erase(recvData.msgCode.find_last_not_of(" ") + 1);
	recvData.sendTime = recvStr.substr(8, 14);
	recvData.bfno = recvStr[22];
	recvData.ledNo1 = recvStr[23];
	recvData.data1 = recvStr.substr(24, 400);
	//ȥ��β���շ���
	recvData.data1.erase(recvData.data1.find_last_not_of(" ") + 1);
	recvData.ledNo2 = recvStr[424];
	recvData.data2 = recvStr.substr(425, 400);
	recvData.data2.erase(recvData.data2.find_last_not_of(" ") + 1);
	recvData.ledNo3 = recvStr[825];
	recvData.data3 = recvStr.substr(826, 400);
	recvData.data3.erase(recvData.data3.find_last_not_of(" ") + 1);

	vector<string> resStr;//�����������ָ�֮����ַ���
	vector<pair<string, string> > bufferData1;
	resStr=split3(recvData.data1, '|');//�ָ��ַ���
	for (vector<string>::iterator it = resStr.begin(); it != resStr.end();it+=2) {
		bufferData1.push_back(make_pair(*it, *(it + 1)));//�洢�����������������ʹ��
	}
	resStr.clear();
	vector<pair<string, string> > bufferData2;
	resStr = split3(recvData.data2, '|');//�ָ��ַ���
	for (vector<string>::iterator it = resStr.begin(); it != resStr.end(); it += 2) {
		bufferData2.push_back(make_pair(*it, *(it + 1)));//�洢�����������������ʹ��
	}
	resStr.clear();
	vector<pair<string, string> > bufferData3;
	resStr = split3(recvData.data3, '|');//�ָ��ַ���
	for (vector<string>::iterator it = resStr.begin(); it != resStr.end(); it += 2) {
		bufferData3.push_back(make_pair(*it, *(it + 1)));//�洢�����������������ʹ��
	}


	//recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
	//DataPackage *dataPackage= (DataPackage*)szRecv;
	//std::cout << "�յ����ݳ��ȣ�" << dataPackage->dataLength << endl;
	//std::cout << "socket = " << _cSock<<",�������£�---------" << endl;
	//std::cout << "--���ĺţ�" << dataPackage->msgCode << endl;
	//std::cout << "--����ʱ�䣺" << dataPackage->sendTime << endl;
	//std::cout << "--��¯�ţ�" << dataPackage->bfno << endl;
	//std::cout << "--����1��" << dataPackage->ledNo1 << endl;
	//std::cout << "--����1��" << dataPackage->data1 << endl;
	//std::cout << "--����2��" << dataPackage->ledNo2 << endl;
	//std::cout << "--����2��" << dataPackage->data2 << endl;
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
