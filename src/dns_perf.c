
#include "dns_perf_inc.h"
#include "linked_list_api.h"

void *thread_body(void *arg)
{
    struct thread_arg *args = (struct thread_arg *)arg; // typecasting arguments to working structure
    if (args == NULL)
    {
        fprintf(stderr, "Error: thread arguments are NULL\n");
        return NULL;
    }

    printf("Thread started with IP: %s, Port: %d, P: %d, Working time: %d seconds\n", args->ip, args->port, args->P, args->working_time);

    int sockfd;
    int M = 40;                                                           // TODO M calculation
    struct timeval end_tv, start_time, current_time, last_operation_time; // declaration of structures for working with time
    // struct timeval test_start, test_end; //for calculating the number of sending intervals
    bool is_this_first_iteration = true;                  // flag for skiping time check
    long long start_us, end_us;                           // variables for calculating the total running time of a thread
    long long target_time = args->working_time * 1000000; // thread running time in us
    double elapsed_us;
    double run_time_s;
    double micro_sec_for_send = 1000000 / M; // sending interval size
    struct timeval timeout;                  // timeout structure for package receive
    timeout.tv_sec = 0;
    timeout.tv_usec = 10000;                                         // 10ms
    int packets_per_sending = (args->P + M - 1) / M;                 // calculation of the number of packets sent in 1 interval, rounding up
    unsigned char buffer[packets_per_sending][MAX_UDP_MESSAGE_SIZE]; // packet buffer
    struct sockaddr_in servaddr, cliaddr[packets_per_sending];       // Structure for storing server addresses when receiving and sending
                                                                     // getting the start time of the thread

    struct mmsghdr msg[packets_per_sending];
    struct iovec iov[packets_per_sending];

    // Create UDP socket for DNS queries
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        fprintf(stderr, "socket creation failed");
        return NULL;
    }
    /*setting a timeout in a socket for receiving packets,
    we do it in a socket because due to a bug in
    recvmmsg setting a timeout inside a function is impossible */
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        fprintf(stderr, "setsockopt SO_RCVTIMEO failed");
        close(sockfd);
        return NULL;
    }
    iterator_t iterator;
    create_iterator(&iterator,get_dns_packet_list());

    struct thread_statistics *statistic = malloc(sizeof(struct thread_statistics));
    if (statistic == NULL)
    {
        fprintf(stderr, "Error: memory allocation for statistics failed\n");
        close(sockfd);
        return NULL;
    }
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    // Configure server address structure
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(args->ip);
    servaddr.sin_port = htons(args->port);

    gettimeofday(&start_time, NULL);
    while (1)
    {
        if (stop_threads) // stop work flag check
            break;

        if (target_time != 0)
        {
            gettimeofday(&current_time, NULL);
            long elapsed_time = (current_time.tv_sec - start_time.tv_sec) * 1000000 + (current_time.tv_usec - start_time.tv_usec);
            // Check if we have reached the target execution time
            if (elapsed_time >= target_time)
            {
                break; // if yes exit loop
            }
        }
        // if this non-first iteration .then check passed time since last iteration
        if (!is_this_first_iteration)
        {
            gettimeofday(&current_time, NULL);
            // Calculate time elapsed since last packet sending operation
            long time_since_last_op = (current_time.tv_sec - last_operation_time.tv_sec) * 1000000 + (current_time.tv_usec - last_operation_time.tv_usec);
            // Skip iteration if not enough time has passed
            if (time_since_last_op < micro_sec_for_send)
            {

                continue;
            }
        }
        is_this_first_iteration = false; //// Mark that first iteration has completed

        gettimeofday(&last_operation_time, NULL); // mark curent time for iteration calculation

        for (int i = 0; i < packets_per_sending; i++)
        {
            memset(&msg[i], 0, sizeof(struct mmsghdr));
            memset(&iov[i], 0, sizeof(struct iovec));
            dns_packet_t *dns_packet = iterator_next(&iterator); //get dns packet from iterator 

            // Configure I/O vector with packet data
            iov[i].iov_base = dns_packet->packet;
            iov[i].iov_len = dns_packet->packet_length;

            // Configure message header
            msg[i].msg_hdr.msg_iov = &iov[i];
            msg[i].msg_hdr.msg_name = &servaddr;
            msg[i].msg_hdr.msg_namelen = sizeof(servaddr);
            msg[i].msg_hdr.msg_iovlen = 1;
            msg[i].msg_hdr.msg_control = NULL;
            msg[i].msg_hdr.msg_controllen = 0;
            msg[i].msg_hdr.msg_flags = 0;
        }
        int sent = sendmmsg(sockfd, msg, packets_per_sending, MSG_DONTWAIT);

        if (sent < 0)
        {
            // fprintf(stderr, "sendmmsg failed \n");
            statistic->ptk_nsent += packets_per_sending; // Update statistics for failed packet sends
        }
        else
        {
            statistic->pkt_sent += sent;

            if (sent < packets_per_sending)
            {
                statistic->ptk_nsent += (packets_per_sending - sent); // Update statistics with amount of packet which was successfully sent
            }
        }
        // Prepare buffer for receive amount of packets we send
        for (int i = 0; i < packets_per_sending; i++)
        {
            memset(&msg[i], 0, sizeof(struct mmsghdr));
            memset(&iov[i], 0, sizeof(struct iovec));
            // Configure I/O vector to receive data into buffer
            iov[i].iov_base = buffer[i];
            iov[i].iov_len = MAX_UDP_MESSAGE_SIZE;
            // Configure message header for receiving
            msg[i].msg_hdr.msg_iov = &iov[i];
            msg[i].msg_hdr.msg_name = &cliaddr[i];
            msg[i].msg_hdr.msg_namelen = sizeof(struct sockaddr_in);
            msg[i].msg_hdr.msg_iovlen = 1;
            msg[i].msg_hdr.msg_control = NULL;
            msg[i].msg_hdr.msg_controllen = 0;
            msg[i].msg_hdr.msg_flags = 0;
        }

        int received = recvmmsg(sockfd, msg, packets_per_sending, 0, NULL);
        if (received < 0)
        {
            // fprintf(stderr, "recvmmsg failed \n");
        }
        else
        {
            statistic->pkt_recv += received; // Update statistics with successfully received packets
        }
        // gettimeofday(&test_end, NULL);
    }

    gettimeofday(&end_tv, NULL); // getting the end time of the thread
    // getting the start time of the thread
    start_us = start_time.tv_sec * 1000000 + start_time.tv_usec;
    end_us = end_tv.tv_sec * 1000000 + end_tv.tv_usec;
    // Calculate elapsed time and store in statistics
    elapsed_us = (double)(end_us - start_us);

    /*test variables for calculating the number of sending intervals
    double elapsed_test_time = (double)(test_end.tv_usec - last_operation_time.tv_usec);
    double count = 1000000 / elapsed_test_time;
    */

    run_time_s = (double)elapsed_us / 1000000.0;
    statistic->run_time.tv_sec = run_time_s;
    statistic->run_time.tv_usec = (int)elapsed_us % 1000000;

    statistic->estimated_qps = statistic->pkt_sent / run_time_s;
    statistic->estimated_rps = statistic->pkt_recv / run_time_s;

    statistic->target_qps = args->P;

    close(sockfd);


    pthread_exit((void *)statistic); //// Exit thread and return statistics structure
}