#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "Data_Structures.h"

void clist_init(clist_t *l)
{
        l->head = NULL;
        pthread_mutex_init(&l->lock,NULL);
}

int clist_insert(clist_t *l,int companyid,int stocks)
{
        cnode_t *new_node,*cur,*prev;

        if(companyid < 1 || stocks < 0)
        {
                return -1;
        }
        new_node = (cnode_t *) malloc(sizeof(cnode_t));
        if(!new_node) return -1;

        new_node->companyid = companyid;
        new_node->stocks = stocks;
        pthread_mutex_init(&new_node->lock,NULL);

        /***********************************
         * Locking the list for insertion. *
         ***********************************/
        pthread_mutex_lock(&l->lock);
        /*********************************************
         * Checks if there are no items in the list. *
         *********************************************/
        if(l->head == NULL)
        {
                l->head = new_node;
                new_node->next = NULL;
                pthread_mutex_unlock(&l->lock);
        }
        else
        {
                cur = l->head;
                pthread_mutex_lock(&cur->lock);
                /*****************************
                 * Case that the item will   *
                 * be added inside the list. *
                 *****************************/
                while(cur->next != NULL && companyid >= cur->companyid)
                {
                        if(cur == l->head)
                        {
                                pthread_mutex_unlock(&l->lock);
                        }
                        else
                        {
                                pthread_mutex_unlock(&prev->lock);

                        }
                        prev = cur;
                        cur = cur->next;
                        pthread_mutex_lock(&cur->lock);
                }
                /*********************************
                 * Case that the item will be    *
                 * added at the end of the list. *
                 *********************************/
                if(cur->next == NULL && companyid >= cur->companyid)
                {
                        cur->next = new_node;
                        new_node->next = NULL;
                }
                else
                {
                        if(cur == l->head)
                        {
                                new_node->next = l->head;
                                l->head = new_node;
                        }
                        else
                        {
                                prev->next = new_node;
                                new_node->next = cur;
                        }
                }
                /******************************************
                 * Checks if the new item will be second. *
                 ******************************************/
                if(cur == l->head)
                {
                        pthread_mutex_unlock(&l->lock);
                }
                else
                {
                        pthread_mutex_unlock(&prev->lock);
                }
                pthread_mutex_unlock(&cur->lock);
        }
        return 0;
}

cnode_t *clist_remove(clist_t *l,int companyid)
{
        cnode_t *prev, *cur;

        /********************************
         * Checks if the list is empty. *
         ********************************/
        if(l->head == NULL)
        {
                return NULL;
        }
        if(companyid < 1)
        {
                return NULL;
        }
        cur = l->head;
        /*****************************************
         * Locks the list, in case the node that *
         * is going to be deleted is the first.  *
         *****************************************/
        pthread_mutex_lock(&l->lock);
        while(cur != NULL)
        {
                pthread_mutex_lock(&cur->lock);
                if(cur->companyid == companyid)
                {
                        /************************************
                         * Checks if the node that is going *
                         * to be deleted is the first.      *
                         ************************************/
                        pthread_mutex_unlock(&cur->lock);
                        if(cur == l->head)
                        {
                                l->head = cur->next;
                                pthread_mutex_unlock(&l->lock);
                        }
                        else
                        {
                                prev->next = cur->next;
                                pthread_mutex_unlock(&prev->lock);
                        }
                        return cur;
                }
                /**************************************
                 * Checks if the current node that is *
                 * going to checked is not the first  *
                 **************************************/
                if(cur == l->head)
                {
                        pthread_mutex_unlock(&l->lock);
                }
                else
                {
                        pthread_mutex_unlock(&prev->lock);
                }
                prev = cur;
                cur = cur->next;
        }
        pthread_mutex_unlock(&prev->lock);

        return NULL;
}

cnode_t *clist_return(clist_t *l,int companyid)
{
        cnode_t *prev, *cur;

        /********************************
         * Checks if the list is empty. *
         ********************************/
        if(l->head == NULL)
        {
                return NULL;
        }
        if(companyid < 1)
        {
                return NULL;
        }
        cur = l->head;
        /*****************************************
         * Locks the list, in case the node that *
         * is going to be deleted is the first.  *
         *****************************************/
        pthread_mutex_lock(&l->lock);
        while(cur != NULL)
        {
                pthread_mutex_lock(&cur->lock);
                if(cur->companyid == companyid)
                {
                        /************************************
                         * Checks if the node that is going *
                         * to be deleted is the first.      *
                         ************************************/
                        pthread_mutex_unlock(&cur->lock);
                        if(cur == l->head)
                        {
                                pthread_mutex_unlock(&l->lock);
                        }
                        else
                        {
                                pthread_mutex_unlock(&prev->lock);
                        }
                        return cur;
                }
                /**************************************
                 * Checks if the current node that is *
                 * going to checked is not the first  *
                 **************************************/
                if(cur == l->head)
                {
                        pthread_mutex_unlock(&l->lock);
                }
                else
                {
                        pthread_mutex_unlock(&prev->lock);
                }
                prev = cur;
                cur = cur->next;
        }
        pthread_mutex_unlock(&prev->lock);

        return NULL;
}

int clist_change(clist_t *l,int companyid,int new_stocks)
{
        cnode_t *prev, *cur;

        /********************************
         * Checks if the list is empty. *
         ********************************/
        if(l->head == NULL || companyid < 1 || new_stocks < 0)
        {
                return -1;
        }
        cur = l->head;
        /*****************************************
         * Locks the list, in case the node that *
         * is going to be deleted is the first.  *
         *****************************************/
        pthread_mutex_lock(&l->lock);
        while(cur != NULL)
        {
                pthread_mutex_lock(&cur->lock);
                if(cur->companyid == companyid)
                {
                        cur->stocks += new_stocks;
                        pthread_mutex_unlock(&cur->lock);
                        if(cur == l->head)
                        {
                                pthread_mutex_unlock(&l->lock);
                        }
                        else
                        {
                                pthread_mutex_unlock(&prev->lock);
                        }
                        return 0;
                }
                /**************************************
                 * Checks if the current node that is *
                 * going to checked is not the first  *
                 **************************************/
                if(cur == l->head)
                {
                        pthread_mutex_unlock(&l->lock);
                }
                else
                {
                        pthread_mutex_unlock(&prev->lock);
                }
                prev = cur;
                cur = cur->next;
        }
        pthread_mutex_unlock(&prev->lock);

        return -1;
}

void print_clist(clist_t l)
{
        cnode_t *cur,*prev;

        if(l.head == NULL)
        {
                printf("There no stocks.\n");
                return;
        }
        cur = l.head;
        while(cur != NULL)
        {
                printf("%d ",cur->companyid);
                printf("%d\n",cur->stocks);
                prev = cur;
                cur = cur->next;
        }
}

void uslist_init(uslist_t *l)
{
        l->head = NULL;
        pthread_mutex_init(&l->lock,NULL);
}

int uslist_insert(uslist_t *l,int userid,int stocks,clist_t *cl)
{
        usnode_t *new_node,*cur,*prev;

        if(userid < 1 || stocks < 0)
        {
                return -1;
        }

        new_node = (usnode_t *) malloc(sizeof(usnode_t));
        if(!new_node) return -1;

        new_node->userid = userid;
        new_node->stocks = stocks;
        if(cl == NULL)
        {
                clist_init(&new_node->cl);
        }
        else
        {
                new_node->cl = *cl;
        }
        pthread_mutex_init(&new_node->lock,NULL);
        /***********************************
         * Locking the list for insertion. *
         ***********************************/
        pthread_mutex_lock(&l->lock);
        /*********************************************
         * Checks if there are no items in the list. *
         *********************************************/
        if(l->head == NULL)
        {
                l->head = new_node;
                new_node->next = NULL;
                pthread_mutex_unlock(&l->lock);
                return 0;
        }
        else
        {
                cur = l->head;
                pthread_mutex_lock(&cur->lock);
                /************************************
                 * Case that the item will be       *
                 * added in the middle of the list. *
                 ************************************/
                while(cur->next != NULL && userid >= cur->userid)
                {
                        if(cur == l->head)
                        {
                                pthread_mutex_unlock(&l->lock);
                        }
                        else
                        {
                                pthread_mutex_unlock(&prev->lock);
                        }
                        prev = cur;
                        cur = cur->next;
                        pthread_mutex_lock(&cur->lock);
                }
                /************************************************
                 * Case that the item will be added at the end. *
                 ************************************************/
                if(cur->next == NULL && userid >= cur->userid)
                {
                        cur->next = new_node;
                        new_node->next = NULL;
                }
                else
                {
                        if(cur == l->head)
                        {
                                new_node->next = l->head;
                                l->head = new_node;
                        }
                        else
                        {
                                prev->next = new_node;
                                new_node->next = cur;
                        }
                }
                if(cur == l->head)
                {
                        pthread_mutex_unlock(&l->lock);
                }
                else
                {
                        pthread_mutex_unlock(&prev->lock);
                }
                pthread_mutex_unlock(&cur->lock);
        }
        return 0;
}

usnode_t *uslist_remove(uslist_t *l,int userid)
{
        usnode_t *prev, *cur;

        if(userid < 1)
        {
                return 1;
        }

        if(l->head == NULL)
        {
                return NULL;
        }
        cur = l->head;
        /*****************************************
         * Locks the list, in case the node that *
         * is going to be searched is the first. *
         *****************************************/
        pthread_mutex_lock(&l->lock);
        while(cur != NULL)
        {
                pthread_mutex_lock(&cur->lock);
                if(cur->userid == userid)
                {
                        /************************************
                         * Checks if the node that is going *
                         * to be searched is the first.     *
                         ************************************/
                        pthread_mutex_unlock(&cur->lock);
                        if(cur == l->head)
                        {
                                l->head = cur->next;
                                pthread_mutex_unlock(&l->lock);
                        }
                        else
                        {
                                prev->next = cur->next;
                                pthread_mutex_unlock(&prev->lock);
                        }
                        return cur;

                }
                /**************************************
                 * Checks if the current node that is *
                 * going to checked is not the first  *
                 **************************************/
                if(cur == l->head)
                {
                        pthread_mutex_unlock(&l->lock);
                }
                else
                {
                        pthread_mutex_unlock(&prev->lock);
                }
                prev = cur;
                cur = cur->next;
        }
        pthread_mutex_unlock(&prev->lock);

        return NULL;
}

usnode_t *uslist_return(uslist_t *l,int userid)
{
        usnode_t *prev, *cur;

        if(l->head == NULL || userid < 1)
        {
                return NULL;
        }
        cur = l->head;
        /*****************************************
         * Locks the list, in case the node that *
         * is going to be searched is the first. *
         *****************************************/
        pthread_mutex_lock(&l->lock);
        while(cur != NULL)
        {
                pthread_mutex_lock(&cur->lock);
                if(cur->userid == userid)
                {
                        /************************************
                         * Checks if the node that is going *
                         * to be searched is the first.     *
                         ************************************/
                        pthread_mutex_unlock(&cur->lock);
                        if(cur == l->head)
                        {
                                pthread_mutex_unlock(&l->lock);
                        }
                        else
                        {
                                pthread_mutex_unlock(&prev->lock);
                        }
                        return cur;

                }
                /**************************************
                 * Checks if the current node that is *
                 * going to checked is not the first  *
                 **************************************/
                if(cur == l->head)
                {
                        pthread_mutex_unlock(&l->lock);
                }
                else
                {
                        pthread_mutex_unlock(&prev->lock);
                }
                prev = cur;
                cur = cur->next;
        }
        pthread_mutex_unlock(&prev->lock);

        return NULL;
}

int uslist_change(uslist_t *l,int userid,int companyid,int new_stocks)
{
        usnode_t *prev, *cur;

        if(l->head == NULL)
        {
                return -1;
        }
        cur = l->head;
        /*****************************************
         * Locks the list, in case the node that *
         * is going to be searched is the first. *
         *****************************************/
        pthread_mutex_lock(&l->lock);
        while(cur != NULL)
        {
                pthread_mutex_lock(&cur->lock);
                if(cur->userid == userid)
                {
                        if(clist_change(&cur->cl,companyid,new_stocks)== -1)
                        {
                                printf("Company doesn't exists.\n");
                                pthread_mutex_unlock(&cur->lock);
                                if(cur == l->head)
                                {
                                        pthread_mutex_unlock(&l->lock);
                                }
                                else
                                {
                                        pthread_mutex_unlock(&prev->lock);
                                }
                                return -1;
                        }
                        pthread_mutex_unlock(&cur->lock);
                        if(cur == l->head)
                        {
                                pthread_mutex_unlock(&l->lock);
                        }
                        else
                        {
                                pthread_mutex_unlock(&prev->lock);
                        }
                        return 0;
                }
                /**************************************
                 * Checks if the current node that is *
                 * going to checked is not the first  *
                 **************************************/
                if(cur == l->head)
                {
                        pthread_mutex_unlock(&l->lock);
                }
                else
                {
                        pthread_mutex_unlock(&prev->lock);
                }
                prev = cur;
                cur = cur->next;
        }
        pthread_mutex_unlock(&prev->lock);

        return -1;
}

void splist_init(splist_t *l)
{
        l->head = NULL;
        pthread_mutex_init(&l->lock,NULL);
}

int splist_insert(splist_t *l,float price,int companyid)
{
        spnode_t *new_node,*cur,*prev;

        if(price < 0.00 || companyid < 1)
        [
                return -1;
        ]

        new_node = (spnode_t *) malloc(sizeof(spnode_t));
        if(!new_node) return -1;

        new_node->price = price;
        new_node->companyid = companyid;
        pthread_mutex_init(&new_node->lock,NULL);

        pthread_mutex_lock(&l->lock);
        /*********************************************
         * Checks if there are no items in the list. *
         *********************************************/
        if(l->head == NULL)
        {
                l->head = new_node;
                new_node->next = NULL;
                pthread_mutex_unlock(&l->lock);
        }
        else
        {
                cur = l->head;
                pthread_mutex_lock(&cur->lock);
                /************************************
                 * Case that the item will be       *
                 * added in the middle of the list. *
                 ************************************/
                while(cur->next != NULL && companyid >= cur->companyid)
                {
                        if(cur == l->head)
                        {
                                pthread_mutex_unlock(&l->lock);
                        }
                        else
                        {
                                pthread_mutex_unlock(&prev->lock);
                        }
                        prev = cur;
                        cur = cur->next;
                        pthread_mutex_lock(&cur->lock);
                }
                /************************************************
                 * Case that the item will be added at the end. *
                 ************************************************/
                if(cur->next == NULL && companyid >= cur->companyid)
                {
                        cur->next = new_node;
                        new_node->next = NULL;
                }
                else
                {
                        if(cur == l->head)
                        {
                                new_node->next = l->head;
                                l->head = new_node;
                        }
                        else
                        {
                                prev->next = new_node;
                                new_node->next = cur;
                        }
                }
                if(cur == l->head)
                {
                        pthread_mutex_unlock(&l->lock);
                }
                else
                {
                        pthread_mutex_unlock(&prev->lock);
                }
                pthread_mutex_unlock(&cur->lock);
        }
        return 0;
}

spnode_t *splist_remove(splist_t *l,int companyid)
{
        spnode_t *prev, *cur;

        if(companyid < 1)
        {
                return -1;
        }

        /********************************
         * Checks if the list is empty. *
         ********************************/
        if(l->head == NULL)
        {
                return NULL;
        }
        cur = l->head;
        /*****************************************
         * Locks the list, in case the node that *
         * is going to be searched is the first. *
         *****************************************/
        pthread_mutex_lock(&l->lock);
        while(cur != NULL)
        {
                pthread_mutex_lock(&cur->lock);
                if(cur->companyid == companyid)
                {
                        /************************************
                         * Checks if the node that is going *
                         * to be searched is the first.     *
                         ************************************/
                        pthread_mutex_unlock(&cur->lock);
                        if(cur == l->head)
                        {
                                l->head = cur->next;
                                pthread_mutex_unlock(&l->lock);
                        }
                        else
                        {
                                prev->next = cur->next;
                                pthread_mutex_unlock(&prev->lock);
                        }
                        return cur;
                }
                /**************************************
                 * Checks if the current node that is *
                 * going to checked is not the first  *
                 **************************************/
                if(cur == l->head)
                {
                        pthread_mutex_unlock(&l->lock);
                }
                else
                {
                        pthread_mutex_unlock(&prev->lock);
                }
                prev = cur;
                cur = cur->next;
        }
        pthread_mutex_unlock(&prev->lock);
        return NULL;
}

int splist_change(splist_t *l,int companyid,float new_price)
{
        spnode_t *prev, *cur;

        if(companyid < 1 || new_price < 0.00)
        {
                return -1;
        }

        /********************************
         * Checks if the list is empty. *
         ********************************/
        if(l->head == NULL)
        {
                return -1;
        }
        cur = l->head;
        /*****************************************
         * Locks the list, in case the node that *
         * is going to be searched is the first. *
         *****************************************/
        pthread_mutex_lock(&l->lock);
        while(cur != NULL)
        {
                pthread_mutex_lock(&cur->lock);
                if(cur->companyid == companyid)
                {
                        cur->price = new_price;
                        pthread_mutex_unlock(&cur->lock);
                        if(cur == l->head)
                        {
                                pthread_mutex_unlock(&l->lock);
                        }
                        else
                        {
                                pthread_mutex_unlock(&prev->lock);
                        }
                        return 0;
                }
                /**************************************
                 * Checks if the current node that is *
                 * going to checked is not the first  *
                 **************************************/
                if(cur == l->head)
                {
                        pthread_mutex_unlock(&l->lock);
                }
                else
                {
                        pthread_mutex_unlock(&prev->lock);
                }
                prev = cur;
                cur = cur->next;
        }
        pthread_mutex_unlock(&prev->lock);
        return -1;
}

void qlist_init(qlist_t *q)
{
        q->head = NULL;
        pthread_mutex_init(&q->lock,NULL);
}

int qlist_insert(qlist_t *q,command_t c)
{
        qnode_t *new_node,*cur,*prev;

        new_node = (qnode_t *) malloc(sizeof(qnode_t));
        if(new_node == NULL) return -1;

        new_node->c.commandid = c.commandid;
        new_node->c.userid = c.userid;
        new_node->c.companyid = c.companyid;
        new_node->c.stocks = c.stocks;
        new_node->c.command_type = c.command_type;
        new_node->c.price = c.price;
        new_node->c.timestamp = c.timestamp;
        new_node->used = 0;
        new_node->can_be_deleted = 1;
        pthread_mutex_init(&new_node->lock,NULL);

        pthread_mutex_lock(&q->lock);
        /*********************************************
         * Checks if there are no items in the list. *
         *********************************************/
        if(q->head == NULL)
        {
                q->head = new_node;
                new_node->next = NULL;
                pthread_mutex_unlock(&q->lock);
        }
        else
        {
                cur = q->head;
                pthread_mutex_lock(&cur->lock);
                while(cur->next != NULL && c.commandid >= cur->commandid)
                {
                        /****************************************
                         * Checks if the cur is the first node. *
                         ****************************************/
                        if(cur == q->head)
                        {
                                pthread_mutex_unlock(&q->lock);
                        }
                        else
                        {
                                pthread_mutex_unlock(&prev->lock);

                        }
                        prev = cur;
                        cur = cur->next;
                        pthread_mutex_lock(&cur->lock);
                }
                if(cur->next == NULL && c.commandid >= cur->commandif)
                {
                        cur->next = new_node;
                        new_node->next = NULL;
                }
                else
                {
                        if(cur == l->head)
                        {
                                new_node->next = l->head;
                                l->head = new_node;
                        }
                        else
                        {
                                new_node->next = cur;
                                prev->next = new_node;
                        }
                }
                pthread_mutex_unlock(&cur->lock);
                if(cur == q->head)
                {
                        pthread_mutex_unlock(&q->lock);
                }
                else
                {
                        pthread_mutex_unlock(&prev->lock);
                }
        }
        return 0;
}

qnode_t *qlist_get_node(qlist_t *q)
{
        qnode_t *cur,*prev;

        /********************************
         * Checks if the list is empty. *
         ********************************/
        if(q->head == NULL)
        {
                return NULL;
        }

        /*****************************************
         * Locks the list, in case the node that *
         * is going to be searched is the first. *
         *****************************************/
        pthread_mutex_lock(&q->lock);
        while(cur != NULL)
        {
                pthread_mutex_lock(&cur->lock);
                /**************************************
                 * Checks if the node is not used and *
                 * the node has not found a match.    *
                 **************************************/
                if(cur->used == 0 && cur->can_be_deleted == 1)
                {
                        cur->used = 1;
                        /************************************
                         * Checks if the node that is going *
                         * to be searched is the first.     *
                         ************************************/
                        pthread_mutex_unlock(&cur->lock);
                        if(cur == q->head)
                        {
                                pthread_mutex_unlock(&q->lock);
                        }
                        else
                        {
                                pthread_mutex_unlock(&prev->lock);
                        }
                        return cur;
                }
                /**************************************
                 * Checks if the current node that is *
                 * going to checked is not the first  *
                 **************************************/
                if(cur == q->head)
                {
                        pthread_mutex_unlock(&q->lock);
                }
                else
                {
                        pthread_mutex_unlock(&prev->lock);
                }
                prev = cur;
                cur = cur->next;
        }
        pthread_mutex_unlock(&prev->lock);
        return NULL;
}

qnode_t *qlist_search_match(qlist_t *q,qnode_t *c)
{
        qnode_t *prev, *cur;

        /********************************
         * Checks if the list is empty. *
         ********************************/
        if(q->head == NULL)
        {
                return NULL;
        }
        cur = q->head;
        /*****************************************
         * Locks the list, in case the node that *
         * is going to be searched is the first. *
         *****************************************/
        pthread_mutex_lock(&q->lock);
        while(cur != NULL)
        {
                pthread_mutex_lock(&cur->lock);

                /****************************
                 * Checks if the node is in *
                 * the list or is deleted.  *
                 ****************************/
                if(pthread_mutex_lock(&c->lock) == 0)
                {
                        if(command_check(c->c,cur->c) == 0)
                        {
                                /*******************************************
                                 * Ensures that the node will not be       *
                                 * deleted, while the  match is performed. *
                                 *******************************************/
                                c->can_be_deleted = 0;
                                pthread_mutex_unlock(&c->lock);

                                /***********************************
                                 * Checks if the node that is      *
                                 * found for a match is the first. *
                                 ***********************************/
                                pthread_mutex_unlock(&cur->lock);

                                /************************************
                                 * Gets the timestamp of the match. *
                                 ************************************/
                                if(cur == q->head)
                                {
                                        q->head = cur->next;
                                        pthread_mutex_unlock(&q->lock);
                                }
                                else
                                {
                                        prev->next = cur->next;
                                        pthread_mutex_unlock(&prev->lock);
                                }
                                return cur;
                        }
                }
                else
                {
                        printf("Lock deleted.\n");
                        return NULL;
                }
                if(pthread_mutex_unlock(&c->lock) != 0)
                {
                        printf("Lock deleted.\n");
                        return NULL;
                }
                /**************************************
                 * Checks if the current node that is *
                 * going to checked is not the first  *
                 **************************************/
                if(cur == q->head)
                {
                        pthread_mutex_unlock(&q->lock);
                }
                else
                {
                        pthread_mutex_unlock(&prev->lock);
                }
                prev = cur;
                cur = cur->next;
        }
        pthread_mutex_unlock(&prev->lock);
        return NULL;

}

int qlist_delete(qlist_t *q,int commandid)
{
        qnode_t *cur,*prev;

        if(commandid < 1)
        {
                return -1;
        }

        /********************************
         * Checks if the list is empty. *
         ********************************/
        if(q->head == NULL)
        {
                printf("Error. Empty list.\n");
                return -1;
        }
        else
        {
                /*****************************************
                 * Locks the list, in case the node that *
                 * is going to be searched is the first. *
                 *****************************************/
                pthread_mutex_lock(&q->lock);
                cur = l->head;
                pthread_mutex_lock(&cur->lock);
                while(cur->c.commandid != commandid)
                {
                        /*************************************
                         * Checks if the current node that   *
                         * is going to checked is the first. *
                         *************************************/
                        if(cur == q->head)
                        {
                                pthread_mutex_unlock(&q->lock);
                        }
                        else
                        {
                                pthread_mutex_unlock(&prev->lock);
                        }
                        prev = cur;
                        cur = cur->next;
                        pthread_mutex_lock(&cur->lock);
                }
                /************************************
                 * Checks if the node wasn't found. *
                 ************************************/
                if(cur == NULL)
                {
                        if(cur = q->head)
                        {
                                pthread_mutex_unlock(&q->lock);
                        }
                        else
                        {
                                pthread_mutex_unlock(&prev->lock);
                        }
                        printf("Error. Node doesn't exist.\n");
                        return -1;
                }
                else
                {
                        /*********************************************
                         * Checks if the deletion is performed while *
                         * the node is used for searching a match.   *
                         *********************************************/
                        if(cur->can_be_deleted = 0)
                        {
                                /********************************
                                 * Checks if the node has found *
                                 * a match and will be deleted. *
                                 ********************************/
                                if(cur->used = 0)
                                {
                                        /************************************
                                         * Checks if the node that is going *
                                         * to be deleted is the first.      *
                                         ************************************/
                                        if(cur == q->head)
                                        {
                                                q->head = cur->next;
                                                pthread_mutex_unlock(&q->lock);
                                        }
                                        else
                                        {
                                                prev->next = cur->next;
                                                pthread_mutex_unlock(&
                                                                    prev->lock);
                                        }
                                        free(cur);
                                        return 0;
                                }
                                else
                                {
                                        printf("Error. Node found match.\n");
                                        return 2;
                                }
                        }
                        else
                        {
                                /************************************
                                 * Checks if the node that is going *
                                 * to be deleted is the first.      *
                                 *************************************/
                                if(cur == q->head)
                                {
                                        q->head = cur->next;
                                        pthread_mutex_unlock(&q->lock);
                                }
                                else
                                {
                                        prev->next = cur->next;
                                        pthread_mutex_unlock(&prev->lock);
                                }
                                free(cur);
                                return 1;
                        }
                }
        }
}

int command_check(command_t c1,command_t c2)
{
        if(c1.companyid == c2.companyid
           && c1.price >= c2.price)
        {
                return 0;
        }

        return -1;
}

void flist_init(flist_t *l)
{
        l->head = NULL;
        pthread_mutex_init(&l->lock,NULL);
}

int flist_insert(flist_t *l,purchase_t p)
{
        flist_t *new_node,*cur,*prev;

        new_node = (flist_t *) malloc(sizeof(flist_t));

        if(new_node==NULL) return -1;

        pthread_mutex_init(&new_node->lock,NULL);
        new_node->p.commandid1 = p.commandid1;
        new_node->p.commandid2 = p.commandid2;
        new_node->p.quantity = p.quantity;
        new_node->p.price = p.price;
        new_node->p.timestamp = p.timestamp;

        /***********************************************
         * Checks if there are no entries on the list. *
         ***********************************************/
        pthread_mutex_lock(&l->lock);
        if(l->head == NULL)
        {
                l->head = new_node;
                new_node->next = NULL;
                pthread_mutex_unlock(&l->lock);
        }
        else
        {
                cur = l->head;
                pthread_mutex_lock(&cur->lock);
                while((compare_timestamp(cur->p.timestamp,p.timestamp) == 0)
                        && (cur->next != NULL))
                {
                        if(cur == l->head)
                        {
                                pthread_mutex_unlock(&l->lock);
                        }
                        else
                        {
                                pthread_mutex_unlock(&prev->lock);
                        }
                        prev = cur;
                        cur = cur->next;
                        pthread_mutex_lock(&cur->lock);
                }
                /******************************************
                 * Checks if the node will be added last. *
                 ******************************************/
                if(cur->next == NULL && (compare_timestamp(cur->p.timestamp,
                                                           p.timestamp) == 0))
                {
                        cur->next = new_node;
                        new_node->next = NULL;
                }
                else
                {
                        /*******************************************
                         * Checks if the node will be added first. *
                         *******************************************/
                        if(cur == l->head)
                        {
                                l->head = new_node;
                                new_node->next = cur;
                        }
                        else
                        {
                                prev->next = new_node;
                                new_node->next = cur;
                        }
                }
                if(cur == l->head)
                {
                        pthread_mutex_unlock(&l->lock);
                }
                else
                {
                        pthread_mutex_unlock(&prev->lock);
                }
                pthread_mutex_unlock(&cur->lock);

        }

        return 0;
}

fnode_t *flist_remove(flist_t *l,int commandid)
{
        fnode_t *prev, *cur;

        if(commandid < 1)
        {
                return -1;
        }

        /********************************
         * Checks if the list is empty. *
         ********************************/
        if(l->head == NULL)
        {
                return NULL;
        }
        cur = l->head;
        /*****************************************
         * Locks the list, in case the node that *
         * is going to be searched is the first. *
         *****************************************/
        pthread_mutex_lock(&l->lock);
        while(cur != NULL)
        {
                pthread_mutex_lock(&cur->lock);
                if(cur->p.commandid1 == companyid || cur->p.command2)
                {
                        /************************************
                         * Checks if the node that is going *
                         * to be searched is the first.     *
                         ************************************/
                        pthread_mutex_unlock(&cur->lock);
                        if(cur == l->head)
                        {
                                l->head = cur->next;
                                pthread_mutex_unlock(&l->lock);
                        }
                        else
                        {
                                prev->next = cur->next;
                                pthread_mutex_unlock(&prev->lock);
                        }
                        return cur;
                }
                /**************************************
                 * Checks if the current node that is *
                 * going to checked is not the first  *
                 **************************************/
                if(cur == l->head)
                {
                        pthread_mutex_unlock(&l->lock);
                }
                else
                {
                        pthread_mutex_unlock(&prev->lock);
                }
                prev = cur;
                cur = cur->next;
        }
        pthread_mutex_unlock(&prev->lock);
        return NULL;
}

fnode_t *flist_delete(flist_t *l)
{
        fnode_t *cur;

        if(l->head == NULL)
        {
                return NULL;
        }

        pthread_mutex_lock(&l->lock);
        cur = l->head;
        pthread_mutex_lock(&cur->lock);
        l->head = cur->next;
        pthread_mutex_unlock(&l->lock);
        pthread_mutex_unlock(&cur->lock);

        return cur;
}

int compare_timestamp(struct tm t1,struct tm t2)
{
        if(t1.tm_year <= t2.tm_year)
        {
                if(t1.tm_mon <= t2.tm_mon)
                {
                        if(t1.tm_mday <= t2.tm_mday)
                        {
                                if(t1.tm_hour <= t2.tm_hour)
                                {
                                        if(t1.tm_min <= t2.tm_min)
                                        {
                                                if(t1.tm_sec <= t2.tm_sec)
                                                {
                                                        return 0;
                                                }
                                        }
                                }
                        }
                }
        }

        return -1;
}
