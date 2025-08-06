#include "main_inc.h"
#include "dns_module_api.h"
#include "dns_module_inc.h"

#define PORT 8080

int main()
{
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
            struct dns_header *header = (struct dns_header *)buffer;
            printf("Before: QR=%d, RD=%d, RA=%d\n", header->flags.qr, header->flags.rd, header->flags.ra);//
            header->flags.qr = QR_RESPONSE;
            if (header->flags.rd == RD_RECURSION_ALLOWED)
            {
                header->flags.ra = RA_RECURSION_ALLOWED;
            }

            printf("After: QR=%d, RD=%d, RA=%d\n", header->flags.qr, header->flags.rd, header->flags.ra);

            sendto(sockfd, (char *)buffer, recv_len, MSG_CONFIRM, (struct sockaddr *)&cliaddr, client_address_len);
        }
    }
    close(sockfd);
    return 0;
}