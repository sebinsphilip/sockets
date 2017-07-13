#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int  main (int argc, char *argv[])
{
    struct addrinfo hints , *p , *res;
    int status ;
    char ip_str[INET6_ADDRSTRLEN];
    memset (&hints, 0, sizeof hints);

    if (2 != argc)
    {
        fprintf (stderr, "Usage: showip www.google.com\n");
        return 0;
    }

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    fprintf (stdout, "given ip:%s ", argv[1]);

    if (0 != (status = getaddrinfo (argv[1], NULL, &hints, &res)))
    {
        fprintf (stderr, "getaddrinfo () returned error: %s\n", gai_strerror (status));
        return 2;
    }

    for  (p = res; p!=NULL; p = p->ai_next)
    {
        void *addr = NULL;
        char * ipVer = NULL;
        if (AF_INET == p->ai_family)
        {
            struct sockaddr_in *sSockAddr = NULL;
            sSockAddr = (struct sockaddr_in *) p->ai_addr;
            addr = &(sSockAddr->sin_addr);
            ipVer = "IPv4";
        }
        else if (AF_INET6 == p->ai_family)
        {
            struct sockaddr_in6 *sSockAddr6 = NULL;
            sSockAddr6 = (struct sockaddr_in6 *) p->ai_addr;
            addr =  &(sSockAddr6->sin6_addr);
            ipVer = "IPv6";
        }
        inet_ntop (p->ai_family, addr, ip_str, sizeof ip_str);
        fprintf (stdout, "%s ip:%s  ver:%s\n",argv[1], ip_str, ipVer);
    }
    freeaddrinfo (res);
}
