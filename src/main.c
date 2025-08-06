
#include "main_inc.h"
#include "utils.h"
#include "linked_list_api.h"
#include "dns_perf_api.h"

void help(void);

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
    int M;

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
            if (number_of_threads <= 0)
            {
                fprintf(stderr, "Error: number of threads value must be a positive integer\n \n");
                help();
                return 1;
            }
            break;
        case 't':
            working_time = atoi(optarg);
            if (working_time <= 0)
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
    // TODO: M calculation

    get_lines_from_file(file);
    pthread_t *threads = malloc(number_of_threads * sizeof(pthread_t));

    struct thread_arg args;
    args.P = P;
    args.working_time = working_time;
    args.ip = ip;
    args.port = port;

    for (int i = 0; i < number_of_threads; i++)
    {
        if (pthread_create(&threads[i], NULL, thread_body, &args) != 0) // creating a thread for each request
        {
            fprintf(stderr, "Error: thread could not be created\n");
            return 1;
        }
    }

    for (int i = 0; i < number_of_threads; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    deleteList();
    return 0;
    // TODO delete list after end of program
}

void help(void)
{
    fprintf(stdout, "This utility is designed to measure the performance and stability of DNS servers.\n");
    fprintf(stdout, "Usage: [-i <ip address>] [-a <port>] [-q <number of requests per second>] [-c <number of threads>] [-t <limitation on the operating time of the utility, 0-unlimited operation>] [-f <name of file with DNS request>]\n");
}