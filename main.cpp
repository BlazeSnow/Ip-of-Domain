// Copyright (C) 2024 BlazeSnow
// 保留所有权利
// 本程序以GNU General Public License v3.0的条款发布
#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

bool testIPConnectivity(const std::string& ip) {
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		std::cerr << "ERROR：Socket creation failed" << std::endl;
		return false;
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(80);  // 测试 80 端口的连通性，您可以根据需要修改

	if (inet_pton(AF_INET, ip.c_str(), &(addr.sin_addr)) <= 0) {
		std::cerr << "ERROR：Invalid IP address" << std::endl;
		closesocket(sock);
		return false;
	}

	DWORD startTime = GetTickCount();  // 记录开始时间
	int result = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
	DWORD endTime = GetTickCount();    // 记录结束时间

	if (result == SOCKET_ERROR) {
		std::cerr << "花费时间：" << (endTime - startTime) << "ms    ";
		closesocket(sock);
		return false;
	}

	closesocket(sock);
	return true;
}

void getIPAddressesAndTestConnectivity(const std::string& domain) {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed" << std::endl;
		return;
	}

	struct hostent* host = gethostbyname(domain.c_str());
	if (host == NULL) {
		std::cerr << "gethostbyname failed" << std::endl;
		WSACleanup();
		return;
	}

	for (int i = 0; host->h_addr_list[i] != NULL; ++i) {
		struct in_addr addr;
		memcpy(&addr, host->h_addr_list[i], sizeof(struct in_addr));
		std::string ip = inet_ntoa(addr);
		std::cout << "IP: " << ip << "      \t连通性: ";
		if (testIPConnectivity(ip)) {
			std::cout << "成功" << std::endl;
		}
		else {
			std::cout << "失败" << std::endl;
		}
	}

	WSACleanup();
}

int main() {
	system("chcp 54936");
	system("cls");
	cout << "Copyright (C) 2024 BlazeSnow.保留所有权利。" << endl;
	cout << "本程序以GNU General Public License v3.0的条款发布。" << endl;
	cout << "当前程序版本号：v1.0.0" << endl;
	cout << "https://github.com/BlazeSnow/Ip_of_Domain" << endl
		<< endl;

	vector<string> domains;

	fstream file("Ip_of_Domain.txt", ios::in);
	if (file.is_open()) {
		while (!file.eof()) {
			string read;
			file >> read;
			domains.push_back(read);
		}
		file.close();
	}
	else {
		file.close();
		fstream file("Ip_of_Domain.txt", ios::out);
		if (file.is_open()) {
			vector<string> domains_output = { "api.onedrive.com",
				"chi01pap001.storage.live.com",
				"d.docs.live.net"
			};
			for (const auto i : domains_output) {
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
		std::cout << domain << "：" << std::endl;
		getIPAddressesAndTestConnectivity(domain);
		std::cout << std::endl;
	}

	system("pause");

	return 0;
}