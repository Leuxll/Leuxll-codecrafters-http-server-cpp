#include <string>
#include <vector>
#include <sstream>

#include "Request.hpp"
#include "Util.hpp"

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
    std::vector<std::string> user_agent_toks = split_message(user_agent_line, " ");
    user_agent = user_agent_toks[1];

    if (method == "POST") {
        body = split_request[split_request.size() - 1];
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