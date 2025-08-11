#ifndef LINKED_LIST_API_H
#define LINKED_LIST_API_H

#include "linked_list_inc.h"




typedef struct dns_packet_t  {
    unsigned char packet[MAX_UDP_MESSAGE_SIZE];
    int packet_length;
    struct dns_packet_t * next;
}dns_packet_t;

typedef struct iterator_t 
{
    dns_packet_t * packet;//pointer to the current packet in the linked list
    dns_packet_t * head; //pointer to the beginning of the list, for cyclic iteration
} iterator_t;


void list_pool_init(void);   
void list_init(void);       



bool list_push( unsigned char *packet, int length);

dns_packet_t *get_dns_packet_list(void);
void deleteList(void);       
void create_iterator(iterator_t *iterator, dns_packet_t *list);
dns_packet_t *iterator_next(iterator_t *it);

#endif