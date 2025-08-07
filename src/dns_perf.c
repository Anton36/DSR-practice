
#include "dns_perf_inc.h"
#include "linked_list_api.h"
#define NUMBER_OF_MESSAGE 100
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
    unsigned char buffer[NUMBER_OF_MESSAGE][MAX_UDP_MESSAGE_SIZE];
    struct sockaddr_in servaddr, cliaddr[NUMBER_OF_MESSAGE];
    int recv_len;

    struct mmsghdr msg[NUMBER_OF_MESSAGE]; // TODO change to calculated value k-packet
    struct iovec iov[NUMBER_OF_MESSAGE];   // TODO change to calculated value k-packet

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

    for (int i = 0; i < NUMBER_OF_MESSAGE; i++)
    {
        memset(&msg[i], 0, sizeof(struct mmsghdr));
        memset(&iov[i], 0, sizeof(struct iovec));
        dns_packet_t *dns_packet = get_dns_packet_from_iterator(iter);
        iov[i].iov_base = dns_packet->packet;
        iov[i].iov_len = dns_packet->packet_length;
        msg[i].msg_hdr.msg_iov = &iov[i];
        msg[i].msg_hdr.msg_name = &servaddr;
        msg[i].msg_hdr.msg_namelen = sizeof(servaddr);
        msg[i].msg_hdr.msg_iovlen = 1;
        msg[i].msg_hdr.msg_control = NULL;
        msg[i].msg_hdr.msg_controllen = 0;
        msg[i].msg_hdr.msg_flags = 0;
    }
    int sent = sendmmsg(sockfd, msg, NUMBER_OF_MESSAGE, 0);

    for (int i = 0; i < NUMBER_OF_MESSAGE; i++)
    {
        memset(&msg[i], 0, sizeof(struct mmsghdr));
        memset(&iov[i], 0, sizeof(struct iovec));
        iov[i].iov_base = buffer[i];
        iov[i].iov_len = MAX_UDP_MESSAGE_SIZE;
        msg[i].msg_hdr.msg_iov = &iov[i];
        msg[i].msg_hdr.msg_name = &cliaddr[i];
        msg[i].msg_hdr.msg_namelen = sizeof(struct sockaddr_in);
        msg[i].msg_hdr.msg_iovlen = 1;
        msg[i].msg_hdr.msg_control = NULL;
        msg[i].msg_hdr.msg_controllen = 0;
        msg[i].msg_hdr.msg_flags = 0;
    }

    int received = recvmmsg(sockfd, msg, NUMBER_OF_MESSAGE, 0, NULL);
    if (received < 0)
    {
        fprintf(stderr, "recvmmsg failed \n");
        close(sockfd);
        
    }

    close(sockfd);
    delete_iterator(iter); // deleting the iterator

    pthread_exit((void *)statistic);
}