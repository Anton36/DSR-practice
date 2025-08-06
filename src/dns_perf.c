
#include "dns_perf_inc.h"
#include "linked_list_api.h"

void *thread_body(void *arg)
{
    struct thread_arg *args = (struct thread_arg *)arg;
    if (args == NULL)
    {
        fprintf(stderr, "Error: thread arguments are NULL\n");
        return NULL;
    }

    printf("Thread started with IP: %s, Port: %d, P: %d, Working time: %d seconds\n", args->ip, args->port, args->P, args->working_time);

    int sockfd;
    unsigned char buffer[MAX_UDP_MESSAGE_SIZE];
    struct sockaddr_in servaddr, cliaddr;
    int recv_len;
    socklen_t client_address_len = sizeof(cliaddr);
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        fprintf(stderr, "socket creation failed");
        exit(EXIT_FAILURE);
    }
    dns_packet_t *dns_packet_query = get_dns_packet_list(); // getting the list of DNS packets
    iterator_t *iter = create_iterator(dns_packet_query);   // creating an iterator for the list
    if (iter == NULL)
    {
        fprintf(stderr, "Error: iterator could not be created\n");
        close(sockfd);
        return NULL;
    };
    struct thread_statistics *statistic = malloc(sizeof(struct thread_statistics));
    if (statistic == NULL)
    {
        fprintf(stderr, "Error: memory allocation for statistics failed\n");
        delete_iterator(iter);
        close(sockfd);
        return NULL;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(args->ip);
    servaddr.sin_port = htons(args->port);

    dns_packet_t *dns_packet;
    dns_packet = get_dns_packet_from_iterator(iter);

    sendto(sockfd, dns_packet->packet, dns_packet->packet_length, MSG_CONFIRM, (struct sockaddr *)&servaddr, sizeof(servaddr));
    memset(&buffer, 0, sizeof(buffer));
    recv_len = recvfrom(sockfd, buffer, MAX_UDP_MESSAGE_SIZE - 1, 0, (struct sockaddr *)&cliaddr, &client_address_len);

    close(sockfd);
    delete_iterator(iter); // deleting the iterator

    pthread_exit((void *)statistic);
}