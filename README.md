# 📟 Console Chat Application

A simple terminal-based chat application using C++ and sockets that allows multiple clients to connect to a server and exchange messages in real-time.

---

## 📁 Project Structure

```
.
├── CMakeLists.txt
├── server.cpp
└── client.cpp
```

---

## ⚙️ Features

* Multi-user chat support
* Real-time message broadcasting
* Simple client-server architecture using TCP sockets
* Console-based interaction

---

## 🛠️ Requirements

* C++17 or higher
* CMake 3.10 or higher
* POSIX-compliant system (e.g., Linux, macOS)

  > 💡 Note: For Windows, socket APIs like `winsock2.h` need additional setup.

---

## 🚀 Build Instructions

### 1. Clone the repository

```bash
git clone <your-repo-url>
cd <your-project-folder>
```

### 2. Build with CMake

```bash
mkdir build
cd build
cmake ..
make
```

This will generate two executables: `server` and `client`.

---

## 🖥️ How to Run

### Start the Server

```bash
./cmake-build-debug/Chat_Server
```

The server will start and listen on a predefined port (e.g., 8080 or configurable).

### Run One or More Clients

In new terminal windows:

```bash
./cmake-build-debug/Chat_Client
```

Each client can send messages that are broadcast to all connected users.

---

## 🧠 How It Works

* The server listens for incoming TCP connections.
* Each connected client runs in its own thread or process (based on your implementation).
* When a client sends a message, the server receives it and forwards it to all other clients.
* Clients listen for messages from the server and display them in the console.

---
