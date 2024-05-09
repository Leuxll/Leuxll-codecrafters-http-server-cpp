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

std::vector<std::string> split_message(const std::string &message, const std::string& delim) {
  std::vector<std::string> toks;
  std::stringstream ss = std::stringstream{message};
  std::string line;
  while (getline(ss, line, *delim.begin())) {
    toks.push_back(line);
    ss.ignore(delim.length() - 1);
  }
  return toks;
}

std::string generate_response(std::vector<std::string> parsed_message) {
  std::string first_line = parsed_message[0];
  std::string path = split_message(first_line, " ")[1];

  std::cout << first_line << std::endl;

  std::string response;
  if (path == "/") {
    response = "HTTP/1.1 200 OK\r\n\r\n";
  } else if (path == "/user-agent") {
    std::string third_line = parsed_message[2];
    std::string user_agent = split_message(third_line, " ")[1];
    response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: ";
    response += std::to_string(user_agent.length());
    response = response + "\r\n\r\n" + user_agent;
  } else {

    std::vector<std::string> split_path = split_message(path, "/");
    if (split_path[1] == "echo") {
      std::string echo_string = split_path[2];
      response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: ";
      response += std::to_string(echo_string.length());
      response = response + "\r\n\r\n" + echo_string;
    } else {
      response = "HTTP/1.1 404 Not Found\r\n\r\n";
    }

  }

  return response;

}

int main(int argc, char **argv) {
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
  
  int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
  if (client_fd < 0) {
    std::cerr << "Error in accepting client" << std::endl;
  } else {
    std::cout << "Client connected\n";
  }

  char buffer[1024];
  int ret = read(client_fd, buffer, sizeof(buffer));
  if (ret < 0) {
    std::cerr << "Error in reading from client socket" << std::endl;
  } else if (ret == 0) {
    std::cout << "No bytes read" << std::endl;
  } else {
    std::string request(buffer);
    std::cout << "Request: " << request << std::endl;

    std::vector<std::string> parsed_message = split_message(request, "\r\n");
    std::string response = generate_response(parsed_message);
    std::cout << "Response: " << response << std::endl;
    write(client_fd, response.c_str(), response.length());
  }
  
  
  close(server_fd);

  return 0;
}
