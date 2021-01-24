#ifndef _MYSOCKET_H_
#define _MYSOCKET_H_
#include <string>
#include <WinSock2.h>
#include <iostream>
class MySocket {
public:
	enum TypeSocket
	{
		BlockingSocket,
		UnBlockingSocket
	};
	MySocket();
	MySocket(SOCKET socket_conn);
	virtual ~MySocket();
	std::string ReceiveLine();
	bool SendLine(std::string buffer);
	void Close();
	bool operator==(unsigned long code) {
		return socket_ == code;
	}
	SOCKET socket_;
	void SendFileBuf(std::string path);
private:
	static bool init_winsocket;
};

class SocketClient :public MySocket {
public:
	SocketClient(const std::string& host, int port);
};

class SocketServer :public MySocket {
public:
	SocketServer(int port //端口
		, int connections, //接受的连接数
		TypeSocket type = BlockingSocket);
	MySocket* Accept();
};



#endif //!_MYSOCKET_H_