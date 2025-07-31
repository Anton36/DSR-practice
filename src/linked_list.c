#include "linked_list_api.h"
#include "linked_list_inc.h"
#include "main_inc.h"
#include "dns_module_api.h"
//list variables are declared statically in this file for encapsulation
static dns_packet_t *dns_packet_query = NULL; // list with dns packets generated from file
static dns_packet_t *start_of_list = NULL;    // starting point for iterators

void list_push(unsigned char *data, int length)
{
    dns_packet_t *temp =(dns_packet_t *)malloc(sizeof(dns_packet_t)); //creating and allocating memory for a new node
    temp->packet=(unsigned char*)malloc(length); //allocating space for a package
    memcpy(temp->packet,data,length); //transferring packet data to node memory
    temp->packet_length = length;
    temp->next = dns_packet_query;
    dns_packet_query=temp;

}

void deleteList(void) //function to clear the list of DNS requests
{
    dns_packet_t *temp = NULL;
    dns_packet_t *current = dns_packet_query;
    while(current != NULL)
    {
        temp =current->next;
        free(current->packet);
        free(current);
        current = temp;
    }
    dns_packet_query = NULL;

}

dns_packet_t* get_dns_packet_list(void) //function to get a pointer to a list, will be used in the iterator
{
    return dns_packet_query;
}


