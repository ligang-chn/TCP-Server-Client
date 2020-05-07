#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Windows.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <iostream>
#include <string.h>
#include <thread>

using namespace std;

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
	char bfno;
	char ledNo1;
	char data1[400];
	char ledNo2;
	char data2[400];
};

int main() {
	//启动Windows socket 2.x环境
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//-------------------------
	//1)建立一个socket套接字
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	std::cout << "*********Client********" << endl;
	if (INVALID_SOCKET == _sock) {
		std::cout << "ERROR,建立socket失败###########" << endl;
	}
	else
	{
		std::cout << "建立socket成功*****************" << endl;
	}
	//2)connect服务器
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);//host to net unsigned short
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//inet_addr("127.0.0.1");
	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (SOCKET_ERROR == ret) {
		std::cout << "ERROR,连接server失败###########" << endl;
	}
	else
	{
		std::cout << "连接server成功*****************" << endl;
	}

	while (true)
	{
		DataPackage dataPackage;
		strcpy_s(dataPackage.msgCode, "GALED01");
		strcpy_s(dataPackage.sendTime, "2020041609401");
		dataPackage.bfno='1';
		dataPackage.ledNo1= '1';
		strcpy_s(dataPackage.data1, "BV,222.33,BP,11.2,线渣面，1.34");
		dataPackage.ledNo2='2';
		strcpy_s(dataPackage.data2, "BV,444.33,BP,18.2,线渣面，9.34");
		send(_sock, (const char*)&dataPackage, sizeof(dataPackage), 0);
		Sleep(3 * 1000);

#if 0
		//3)输入请求命令
		char cmdBuf[128] = {};
		std::cin >> cmdBuf;
		//4）处理请求命令
		if (0 == strcmp(cmdBuf, "exit")) {
			std::cout << "收到exit命令，任务结束" << endl;
			break;
		}
		else if (0 == strcmp(cmdBuf, "start")) {
			DataPackage dataPackage;
			strcpy_s(dataPackage.msgCode, "GALED1");
			strcpy_s(dataPackage.sendTime, "2020/4/11 09");
			strcpy_s(dataPackage.bfno, "1");
			strcpy_s(dataPackage.ledNo1, "1");
			strcpy_s(dataPackage.data1, "BV,222.33,BP,11.2");
			strcpy_s(dataPackage.ledNo2, "2");
			strcpy_s(dataPackage.data2, "BV,444.33,BP,18.2");
			send(_sock, (const char*)&dataPackage, sizeof(dataPackage), 0);
		}
#endif
	}


	//7)关闭套接字
	closesocket(_sock);
	WSACleanup();
	std::cout << "已退出，任务结束*" << endl;
	getchar();

	return 0;
}