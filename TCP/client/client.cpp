#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Windows.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <iostream>
#include <string.h>

using namespace std;

//网络报文格式定义
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
struct Login : public DataHeader
{
	Login() {
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char username[32];
	char password[32];
};
struct LoginResult : public DataHeader
{
	LoginResult() {
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};

struct Loginout : public DataHeader
{
	Loginout() {
		dataLength = sizeof(Loginout);
		cmd = CMD_LOGINOUT;
	}
	char username[32];
};

struct LoginoutResult : public DataHeader
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
		//3)输入请求命令
		char cmdBuf[128] = {};
		std::cin >> cmdBuf;
		//4）处理请求命令
		if (0 == strcmp(cmdBuf, "exit")) {
			std::cout << "收到exit命令，任务结束" << endl;
			break;
		}
		else if (0 == strcmp(cmdBuf, "login")) {
			//5）向服务器发送请求命令
			Login login ;
			strcpy_s(login.username, "ligang");
			strcpy_s(login.password, "123456");
			send(_sock, (const char*)&login, sizeof(login), 0);

			//接收服务器返回的数据
			LoginResult loginRet = {};
			recv(_sock, (char*)&loginRet, sizeof(loginRet), 0);
			std::cout << "LoginResult: " << loginRet.result << endl;
		}
		else if (0 == strcmp(cmdBuf, "loginout")) {
			Loginout logout;
			strcpy_s(logout.username, "ligang");
			
			send(_sock, (const char*)&logout, sizeof(logout), 0);
			//接收服务器返回的数据			
			LoginoutResult logoutRet = {};
			recv(_sock, (char*)&logoutRet, sizeof(logoutRet), 0);
			std::cout << "LoginResult: " << logoutRet.result << endl;

		}
		else {
			std::cout << "不支持的命令，请重新输入！" << endl;
		}
	}


	//7)关闭套接字
	closesocket(_sock);
	WSACleanup();
	std::cout << "已退出，任务结束*" << endl;
	getchar();

	return 0;
}