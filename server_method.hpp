#include <iostream>
#include <string>
#include <map>
#include <vector>

namespace handler {
    std::map<std::string, std::string> dataToMap(std::string data){
        std::map <std::string, std::string> request;
        std::string space = " ", newline = "\r\n";
        size_t pos = 0;

        // reauest line
        request.insert(std::pair<std::string, std::string> ((std::string) "method", data.substr(0, data.find(space))));
        data.erase(0, data.find(space) + space.length());
        request.insert(std::pair<std::string, std::string> ((std::string) "path", data.substr(0, data.find(space))));
        data.erase(0, data.find(space) + space.length());
        request.insert(std::pair<std::string, std::string> ((std::string) "version", data.substr(0, data.find(newline))));
        data.erase(0, data.find(newline) + newline.length());

        // header lines
        while ((pos = data.find(space)) != std::string::npos) {
            std::string feature = "", content = "";
            feature = data.substr(0, pos);
            data.erase(0, pos + space.length());
            if ((pos = data.find(newline) != std::string::npos)) {    
                content = data.substr(0, pos);
                data.erase(0, pos + newline.length());
            }
            request.insert(std::pair<std::string, std::string> (feature, content));
        }
        return request;
    }
}