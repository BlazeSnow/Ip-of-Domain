#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

const int CONNECT_TIMEOUT = 5;

vector<string> getIPs(const string &domain) {
    vector<string> ips;
    WSADATA wsaData;
    struct addrinfo *res = nullptr;
    struct addrinfo hints;
    char ipstr[INET6_ADDRSTRLEN];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup 失败。\n");
        return ips;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(domain.c_str(), nullptr, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo 错误: %s\n", gai_strerror(status));
        WSACleanup();
        return ips;
    }

    for (struct addrinfo *p = res; p != nullptr; p = p->ai_next) {
        void *addr = nullptr;

        if (p->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
        } else {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
        }

        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);

        SOCKET sock = socket(p->ai_family, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) {
            fprintf(stderr, "Socket 创建失败: %d\n", WSAGetLastError());
            continue;
        }

        DWORD timeout = CONNECT_TIMEOUT * 1000;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));

        if (p->ai_family == AF_INET) {
            ((struct sockaddr_in *)p->ai_addr)->sin_port = htons(80);
        } else {
            ((struct sockaddr_in6 *)p->ai_addr)->sin6_port = htons(80);
        }

        bool connected = (connect(sock, p->ai_addr, p->ai_addrlen) == 0);
        closesocket(sock);

        string result = string(ipstr) + "      \t" + (connected ? "连接成功" : "错误：无法连接");
        ips.push_back(result);
    }

    freeaddrinfo(res);
    WSACleanup();
    return ips;
}

void getIPAddressesAndTestConnectivity(const string &domain) {
    vector<string> ips = getIPs(domain);

    for (const auto &ip : ips) {
        printf("%s\n", ip.c_str());
    }
}

int main() {
    system("chcp 54936");
    system("cls");
    printf("Copyright (C) 2024-2026 BlazeSnow.保留所有权利。\n");
    printf("本程序以GNU General Public License v3.0的条款发布。\n");
    printf("当前程序版本号：v1.0.1\n");
    printf("https://github.com/BlazeSnow/CppWorkspace\n\n");

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
    } else {
        fstream file("Ip_of_Domain.txt", ios::out);
        if (file.is_open()) {
            vector<string> domains_output = {"api.onedrive.com", "chi01pap001.storage.live.com", "d.docs.live.net"};
            for (const auto &i : domains_output) {
                file << i << endl;
            }
            printf("创建文件\"Ip_of_Domain.txt\"成功\n");
            printf("目录为：%s\n", filesystem::current_path().string().c_str());
            file.close();
            system("pause");
            return 0;
        }
    }

    for (const auto &domain : domains) {
        printf("%s：\n", domain.c_str());
        getIPAddressesAndTestConnectivity(domain);
        printf("\n");
    }

    system("pause");
    return 0;
}
