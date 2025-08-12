#include "utils.h"
#include "main_inc.h"
#include "dns_module_api.h"
volatile sig_atomic_t stop_threads = false; // flag for stoping threads
int check_valid_ip(const char *ip)
{
    int dotsCount = 0; // number of dots in the address
    int lastPart = 0;  // value of the last detected octet in the address (to check if it exceeds 255)
    bool bLastCharIsDot = false;

    // Handle empty or NULL IP
    if (ip == NULL || *ip == '\0')
    {
        return false;
    }

    while (*ip)
    {
        char ch = *ip++; // increment the pointer along the IP address

        if (ch == '.')
        {
            // check or a dot at the start
            if (bLastCharIsDot)
            {
                return false;
            }

            dotsCount++;
            if (lastPart > 255)
            {
                return false; // if the last octet exceeds 255, consider the address invalid
            }
            lastPart = 0;
            bLastCharIsDot = true; // set flag indicating that the previous character was a dot
        }
        else if (isdigit(ch)) // if a digit is detected
        {
            // add it to our octet
            lastPart = lastPart * 10 + (ch - '0');
            bLastCharIsDot = false; // reset dot flag as we are processing a digit
        }
        else
        {
            return false; // invalid character in the IP address
        }
    }

    // Final check: last part should be <= 255 and there should be exactly 3 dots
    if (lastPart > 255 || dotsCount != 3 || bLastCharIsDot)
    {
        return false;
    }
    return true;
}
void get_lines_from_file(const char *file) // function for reading file with dns requests line by line
{
    FILE *fp = fopen(file, "r");
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    if (fp == NULL)
    {
        fprintf(stderr, "ERROR: file not exist \n");
        exit(EXIT_FAILURE);
    }
    while ((read = getline(&line, &len, fp)) != -1)
    {
        make_dns_request(line); // each line is sent to a function to form a request
    }
    fclose(fp);
    if (line)
        free(line);
}



int parse_arguments(int argc, char *argv[], struct app_config *config)
{
    int rez = 0;

    memset(config, 0, sizeof(struct app_config));

    if (argc == 1)
    {
        fprintf(stderr, "Error: no arguments provided\n\n");
        help();
        return 1;
    }

    while ((rez = getopt(argc, argv, ":i:a:q:c:t:f:")) != -1)
    {
        switch (rez)
        {
        case 'i':
            if (check_valid_ip(optarg) == 0)
            {
                fprintf(stderr, "Error: invalid IP address format\n \n");
                help();
                return 1;
            }
            config->ip = optarg;
            break;
        case 'a':
            config->port =atoi(optarg);
            if (config->port < 0)
            {
                fprintf(stderr, "Error: port value must be an positive integer\n \n");
                help();
                return 1;
            }
            
            break;
        case 'q':
             config->QPS =atoi(optarg);
            if (config->QPS < 0)
            {
                fprintf(stderr, "Error: QPS value must be an positive integer\n \n");
                help();
                return 1;
            }
            config->QPS = atoi(optarg);
            break;
        case 'c':
            config->number_of_threads = atoi(optarg);
            if (config->number_of_threads < 0)
            {
                fprintf(stderr, "Error: number of threads value must be a positive integer\n \n");
                help();
                return 1;
            }
            break;
        case 't':
            config->working_time = atoi(optarg);
            if (config->working_time < 0)
            {
                fprintf(stderr, "Error: working time value must be a positive integer or zero\n \n");
                help();
                return 1;
            }
            break;
        case 'f':
            config->file = optarg;
            FILE *fp = fopen(optarg, "r");
            if (fp == NULL)
            {
                fprintf(stderr, "Error: file not found\n \n");
                help();
                return 1;
            }
            fclose(fp);
            break;
        case ':':
            fprintf(stderr, "Error: option %c needs a value\n \n", optopt);
            help();
            return 1;
        case '?':
            fprintf(stderr, "Error: unknown option %c\n \n", optopt);
            help();
            return 1;
        }
    }
    return 0;
}

void help(void)
{
    fprintf(stdout, "This utility is designed to measure the performance and stability of DNS servers.\n");
    fprintf(stdout, "Usage: [-i <ip address>] [-a <port>] [-q <number of requests per second>] [-c <number of threads>] [-t <limitation on the operating time of the utility, 0-unlimited operation>] [-f <name of file with DNS request>]\n");
}