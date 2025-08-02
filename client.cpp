//
// Created by Preet Rana on 23/07/25.
//

#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <thread>

#define PORT 8080
#define BUFFER_SIZE 1024

void receiveMessage(int sock) {
    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = read(sock, buffer, BUFFER_SIZE);
        if (bytes_read <= 0) {
            std::cout << "Server Disconnected\n";
            break;
        }
        std::cout << "\n" << buffer <<"\n";
        std::cout.flush();
    }
}

void sendMessage(int sock, std::string& username) {

    while (true) {
        std::string input;
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "exit")
            break;

        std::string msg = input;
        send(sock, msg.c_str(), msg.length(), 0);
    }
    send(sock, "exit", 4, 0);
}

int main() {

    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    std::string username;
    std::cout << "Enter your username: ";
    std::cin >> username;

    // creating socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation failed.\n";
        return -1;
    }

    // define server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported.\n";
        return -1;
    }

    // connet to server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection with the server failed.\n";
        return -1;
    }

    send(sock, username.c_str(), username.length(), 0);

    std::cout << "Connected to server.\n";

    // send message
    std::thread recive_thread(receiveMessage, sock);

    // Read response
    std::thread send_thread(sendMessage, sock, std::ref(username));

    send_thread.join();
    recive_thread.detach();

    close(sock);

    return 0;
}