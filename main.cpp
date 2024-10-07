#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <filesystem>

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

		// 创建一个 socket
		SOCKET sock = socket(p->ai_family, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET) {
			cerr << "Socket creation failed: " << WSAGetLastError() << endl;
			continue;
		}

		// 设置端口号为 80
		if (p->ai_family == AF_INET) {
			((struct sockaddr_in*)p->ai_addr)->sin_port = htons(80);
		}
		else {
			((struct sockaddr_in6*)p->ai_addr)->sin6_port = htons(80);
		}

		// 尝试连接
		bool connected = (connect(sock, p->ai_addr, p->ai_addrlen) == 0);

		// 关闭 socket
		closesocket(sock);

		// 将结果保存到 ips 矢量中
		string result = string(ipstr) + " (" + ipver + ") - Port 80: " + (connected ? "Connected" : "Cannot Connect");
		ips.push_back(result);
	}

	freeaddrinfo(res); // 释放结果链表
	WSACleanup();      // 清理 Winsock
	return ips;
}

void getIPAddressesAndTestConnectivity(const string& domain) {
	vector<string> ips = getIPs(domain);

	for (const auto& ip : ips) {
		cout << ip << endl;
	}
}

int main() {
	system("chcp 54936");
	system("cls");
	cout << "Copyright (C) 2024 BlazeSnow.保留所有权利。" << endl;
	cout << "本程序以GNU General Public License v3.0的条款发布。" << endl;
	cout << "当前程序版本号：v1.0.0" << endl;
	cout << "https://github.com/BlazeSnow/Ip-of-Domain" << endl
		<< endl;

	vector<string> domains;

	fstream file("Ip_of_Domain.txt", ios::in);
	if (file.is_open()) {
		while (!file.eof()) {
			string read;
			file >> read;
			if (!read.empty()) {
				domains.push_back(read);
			}
		}
		file.close();
	}
	else {
		fstream file("Ip_of_Domain.txt", ios::out);
		if (file.is_open()) {
			vector<string> domains_output = { "api.onedrive.com",
				"chi01pap001.storage.live.com",
				"d.docs.live.net"
			};
			for (const auto& i : domains_output) {
				file << i << endl;
			}
			cout << "创建文件\"Ip_of_Domain.txt\"成功" << endl;
			cout << "目录为：" << filesystem::current_path() << endl;
			file.close();
			system("pause");
			return 0;
		}
	}

	for (const auto& domain : domains) {
		cout << domain << "：" << endl;
		getIPAddressesAndTestConnectivity(domain);
		cout << endl;
	}

	system("pause");

	return 0;
}
