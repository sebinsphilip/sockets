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

typedef struct userList_t {
        char acSockId[2];
        char acUserName[100];
        struct userList_t * next;
}userList_node, *userList_pointer;

userList_pointer create_ListNode();
void delete_ListNode (userList_pointer p);
void delete_Node (userList_pointer *head, int iSockFd);
void attach_ListNode (userList_pointer *head, int iSockFd, char *acName);
char *getClientName_ListNode (userList_pointer head, int iSockFd);
