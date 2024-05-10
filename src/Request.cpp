#include <string>
#include <vector>
#include <sstream>

#include "include/Request.hpp"
#include "include/Util.hpp"

Request::Request(std::string request, std::string directory) {

    if (directory != "") {
        dir = directory;
    }

    std::vector<std::string> split_request = split_message(request, "\r\n");
    
    std::string header_firstline = split_request[0];
    std::vector<std::string> header_toks = split_message(header_firstline, " ");
    method = header_toks[0];
    path = header_toks[1];

    std::string user_agent_line = split_request[2];
    if (user_agent_line != "") {
        std::vector<std::string> user_agent_toks = split_message(user_agent_line, " ");
        user_agent = user_agent_toks[1];
    }

    if (method == "POST") {
        bool content_length_found = false;
        for (const auto& line : split_request) {
            if (line.find("Content-Length: ") == 0) {
                int content_length = std::stoi(line.substr(16));
                size_t body_start = request.find("\r\n\r\n") + 4;
                body = request.substr(body_start, content_length);
                content_length_found = true;
                break;
            }
        }
        if (!content_length_found) {
            // Handle the case where Content-Length header was not found
            // You can set the body to an empty string or throw an exception
            body = "";
        }
    }
}

std::string Request::get_method() {
    return method;
}

std::string Request::get_path() {
    return path;
}

std::string Request::get_user_agent() {
    return user_agent;
}

std::string Request::get_body() {
    return body;
}

std::string Request::get_dir() {
    return dir;
}