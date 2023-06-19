#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Data_Structures.h"

int main(int argc, char *argv[])
{
        int csock;
        struct sockaddr_in addr;
        clist_t companies;
        uslist_t users;
        int userid,stocks,companyid,current_companies,current_users,j,i;

        inet_aton(argv[1],addr.sin_addr);
        adddr.sin_port = htonl(argv[2]);
        addr.sin_family = AF_INET;

        if((csock = socket(AF_INET,SOCK_STREAM,0))!=0)
        {
                printf("Socket error.\n");
                exit(1);
        }
        if(connect(csock,(struct sockaddr *) &addr,sizeof(addr))<0)
        {
                printf("Connect Error.\n");
                exit(1);
        }

        clist_init(&companies);
        uslist_init(&users);

        recv(csock,current_companies,sizeof(current_companies),0);
        recv(csock,current_users,sizeof(current_users),0);
        for(i=0;i<current_users,i++)
        {
                recv(csock,userid,sizeof(userid),0);
                recv(csock,stocks,sizeof(stocks),0);
                uslist_insert(&users,userd,stocks,NULL);
                for(j=0;j<current_companies;j++)
                {
                        recv(csock,companyid,sizeof(companyid),0);
                        recv(csock,stocks,sizeof(stocks),0);
                        clist_insert(&users.cl,companyid,stocks);
                }
        }
        for(j=0;j<current_companies;j++)
        {
                recv(csock,companyid,sizeof(companyid),0);
                recv(csock,stocks,sizeof(stocks),0);
                clist_insert(companies,companyid,stocks);
        }

        recv(csock,stocks,sizeof(stocks),0);



}
