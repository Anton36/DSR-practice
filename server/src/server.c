#include "main_inc.h"
#include "dns_module_api.h"
#include "dns_module_inc.h"

#define PORT 8080

int main()
{
    int sockfd;
    char buffer[MAX_UDP_MESSAGE_SIZE];
    struct sockaddr_in servaddr, cliaddr;
    int recv_len;
    socklen_t client_address_len = sizeof(cliaddr);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        fprintf(stderr, "socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        fprintf(stderr, "bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    client_address_len = sizeof(cliaddr);
    printf("starting server \n");
    while (true)
    {
        memset(&buffer, 0, sizeof(buffer));
        recv_len = 0;

        recv_len = recvfrom(sockfd, (char *)buffer, MAX_UDP_MESSAGE_SIZE - 1, 0, (struct sockaddr *)&cliaddr, &client_address_len);
        if (recv_len > 0)
        {
            buffer[recv_len] = '\0';
            printf("Client with address %s:%d sent packet [length = %d]:\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), recv_len);
            struct dns_header *dns = (struct dns_header *)buffer;
            dns->qr = QR_RESPONSE;
            if (dns->rd == RD_RECURSION_ALLOWED)
            {
                dns->ra = RA_RECURSION_ALLOWED;
            }

            sendto(sockfd, (char *)buffer, recv_len, MSG_CONFIRM, (struct sockaddr *)&cliaddr, client_address_len);
        }
    }
    close(sockfd);
    return 0;
}