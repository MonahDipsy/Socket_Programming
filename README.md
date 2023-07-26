# Assignment README

## Author
Monicah Odipo

## Description
The goal of this is to implement a simple client-server communication using shared memory and sockets in C++.

## File Structure
The assignment directory contains the following files:

- client.cpp: Contains the implementation of the client program.
- server.cpp: Contains the implementation of the server program.
- client.h: Header file containing the declarations of client functions and structs.
- server.h: Header file containing the declarations of server functions and structs.
- Makefile: A Makefile to compile the client and server programs.
- client_files_directory: A directory to store client-specific files.
- server_info_files: A directory to store server info files.

## Compilation and Execution
To compile the client and server programs, run the following command in the terminal:

```bash
make
To run the server, execute the following command:

bash
Copy code
./server
To run the client, execute the following command:

bash
Copy code
./client
Implementation Details
The server program listens for incoming client connections on a specified port and communicates with the client using both shared memory and sockets. The server creates a shared memory segment to share data with the client. It also creates an info file with relevant server information.

The client program connects to the server using the server's IP address and port number obtained from the info file. It then reads data from the shared memory segment to receive messages from the server.

Additional Notes
The server program creates the "server_info_files" directory to store server info files, and the client program creates the "client_files_directory" directory to store client-specific files.

The communication between the client and server is established through sockets, and data is shared using shared memory.

The Makefile generates two static libraries, "libclient.a" and "libserver.a", when running the "make" command.

Tested on Ubuntu version 22.04 LTS


References
1. https://wiki.cs.huji.ac.il/wiki/Private_Virtual_Machines
2. https://wiki.cs.huji.ac.il/wiki/Containers

