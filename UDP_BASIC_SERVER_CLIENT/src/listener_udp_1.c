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

#define MYPORT "3645"
#define MAXBUFLEN 100

void *get_in_addr (struct sockaddr *s)
{
    if (AF_INET == s->sa_family)
    {
        return &((struct sockaddr_in*)s)->sin_addr;
    }
    return &((struct sockaddr_in6*)s)->sin6_addr;
}

int main ()
{
    struct addrinfo sHints, *sServerInfo, *p;
    struct sockaddr_storage their_addr;
    int iSockFd, iStatus, iNumBytes;
    char acBuf[MAXBUFLEN], acAddress[INET6_ADDRSTRLEN];
    socklen_t sAddrLen;

    memset (&sHints, 0, sizeof sHints);
    //sHints.ai_flags = AI_PASSIVE;
    sHints.ai_family = AF_UNSPEC;
    sHints.ai_socktype = SOCK_DGRAM;

    if (0 != (iStatus = getaddrinfo ("127.0.0.6", MYPORT, &sHints, &sServerInfo)))
    {
        fprintf (stderr, "listener: getaddrinfo %s\n", gai_strerror (iStatus));
        return 1;
    }
    for (p = sServerInfo; NULL != p; p=p->ai_next)
    {
        if (-1 == (iSockFd = socket (p->ai_family, p->ai_socktype, p->ai_protocol)))
        {
            perror ("listener: socket");
            continue;
        }
        if (-1 == bind (iSockFd, p->ai_addr, p->ai_addrlen))
        {
            perror ("listener: bind");
            continue;
        }
        break;
    }
    if (NULL == p)
    {
        fprintf (stderr, "listener: failed to bind socket\n");
        return 2;
    }
    fprintf (stdout, "listener waiting for message ...\n");
    freeaddrinfo (sServerInfo);
    sAddrLen =  sizeof (their_addr);
    if (-1 == (iNumBytes = recvfrom (iSockFd, acBuf, MAXBUFLEN-1, 0,
                    (struct sockaddr*)&their_addr, &sAddrLen)))
    {
        perror ("listener: recvfrm");
        exit (1);
    }
    acBuf[iNumBytes] = '\0';
    fprintf (stdout, "listener: received %d bytes from %s\n", iNumBytes,
            inet_ntop (their_addr.ss_family, get_in_addr((struct sockaddr*)&their_addr),
                acAddress, sizeof acAddress));
    fprintf (stdout, "listener: packet contains :%s\n", acBuf);
    close (iSockFd);
    return 0;
}
