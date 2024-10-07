#include <iostream>
#include <string>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h> // For getaddrinfo, inet_ntop, etc.

#pragma comment(lib, "Ws2_32.lib") // 链接 Windows 套接字库

using namespace std;

vector<string> getIPs(const string& domain) {
	vector<string> ips;
	WSADATA wsaData;
	struct addrinfo* res = nullptr;
	struct addrinfo hints;
	char ipstr[INET6_ADDRSTRLEN];  // 支持IPv4和IPv6

	// 初始化 Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		cerr << "WSAStartup failed." << endl;
		return ips;
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;    // 支持 IPv4 和 IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

	int status = getaddrinfo(domain.c_str(), nullptr, &hints, &res);
	if (status != 0) {
		cerr << "getaddrinfo: " << gai_strerror(status) << endl;
		WSACleanup();
		return ips;
	}

	for (struct addrinfo* p = res; p != nullptr; p = p->ai_next) {
		void* addr = nullptr;
		string ipver;

		// 获取地址，根据地址族区分IPv4和IPv6
		if (p->ai_family == AF_INET) { // IPv4
			struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		}
		else { // IPv6
			struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}

		// 转换IP地址为字符串格式
		inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
		ips.push_back(string(ipstr) + " (" + ipver + ")");
	}

	freeaddrinfo(res); // 释放结果链表
	WSACleanup();      // 清理 Winsock
	return ips;
}

int main() {
	string domain = "example.com";
	vector<string> ips = getIPs(domain);

	cout << "IP addresses for domain " << domain << ":\n";
	for (const auto& ip : ips) {
		cout << ip << endl;
	}

	return 0;
}
