#include <iostream>
#include <string>
#include <map>
#include <vector>

namespace handler {
    std::map<std::string, std::string> dataToMap(std::string data){
        std::map <std::string, std::string> request;
        std::string space = " ", colon_space = ": ", newline = "\r\n";
        size_t pos = 0;
        
        // reauest line
        request.insert(std::pair<std::string, std::string> ((std::string) "method", data.substr(0, data.find(space))));
        data.erase(0, data.find(space) + space.length());
        request.insert(std::pair<std::string, std::string> ((std::string) "path", data.substr(0, data.find(space))));
        data.erase(0, data.find(space) + space.length());
        request.insert(std::pair<std::string, std::string> ((std::string) "version", data.substr(0, data.find(newline))));
        data.erase(0, data.find(newline) + newline.length());
        
        // header lines
        while ((pos = data.find(newline)) != std::string::npos) {
            //puts("IN");
            std::string feature = "", content = "", line = data.substr(0, pos);
            std::cout << line << '\n';

            size_t middle = line.find(colon_space);
            feature = line.substr(0, middle);
            line.erase(0, middle + colon_space.length());
            content = line;

            data.erase(0, pos + newline.length());
            request.insert(std::pair<std::string, std::string> (feature, content));
        }
        return request;
    }
    std::map<std::string, std::string> getResponse(std::map<std::string, std::string> request) {
        
    }
}
/*
POST /cgi-bin/process.cgi HTTP/1.1 
User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)
Host: www.tutorialspoint.com
Content-Type: application/x-www-form-urlencoded
Content-Length: length
Accept-Language: en-us
Accept-Encoding: gzip, deflate
Connection: Keep-Alive

*/