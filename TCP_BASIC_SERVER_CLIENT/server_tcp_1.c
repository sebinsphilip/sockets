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

#define PORT "3490"
#define BACKLOG 10

void sigchld_handler (int s)
{
        while (waitpid (-1, NULL, WNOHANG) > 0);
}

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
        struct addrinfo *sServerInfo, sHints, *p;
        int iSockFd , iNewFd, iStatus, yes=1;
        struct sigaction sa;
        socklen_t sin_size;
        struct sockaddr_storage their_addr;

        memset (&sHints , 0, sizeof sHints);
        sHints.ai_family = AF_UNSPEC;
        sHints.ai_socktype = SOCK_STREAM;
        //sHints.ai_flags = AI_PASSIVE;
        char acAddress[INET6_ADDRSTRLEN];

        if (0 != (iStatus = getaddrinfo ("127.0.0.27", PORT, &sHints, &sServerInfo)))
        {
                fprintf (stdout, "getaddrinfo : %s\n", gai_strerror (iStatus));
                return 1;
        }

        for (p = sServerInfo; p!=NULL; p=p->ai_next)
        {
                if (-1 == (iSockFd = socket (p->ai_family, p->ai_socktype, 
                                p->ai_protocol)))
                {
                        perror ("server: socket ()");
                        continue;
                }

                if (-1 == (setsockopt (iSockFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int))))
                {
                        perror ("server: setsockopt ()");
                        continue;
                }

                if (-1 == (bind (iSockFd, p->ai_addr, p->ai_addrlen)))
                {
                        close (iSockFd);
                        perror ("server: bind ()");
                        continue;
                }

                break;
        }
        if (NULL == p)
        {
                fprintf (stderr, "server: failed to bind\n");
                return 2;
        }
        freeaddrinfo (sServerInfo);
        if (-1 == listen (iSockFd, BACKLOG))
        {
                perror ("server: listen ()");
                exit (1);
        }

        sa.sa_handler = sigchld_handler;
        sigemptyset (&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        if (-1 == sigaction (SIGCHLD, &sa, NULL))
        {
                perror ("server: sigaction()");
                exit (1);
        }

        printf ("sever: waiting for connections...\n");
        while (1)
        {
                sin_size = sizeof their_addr;
                iNewFd = accept (iSockFd, (struct sockaddr *)&their_addr, &sin_size);
                if (-1 == iNewFd)
                {
                        perror ("server: accept ()");
                        continue;
                }

                inet_ntop (their_addr.ss_family, 
                                get_in_addr ((struct sockaddr *)&their_addr),
                                acAddress, sizeof acAddress);
                printf ("server: got connection from %s", acAddress);
                if (!fork ())
                {
                        close (iSockFd);
                        if (-1 == send (iNewFd, "Hello world!", 13, 0))
                        {
                                perror ("server: send ()");
                        }
                        close (iNewFd);
                        exit (0);
                }
                close (iNewFd);
        }
return 0;
}
