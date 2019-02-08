// udpexample.cpp

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>

int resolvehelper(const char* hostname, int family, const char* service, sockaddr_storage* pAddr);
void error(std::string err);

const int BUFFSIZE = 1024;
const char * PORT= "5800";
const char * IP = "10.37.92.43";


int main(int argc, char * argv[])
{
    int sock, result;
    struct sockaddr_in addr;
    char * buff = "hello";
    
    
    addr.sin_family = AF_INET;
    result = inet_aton(IP, &addr.sin_addr);
    if(result == 0)
        error("Error with inet_aton");
    addr.sin_port = htons(atoi(PORT));
    
    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(result == -1)
        error("Error in socket.");
    
    while(true) // Enter a loop.
    {
        result = sendto(sock, buff, /*strlen(buff)*/ 5, 0, (struct sockaddr *) &addr, sizeof(addr));
        if(result == -1)
            error("Error sending data.");
    }
    
    // Close Socket
    //close(sock);
  return 0;
}


int resolvehelper(const char* hostname, int family, const char* service, sockaddr_storage* pAddr)
{
    int result;
    addrinfo* result_list = NULL;
    addrinfo hints = {};
    hints.ai_family = family;
    hints.ai_socktype = SOCK_DGRAM; // without this flag, getaddrinfo will return 3x the number of addresses (one for each socket type).
    result = getaddrinfo(hostname, service, &hints, &result_list);
    if (result == 0)
    {
        //ASSERT(result_list->ai_addrlen <= sizeof(sockaddr_in));
        //memcpy(pAddr, result_list->ai_addr, result_list->ai_addrlen);
        freeaddrinfo(result_list);
    }
    
    return result;
}
    
    
void error(std::string err)
{
    std::cout << err << "\nPress enter to continue..." << std::endl;
    std::cin.get();
    std::system("clear");
}
