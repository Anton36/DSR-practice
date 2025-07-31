#ifndef LINKED_LIST_API_H
#define LINKED_LIST_API_H

#include "linked_list_inc.h"




typedef struct dns_packet_t  {
    unsigned char * packet;
    int packet_length;
    struct dns_packet_t * next;
}dns_packet_t;



void deleteList(void);
void list_push(unsigned char *data, int length);
dns_packet_t* get_dns_packet_list(void);
void print_all_dns_packets(void);

#endif