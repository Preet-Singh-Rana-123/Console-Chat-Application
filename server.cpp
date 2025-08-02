//
// Created by Preet Rana on 23/07/25.
//

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <map>
#include <thread>
#include <vector>
#include <mutex>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <ctime>

#define PORT 8080
#define BUFFER_SIZE 1024

std::vector<int> clientSockets;
std::mutex clientsMutex;
std::map<int, std::string> clientUsername;

std::string getCurrentTime() {
    std::time_t now = std::time(nullptr);
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", std::localtime(&now));
    return std::string(buffer);
}

void broadcastMessage(const std::string& msg, int sock) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (int clientSocket : clientSockets) {
        if (clientSocket != sock) {
            send(clientSocket, msg.c_str(), msg.length(), 0);
        }
    }
}

void handleClient(int sock) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    int bytesRead = read(sock, buffer, BUFFER_SIZE);
    if (bytesRead <= 0) {
        close(sock);
        return;
    }
    std::string username(buffer, bytesRead);

    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        clientUsername[sock] = username;
    }

    std::cout << username << " joined the chat\n";
    broadcastMessage(username + " has joined the chat.", sock);

    while (true) {
        char newBuffer[BUFFER_SIZE];
        memset(newBuffer, 0, BUFFER_SIZE);
        int bytesRead = read(sock, newBuffer, BUFFER_SIZE);
        if (bytesRead <= 0) {
            break;
        }

        std::string msg(newBuffer);
        if (msg == "exit")
            break;

        std::string formattedMsg = "[" + getCurrentTime() + "] " + username + ": " + msg;
        broadcastMessage(formattedMsg, sock);
    }

    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        std::string leftUser = clientUsername[sock];
        clientSockets.erase(std::remove(clientSockets.begin(), clientSockets.end(), sock), clientSockets.end());
        clientUsername.erase(sock);
        broadcastMessage(leftUser + " has left the chat.", sock);
    }

    close(sock);
}

void acceptClient(int server_fd) {
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    while (true) {
        int newSocket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (newSocket < 0) {
            perror("accept failed");
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clientSockets.push_back(newSocket);
        }

        std::thread(handleClient, newSocket).detach();
    }
}

int main() {

    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Creating socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Define Address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // bind socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listent to connection
    if (listen(server_fd, 5) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << "\n";

    std::thread(acceptClient, server_fd).detach();

    std::string command;
    while (true) {
        std::getline(std::cin, command);
        if (command == "/list") {
            std::lock_guard<std::mutex> lock(clientsMutex);
            std::cout << "Connected Clients:- \n";
            for (auto& [sock, name] : clientUsername) {
                std::cout << "- " << name << "\n";
            }
        }else if (command.rfind("/kick ", 0) == 0) {
            std::string target = command.substr(6);
            int targetSocket = -1;

            {
                std::lock_guard<std::mutex> lock(clientsMutex);
                for (const auto& [sock, name] : clientUsername) {
                    if (name == target) {
                        targetSocket = sock;
                        break;
                    }
                }
            }
            if (targetSocket != -1) {
                send(targetSocket, "You were kicked by the server.", 30, 0);
                close(targetSocket);
            }else {
                std::cout << "User not found.\n";
            }
        }
    }

    close(server_fd);

    return 0;
}
