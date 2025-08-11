#include "linked_list_api.h"
#include "linked_list_inc.h"
#include "main_inc.h"
#include "dns_module_api.h"
// list variables are declared statically in this file for encapsulation
static dns_packet_t pool[MAX_LIST_SIZE]; // Static array of nodes
static dns_packet_t *head = NULL;        // Head of the free-list
static dns_packet_t *start_head = NULL;  // Head of the active static list

void list_init(void)
{
    for (int i = 0; i < MAX_LIST_SIZE - 1; ++i)
    {
        pool[i].next = &pool[i + 1]; // Link each pool node to the next one
        pool[i].packet_length = 0;
    }
    pool[MAX_LIST_SIZE - 1].next = NULL; // the last pool int the list ending the list
    pool[MAX_LIST_SIZE - 1].packet_length = 0;
    head = &pool[0]; // give free list link to the first pool element
}

dns_packet_t *alloc_node(void)
{
    dns_packet_t *node = head; // Take the first free node from the free-list
    if (!node)
    {
        return NULL;
    }
    head = node->next; // advance the list
    node->next = NULL; // detach node from list
    node->packet_length = 0;
    return node;
}

bool list_push(unsigned char *packet, int length)
{
    dns_packet_t *node = alloc_node(); // allocating node from pool
    if (node == NULL)
    {
        fprintf(stderr, "pool is full");
        return false;
    }
    memcpy(node->packet, packet, length); // copy dns packet to node
    node->packet_length = length;         // copy packet length
    node->next = start_head;              // insert node at the begining of list
    start_head = node;                    // node become first in list
    return true;
}

void deleteList(void)
{
    dns_packet_t *node = start_head;
    while (node != NULL)
    {
        // puts node back into poll
        dns_packet_t *next = node->next;
        node->next = head;
        head = node;
        node = next;
    }
    start_head = NULL;
}

dns_packet_t *get_dns_packet_list(void) // function to get a pointer to a list, will be used in the iterator
{
    return start_head;
}

void create_iterator(iterator_t *iterator, dns_packet_t *list)
{
    iterator->head = list;
    iterator->packet = list;
}

dns_packet_t *iterator_next(iterator_t *iterator)
{
    if (!iterator->packet) // if current = NULL
    {
        iterator->packet = iterator->head; // start from beging (cycle iteration)
    }
    dns_packet_t *packet = iterator->packet; // take node
    if (iterator->packet != NULL)
    {
        iterator->packet = iterator->packet->next; // advance to the next node
    }

    return packet;
}
