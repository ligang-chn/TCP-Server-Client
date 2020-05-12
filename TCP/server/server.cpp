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
	char msgCode[8];//电文号
	char sendTime[14];//发送时间
	char bfno;//高炉号
	char ledNo1;//大屏号1
	char data1[400];
	char ledNo2;//大屏号2
	char data2[400];
	char ledNo3;//大屏号3
	char data3[400];
};

struct DataStr
{
	string msgCode;//电文号
	string sendTime;//发送时间
	char bfno;//高炉号
	char ledNo1;//大屏号1
	string data1;
	char ledNo2;//大屏号2
	string data2;
	char ledNo3;//大屏号3
	string data3;
	
};

vector<string> split3(const string &str, const char pattern)
{
	vector<string> res;
	stringstream ss(str);   //读取str到字符串流中
	string temp;
	//使用getline函数从字符串流中读取,遇到分隔符时停止,和从cin中读取类似
	//注意,getline默认是可以读取空格的
	while (getline(ss, temp, pattern))
	{
		res.push_back(temp);
	}
	return res;
}

std::vector<SOCKET> g_clients;

int processor(SOCKET _cSock) {
	//缓冲区
	char szRecv[1500] = {};
	//5)接收客户端数据,首先接收数据长度
	//int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	//DataHeader* header = (DataHeader*)szRecv;
	int nLen = recv(_cSock, szRecv, sizeof(DataPackage), 0);
	if (nLen <= 0) {
		std::cout << "客户端已退出，任务结束" << endl;
		return -1;
	}
	//char数组转换成string，方便处理
	string recvStr = szRecv;
	DataStr recvData;
	recvData.msgCode = recvStr.substr(0, 8);
	recvData.msgCode.erase(recvData.msgCode.find_last_not_of(" ") + 1);
	recvData.sendTime = recvStr.substr(8, 14);
	recvData.bfno = recvStr[22];
	recvData.ledNo1 = recvStr[23];
	recvData.data1 = recvStr.substr(24, 400);
	//去除尾部空符串
	recvData.data1.erase(recvData.data1.find_last_not_of(" ") + 1);
	recvData.ledNo2 = recvStr[424];
	recvData.data2 = recvStr.substr(425, 400);
	recvData.data2.erase(recvData.data2.find_last_not_of(" ") + 1);
	recvData.ledNo3 = recvStr[825];
	recvData.data3 = recvStr.substr(826, 400);
	recvData.data3.erase(recvData.data3.find_last_not_of(" ") + 1);

	vector<string> resStr;//接收数据区分割之后的字符串
	vector<pair<string, string> > bufferData1;
	resStr=split3(recvData.data1, '|');//分割字符串
	for (vector<string>::iterator it = resStr.begin(); it != resStr.end();it+=2) {
		bufferData1.push_back(make_pair(*it, *(it + 1)));//存储到缓存区，方便后续使用
	}
	resStr.clear();
	vector<pair<string, string> > bufferData2;
	resStr = split3(recvData.data2, '|');//分割字符串
	for (vector<string>::iterator it = resStr.begin(); it != resStr.end(); it += 2) {
		bufferData2.push_back(make_pair(*it, *(it + 1)));//存储到缓存区，方便后续使用
	}
	resStr.clear();
	vector<pair<string, string> > bufferData3;
	resStr = split3(recvData.data3, '|');//分割字符串
	for (vector<string>::iterator it = resStr.begin(); it != resStr.end(); it += 2) {
		bufferData3.push_back(make_pair(*it, *(it + 1)));//存储到缓存区，方便后续使用
	}


	//recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
	//DataPackage *dataPackage= (DataPackage*)szRecv;
	//std::cout << "收到数据长度：" << dataPackage->dataLength << endl;
	//std::cout << "socket = " << _cSock<<",数据如下：---------" << endl;
	//std::cout << "--电文号：" << dataPackage->msgCode << endl;
	//std::cout << "--发送时间：" << dataPackage->sendTime << endl;
	//std::cout << "--高炉号：" << dataPackage->bfno << endl;
	//std::cout << "--大屏1：" << dataPackage->ledNo1 << endl;
	//std::cout << "--数据1：" << dataPackage->data1 << endl;
	//std::cout << "--大屏2：" << dataPackage->ledNo2 << endl;
	//std::cout << "--数据2：" << dataPackage->data2 << endl;
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
