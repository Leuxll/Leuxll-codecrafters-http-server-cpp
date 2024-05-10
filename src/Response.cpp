#include <string>
#include <fstream>

#include "include/Request.hpp"
#include "include/Response.hpp"
#include "include/Util.hpp"

Response::Response(Request req) {
    if (req.get_method() == "GET") {
        if (req.get_path() == "/") {
            status = "HTTP/1.1 200 OK";
        } else if (req.get_path() == "/user-agent") {
            status = "HTTP/1.1 200 OK";
            content_type = "text/plain";
            body = req.get_user_agent();
            content_length = body.length();
        } else {
            std::vector<std::string> split_path = split_message(req.get_path(), "/");
            if (split_path[1] == "echo") {
                std::string echo_string = split_path[2];
                status = "HTTP/1.1 200 OK";
                content_type = "text/plain";
                body = echo_string;
                content_length = body.length();
            } else if (split_path[1] == "files") {
                std::string file_name = split_path[2];
                std::string file_path = req.get_dir() + "/" + file_name;
                std::ifstream file(file_path, std::ios::binary | std::ios::ate);
                if (file) {
                    std::streamsize file_size = file.tellg();
                    file.seekg(0, std::ios::beg);
                    std::vector<char> file_contents(file_size);
                    if (file.read(file_contents.data(), file_size)) {
                        status = "HTTP/1.1 200 OK";
                        content_type = "application/octet-stream";
                        body.assign(file_contents.begin(), file_contents.end());
                        content_length = file_size;
                    }
                } else {
                    status = "HTTP/1.1 404 Not Found";
                }
            }
        }
    } else if (req.get_method() == "POST") {
        status = "HTTP/1.1 201 Created\r\n\r\n";
        std::vector<std::string> split_path = split_message(req.get_path(), "/");
        std::string filename = split_path[2];
        std::string file_path = req.get_dir() + "/" + filename;
        std::ofstream file(file_path);
        if (file) {
            std::string body = req.get_body();
            file.write(body.c_str(), body.size());
        }
    }
}

std::string Response::generate_response() {
    std::string response = status;
    response += "\r\nContent-Type: " + (content_type.empty() ? "text/plain" : content_type);
    response += "\r\nContent-Length: " + std::to_string(body.size());
    response += "\r\n\r\n";  // Always add the empty line after the headers
    response += body;
    return response;
}