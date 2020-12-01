#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include <assert.h>
#include "server_method.hpp"
#include <string>
#include <iostream>

int main(int argc, char **argv) {
    struct sockaddr_in svr_addr;
    unsigned short port = (unsigned short) atoi(argv[1]);
    int skt = socket(AF_INET, SOCK_STREAM, 0);
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_port = htons(port);
    if(inet_pton(AF_INET, "127.0.0.1", &svr_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(skt, (struct sockaddr *)&svr_addr, sizeof(svr_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    }
    char packet[4096] = "POST /cgi-bin/process.cgi HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: www.tutorialspoint.com\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: length\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: Keep-Alive\r\n";
    send(skt , packet, strlen(packet) , 0); 
    printf("Packet sent\n"); 
    return 0; 
}