#include "tcp_2_chat.h"

userList_pointer create_ListNode()
{
	userList_pointer head = (userList_pointer)malloc(sizeof( userList_node));
	if (head != NULL)
		head->next = NULL;
	return head;
}
void delete_ListNode (userList_pointer p)
{
	userList_pointer temp;
	if (p!=NULL)
	{
		while(p->next != NULL)
		{
			temp = p;
			p = p->next;
			free(temp);
		}
	free(p);
	}
	p	= NULL;

}
void delete_Node (userList_pointer *head, int iSockFd)
{
        userList_pointer prev = NULL, p = *head;
        char acName[100];
        sprintf (acName, "%d", iSockFd);
        if (NULL == p)
        {
                return;
        }
        while (NULL != p)
        {
                if (0 == strcmp (acName, p->acSockId))
                {
                        if (NULL != prev)
                                prev->next = p->next;
                        free (p);
                        if (NULL == prev)
                                *head = NULL;
                        return;
                }
                prev = p;
                p = p->next;
        }

}
void attach_ListNode (userList_pointer *head, int iSockFd, char *acName)
{
        userList_pointer new_node = create_ListNode ();
        if (NULL == new_node)
        {
                printf ("NNK!!!\n");
        }
        userList_pointer p = *head;
        new_node->next = NULL;
        sprintf (new_node->acSockId, "%d", iSockFd ); 
        strcpy (new_node->acUserName, acName);
        printf ("new_node->acUserName:%s", new_node->acUserName);

        if (NULL == *head)
        {
                *head = new_node;
                return;
        }
        while (NULL != p->next)
                p = p->next;
        p->next = new_node;
}

char *getClientName_ListNode (userList_pointer head, int iSockFd)
{
        userList_pointer p = head;
        char acName[100];
        sprintf (acName , "%d", iSockFd);
        if (NULL == head)
        {
                printf ("No way!!\n");
        }
        while (NULL != p)
        {
                printf (">>>%s\n", p->acUserName);
                if (0 == strcmp (acName, p->acSockId))
                {
                        return p->acUserName;
                }
                p = p->next;
        }
        return "";
}
