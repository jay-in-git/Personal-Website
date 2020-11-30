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
#include <string>
#include <iostream>

typedef struct {
    unsigned short port;
    int listen_fd;
} server;

typedef struct {
    int connect_fd;  
    char buf[2048];  
    size_t buf_len;  
	int item;
} request;

int handleRequest();

server initServer(unsigned short port) {
    server svr;
    struct sockaddr_in servaddr;
    svr.port = port;
    svr.listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (svr.listen_fd < 0) perror("socket");
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    if (bind(svr.listen_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind");
        exit(1);
    }
    if (listen(svr.listen_fd, 1024) < 0) {
        perror("listen");
        exit(1);
    }
    return svr;
}

std::string strip(char *buf) {
    char* p1 = strstr(buf, "\015\012");
    if (p1 == NULL) p1 = strstr(buf, "\012");
    if (p1 == NULL){
        perror("Read miss ending...");
        exit(1);
    }
    buf[p1 - buf] = '\0';
    std::string result;
    result.append(buf, p1 - buf);
    return result;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        perror("Usage: ./server <port_num>");
        return -1;
    }
    unsigned short run_port = atoi(argv[1]);
    if (run_port < 0) {
        perror("Bad bad port");
        return -1;
    }
    server svr = initServer(run_port);

    int connect_fd;
    pid_t pid_list[4096], fork_num = 0;

    struct sockaddr_in cliaddr;
    unsigned long len = sizeof(cliaddr);
    while (1) {
        connect_fd = accept(svr.listen_fd, (struct sockaddr*)&cliaddr, (socklen_t*)&len);
        fprintf(stderr, "new connection %d\n", connect_fd);
        while (connect_fd < 0) {
            if (errno == EINTR || errno == EAGAIN) connect_fd = accept(svr.listen_fd, (struct sockaddr*)&cliaddr, (socklen_t*)&len);  // try again
            if (errno == ENFILE) {
                (void) fprintf(stderr, "out of file descriptor table ...\n");
                continue;
            }
            perror("accept");
        }
        if ((pid_list[fork_num++] = fork()) == 0) {
            char buffer[4096];
            read(connect_fd, buffer, 4096);

            std::string request = strip(buffer);

            exit(0);
        }
        close(connect_fd); // can it work?
        int status;
        for (int i = 0; i < fork_num; i++) waitpid(pid_list[i], &status, WNOHANG);
    }
}