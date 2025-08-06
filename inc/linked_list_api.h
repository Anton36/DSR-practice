#ifndef LINKED_LIST_API_H
#define LINKED_LIST_API_H

#include "linked_list_inc.h"




typedef struct dns_packet_t  {
    unsigned char * packet;
    int packet_length;
    struct dns_packet_t * next;
}dns_packet_t;

typedef struct iterator_t 
{
    dns_packet_t * packet;//pointer to the current packet in the linked list
    dns_packet_t * head; //pointer to the beginning of the list, for cyclic iteration
} iterator_t;



void deleteList(void);
void list_push(unsigned char *data, int length);
dns_packet_t* get_dns_packet_list(void);

iterator_t * create_iterator(dns_packet_t *list);
void delete_iterator(iterator_t * iter);
dns_packet_t * get_dns_packet_from_iterator(iterator_t * iter);


#endif