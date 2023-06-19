#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H
#include <pthread.h>

/*******************
 * Data Structures *
 *******************/

/**********************************************
 * cnode_t: a thread-safe node in a list of   *
 *          companies. It contains companyids *
 *          and the stocks of each company.   *
 **********************************************/
typedef struct company_node{
        int companyid,stocks;
        pthread_mutex_t lock;
        struct company_node *next;
}cnode_t;

/*********************************************
 * clist_t: a thread-safe list of companies. *
 *********************************************/
typedef struct company_list{
        cnode_t *head;
        pthread_mutex_t lock;
}clist_t;

/************************************************************************
 * usnode_t: a thread-safe node of a user of the system. The fields are *
 *          his's id and the sum of his stocks and a list of companies  *
 *          and the number of stocks that owns from each company.       *
 ************************************************************************/
typedef struct users_node{
        int userid,stocks;
        clist_t cl;
        pthread_mutex_t lock;
        struct users_node *next;
}usnode_t;

/***************************************************************
 * uslist_t: a thread-safe type of a list that contains users. *
 ***************************************************************/
typedef struct users_list{
        usnode_t *head;
        pthread_mutex_t lock;
}uslist_t;

/***********************************************
 * spnode_t: a thread-safe node type of a list *
 *           with the prices of the stocks.    *
 ***********************************************/
typedef struct stock_prices_node{
        float price;
        int companyid;
        pthread_mutex_t lock;
        struct stock_prices_node *next;
}spnode_t;

/***************************************************************
 * splist_t: a thread-safe list with the prices of the stocks. *
 ***************************************************************/
typedef struct stock_prices_list{
        spnode_t *head;
        pthread_mutex_t lock;
}splist_t;

/*********************************************************************
 * command_t: A type of a command, given by a user. The fields are   *
 *           it's id, user's id, company's id, the number of stocks, *
 *           the type of command, the pice and the timestamp.        *
 *********************************************************************/
typedef struct command{
        int commandid,userid, companyid, stocks, command_type;
        float price;
        struct tm timestamp;
}command_t;

/******************************************************************
 * qnode_t: a node type for the buyibg and selling queue. The     *
 *          fields are the command, a boolean variable to show if *
 *          the node is currently used for searching a match and  *
 *          a boolean variable that shows if it can be deleted.   *
 ******************************************************************/
typedef struct queue_node{
        command_t c;
        int used, can_be_deleted;
        pthread_mutex_t lock;
        struct queue_node *next;
}qnode_t;

/***************************************************
 * qlist_t: a queue list that contains the         *
 *         commmands either for buying or selling. *
 ***************************************************/
typedef struct queue_list{
        qnode_t *head;
        pthread_mutex_t lock;
}qlist_t;

/********************************************************************
 * purchase: the purchase that will be added to a file. Fields      *
 *           are the commandids of the buying and selling commands, *
 *           the quantity of stocks purchased, the price of the     *
 *           purchased stocks and the timestamp of the purchase.    *
 ********************************************************************/
typedef struct purchase{
        int commandid1,commandid2,quantity;
        float price;
        struct tm timestamp;
}purchase_t;

/*******************************************************
 * fnode_t: a node at thread-safe list with purchases. *
 *******************************************************/
typedef struct file_node{
        purchase_t p;
        pthread_mutex_t lock;
        struct file_node *next;
}fnode_t;

/*******************************************************************
 * flist_t: a thread-safe list with purchses, sorted by timestamp. *
 *******************************************************************/
typedef struct file_queue{
        fnode_t *head;
        pthread_mutex_t lock;
}flist_t;

/*************
 * Functions *
 *************/

/************************************
 * clist_init: initializes a clist. *
 ************************************/
void clist_init(clist_t *l);

/*************************************************
 * clist_insert: inserts a new item in the       *
 *               clist. The insertion is sorted. *
 *               Returns -1 if it fails.         *
 *************************************************/
int clist_insert(clist_t *l,int companyid, int stocks);

/**************************************************************
 * clist_remove: Removes a node from  a clist. Returns the    *
 *               address of the node that was requested or    *
 *               NULL if it doesn't exists or if list's empty.*
 **************************************************************/
cnode_t *clist_remove(clist_t *l,int companyid);

/***************************************************
 * clist_return: returns the company node, without *
 *                 removing it from the list.      *
 ***************************************************/
cnode_t *clist_return(clist_t *l,int companyid);

/***********************************************************
 * clist_change: changes the number of stocks of a company *
 *               that a user owns. Returs -1 if the lists  *
 *               is empty or if the node doesn't exist.    *
 ***********************************************************/
int clist_change(clist_t *l,int companyid,int new_stocks);

/********************************
 * print_clist: prints a clist. *
 ********************************/
void print_clist(clist_t l);

/**************************************
 * uslist_init: initializes a uslist. *
 **************************************/
void uslist_init(uslist_t *l);

/****************************************************
 * uslist_insert: inserts a new user in the uslist. *
 *                The insertion is sorted. Returns  *
 *                -1 if it fails.                   *
 ****************************************************/
int uslist_insert(uslist_t *l,int userid,int stocks, clist_t *cl);

/***************************************************************
 * uslist_remove: Removes a node from  a uslist. Returns the   *
 *               address of the node that was requested or     *
 *               NULL if it doesn't exists or if list's empty. *
 ***************************************************************/
usnode_t *uslist_remove(uslist_t *l,int userid);

/*************************************************
 * uslist_return: returns the user node, without *
 *                removing it from the list.     *
 *************************************************/
usnode_t *uslist_return(uslist_t *l,int userid);

/************************************************************
 * uslist_change: changes the number of stocks of a company *
 *                that a user owns. Returs -1 if the list   *
 *                is empty or if the node doesn't exist.    *
 ************************************************************/
int uslist_change(uslist_t *l,int userid,int companyid,int new_stocks);

/**************************************
 * splist_init: initializes a splist. *
 **************************************/
void splist_init(splist_t *l);

/***************************************************
 * splist_insert: insert a new company in the      *
 *                splist. The insertion is sorted. *
 *                Returns -1 if it fails.          *
 ***************************************************/
int splist_insert(splist_t *l,float price,int companyid);

/***************************************************************
 * splist_remove: Removes a node from  a splist. Returns the   *
 *               address of the node that was requested or     *
 *               NULL if it doesn't exists or if list's empty. *
 ***************************************************************/
spnode_t *splist_remove(splist_t *l,int companyid);

/****************************************************************
 * splist_change: changes the price of a stock .Returns -1, if  *
 *                the list is empty or the node doesn't exists. *
 ****************************************************************/
int splist_change(splist_t *l,int companyid,float new_price);

/************************************
 * qlist_init: initializes a qlist. *
 ************************************/
void qlist_init(qlist_t *q);

/***************************************************
 * qlist_insert: inserts a new node at the end of  *
 *               a qlist. Arguments are a list and *
 *               the info of the fields of a node. *
 ***************************************************/
int qlist_insert(qlist_t *q,command_t c);

/****************************************************
 * qlist_get_node: Returns the address of the first *
 *               node that is not used for a match. *
 ****************************************************/
qnode_t *qlist_get_node(qlist_t *q);

/****************************************************************
 * qlist_search_match: Checks one list for match. The arguments *
 *                     are the list and one node for matching.  *
 ****************************************************************/
qnode_t *qlist_search_match(qlist_t *q,qnode_t *c);

/**************************************************************************
 * qlist_delete: Deletes a node from the list. Checks if the node is used *
 *               in a match or if a match was found and the node will be  *
 *               deleted. Returns -1 if the deletion fails. Returns 0 if  *
 *               the deletion was made before the match, else returns 1.  *
 **************************************************************************/
int qlist_delete(qlist_t *q,int commandid);

/***********************************************************************
 * command_check: checks if a command for buying a stock and a         *
 *                command for selling a stock are for the same         *
 *                stock, the number of stocks sold are more than       *
 *                the purchased and the price that buyer purchase      *
 *                them is bigger than the price the seller sells them. *
 ***********************************************************************/
int command_check(command_t c1,command_t c2);

/*************************************************
 * flist_init: initializes a list with purchases *
 *             that will be added to a file.     *
 *************************************************/
void flist_init(flist_t *l);

/*************************************************
 * flist_insert: Sorted insertion of a purchase. *
 *************************************************/
int flist_insert(flist_t *l,purchase_t p);

/**********************************************************
 * flist_remove: removes a node from an list. Arguments   *
 *               are the list and command id of the node. *
 **********************************************************/
fnode_t *flist_remove(flist_t *l,int commandid);

/*********************************************
 * flist_delete: Deletion of the first node. *
 *********************************************/
fnode_t *flist_delete(flist_t *l);

/********************************************************************
 * compare_timestamp: Comparison of two timestamps, by year, month, *
 *                    day of the month, hour, minute and second.    *
 ********************************************************************/
int compare_timestamp(struct tm t1,struct tm t2);

#endif
