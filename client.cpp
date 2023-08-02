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

int main() {
    server_setup_information setup_info;
    setup_info.port = 8080; // Replace with your server's port number
    setup_info.shm_pathname = "/dev/shm/server_shm1"; // Replace with your server's shared memory pathname
    setup_info.shm_proj_id = 1234; // Replace 1234 with your server's shared memory project ID
    setup_info.info_file_name = "server_info.txt"; // Replace with your server's info file name
    setup_info.info_file_directory = "/home/Nicah/server_info_files"; // Replace with the path to your server's info files

    // Check if the info file name and directory are valid
    if (setup_info.info_file_name.empty() || setup_info.info_file_directory.empty()) {
        std::cerr << "system error: info_file_name or info_file_directory is empty.\n";
        exit(1);
    }

    // Connect to the shared memory segment created by the server
    key_t shm_key = ftok(setup_info.shm_pathname.c_str(), setup_info.shm_proj_id);
    int shmid = shmget(shm_key, SHARED_MEMORY_SIZE, 0666);
    if (shmid == -1) {
        std::cerr << "system error: Opening shared memory segment failed.\n";
        exit(1);
    }

    // Attach the shared memory segment to the address space of the calling process
    char* shm_ptr = (char*)shmat(shmid, nullptr, 0);
    if (shm_ptr == (char*)-1) {
        std::cerr << "system error: Attaching to shared memory segment failed.\n";
        shmctl(shmid, IPC_RMID, nullptr);
        exit(1);
    }

    // Read the message from the shared memory segment
    std::string shm_msg(shm_ptr);
    std::cout << "Message received from server: " << shm_msg << std::endl;

    // Detach the shared memory segment from the address space of the calling process
    shmdt(shm_ptr);

    return 0;
}



