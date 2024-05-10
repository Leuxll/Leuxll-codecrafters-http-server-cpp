// Response.hpp

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include "include/Request.hpp"

class Response {

    std::string status;
    std::string content_type;
    int content_length;
    std::string body;

    public:
        Response(Request request);
        std::string generate_response();
};

#endif