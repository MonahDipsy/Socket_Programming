#include "server.h"
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <cstdlib>

void start_communication(const server_setup_information& setup_info, live_server_info& server) {
    // Create a socket
    server.server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server.server_fd == -1) {
        std::cerr << "system error: Socket creation failed.\n";
        exit(1);
    }

    // Set up server address
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(setup_info.port);

    // Bind the socket to the specified IP and port
    if (bind(server.server_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "system error: Socket binding failed.\n";
        close(server.server_fd);
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server.server_fd, 1) < 0) {
        std::cerr << "system error: Listening on socket failed.\n";
        close(server.server_fd);
        exit(1);
    }

    // Check if the info file name and directory are valid
    if (setup_info.info_file_name.empty() || setup_info.info_file_directory.empty()) {
        std::cerr << "system error: info_file_name or info_file_directory is empty.\n";
        close(server.server_fd);
        exit(1);
    }

    // Create a shared memory segment
    key_t shm_key = ftok(setup_info.shm_pathname.c_str(), setup_info.shm_proj_id);
    server.shmid = shmget(shm_key, SHARED_MEMORY_SIZE, IPC_CREAT | 0666);
    if (server.shmid == -1) {
        std::cerr << "system error: Shared memory segment creation failed.\n";
        close(server.server_fd);
        exit(1);
    }

    // Write the server's IP address and port number to the shared memory segment
    char* shm_ptr = (char*)shmat(server.shmid, nullptr, 0);
    if (shm_ptr == (char*)-1) {
        std::cerr << "system error: Attaching to shared memory segment failed.\n";
        close(server.server_fd);
        shmctl(server.shmid, IPC_RMID, nullptr);
        exit(1);
    }

    std::stringstream shm_message;
    shm_message << server_address.sin_addr.s_addr << " " << ntohs(server_address.sin_port);
    std::strcpy(shm_ptr, shm_message.str().c_str());

    // Detach the shared memory segment from the address space of the calling process
    shmdt(shm_ptr);

    // Accept a connection from a client
    server.client_fd = accept(server.server_fd, nullptr, nullptr);
    if (server.client_fd == -1) {
        std::cerr << "system error: Accepting client connection failed.\n";
        close(server.server_fd);
        shmctl(server.shmid, IPC_RMID, nullptr);
        exit(1);
    }

    // Send the message from the shared memory segment to the client
    send(server.client_fd, shm_message.str().c_str(), shm_message.str().size(), 0);
}

void create_info_file(const server_setup_information& setup_info, live_server_info& server) {
    // Check if the info file name and directory are valid
    if (setup_info.info_file_name.empty() || setup_info.info_file_directory.empty()) {
        std::cerr << "system error: info_file_name or info_file_directory is empty.\n";
        close(server.server_fd);
        shmctl(server.shmid, IPC_RMID, nullptr);
        exit(1);
    }

    // Create and open the info file for writing
    std::ofstream info_file(setup_info.info_file_directory + "/" + setup_info.info_file_name);
    if (!info_file) {
        std::cerr << "system error: Unable to create info file.\n";
        close(server.server_fd);
        shmctl(server.shmid, IPC_RMID, nullptr);
        exit(1);
    }

    // Write the server's setup information to the info file
    info_file << setup_info.ip_address << "\n"
              << setup_info.port << "\n"
              << setup_info.shm_pathname << "\n"
              << setup_info.shm_proj_id;

    // Store the info file path in the live_server_info struct for later reference
    server.info_file_path = setup_info.info_file_directory + "/" + setup_info.info_file_name;

    // Close the info file
    info_file.close();
}

int main() {
    server_setup_information setup_info;
    // Set up the setup_info with the appropriate values for your server
    setup_info.port = 7070; // Replace with your desired port number
    setup_info.shm_pathname = "/dev/shm/server_shm1"; // Replace with your desired shared memory pathname
    setup_info.shm_proj_id = 1234; // Replace 1234 with your desired shared memory project ID
    setup_info.info_file_name = "server_info.txt"; // Replace with your desired server info file name
    setup_info.info_file_directory = "/home/Nicah/server_info_files"; // Replace with the correct path to the directory containing the server info files

    std::string shm_msg = "Hello from server's shared memory!";
    std::string socket_msg = "Hello from server's socket!";

    live_server_info server;
    start_communication(setup_info, server);
    create_info_file(setup_info, server);

    char* shm_ptr = (char*)shmat(server.shmid, nullptr, 0);
    if (shm_ptr == (char*)-1) {
        std::cerr << "system error: Attaching to shared memory segment failed.\n";
        close(server.server_fd);
        close(server.client_fd);
        shmctl(server.shmid, IPC_RMID, nullptr);
        unlink(server.info_file_path.c_str());
        exit(1);
    }

    std::strcpy(shm_ptr, shm_msg.c_str());
    shmdt(shm_ptr);

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(server.server_fd, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 120;
    timeout.tv_usec = 0;

    int select_result = select(server.server_fd + 1, &readfds, nullptr, nullptr, &timeout);
    if (select_result == -1) {
        std::cerr << "system error: Select failed.\n";
        close(server.server_fd);
        shmctl(server.shmid, IPC_RMID, nullptr);
        unlink(server.info_file_path.c_str());
        exit(1);
    }

    if (select_result > 0) {
        server.client_fd = accept(server.server_fd, nullptr, nullptr);
        if (server.client_fd == -1) {
            std::cerr << "system error: Accepting client connection failed.\n";
            close(server.server_fd);
            shmctl(server.shmid, IPC_RMID, nullptr);
            unlink(server.info_file_path.c_str());
            exit(1);
        }
    } else {
        server.client_fd = -1;
    }

    if (server.client_fd != -1) {
        if (send(server.client_fd, socket_msg.c_str(), socket_msg.size(), 0) < 0) {
            std::cerr << "system error: Sending message via socket failed.\n";
            close(server.server_fd);
            close(server.client_fd);
            shmctl(server.shmid, IPC_RMID, nullptr);
            unlink(server.info_file_path.c_str());
            exit(1);
        }
    }

    sleep(10);

    close(server.server_fd);
    if (server.client_fd != -1) {
        close(server.client_fd);
    }
    shmctl(server.shmid, IPC_RMID, nullptr);
    unlink(server.info_file_path.c_str());

    return 0;
}



