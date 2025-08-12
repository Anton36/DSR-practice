
#include "main_inc.h"
#include "utils.h"
#include "linked_list_api.h"
#include "dns_perf_api.h"


int main(int argc, char *argv[])
{

    int P;
    struct app_config config;

    signal(SIGINT, sigint_handler); // intercept SIGINT signal to softly terminate threads when pressing ctrl + c

    if (parse_arguments(argc, argv, &config) != 0)
    {
        return 1;
    }

    // informational output
    printf("IP address: %s\n", config.ip);
    printf("Port: %d\n", config.port);
    printf("QPS: %d\n", config.QPS);
    printf("Number of threads: %d\n", config.number_of_threads);
    printf("Working time: %d seconds\n", config.working_time);
    printf("File: %s\n", config.file);

    P = config.QPS / config.number_of_threads; // calculating the number of requests per second for each thread
    list_init();                               // init list pool
    get_lines_from_file(config.file);

    pthread_t *threads = malloc(config.number_of_threads * sizeof(pthread_t)); // allocating memory for thread identifiers

    struct thread_arg args; // structure for the arguments that we will pass to the threads
    args.P = P;
    args.working_time = config.working_time;
    args.ip = config.ip;
    args.port = config.port;

    for (int i = 0; i < config.number_of_threads; i++)
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

    for (int i = 0; i < config.number_of_threads; ++i)
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
    avg_working_time_sec = total_working_time_sec / config.number_of_threads;
    avg_working_time_us = total_working_time_us / config.number_of_threads;
    avg_qps = total_qps / config.number_of_threads;
    avg_rps = total_rps / config.number_of_threads;

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



void sigint_handler(int signum)
{

    stop_threads = true;
}

