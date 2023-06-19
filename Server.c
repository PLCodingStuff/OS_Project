#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/sem.h> //h semaphore.h////////////////////////////////////asdasd
#include "Data_Structures.h"

/*********************************************************
 * working_thread_function: the function that queue      *
 *                          working threads are running. *
 *********************************************************/
void *working_thread_function(void *arg);

/***********************************************************************
 * request_processing_thread_function: the function that request       *
 *                                     processing threads are running. *
 ***********************************************************************/
void *request_processing_thread_function(void *arg);

/*************************************************************
 * filepurchase: writes a purchase to a file with purchases. *
 *************************************************************/
void filepurchase(purchase_t pur);

/*******************************************************************
 * writetofile: write to a sorted list. If the list gets to 10     *
 *              elements, then every element is written on a file. *
 *******************************************************************/
void writetofile(purchase_t pur);

/**************************************************************
 * initialization_function: initializes every data structure. *
 **************************************************************/
void initialization_function();

/************************************************************
 * send_users: sends the users to a client. The arguments   *
 *             are the cliend socket and a list with users. *
 ************************************************************/
void send_users(int csock);

/*****************************************************************
 * send_companies: send companies to a client. The arguments are *
 *                 the client socket and a list with companies.  *
 *****************************************************************/
void send_companies(int csock);

/*********************************************************************
 * command_tokenizer: returns the command given from client in       *
 *                    string form. Arguments are the command string. *
 *********************************************************************/
command_t command_tokenizer(char string[]);

/***************************************************************
 * search_purchases: searches purchases from the begining of   *
 *                   the file until a specific timestamp. The  *
 *                   arguments are the upper bound timestamp,  *
 *                   the id of the user who's purchases are    *
 *                   searhced and the client's socket for      *
 *                   sending the purchses of the file.         *
 ***************************************************************/
void search_purchases(struct tm timestamp,int userid,int csock);

/*********************************************************
 * purchase_tokenizer: tokenizes a string into a         *
 *                     purchase. Argument is the string. *
 *********************************************************/
purchase_t purchase_tokenizer(char string[]);

/***********************************************************
 * timestamp_tokenizer: tokenizes a string into a          *
 *                      timestamp. Argument is the string. *
 ***********************************************************/
struct tm timestamp_tokenizer(char timestamp[]);

/*********************************************
 * bqueue: queue containing buying commands. *
 *********************************************/
qlist_t bqueue;
/**********************************************
 * squeue: queue containing selling commands. *
 **********************************************/
qlist_t squeue;
/********************************************************
 * prices: list containing the priceses of every stock. *
 ********************************************************/
splist_t prices;
/********************************************
 * users: list containing subscribed users. *
 ********************************************/
uslist_t users;
/*****************************************
 * companies: list containing companies. *
 *****************************************/
clist_t companies;
/******************************************************
 * purchases: sorted list containing the purchases    *
 *            that are ready to be written on a file. *
 ******************************************************/
flist_t purchases;
/********************************************************
 * ready_purchases: a list with all the purchases made. *
 *                  Request processing threads check    *
 *                  the list for purchases that made    *
 *                  for their clients and remove them.  *
 ********************************************************/
flist_t ready_purchases;
/**************************************************************
 * empty: mutex for the semaphore used at purchase list.      *
 * purwritelock: mutex for locking the filepurchase function. *
 **************************************************************/
pthread_mutex_t empty,purwritelock;

/*
 * Kapoioi simaphoroi
 */


/*******************************************************************
 * current_users: holds the number of current saved users.         *
 * current_companies: holds the number of current saved companies. *
 * commandid: its the max command id number at each time.          *
 * client_counter: counts how many active clients are.             *
 *******************************************************************/
int current_users,current_companies,commandid,client_counter;

int main(int argc, char *argv[])
{
        int i,s,client;
        struct sockaddr_in addr;
        pthread_t workers[atoi(argv[3])],clients[atoi(argv[4])];


        addr.sin_family = AF_INET;
        inet_aton(argv[1],&client_addr.sin_addr);
        addr.sin_port = htonl(argv[2]);

        if((s = socket(AF_INET,SOCK_STREAM,0))!=0)
        {
                printf("Socket Error.\n");
                printf("Server Terminating...\n");
                exit(1);
        }
        if(bind(s,(struct sockaddr_in *)&addr,sizeof(addr))!=0)
        {
                printf("Bind Error.\n");
                printf("Server Terminating...\n");
                exit(1);
        }
        if(listen(s,atoi(argv[4]))!=0)
        {
                printf("Listen Error.\n");
                printf("Server Terminating...\n");
                exit(1);
        }

        initialization_function();
        read_companies();
        read_users();


        for(i = 0;i < atoi(argv[3]);i++)
        {
                pthread_create(workers[i],NULL,working_thread_function,NULL);
        }

        /************************************************
         * Server starts getting requests from clients. *
         ************************************************/
        while(1)
        {
                if(client_counter < atoi(argv[4]))
                {
                        client = accept(s,NULL,NULL);
                        pthread_create(clients[counter],NULL,
                                       request_processing_thread_function,
                                       (void *) &client);
                        counter++;
                }
                else
                {
                        printf("Maximum number of clients reached.\n");
                }
        }
}

void *request_processing_thread_function(void *arg)
{
        int sock,category,ret,userid;
        char buf[1024];
        command_t com;
        struct tm timestamp;
        flist_t *cur;

        sock = *((int *) arg);

        /***************************************
         * Send to client users and companies. *
         ***************************************/
        send(sock,current_users,sizeof(current_users),0);
        send(sock,current_companies,sizeof(current_companies),0);
        send_users(sock);
        send_companies(sock,companies);
        recv(sock,userid,sizeof(userid),0);
        if(userid == 0)
        {
                userid = uslist_insert(&users,++current_users,
                                    0,NULL);
                send(sock,userid,sizeof(userid),0);
        }
        while(1)
        {
                /***************************************
                 * Recieve category of user's command. *
                 ***************************************/
                recv(sock,category,sizeof(category),0);
                switch (category)
                {
                        /*********************************************
                         * Case that a user connected to the system. *
                         *********************************************/
                        case 0:
                                recv(sock,userid,sizeof(userid),0);
                                if(userid == 0)
                                {
                                        userid = uslist_insert(&users,++current_users,
                                                            0,NULL);
                                        send(sock,userid,sizeof(userid),0);
                                }
                                recv(sock,timestamp,sizeof(timestamp),0);
                                search_purchases(timestamp,us,sock);
                        /*************************************************
                         * Case the command is a buying/selling command. *
                         *************************************************/
                        case 1:
                                recv(sock,buff,1024,0);
                                com = command_tokenizer(buff);
                                if(com.command_type==1 || com.command_type==2)
                                {
                                        qlist_insert(&squeue,com);
                                }
                                else
                                {
                                        qlist_insert(&bqueue,com);
                                }
                        /*******************************************
                         * Case the command is an exiting command. *
                         *******************************************/
                        case 2:
                                //simaphorosclient_counter--;
                        /*************************************************
                         * Case the command is a deletion of a commmand. *
                         *************************************************/
                        case 3:
                                recv(sock,buff,1024,0);
                                com = command_tokenizer(buff);
                                if(com.userid == 0)
                                {
                                        send(sock,-1,sizeof(-1),0);
                                        break;
                                }
                                if(com.command_type==1 || com.command_type==2)
                                {
                                        ret = qlist_delete(&squeue,com);
                                }
                                else
                                {
                                        ret = qlist_delete(&bqueue,com);
                                }
                                send(sock,ret,sizeof(ret),0);
                }
                /*********************************************
                 * Checks if there is any new purchase made. *
                 *********************************************/
                cur = flist_remove(&ready_purchases,userid);
                while(cur!=NULL)
                {
                        send(sock,cur->p,sizeof(cur->p),0);
                        cur = flist_remove(&ready_purchases,userid);
                }
        }
}

void *working_thread_function(void *arg)
{
        qnode_t *bnode,*snode;
        int stocks;
        purchase_t pur;
        time_t t;


        snode = NULL;

        while(1)
        {
                /******************************************************
                 * Searchs for a node at buying list. It focuses on   *
                 * buying list, because in general sellers sell their *
                 * products focused on the requests of the buyers.    *
                 ******************************************************/
                bnode = qlist_get_node(&bqueue);
                if(bnode != NULL)
                {
                        /*****************************************
                         * If a node that is not used was found, *
                         * a match is searched in the other list *
                         *****************************************/
                        snode = qlist_search_match(&squeue,snode);
                }
                if(snode != NULL)
                {
                        pur.commandid1 = bnode->c.commandid;
                        pur.commandid2 = snode->c.commandid;
                        pur.price = bnode->c.price;
                        time(&t);
                        pur.timestamp = localtime(&t);
                        /********************************************
                         * If a match was found, an update for      *
                         * prices and users lists is done for each. *
                         ********************************************/
                        if(splist_change(&prices,bnode->c.companyid,
                                         bnode->c.price.)!= 0)
                        {
                                printf("Error: Unable to ");
                                printf("change stock price.\n");
                        }

                        /*******************************************
                         * Seller sold less stocks than he wanted. *
                         *******************************************/
                        if(bnode->c.stocks <= snode->c.stocks)
                        {
                                if(uslist_change(&users,bnode->c.userid,
                                                 bnode->c.companyid,
                                                 bnode->c.stocks)!=0)
                                {
                                        printf("Error: Unable to ");
                                        printf("change user stocks.\n");
                                }
                                if(uslist_change(&users,snode->c.userid,
                                                 snode->c.companyid,
                                                 bnode->c.stocks*=(-1))!=0)
                                {
                                        printf("Error: Unable to ");
                                        printf("change user stocks.\n");
                                }
                                pur.quantity = bnode->c.stocks;
                                /*******************************************
                                 * If seller sold less, delete buyer only. *
                                 *******************************************/
                                if(bnode->c.stocks < snode->c.stocks)
                                {
                                        snode->c.stocks-=bnode->c.stocks;
                                        if(qlist_insert(&squeue,snode->c)!=0)
                                        {
                                                printf("Error. Buying queue ");
                                                printf("node wasn't added\n");
                                        }
                                        if(qlist_delete(&bqueue,bnode->c.commandid)!=0)
                                        {
                                                //Steile kati ston client.
                                        }
                                }
                                /********************************************
                                 * If both wanted to purchase/sell the      *
                                 * same amount of stocks, then delete both. *
                                 ********************************************/
                                else
                                {
                                        if(qlist_delete(&bqueue,bnode->c.commandid)!=0)
                                        {
                                                //Steile kati ston client.
                                        }
                                        if(qlist_delete(&squeue,snode->c.commandid)!=0)
                                        {
                                                //steile minima sto client.
                                        }
                                }
                        }
                        /***********************************************
                         * Buyer purchased less stocks than he wanted. *
                         ***********************************************/
                        else
                        {
                                if(uslist_change(&users,bnode->c.userid,
                                                 bnode->c.companyid,
                                                 snode->c.stocks)!=0)
                                {
                                        printf("Error: Unable to ");
                                        printf("change user stocks.\n");
                                }
                                if(uslist_change(&users,snode->c.userid,
                                                 snode->c.companyid,
                                                 snode->c.stocks*=(-1))!=0)
                                {
                                        printf("Error: Unable to ");
                                        printf("change user stocks.\n");
                                }
                                pur.quantity = snode->c.stocks;
                                bnode->c.stocks -= snode->c.stocks;
                                if(qlist_insert(&squeue,bnode->c)!=0)
                                {
                                        printf("Error. Selling queue node");
                                        printf(" wasn't added.\n");
                                }
                                /**********************************
                                 * Delete seller, if the buyer    *
                                 * purchased less than he wanted. *
                                 **********************************/
                                if(qlist_delete(&squeue,bnode->c.commandid)!=0)
                                {
                                        //steile minima sto client.
                                }
                        }
                        /************************************
                         * Writting stock purchase at file. *
                         ************************************/
                        writetofile(pur);
                        //enimerwse loipa request_procesing threads gia tin allagi
                }
        }
}

void filepurchase(purchase_t pur)
{
        FILE *fp;

        if ((fp = fopen("stock purchases.txt", "a+")) == NULL ){
            printf("File error.\n");
            return;
        }

        fprintf(fp,"%d ",pur.commandid1);
        fprintf(fp,"%d ",pur.commandid2);
        fprintf(fp,"%d ",pur.quantity);
        fprintf(fp,"%f ",pur.price);
        fprintf(fp,"%s ",asctime(pur.commandid1));
        fprintf(fp,"\n");

        fclose(fp);
}

void writetofile(purchase_t pur)
{
        fnode_t *node;
        int i;

        //wait(&kenesthesis)
        flist_insert(&purchases,pur);
        //if(kenesthesis.arithmos <= 0)
        {
                while((node = flist_delete(&purchases)) != NULL)
                {
                        pthread_mutex_lock(&purwritelock);
                        filepurchase(node->p);
                        pthread_mutex_unlock(&purwritelock);
                        free(node);
                }
                /********************************************************
                 * In case that more than one thread got at this point. *
                 ********************************************************/
                pthread_mutex_lock(&empty);
                //if(kenesthesis <= 0)
                {
                         for(i=0;i<10;i++)
                        {
                                //signal(&kenesthesis);
                        }
                }
                pthread_mutex_unlock(&empty);
        }
}

void initialization_function()
{
        clist_init(&companies);
        uslist_init(&users);
        splist_init(&prices);
        qlist_init(&bqueue);
        qlist_init(&squeue);
        flist_init(&purchases);
        flist_init(&ready_purchases);
        pthread_mutex_init(&empty);
        pthread_mutex_init(&purwritelock);
        client_counter = 0;
        current_users = 0;
        current_companies = 0;

}

void send_users(int csock)
{
        usnode_t *cur,tosend;
        int i;

        for(i=1;i<current_users;i++)
        {
                cur = uslist_return(&users,i);
                send(csock,cur->userid,sizeof(int),0);
                send(csock,cur->stocks,sizeof(int),0);
                send_companies(csock,cur->cl);
        }
}

void send_companies(int csock,clist_t l)
{
        cnode_t *cur;
        int i;

        for(i=1;i<current_companies;i++)
        {
                cur = clist_return(&l,i);
                send(csock,cur->companyid,sizeof(int),0);
                send(csock,cur->stocks,sizeof(int),0);
        }
}

command_t command_tokenizer(char string[])
{
        char *token;
        command_t c;

        token = strtok(string," ");
        if(token != NULL)
        {
                c.userid = atoi(token);
                token = strtok(NULL," ");
                if(token != NULL)
                {
                        c.companyid = atoi(token);
                        token = strtok(NULL," ");
                        if(token != NULL)
                        {
                                c.command_type = atoi(token);
                                token = strtok(NULL," ");
                                if(token != NULL)
                                {
                                        c.stocks = atof(token);
                                        token = strtok(NULL," ");
                                        if(token != NULL)
                                        {
                                                c.price =
                                                atof(token);
                                                token = strtok(NULL,
                                                               "\n");
                                                if(token != NULL)
                                                {
                                                        c.timestamp = timestamp_tokenizer(token);
                                                        if(c.timestamp.tm_sec == -1)
                                                        {
                                                                c.userid = 0;
                                                        }
                                                }
                                                else
                                                {
                                                        c.userid = 0;
                                                }
                                        }
                                        else
                                        {
                                                c.userid = 0;
                                        }
                                }
                                else
                                {
                                        c.userid = 0;
                                }
                        }
                        else
                        {
                                c.userid = 0;
                        }
                }
                else
                {
                        c.userid = 0;
                }
        }
        else
        {
                c.userid = 0;
        }
        return c.
}

void search_purchases(struct tm timestamp,int userid,int csock)
{
        FILE *fp;
        char line[100];
        purchase_t p;

        if((fp = fopen("stock purchases.txt","r")) == NULL)
        {
                printf("File Error.\n");
                return;
        }
        while(fgets(line,100,fp)!=NULL)
        {
                p = purchase_tokenizer(line);
                if(p.commandid1 == 0)
                {
                        printf("Command error.\n");
                        continue;
                }
                if(compare_timestamp(p.timestamp,timestamp)==0)
                {
                        if(p.command1 == userid || p.command2 == userid)
                        {
                                send(csock,p,sizeof(p),0);
                        }
                }
                else
                {
                        p.command1 = 0;
                        p.command2 = 0;
                        send(csock,p,sizeof(p),0);
                        break;
                }
        }

}

purchase_t purchase_tokenizer(char string[])
{
        char *token;
        purchase_t pur;

        token = strtok(string," ");
        if(token != NULL)
        {
                pur.commandid1 = atoi(token);
                token = strtok(NULL," ");
                if(token != NULL)
                {
                        pur.commandid2 = atoi(token);
                        token = strtok(NULL," ");
                        if(token != NULL)
                        {
                                pur.quantity = atoi(token);
                                token = strtok(NULL," ");
                                if(token != NULL)
                                {
                                        pur.price = atof(token);
                                        token = strtok(NULL," ");
                                        if(token != NULL)
                                        {
                                                pur.timestamp = timestamp_tokenizer(token);
                                                if(pur.timestamp.tm_sec == -1)
                                                {
                                                        pur.commandid1 = 0;
                                                }
                                        }
                                        else
                                        {
                                                pur.commandid1 = 0;
                                        }
                                }
                                else
                                {
                                        pur.commandid1 = 0;
                                }
                        }
                        else
                        {
                                pur.commandid1 = 0;
                        }
                }
                else
                {
                        pur.commandid1 = 0;
                }
        }
        else
        {
                pur.commandid1 = 0;
        }

        return pur;
}

struct tm timestamp_tokenizer(char timestamp[])
{
        char *token;
        struct tm timestamp;

        token = strtok(timestamp," ");
        if(token!=NULL)
        {
                timestamp.tm_sec = atoi(token);
                token = strtok(timestamp," ");
                if(token!=NULL)
                {
                        timestamp.tm_min = atoi(token);
                        token = strtok(timestamp," ");
                        if(token!=NULL)
                        {
                                timestamp.tm_hour = atoi(token);
                                token = strtok(timestamp," ");
                                if(token!=NULL)
                                {
                                        timestamp.tm_mday = atoi(token);
                                        token = strtok(timestamp," ");
                                        if(token!=NULL)
                                        {
                                                timestamp.tm_mon = atoi(token);
                                                token = strtok(timestamp," ");
                                                if(token!=NULL)
                                                {
                                                        timestamp.tm_year =
                                                        atoi(token);
                                                        token = strtok(timestamp,
                                                                        " ");
                                                        if(token!=NULL)
                                                        {
                                                                timestamp.tm_wday = atoi(token);
                                                                token = strtok(timestamp," ");
                                                                if(token!=NULL)
                                                                {
                                                                        timestamp.tm_yday = atoi(token);
                                                                        token = strtok(timestamp,"\n");
                                                                        if(token!=NULL)
                                                                        {
                                                                                timestamp.tm_isdst = atoi(token);
                                                                        }
                                                                        else
                                                                        {
                                                                                timestamp.tm_sec = -1;
                                                                        }
                                                                }
                                                                else
                                                                {
                                                                        timestamp.tm_sec = -1;
                                                                }
                                                        }
                                                        else
                                                        {
                                                                timestamp.tm_sec = -1;
                                                        }
                                                }
                                                else
                                                {
                                                        timestamp.tm_sec = -1;
                                                }
                                        }
                                        else
                                        {
                                                timestamp.tm_sec = -1;
                                        }
                                }
                                else
                                {
                                        timestamp.tm_sec = -1;
                                }
                        }
                        else
                        {
                                timestamp.tm_sec = -1;
                        }
                }
                else
                {
                        timestamp.tm_sec = -1;
                }
        }
        else
        {
                timestamp.tm_sec = -1;
        }
        return timestamp;
}

void read_companies()
{
        FILE *fp;
        char *token,line[100];
        int companyid,stocks;

        if((fp = fopen("companies.txt","r")) == NULL)
        {
                printf("Error opening companies.txt\n");
                return;
        }
        while(fgets(line,100,fp)!=NULL)
        {
                token = strtok(line," ");
                if(token!=NULL)
                {
                        companyid = atoi(token);
                        token = strtok(NULL,"\n");
                        if(token != NULL)
                        {
                                stocks = atoi(token);
                        }
                        else
                        {
                                companyid = 0;
                        }
                }
                else
                {
                        companyid = 0;
                }
                if(clist_insert(&sompanies,companyid,sotcks) != 0 )
                {
                        printf("Reading file error.\n");
                        return;
                }
        }
}

void read_users()
{
        FILE *fp;
        char *token,line[256];
        int userid,stocks,companyid;

        if((fp = fopen("users.txt","r")) == NULL)
        {
                printf("Error opening users.txt\n");
                return;
        }
        while(fgets(line,256,fp) != NULL)
        {
                token = strtok(line," ");
                if(token != NULL)
                {
                        userid = atoi(token);
                        token = strtok(NULL," ");
                        if(token != NULL)
                        {
                                stocks = atoi(token);
                                token = strtok(NULL," ");
                                if(token !=NULL)
                                {
                                        companyid = atoi(token);
                                }
                                else
                                {
                                        userid = 0;
                                }
                        }
                        else
                        {
                                userid =0;
                        }
                }
                else
                {
                        userid = 0;
                }

                if(userid != 0 )
                {
                        uslist_insert(&users,userid,stocks,NULL);
                        while((token = strtok(NULL," \n\0")) != NULL)
                        {

                                clist_insert(&users.cl,companyid,atoi(token));
                        }
                }
                else
                {
                        printf("File Error\n");
                        return;
                }
        }
}
