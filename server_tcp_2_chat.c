#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>

#define PORT "9034"
#define BACKLOG 10

void *get_in_addr(struct sockaddr *sa)
{
    if (AF_INET == sa->sa_family)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main ()
{
    fd_set master, read_fds;
    int fdmax;
    struct addrinfo *sServerInfo, sHints, *p;
    struct sockaddr_storage sRemoteAddr;
    socklen_t addrlen;
    int iListener , iNewFd, iStatus, yes=1, i, j;

    char acBuf[256];
    int iNumBytes;

    char acRemoteIP[INET6_ADDRSTRLEN];

    FD_ZERO (&master);
    FD_ZERO (&read_fds);
    memset (&sHints, 0, sizeof sHints);
    sHints.ai_family = AF_UNSPEC;
    sHints.ai_socktype = SOCK_STREAM;
    sHints.ai_flags = AI_PASSIVE;

    if (0 != (iStatus = getaddrinfo (NULL, PORT, &sHints, &sServerInfo)))
    {
        fprintf (stderr, "server: getaddrinfo %s\n", gai_strerror (iStatus));
        exit (1);
    }

    for (p=sServerInfo; NULL!=p; p = p->ai_next)
    {
        if (-1 == (iListener = socket (p->ai_family, p->ai_socktype, p->ai_protocol)))
        {
            perror ("server: socket");
            continue;
        }
        setsockopt (iListener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int));
        if (-1 == (bind (iListener, p->ai_addr, p->ai_addrlen)))
        {
            perror ("server: bind");
            close (iListener);
            continue;
        }
        break;
    }
    if (NULL == p)
    {
        fprintf ("server: failed to bind\n");
        exit (2);
    }
    freeaddrinfo (sServerInfo);
    if (-1 == listen (iListener,10))
    {
        perror ("server: listen");
        exit (3);
    }

    FD_SET (iListener, &master);
    fdmax = iListener;
    for (;;)
    {
        read_fds = master;
        if (-1 == (select (fdmax+1, &read_fds, NULL, NULL, NULL)))
        {
            perror ("server: select");
            exit (4);
        }
        for (i = 0;i <= fdmax; i++)
        {
            if (FD_ISSET (i, &read_fds))
            {
                if (i == iListener)
                {
                    //Handle new connection
                    addrlen = sizeof sRemoteAddr;
                    if (-1 == (iNewFd = accept (iListener, (struct sockaddr*)sRemoteAddr, &addrlen)))
                    {
                        perror ("server: accept");
                    }
                    else
                    {
                        FD_SET (iNewFd, &master);

                    }
                }

            }
        }

    }

}

