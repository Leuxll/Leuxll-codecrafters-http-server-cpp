#include <string>

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
                FILE *file = fopen(file_path.c_str(), "r");
                if (file) {
                    fseek(file, 0, SEEK_END);
                    long file_size = ftell(file);
                    fseek(file, 0, SEEK_SET);
                    char *file_contents = (char *) malloc(file_size);
                    fread(file_contents, 1, file_size, file);
                    fclose(file);
                    status = "HTTP/1.1 200 OK";
                    content_type = "application/octet-stream";
                    body = file_contents;
                    content_length = file_size;
                } else {
                    status = "HTTP/1.1 404 Not Found";
                }
            } else {
                status = "HTTP/1.1 404 Not Found";
            }
        }
    } else if (req.get_method() == "POST") {
        status = "HTTP/1.1 201 Created\r\n\r\n";
        std::vector<std::string> split_path = split_message(req.get_path(), "/");
        std::string filename = split_path[2];
        std::string file_path = req.get_dir() + "/" + filename;

        FILE *file = fopen(file_path.c_str(), "w");
        if (file) {
            std::string body = req.get_body();
            fwrite(body.c_str(), 1, body.length(), file);  // Only write body.length() bytes
            fclose(file);
        }   
    }
}

std::string Response::generate_response() {
    std::string response = status;
    if (content_type != "") {
        response += "\r\n" "Content-Type: " + content_type;
        response += "\r\n" "Content-Length: " + std::to_string(content_length);
    }
    response += "\r\n\r\n";  // Always add the empty line after the headers
    if (body != "") {
        response += body;
    }
    return response;
}