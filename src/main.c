
#include "main_inc.h"
#include "utils.h"
#include "linked_list_api.h"
#include "dns_perf_api.h"

void help(void);
void sigint_handler(int signum);

volatile sig_atomic_t stop_threads = false; // flag for stoping threads

int main(int argc, char *argv[])
{
    int rez = 0;
    char *ip = NULL;
    int port = 0;
    int QPS = 0;
    char *file = NULL;
    int number_of_threads = 0;
    int working_time = 0;
    int P;

    signal(SIGINT, sigint_handler); // intercept SIGINT signal to softly terminate threads when pressing ctrl + c
    if (argc == 1)
    {
        fprintf(stderr, "Error: no arguments provided\n\n");
        help();
        return 1;
    }

    while ((rez = getopt(argc, argv, ":i:a:q:c:t:f:")) != -1) // function for parsing command-line arguments in which we pass input data
    {
        switch (rez)
        {
        case 'i':
            if (check_valid_ip(optarg) == 0) // checking the validity of the IP address (compliance with the range of numbers, number of dots)
            {
                fprintf(stderr, "Error: invalid IP address format\n \n");
                help(); // in case of incorrect parameter input, a help message about the utility is displayed
                return 1;
            }
            ip = optarg;
            break;
        case 'a':
            if (atoi(optarg) == 0) // checking if the argument value is a number .. TODO: check for boundary values (to be determined empirically later)
            {
                fprintf(stderr, "Error: port value must be an integer\n \n");
                help();
                return 1;
            }
            port = atoi(optarg);
            break;
        case 'q':
            if (atoi(optarg) == 0)
            {
                fprintf(stderr, "Error: QPS value must be an integer\n \n");
                help();
                return 1;
            }
            QPS = atoi(optarg);
            break;
        case 'c':
            number_of_threads = atoi(optarg);
            if (number_of_threads < 0)
            {
                fprintf(stderr, "Error: number of threads value must be a positive integer\n \n");
                help();
                return 1;
            }
            break;
        case 't':
            working_time = atoi(optarg);
            if (working_time < 0)
            {
                fprintf(stderr, "Error: working time value must be a positive integer or zero\n \n");
                help();
                return 1;
            }

            break;
        case 'f':
            file = optarg;
            FILE *fp = fopen(optarg, "r"); // validating the existence of the file by attempting to open it
            if (fp == NULL)
            {
                fprintf(stderr, "Error: file not found\n \n");
                help();
                return 1;
            }
            fclose(fp);
            break;
        case ':': // auxiliary argument for cases when a value is not provided for one of the arguments
            fprintf(stderr, "Error: option %c needs a value\n \n", optopt);
            help();
            return 1;
        case '?':
            fprintf(stderr, "Error: unknown option %c\n \n", optopt); // auxiliary argument for cases when an unknown argument is passed
            help();
            return 1;
        }
    }
    // informational output
    printf("IP address: %s\n", ip);
    printf("Port: %d\n", port);
    printf("QPS: %d\n", QPS);
    printf("Number of threads: %d\n", number_of_threads);
    printf("Working time: %d seconds\n", working_time);
    printf("File: %s\n", file);

    P = QPS / number_of_threads; // calculating the number of requests per second for each thread

    get_lines_from_file(file);
    pthread_t *threads = malloc(number_of_threads * sizeof(pthread_t)); // allocating memory for thread identifiers

    struct thread_arg args; // structure for the arguments that we will pass to the threads
    args.P = P;
    args.working_time = working_time;
    args.ip = ip;
    args.port = port;

    for (int i = 0; i < number_of_threads; i++)
    {
        if (pthread_create(&threads[i], NULL, thread_body, &args) != 0) // creating a thread for each request
        {
            fprintf(stderr, "Error: thread could not be created\n");
            deleteList();
            free(threads);
            return 1;
        }
    }

    uint64_t total_sent = 0, total_recv = 0, total_nsent = 0;
    double avg_working_time_sec = 0, avg_working_time_us = 0;
    double total_working_time_sec = 0, total_working_time_us = 0;
    int avg_qps = 0, avg_rps = 0;
    int total_qps = 0, total_rps = 0;

    for (int i = 0; i < number_of_threads; ++i)
    {
        struct thread_statistics *st = NULL;
        pthread_join(threads[i], (void **)&st); // waiting for the threads to finish working and get statistic
        if (st != NULL)
        {
            // calculating stats
            total_sent += st->pkt_sent;
            total_recv += st->pkt_recv;
            total_nsent += st->ptk_nsent;
            total_qps += st->estimated_qps;
            total_rps += st->estimated_rps;
            total_working_time_sec += st->run_time.tv_sec;
            total_working_time_us += st->run_time.tv_usec;

            free(st);
        }
    }
    avg_working_time_sec = total_working_time_sec / number_of_threads;
    avg_working_time_us = total_working_time_us / number_of_threads;
    avg_qps = total_qps / number_of_threads;
    avg_rps = total_rps / number_of_threads;

    printf("\n=== TOTAL ===\n");
    printf("Avg thread time:       %llu s, %llu us\n", (unsigned long long)avg_working_time_sec, (unsigned long long)avg_working_time_us);
    printf("Sent:            %llu\n", (unsigned long long)total_sent);
    printf("Recv:            %llu\n", (unsigned long long)total_recv);
    printf("Not sent:        %llu\n", (unsigned long long)total_nsent);
    printf("Target QPS:   %d\n", P);
    printf("Estimated QPS:   %d\n", avg_qps);
    printf("Estimated RPS:   %d\n", avg_rps);
    free(threads);
    deleteList();
    return 0;
}

void help(void)
{
    fprintf(stdout, "This utility is designed to measure the performance and stability of DNS servers.\n");
    fprintf(stdout, "Usage: [-i <ip address>] [-a <port>] [-q <number of requests per second>] [-c <number of threads>] [-t <limitation on the operating time of the utility, 0-unlimited operation>] [-f <name of file with DNS request>]\n");
}

void sigint_handler(int signum)
{

    stop_threads = true;
}