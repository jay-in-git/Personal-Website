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
        request.insert(std::pair<std::string, std::string> ((std::string) "Method", data.substr(0, data.find(space))));
        data.erase(0, data.find(space) + space.length());
        request.insert(std::pair<std::string, std::string> ((std::string) "Path", data.substr(0, data.find(space))));
        data.erase(0, data.find(space) + space.length());
        request.insert(std::pair<std::string, std::string> ((std::string) "Version", data.substr(0, data.find(newline))));
        data.erase(0, data.find(newline) + newline.length());
        
        // header lines
        while ((pos = data.find(newline)) != std::string::npos) {
            //puts("IN");
            std::string feature = "", content = "", line = data.substr(0, pos);

            size_t middle = line.find(colon_space);
            feature = line.substr(0, middle);
            line.erase(0, middle + colon_space.length());
            content = line;

            data.erase(0, pos + newline.length());
            request.insert(std::pair<std::string, std::string> (feature, content));
        }
        request.insert(std::pair<std::string, std::string> ((std::string) "Body", data));
        return request;
    }
    std::string getResponse(std::map<std::string, std::string> request) {
        std::string response = "HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\n";
        if (!request["Method"].compare("GET")) {
            if (!request["Path"].compare("/")) {
                int fd = open("./base.html", O_RDONLY);
                char body[4096];
                read(fd, body, 4096);

                char header[256];
                sprintf(header, "Content-Length: %lu\r\nContent-Type: %s\r\n\r\n", strlen(body), "text/html; charset=UTF-8");

                response += header;                
                response += body;
            }
            else if(!request["Path"].compare("/mo.jpg")) {
                std::vector<char> buffer;   
                FILE* file_stream = fopen("/Users/waffle/Documents/CNproject/mo.jpg", "rb");

                size_t file_size;

                if(file_stream != nullptr)
                {
                    fseek(file_stream, 0, SEEK_END);
                    long file_length = ftell(file_stream);
                    rewind(file_stream);
                    buffer.resize(file_length);
                    file_size = fread(&buffer[0], 1, file_length, file_stream);
                }
                else
                {
                    printf("file_stream is null! file name -> %s\n", "./mo.jpg");
                }
                printf("%lu\n", file_size);
                std::string html = "HTTP/1.1 200 Okay\r\nContent-Type: text/html; charset=ISO-8859-4 \r\n\r\n" + std::string("FILE NOT FOUND!!");

                if( file_size > 0)
                {
                    // HTTP/1.0 200 OK
                    // Server: cchttpd/0.1.0
                    // Content-Type: image/gif
                    // Content-Transfer-Encoding: binary
                    // Content-Length: 41758

                    std::string file_size_str = std::to_string(file_size);
                    std::string file_str(buffer.begin(),buffer.end());
                    html = "HTTP/1.1 200 Okay\r\nContent-Type: image/png; Content-Transfer-Encoding: binary; Content-Length: " + file_size_str + ";charset=ISO-8859-4 \r\n\r\n" + file_str;
                }

                return html;
            }
            else {
                response = "HTTP/1.1 404 NOT FOUND\r\n";
            }
        }
        return response;
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
