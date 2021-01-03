#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

using namespace std;

namespace handler {
    map<string, string> dataToMap(string data){
        map <string, string> request;
        string space = " ", colon_space = ": ", newline = "\r\n";
        size_t pos = 0;
        
        // reauest line
        request.insert(pair<string, string> ((string) "Method", data.substr(0, data.find(space))));
        data.erase(0, data.find(space) + space.length());
        request.insert(pair<string, string> ((string) "Path", data.substr(0, data.find(space))));
        data.erase(0, data.find(space) + space.length());
        request.insert(pair<string, string> ((string) "Version", data.substr(0, data.find(newline))));
        data.erase(0, data.find(newline) + newline.length());
        
        // header lines
        while ((pos = data.find(newline)) != string::npos) {
            //puts("IN");
            string feature = "", content = "", line = data.substr(0, pos);

            size_t middle = line.find(colon_space);
            feature = line.substr(0, middle);
            line.erase(0, middle + colon_space.length());
            content = line;

            data.erase(0, pos + newline.length());
            request.insert(pair<string, string> (feature, content));
        }
        request.insert(pair<string, string> ((string) "Body", data));
        return request;
    }
    string getPass(string account){
        string user, pass;
        ifstream filein("account.txt");
        while(!filein.eof()){
            filein >> user;
            filein >> pass;
            if(user == account){
                return pass;
            }
        }
        filein.close();
        return "";
    }
    void Register(string user, string pass){
        ofstream out;
        out.open("account.txt", ofstream::app);
        out << user << endl;
        out << pass << endl;
        out.flush();
        out.close();
        return;
    }
    int checkCookie(string cookie){
        while(cookie.find(";") != string::npos){
            string tmp = cookie.substr(0, cookie.find(";"));
            string name = tmp.substr(0, tmp.find("="));
            string value = tmp.substr(tmp.find("=") + 1);
            if(name.substr(0,1) == " "){
                name.erase(0, 1);
            }
            if(name == "ning"){
                if(value == "chichi"){
                    return 1;
                }
                else{
                    return 0;
                }
            }
            cookie.erase(0, cookie.find(";") + 1);
        }
        if(cookie.find("=") != string::npos){
            string tmp = cookie.substr(0, cookie.find(";"));
            string name = tmp.substr(0, tmp.find("="));
            string value = tmp.substr(tmp.find("=") + 1);
            if(name.substr(0,1) == " "){
                name.erase(0, 1);
            }
            if(name == "ning"){
                if(value == "chichi"){
                    return 1;
                }
                else{
                    return 0;
                }
            }
        }
        return 0;
    }
    string getUserName(string cookie){
        while(cookie.find(";") != string::npos){
            string tmp = cookie.substr(0, cookie.find(";"));
            string name = tmp.substr(0, tmp.find("="));
            string value = tmp.substr(tmp.find("=") + 1);
            if(name.substr(0,1) == " "){
                name.erase(0, 1);
            }
            if(name == "user"){
                return value;
            }
            cookie.erase(0, cookie.find(";") + 1);
        }
        if(cookie.find("=") != string::npos){
            string tmp = cookie.substr(0, cookie.find(";"));
            string name = tmp.substr(0, tmp.find("="));
            string value = tmp.substr(tmp.find("=") + 1);
            if(name.substr(0,1) == " "){
                name.erase(0, 1);
            }
            if(name == "user"){
                return value;
            }
        }
        return "";
    }
    string getResponse(map<string, string> request) {
        string response = "HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\n";
        if (!request["Method"].compare("GET")) {
            if(!request["Path"].compare("/")){
                if(checkCookie(request["Cookie"])){
                    response = "HTTP/1.1 302 Found\r\nLocation: /profile\r\n";
                }else{
                    response = "HTTP/1.1 302 Found\r\nLocation: /login\r\n";
                }
            }
            else if (!request["Path"].compare("/profile")) {
                if(!checkCookie(request["Cookie"])){
                    response = "HTTP/1.1 302 Found\r\nLocation: /login\r\n";
                }
                else{
                    int fd = open("./base.html", O_RDONLY);
                    char body[4096];
                    read(fd, body, 4096);

                    char header[256];
                    sprintf(header, "Content-Length: %lu\r\nContent-Type: %s\r\n\r\n", strlen(body), "text/html; charset=UTF-8");

                    response += header;                
                    response += body;
                }
            }
            else if (!request["Path"].compare("/video")) {
                if(!checkCookie(request["Cookie"])){
                    response = "HTTP/1.1 302 Found\r\nLocation: /login\r\n";
                }
                else{
                    int fd = open("./video.html", O_RDONLY);
                    char body[4096];
                    read(fd, body, 4096);

                    char header[256];
                    sprintf(header, "Content-Length: %lu\r\nContent-Type: %s\r\n\r\n", strlen(body), "text/html; charset=UTF-8");

                    response += header;                
                    response += body;
                }
            }
            else if(!request["Path"].compare("/Mo.jpg")) {
                vector<char> buffer;   
                FILE* file_stream = fopen("./Mo.jpg", "rb");

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
                    exit(1);
                }
                printf("%lu\n", file_size);
                response = "HTTP/1.1 404 Not Found\r\n\r\n";

                if( file_size > 0)
                {

                    string file_size_str = to_string(file_size);
                    string file_str(buffer.begin(),buffer.end());
                    response = "HTTP/1.1 200 OK\r\nContent-Type: image/png\r\nContent-Transfer-Encoding: binary\r\nContent-Length: " + file_size_str + "; charset=ISO-8859-4\r\n\r\n" + file_str;
                }
            }
            else if(!request["Path"].compare("/jay.jpg")) {
                vector<char> buffer;   
                FILE* file_stream = fopen("./jay.jpg", "rb");

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
                    exit(1);
                }
                printf("%lu\n", file_size);
                response = "HTTP/1.1 404 Not Found\r\n\r\n";

                if( file_size > 0)
                {

                    string file_size_str = to_string(file_size);
                    string file_str(buffer.begin(),buffer.end());
                    response = "HTTP/1.1 200 OK\r\nContent-Type: image/png\r\nContent-Transfer-Encoding: binary\r\nContent-Length: " + file_size_str + "; charset=ISO-8859-4\r\n\r\n" + file_str;
                }
            }
            else if(!request["Path"].compare("/waffle.jpg")) {
                vector<char> buffer;   
                FILE* file_stream = fopen("./waffle.jpg", "rb");

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
                    exit(1);
                }
                printf("%lu\n", file_size);
                response = "HTTP/1.1 404 Not Found\r\n\r\n";

                if( file_size > 0)
                {

                    string file_size_str = to_string(file_size);
                    string file_str(buffer.begin(),buffer.end());
                    response = "HTTP/1.1 200 OK\r\nContent-Type: image/png\r\nContent-Transfer-Encoding: binary\r\nContent-Length: " + file_size_str + "; charset=ISO-8859-4\r\n\r\n" + file_str;
                }
            }
            else if(!request["Path"].compare("/test.mp4")) {
                vector<char> buffer;   
                FILE* file_stream = fopen("./chap0.mp4", "rb");

                size_t file_size;
                string range = request["Range"];
                range = range.substr(range.find("=") + 1, range.find("-"));
                int start = stoi(range);
                fseek(file_stream, 0, SEEK_END);
                int total_size = 69225782;
                if(file_stream != nullptr)
                {
                    fseek(file_stream, start, SEEK_SET);
                    int file_length = total_size - start;
                    if(file_length > 20000){
                        file_length = 20000;
                    }
                    buffer.resize(file_length);
                    file_size = fread(&buffer[0], 1, file_length, file_stream);
                }
                else
                {
                    printf("file_stream is null! file name -> %s\n", "./mo.jpg");
                    exit(1);
                }
                printf("%lu\n", file_size);
                response = "HTTP/1.1 404 Not Found\r\n\r\n";

                if( file_size > 0)
                {

                    string file_size_str = to_string(file_size);
                    string start_str = to_string(start);
                    string end_str = to_string(start + file_size - 1);
                    string file_str(buffer.begin(),buffer.end());
                    response = "HTTP/1.1 206 Partial Content\r\nContent-Length: " + file_size_str + "\r\nAccept-Ranges: bytes\r\nConnection: keep-alive\r\nContent-Range: bytes "+ start_str + "-"+ end_str +"/69225782\r\nContent-Type: video/mp4\r\n\r\n" + file_str;
                }
            }
            else if(!request["Path"].compare("/login")){
                if(checkCookie(request["Cookie"])){
                    response = "HTTP/1.1 302 Found\r\nLocation: /profile\r\n";
                }
                else
                {
                    int fd = open("./login.html", O_RDONLY);
                    char body[4096];
                    read(fd, body, 4096);

                    char header[256];
                    sprintf(header, "Content-Length: %lu\r\nContent-Type: %s\r\n\r\n", strlen(body), "text/html; charset=UTF-8");

                    response += header;                
                    response += body;
                }
            }
            else if(!request["Path"].compare("/logout")){
                response = "HTTP/1.1 302 Found\r\nLocation: /login\r\nSet-Cookie: ning=ning\r\n";
            }
            else if (!request["Path"].compare("/board")) {
                if(!checkCookie(request["Cookie"])){
                    response = "HTTP/1.1 302 Found\r\nLocation: /login\r\n";
                }
                else{
                    ifstream file("./message_board.html");
                ifstream message("./message.txt");
                int tagline = 72, count = 0;
                char buffer[1000];
                char message_buffer[1000];
                string body = "";
                while(!file.eof()){
                    file.getline(buffer, 1000);
                    if(count == tagline){
                        while(!message.eof()){
                            message.getline(message_buffer, 1000);
                            body += message_buffer;
                        }
                    }
                    body += buffer;
                    count ++;
                }
                char header[256];
                sprintf(header, "Content-Length: %lu\r\nContent-Type: %s\r\n\r\n", body.length(), "text/html; charset=UTF-8");

                response += header;                
                response += body;
                }                
            }
            else {
                response = "HTTP/1.1 404 Not Found\r\n";
                int fd = open("./404.html", O_RDONLY);
                char body[4096];
                read(fd, body, 4096);

                char header[256];
                sprintf(header, "Content-Length: %lu\r\nContent-Type: %s\r\n\r\n", strlen(body), "text/html; charset=UTF-8m");

                response += header;                
                response += body;
            }
        }
        else if(!request["Method"].compare("POST")){
            if(!request["Path"].compare("/login")){
                string info = request["Body"];
                string userName = info.substr(0, info.find("&"));
                info.erase(0, info.find("&") + 1);
                string pass = info;
                userName = userName.substr(userName.find("=") + 1);
                pass = pass.substr(pass.find("=") + 1);
                if(getPass(userName) ==  pass){
                    response = "HTTP/1.1 302 Found\r\nLocation: /\r\nSet-Cookie: ning=chichi\r\nSet-Cookie: user=" + userName + "\r\n";
                }
                else{
                    response = "HTTP/1.1 302 Found\r\nLocation: /login\r\n";
                }
            }
            else if(!request["Path"].compare("/register")){
                string info = request["Body"];
                string userName = info.substr(0, info.find("&"));
                info.erase(0, info.find("&") + 1);
                string pass = info;
                userName = userName.substr(userName.find("=") + 1);
                pass = pass.substr(pass.find("=") + 1);
                if(getPass(userName) != ""){
                    response = "HTTP/1.1 302 Found\r\nLocation: /login\r\n";
                }
                else{
                    Register(userName, pass);
                    response = "HTTP/1.1 302 Found\r\nLocation: /\r\nSet-Cookie: ning=chichi\r\nSet-Cookie: user="  + userName + "\r\n";
                }
            }
            else if (!request["Path"].compare("/message")) {
                string info = request["Body"];
                string message = info.substr(info.find("=") + 1);
                size_t pid;
                if ((pid = fork()) == 0) {
                    execlp("python3", "python3", "processMessage.py", getUserName(request["Cookie"]).c_str(), message.c_str() , NULL);
                    exit(0);
                }
                waitpid(pid, NULL, 0);
                //ofstream out;
                //cout << message << endl;
                //out.open("message.txt", ofstream::app);
                //out << "<p>" << message << "</p>" << endl;
                //out.flush();
                //out.close();
                response = "HTTP/1.1 302 Found\r\nLocation: /board\r\n";
            }
            else {
                response = "HTTP/1.1 404 Not Found\r\n";
                int fd = open("./404.html", O_RDONLY);
                char body[4096];
                read(fd, body, 4096);

                char header[256];
                sprintf(header, "Content-Length: %lu\r\nContent-Type: %s\r\n\r\n", strlen(body), "text/html; charset=UTF-8m");

                response += header;                
                response += body;
            }

        }
        else{
            response = "HTTP/1.1 404 Not Found\r\n";
                int fd = open("./404.html", O_RDONLY);
                char body[4096];
                read(fd, body, 4096);

                char header[256];
                sprintf(header, "Content-Length: %lu\r\nContent-Type: %s\r\n\r\n", strlen(body), "text/html; charset=UTF-8m");

                response += header;                
                response += body;
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
