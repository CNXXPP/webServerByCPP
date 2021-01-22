#include "MySocket.h"
#include <iostream>
#include <process.h>
#include <stdio.h>
#include <direct.h>
#pragma comment(lib,"ws2_32.lib")
int sum = 0;
unsigned int __stdcall Connection(void* p) {
	sum++;
	printf("连接地址：%p connection num:%d\n",p,sum);
	MySocket* accept_socket = (MySocket*)p;
	
	std::string url;
	while (true) {
		//浏览器数据返回
		std::string buffer = accept_socket->ReceiveLine();
		int idx = buffer.find("GET",0);
		if (idx == 0) {
			int lastIdx = buffer.find(" HTTP", 0);
			url = buffer.substr(idx+4, lastIdx - idx-3); //获取本次请求地址
			//std::cout << "this request url:" << url << std::endl ;
			printf("连接地址：%p 请求地址 %s:\n", p, url.c_str());
		}
		if (buffer.empty()) {
			break;
		}
		//std::cout << buffer;
		//printf("%s",buffer.c_str());
	}
	int picIdx = url.find(".", 0);
	//printf("连接地址：%p 图片find %d:\n", p, picIdx);
	if (picIdx > 0) { //获取文件
		//获取图片
		std::string file_name = url.substr(1, url.size() - 1);
		std::string path = "./html/" + file_name;
		printf("连接地址：%p 图片请求地址:%s\n", p, path.c_str());
		FILE* fp = fopen(path.c_str(),"rb");
		char temp[4 * 1024]; int len;
		while (true) {
			len = fread(temp,1, 4 * 1024, fp);
			if (len == 0) {
				break;
			}
			send(accept_socket->socket_, temp, len, 0);
		}
	}
	else { //默认返回
		FILE* fp = fopen("./html/1.html", "rb");
		char temp[4 * 1024]; int len;
		while (true) {
			len = fread(temp, 1, 4 * 1024, fp);
			if (len == 0) {
				break;
			}
			send(accept_socket->socket_, temp, len, 0);
		}
		//accept_socket->SendLine("<html><body><h1>hello world</h1><p>hello world</p></body></html>");
	}
	//http一次请求后就断开
	closesocket(accept_socket->socket_);
	delete accept_socket;
	std::cout << "read finish" << std::endl;
	return 0;
}

int main() {
	char* buffer;
	buffer = _getcwd(NULL, 0);
	printf("%s", buffer);
	/*try {
		SocketClient web_read = SocketClient("www.baidu.com",80);
		web_read.SendLine("GET / HTTP/1.0");
		web_read.SendLine("Host: www.baidu.com");
		web_read.SendLine("");
		while (true)
		{
			std::string buf = web_read.ReceiveLine();
			if (buf.empty()) {
				break;
			}
			std::cout << buf;
		}
	}
	catch(const char *error_msg){
		std::cout << error_msg << std::endl;
	}*/

	try
	{
		SocketServer web_server(80,10);
		while (true) {
			MySocket *mySocket = web_server.Accept();
			CloseHandle((HANDLE)_beginthreadex(nullptr,0,Connection,mySocket,0,nullptr));
		}
		 
	}
	catch (const char *e)
	{
		std::cout << e << std::endl;
	}
	catch (...)
	{
		std::cout << "unkown error" << std::endl;
	}
	return 0;
}