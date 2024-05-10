// Request.hpp

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>

class Request {

    std::string dir;
    std::string method;
    std::string path;
    std::string user_agent;
    std::string body;

    public:
        Request(std::string request, std::string directory);
        std::string get_method();
        std::string get_path();
        std::string get_user_agent();
        std::string get_body();
        std::string get_dir();
};

#endif