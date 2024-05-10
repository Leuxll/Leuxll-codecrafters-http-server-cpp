#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>
#include <sstream>
#include <thread>

#include "include/Request.hpp"
#include "include/Response.hpp"

void handle_client(int client_fd, std::string directory) {
  char buffer[1024];
  int ret = read(client_fd, buffer, sizeof(buffer) - 1); // leave space for null terminator
  if (ret < 0) {
    std::cerr << "Error in reading from client socket" << std::endl;
  } else if (ret == 0) {
    std::cout << "No bytes read" << std::endl;
  } else {
    buffer[ret] = '\0'; // null terminate the string
    std::string request(buffer);
    std::cout << "Request: " << request << std::endl;

    Request req = Request(request, directory);
    Response res = Response(req);
    std::string response = res.generate_response();
    std::cout << "Response: " << response << std::endl;
    std::cout << "Response length: " << response.length() << std::endl;

    int bytes_written = write(client_fd, response.c_str(), response.length());
    std::cout << "Bytes written: " << bytes_written << std::endl;

    if (bytes_written != response.length()) {
        std::cerr << "Error: did not write all bytes to client" << std::endl;
    }
  }
  close(client_fd);
}

int main(int argc, char **argv) {

  std::string directory;
  if (argc > 1) {
    directory = argv[2];
  } else {
    directory = "";
  }

  // You can use print statements as follows for debugging, they'll be visible when running tests.
  std::cout << "Logs from your program will appear here!\n";

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
   std::cerr << "Failed to create server socket\n";
   return 1;
  }
  
  // Since the tester restarts your program quite often, setting REUSE_PORT
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
    std::cerr << "setsockopt failed\n";
    return 1;
  }
  
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(4221);
  
  if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
    std::cerr << "Failed to bind to port 4221\n";
    return 1;
  }
  
  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    std::cerr << "listen failed\n";
    return 1;
  }
  
  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);
  
  std::cout << "Waiting for a client to connect...\n";

  while (true) {
    int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
    if (client_fd < 0) {
        std::cerr << "Error in accepting client" << std::endl;
    } else {
        std::cout << "Client connected\n";
    }

    std::thread client_thread(handle_client, client_fd, directory);
    client_thread.detach();
}
  
  
  close(server_fd);

  return 0;
}
