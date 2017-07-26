#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#define SERVERPORT "3645"
#define MAXBUFLEN 100

int main (int argc, char* argv[])
{
    struct addrinfo sHints, *sServerInfo, *p;
    int iSockFd, iStatus, iNumBytes;

    if (3 != argc)
    {
        fprintf (stderr, "talker: usage ./talker hostname message");
        exit (1);
    }

    sHints.ai_family = AF_UNSPEC;
    sHints.ai_socktype = SOCK_DGRAM;

    if (0 != (iStatus = getaddrinfo (argv[1], SERVERPORT, &sHints, &sServerInfo)))
    {
        fprintf (stderr, "talker: getaddrinfo %s\n", gai_strerror (iStatus));
        return 1;
    }

    for (p = sServerInfo; NULL != p; p=p->ai_next)
    {
        if (-1 == (iSockFd = socket (p->ai_family, p->ai_socktype, p->ai_protocol)))
        {
            perror ("talker: socket");
            continue;
        }
        break;
    }
    if (NULL == p)
    {
        fprintf (stderr, "talker: failed to get socket\n");
        return 2;
    }
    if (-1 == (iNumBytes = sendto (iSockFd, argv[2], strlen(argv[2]), 0,
                    p->ai_addr, p->ai_addrlen)))
    {
        perror ("talker: sendto");
        exit (1);
    }
    fprintf (stdout, "talker: successfuly send %d bytes to %s", iNumBytes,
            argv[1]);
    close (iSockFd);
    return 0;

}
