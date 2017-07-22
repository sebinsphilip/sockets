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

int sendall (int iSockFd, char* acBuf, int* len)
{
        int n , total =0, iNumBytesLeft = *len;
        //printf ("server: debug: len:%d", *len);
        while (total < *len)
        {
                n = send (iSockFd, acBuf+total, iNumBytesLeft, 0);
                if (-1 == n)
                {
                        perror ("client: send");
                        break;
                }
                total += n;
                iNumBytesLeft -= n;
        }
        *len = total;
        return -1 == n ?-1:0;
}

void rearrangeClientNames (int iIndex,char *acClientNames[10][2], int uiNumClients)
{
        int i,j;
        if (0 == uiNumClients)
        {
                return;
        }
        for  (i = iIndex; i< uiNumClients; i++)
        {
                strcpy (**((acClientNames + i) + 0) , **((acClientNames + (i+1)) + 0));
                strcpy (**((acClientNames + i) + 1) , **((acClientNames + (i+1)) + 1));
        }
        free (**((acClientNames + i) + 0));
        free (**((acClientNames + i) + 1));
}

int getClientIndex (int i, char  *acClientNames[10][2], int len)
{
        int j;
        char acSockId[10];
        //itoa (acSockId, i);
        sprintf (acSockId, "%d",i);
        for (j = 0;j < len;j++)
        {
                if (0 == strcmp (**((acClientNames + j) + 0), acSockId))
                {
                        printf ("index:%d\n", j);
                        return j;
                }
                else
                {
                        printf ("j:%d :%s  \n",j, **((acClientNames + j) + 0));

                }
        }
        return -1;
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
        fd_set master, read_fds;
        int fdmax, uiNumClients=0, iIndex;
        struct addrinfo *sServerInfo, sHints, *p;
        struct sockaddr_storage sRemoteAddr;
        socklen_t addrlen;
        int iListener , iNewFd, iStatus, yes=1, i, j;

        char acBuf[256], *acClientNames[10][2], acTmp[256];
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

        for (p = sServerInfo; NULL != p; p = p->ai_next)
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
                fprintf (stderr, "server: failed to bind\n");
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
        printf("\nTCPServer Waiting for client on port %s\n", PORT);
        for (;;)
        {
                strcpy (acTmp, "");
                strcpy (acBuf, "");
                read_fds = master;
                if (-1 == (select (fdmax+1, &read_fds, NULL, NULL, NULL)))
                {
                        perror ("server: select");
                        exit (4);
                }
                for (i = 0;i <= fdmax; i++)
                {
                strcpy (acTmp, "");
                strcpy (acBuf, "");
                if (FD_ISSET (i, &read_fds))
                        {
                                if (i == iListener)
                                {
                                        //Handle new connection
                                        addrlen = sizeof sRemoteAddr;
                                        if (-1 == (iNewFd = accept (iListener, (struct sockaddr*)&sRemoteAddr, &addrlen)))
                                        {
                                                perror ("server: accept");
                                        }
                                        else
                                        {
                                                FD_SET (iNewFd, &master);
                                                if (iNewFd > iListener)
                                                {
                                                        fdmax = iNewFd;
                                                }
                                                fflush (stdout);
                                                inet_ntop (sRemoteAddr.ss_family,get_in_addr ((struct sockaddr*)&sRemoteAddr), acRemoteIP, sizeof acRemoteIP);
                                                if (0 >= (iNumBytes = recv (iNewFd, acBuf, 256, 0)))
                                                {
                                                        perror ("server: recv");
                                                }
                                                else
                                                {
                                                        acClientNames[uiNumClients][0] = (char*)malloc (sizeof (iNewFd));
                                                        //itoa (iNewFd, acTmp);
                                                        sprintf (acTmp, "%d", iNewFd);
                                                        strcpy (acClientNames[uiNumClients][0], acTmp);
                                                        //printf ("sever: debug : acBuf:%s: iNumBytes:%d\n", acBuf, iNumBytes);
                                                        acClientNames[uiNumClients][1] = (char*)malloc (sizeof (acBuf));
                                                        strcpy(acClientNames[uiNumClients][1], acBuf);
                                                        printf ("server: New connection from %s[%s] on socket: %d||%s\n", acRemoteIP, acClientNames[uiNumClients][1], iNewFd, 
                                                                        acClientNames[uiNumClients][0]);
                                                        uiNumClients++;

                                                }


                                        }
                                }
                                else
                                {
                                        //Handle data frm client
                                        if (0 >= (iNumBytes = recv (i, acBuf, 256, 0)))
                                        {
                                                if (0 == iNumBytes)
                                                {
                                                        fprintf (stderr, "server: socket %d hung up\n", i);
                                                        if (-1 == (iIndex = getClientIndex (i, acClientNames, uiNumClients)))
                                                        {
                                                                printf ("client: no client index found!\n");
                                                                exit (1);
                                                        }
                                                        rearrangeClientNames (iIndex, acClientNames, uiNumClients);
                                                        uiNumClients--;
                                                }
                                                else
                                                {
                                                        perror ("server: recv");
                                                }
                                                close (i);
                                                FD_CLR (i, &master);
                                        }
                                        else
                                        {
                                                //Send this to all other clients
                                                for (j = 0; j<= fdmax ;j++)
                                                {
                                                        if (FD_ISSET (j, &master))
                                                        {
                                                                if (j != iListener && j!= i)
                                                                {
                                                                        iIndex = getClientIndex (i, acClientNames, uiNumClients);
                                                                        strcpy (acTmp, "");
                                                                        strcpy (acTmp, acClientNames[iIndex][1]);
                                                                        strcat (acTmp, acBuf);
                                                                        //printf ("server: sendall : %s\n", acTmp);
                                                                        iNumBytes = strlen (acTmp);
                                                                        if (-1 == (iNumBytes =sendall (j, acTmp, &iNumBytes)))
                                                                        {
                                                                                perror ("server: send");
                                                                        }
                                                                }
                                                        }
                                                }
                                        }
                                }

                        }
                }

        }

}

