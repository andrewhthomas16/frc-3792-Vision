// UDP.h

#pragma once
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>


class UDP
{
public:
    UDP(); // Constructors
    UDP(std::string _ip, std::string _port);
    
    ~UDP(); // Deconstructor
    
    void setUp();
    void send(const char * msg);
    int resolvehelper(const char* hostname, int family, const char* service, sockaddr_storage* pAddr);
    void error(std::string err);
private:
    char * ip, * port;
    int sock, result;
    struct sockaddr_in addr;
};
