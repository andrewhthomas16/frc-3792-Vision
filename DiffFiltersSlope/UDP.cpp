// UDP.cpp

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <cstring>
#include "UDP.h"


// Constructors
UDP::UDP(){}
UDP::UDP(std::string _ip, std::string _port)
{
    ip = const_cast<char *> (_ip.c_str());
    port = const_cast<char *> (_port.c_str());
    setUp();
}

// Deconstructor
UDP::~UDP(){}

void UDP::setUp()
{
    addr.sin_family = AF_INET;
    result = inet_aton(ip, &addr.sin_addr);
    if(result == 0)
        error("Error with inet_aton");
    addr.sin_port = htons(atoi(port));
    
    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(result == -1)
        error("Error in socket.");
}


void UDP::send(const char * msg)
{
    result = sendto(sock, msg, std::strlen(msg), 0, (struct sockaddr *) &addr, sizeof(addr));
    if(result == -1)
        error("Error sending data.");
}

int UDP::resolvehelper(const char* hostname, int family, const char* service, sockaddr_storage* pAddr)
{
    int r;
    addrinfo* result_list = NULL;
    addrinfo hints = {};
    hints.ai_family = family;
    hints.ai_socktype = SOCK_DGRAM; /* without this flag, getaddrinfo will
                                     return 3x the number of addresses (one
                                     for each socket type).*/
    r = getaddrinfo(hostname, service, &hints, &result_list);
    
    return r;
}


void UDP::error(std::string err)
{
    std::cout << err << "\nPress enter to continue..." << std::endl;
    std::cin.get();
    std::system("clear");
}
