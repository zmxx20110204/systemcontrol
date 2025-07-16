#include <iostream>
#include <thread>
#include <mutex>
#include <unordered_set>
#include <atomic>
#include <memory>
#include <windows.h> 

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

class ChatServer {
public:
    ChatServer(int port) : port_(port), running_(false) {
#ifdef _WIN32
        WSADATA wsa;
        WSAStartup(MAKEWORD(2,2), &wsa);
#endif
    }

    ~ChatServer() { stop(); }

    void start() {
        running_ = true;
        listener_socket_ = socket(AF_INET, SOCK_STREAM, 0);
        
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port_);
        
        bind(listener_socket_, (sockaddr*)&addr, sizeof(addr));
        listen(listener_socket_, 5);
        
        listener_thread_ = std::thread(&ChatServer::listenLoop, this);
        std::cout << "Server started on port " << port_ << std::endl;
    }

    void stop() {
        running_ = false;
#ifdef _WIN32
        closesocket(listener_socket_);
#else
        close(listener_socket_);
#endif
        if(listener_thread_.joinable()) listener_thread_.join();
        
        std::lock_guard<std::mutex> lock(clients_mutex_);
        for(auto client : clients_) {
#ifdef _WIN32
            closesocket(client);
#else
            close(client);
#endif
        }
        clients_.clear();
    }

private:
    void listenLoop() {
        while(running_) {
            sockaddr_in client_addr{};
#ifdef _WIN32
            int len = sizeof(client_addr);
#else
            socklen_t len = sizeof(client_addr);
#endif
            SOCKET client = accept(listener_socket_, (sockaddr*)&client_addr, &len);
            
            if(client != INVALID_SOCKET) {
                std::lock_guard<std::mutex> lock(clients_mutex_);
                clients_.insert(client);
                std::thread(&ChatServer::handleClient, this, client).detach();
                std::cout << "New client connected. Total: " << clients_.size() << std::endl;
            }
        }
    }

    void handleClient(SOCKET client) {
        char buffer[1024];
        while(true) {
            int received = recv(client, buffer, sizeof(buffer), 0);
            if(received <= 0) break;
            
            buffer[received] = '\0';
            std::cout << "Received: " << buffer << std::endl;
            
            // 广播消息给所有客户端（包括发送者）
            std::lock_guard<std::mutex> lock(clients_mutex_);
            for(auto other_client : clients_) {
                send(other_client, buffer, received, 0);
            }
        }
        
        // 客户端断开处理
        std::lock_guard<std::mutex> lock(clients_mutex_);
        clients_.erase(client);
#ifdef _WIN32
        closesocket(client);
#else
        close(client);
#endif
        std::cout << "Client disconnected. Remaining: " << clients_.size() << std::endl;
    }

    int port_;
    SOCKET listener_socket_;
    std::atomic<bool> running_;
    std::thread listener_thread_;
    std::unordered_set<SOCKET> clients_;
    std::mutex clients_mutex_;
};

int main() {
    ChatServer server(17967);
    server.start();
    
    std::cout << "Press Enter to stop server...\n";
    std::cin.get();
    
    server.stop();
    return 0;
}
