#ifndef SERVER_H
#define SERVER_H

#include <string>

const int SHARED_MEMORY_SIZE = 1024; // You can adjust the size as needed

// Define the struct to hold the setup information for the server
struct server_setup_information {
	std::string ip_address; // IP address of the server
	int port;           	// Port number to bind the server socket
	std::string shm_pathname; // Pathname for the shared memory segment
	int shm_proj_id;    	// Project ID for the shared memory segment
	std::string info_file_name; // Name of the server info file
	std::string info_file_directory; // Directory path for the server info files
};

// Define a struct to hold the live server information
struct live_server_info {
	int server_fd; // File descriptor for the server socket
	int client_fd; // File descriptor for the connected client socket
	int shmid; 	// Shared memory segment ID
	std::string info_file_path; // Path to the server info file
};

// Function declarations
void start_communication(const server_setup_information& setup_info, live_server_info& server);
void create_info_file(const server_setup_information& setup_info, live_server_info& server);

#endif // SERVER_H







