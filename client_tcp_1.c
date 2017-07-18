#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT "3490"

#define MAXDATASIZE 100

void *get_in_addr(struct sockaddr *sa)
{
        if (AF_INET == sa->sa_family)
        {
                return &(((struct sockaddr_in*)sa)->sin_addr);
        }
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main (int argc, char *argv[])
{
        int iSockFd, iNumBytes;
        char acBuf[MAXDATASIZE];
        struct addrinfo sHints , *sServerInfo, *p;
        int iStatus;
        char acAddress[INET6_ADDRSTRLEN];

        if (2 != argc)
        {
                fprintf (stderr, "usage: client hostname");
                exit (1);
        }

        memset (&sHints, 0, sizeof sHints);
        sHints.ai_family = AF_UNSPEC;
        sHints.ai_socktype = SOCK_STREAM;

        if (0 != (iStatus = getaddrinfo (argv[1], PORT, &sHints, &sServerInfo)))
        {
                fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (iStatus));
                return 1;
        }

        for (p = sServerInfo; NULL!=p ; p=p->ai_next)
        {
                if (-1 == (iSockFd = socket (p->ai_family, p->ai_socktype, p->ai_protocol)))
                {
                        perror ("client: connect");
                        continue;
                }
                if (-1 == (connect (iSockFd, p->ai_addr, p->ai_addrlen)))
                {
                        close (iSockFd);
                        perror ("client: connect");
                        continue;
                }
                break;
        }
        if (NULL == p)
        {
                fprintf (stderr, "client: failed to connect\n");
                return 2;
        }
        inet_ntop (p->ai_family, get_in_addr ((struct sockaddr*)(p->ai_addr)),
                                acAddress, sizeof acAddress);
        printf ("client: connecting to %s\n", acAddress);
        freeaddrinfo (sServerInfo);
        if (-1 == (iNumBytes = recv (iSockFd, acBuf, MAXDATASIZE-1, 0)))
        {
                perror ("client: recv");
                exit (1);
        }
        acBuf[iNumBytes] = '\0';
        printf ("client: rfeceived '%s'\n", acBuf);
        close (iSockFd);
        return 0;
}
