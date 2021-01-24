#include "MySocket.h"
SocketClient::SocketClient(const std::string& host, int port)
{
	bool isok = false;
	do {
		//失败的时候 指针为空
		hostent* he = gethostbyname(host.c_str());
		if (!he) {
			break;
		}
		sockaddr_in addr = { 0 };
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr = *((in_addr*)he->h_addr);

		if (SOCKET_ERROR == connect(socket_, (sockaddr*)&addr, sizeof(sockaddr))) {
			break;
		}

		isok = true;
	} while (FALSE);
	if (!isok) {
		char error_msg[MAXBYTE] = { 0 };
		DWORD error_code = WSAGetLastError();
		strerror_s(error_msg, WSAGetLastError());
		throw error_msg;
	}

}
bool MySocket::init_winsocket = false;
MySocket::MySocket()
{
	
	if (!init_winsocket) {
		//重复初始化 保证单一性
		WSADATA info;
		if (WSAStartup(MAKEWORD(2, 2), &info)) {
			throw "could not start WSA";
		}
	}
	socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == socket_) {
		throw "invalid socket";
	}
}

MySocket::MySocket(SOCKET socket_conn)
{
	socket_ = socket_conn;
}

MySocket::~MySocket()
{
}

std::string MySocket::ReceiveLine()
{
	struct fd_set rfds;
	struct timeval timeout = { 0,500 };
	
	std::string ret;
	while (true) {
		FD_ZERO(&rfds);  /* 清空集合 */
		FD_SET(socket_, &rfds);  /* 将socket_添加到集合*/

		int select_ret = select(0, &rfds, NULL, NULL, &timeout); //阻塞超过0.5s即返回
		if (select_ret) {
			char buf;
			switch (recv(socket_, &buf, 1, 0)) {
			case 0:
				return ret;
			case -1:
				return "";
			}
			ret += buf;
			if (buf == '\n') {
				break;
			}
		}
		else if (select_ret == 0) {
			break;
		}
		else {
			break;
		}
	}
	return ret;
}

bool MySocket::SendLine(std::string buffer)
{
	buffer += '\n';
	return SOCKET_ERROR != send(socket_,buffer.c_str(),buffer.length(),0);
}

void MySocket::Close()
{
	closesocket(socket_);
}
 
void MySocket::SendFileBuf(std::string path)
{
	FILE* fp = fopen(path.c_str(), "rb");
	if (!fp) {
		fp = fopen("./html/404.html", "rb");
	}
	char temp[4 * 1024]; int len;
	while (true) {
		len = fread(temp, 1, 4 * 1024, fp);
		if (len == 0) {
			break;
		}
		send(socket_, temp, len, 0);
	}
}

SocketServer::SocketServer(int port, int connections, TypeSocket type)
{
	sockaddr_in addr = { 0 };
	addr.sin_family = PF_INET;
	addr.sin_port = htons(port);
	if (type == UnBlockingSocket) {
		//设置非阻塞
		unsigned long arg = 1;
		ioctlsocket(socket_, FIONBIO, &arg);
	}

	if (SOCKET_ERROR == bind(socket_,(sockaddr*)&addr,sizeof(sockaddr))) {
		//抛出异常前，还原现场
		Close();
		throw "bind error";
	}
	listen(socket_, connections);
}

MySocket* SocketServer::Accept()
{
	MySocket* accept_socket = new MySocket(accept(socket_, nullptr, nullptr)); //accept返回一个连接的socket
	if (*accept_socket == INVALID_SOCKET) {
		DWORD error_code = WSAGetLastError();
		throw "invalid accept socket";
	}
	return accept_socket;
}
