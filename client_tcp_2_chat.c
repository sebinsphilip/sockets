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

#define PORT "9034"

#define MAXDATASIZE 256

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
        int iSockFd, iNumBytes, i;
        char acRecvBuf[MAXDATASIZE], acSendBuf[MAXDATASIZE];
        struct addrinfo sHints , *sServerInfo, *p;
        int iStatus, fdmax;
        fd_set master, read_fds;
        char acAddress[INET6_ADDRSTRLEN];

        if (2 != argc)
        {
                fprintf (stderr, "usage: client hostname");
                exit (1);
        }

        FD_ZERO (&master);
        FD_ZERO (&read_fds);

        memset (&sHints, 0, sizeof sHints);
        sHints.ai_family = AF_UNSPEC;
        sHints.ai_socktype = SOCK_STREAM;

        if (0 != (iStatus = getaddrinfo (argv[1], PORT, &sHints, &sServerInfo)))
        {
                fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (iStatus));
                return 1;
        }
        for (p = sServerInfo; NULL != p; p = p->ai_next)
        {
                if (-1 == (iSockFd = socket (p->ai_family, p->ai_socktype, p->ai_protocol)))
                {
                        perror ("client: socket");
                        continue;
                }
                if (-1 == connect (iSockFd, p->ai_addr, p->ai_addrlen))
                {
                        close (iSockFd);
                        perror ("client: connect");
                        continue;
                }
                break;
        }
        if (NULL == p)
        {
                fprintf (stderr, "client: failed to connect to server!\n");
                exit (3);
        }
        inet_ntop (p->ai_family, get_in_addr ((struct sockaddr*)(p->ai_addr)),
                                acAddress, sizeof acAddress);
        printf ("client: connecting to %s\n", acAddress);
        freeaddrinfo (sServerInfo);

        FD_SET (0, &master);
        FD_SET (iSockFd, &master);
        fdmax = iSockFd;

        while (1)
        {
                read_fds = master;
                if (-1 == (select (fdmax+1, &read_fds, NULL, NULL, NULL)))
                {
                        perror ("select");
                        exit (4);
                }
                for (i = 0; i <= fdmax; i++)
                {
                        if (FD_ISSET (i, &read_fds))
                        {
                                if (0 == i)
                                {
                                        //Client writing a message
                                        fgets (acSendBuf, sizeof acSendBuf, stdin);
                                        if (0 == strcmp (acSendBuf, "quit"))
                                        {
                                                exit (0);
                                        }
                                        else
                                        {
                                               if (-1 == (iNumBytes =send (iSockFd, acSendBuf, sizeof acSendBuf, 0)))
                                               {
                                                       perror ("client: send");
                                               }
                                        }
                                }
                                else
                                {
                                        //Message ready to be received in socket
                                        if (-1 == (iNumBytes = recv (iSockFd, acRecvBuf, sizeof acRecvBuf, 0)))
                                        {
                                                perror ("client: recv");
                                        }
                                        else
                                        {
                                                acRecvBuf[iNumBytes] = '\0';
                                                printf ("%s\n",acRecvBuf);
                                                fflush (stdout);
                                        }
                                }
                        }
                }
        }
}
