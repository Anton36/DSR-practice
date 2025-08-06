#include "linked_list_api.h"
#include "linked_list_inc.h"
#include "main_inc.h"
#include "dns_module_api.h"
// list variables are declared statically in this file for encapsulation
static dns_packet_t *dns_packet_query = NULL; // list with dns packets generated from file
static dns_packet_t *start_of_list = NULL;    // starting point for iterators

void list_push(unsigned char *data, int length)
{
    dns_packet_t *temp = malloc(sizeof(dns_packet_t)); // creating and allocating memory for a new node
    temp->packet = malloc(length);                     // allocating space for a package
    memcpy(temp->packet, data, length);                // transferring packet data to node memory
    temp->packet_length = length;
    temp->next = dns_packet_query;
    dns_packet_query = temp;
}

void deleteList(void) // function to clear the list of DNS requests
{
    dns_packet_t *temp = NULL;
    dns_packet_t *current = dns_packet_query;
    while (current != NULL)
    {
        temp = current->next;
        free(current->packet);
        free(current);
        current = temp;
    }
    dns_packet_query = NULL;
}

dns_packet_t *get_dns_packet_list(void) // function to get a pointer to a list, will be used in the iterator
{
    return dns_packet_query;
}

iterator_t *create_iterator(dns_packet_t *list) // creating an iterator entity
{
    iterator_t *iter = malloc(sizeof(iterator_t));
    if (iter == NULL)
    {
        return NULL;
    }
    iter->packet = list; // initialization of the iterator's starting position
    iter->head = list;
    return iter;
}
void delete_iterator(iterator_t *iter) // deleting iterator entity
{
    if (iter != NULL)
    {
        free(iter);
    }
}
dns_packet_t *get_dns_packet_from_iterator(iterator_t *iter)
{
    if (iter->packet == NULL)
    {
        iter->packet = iter->head; // if the iterator has gone through the entire list, then we return to the beginning of the list
        return iter->packet;
    }
    if (iter->packet)
    {
        dns_packet_t *current_packet = iter->packet;
        iter->packet = iter->packet->next; // advance iterator
        return current_packet;
    }
    return NULL;
}
