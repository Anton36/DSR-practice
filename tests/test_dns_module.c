#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "dns_module_api.h"
#include "linked_list_api.h"

// Helper function to verify the structure and content of a DNS qname buffer.
static void assert_qname_labels(const unsigned char *qname, int qname_len, const char *expected_labels[], int num_labels)
{
    const unsigned char *ptr = qname;
    int total_len = 0;

    for (int i = 0; i < num_labels; i++)
    {
        int label_len = strlen(expected_labels[i]);
        ck_assert_msg(total_len + 1 + label_len <= qname_len, "Qname buffer overflow during label check");
        ck_assert_msg(*ptr == label_len, "Label length mismatch");
        ptr += 1 + label_len;
        total_len += 1 + label_len;
    }
}

// Helper function to verify that a DNS header has the correct flags and counts
static void assert_dns_header_query(const struct dns_header *header)
{
    ck_assert_msg(header != NULL, "DNS header pointer should not be NULL");
    ck_assert_msg(header->flags.qr == QR_QUERY, "QR flag should be QUERY");
    ck_assert_msg(header->flags.opcode == OPCODE_STANDARD, "OPCODE should be STANDARD");
    ck_assert_msg(header->flags.tc == TC_NOT_TRUNCATED, "TC flag should be NOT_TRUNCATED");
    ck_assert_msg(header->flags.rd == RD_RECURSION_ALLOWED, "RD flag should be RECURSION_ALLOWED");
    ck_assert_msg(header->flags.aa == AA_NOT_VALID, "AA flag should be NOT_VALID for query");
    ck_assert_msg(header->flags.ra == RA_RECURSION_NOT_ALLOWED, "RA flag should be RECURSION_NOT_ALLOWED initially");
    ck_assert_msg(header->flags.rcode == RCODE_NO_ERROR, "RCODE should be NO_ERROR initially");
    ck_assert_msg(header->flags.z == 0, "Z bits should be 0");

    ck_assert_msg(ntohs(header->qdcount) == 1, "QDCOUNT should be 1");
    ck_assert_msg(ntohs(header->ancount) == 0, "ANCOUNT should be 0");
    ck_assert_msg(ntohs(header->nscount) == 0, "NSCOUNT should be 0");
    ck_assert_msg(ntohs(header->arcount) == 0, "ARCOUNT should be 0");
}

// Convert a simple, valid domain name ("example.com") to DNS qname format.
START_TEST(test_get_qname_from_line_simple)
{
    char domain[] = "yandex.ru";
    int qname_len = 0;
    char *result = get_qname_from_line(domain, &qname_len);

    ck_assert_msg(result != NULL, "Result should not be NULL for valid input");
    ck_assert_msg(qname_len > 0, "Qname length should be positive");

    const char *expected[] = {"yandex", "ru"};
    assert_qname_labels((unsigned char *)result, qname_len, expected, 2);

    free(result);
}
END_TEST

// Convert a domain name containing labels of maximum allowed length (DNS_MAXLABEL = 63)
START_TEST(test_get_qname_from_line_long_label)
{

    char long_label[DNS_MAXLABEL + 1];
    memset(long_label, 'a', DNS_MAXLABEL);
    long_label[DNS_MAXLABEL] = '\0';

    char domain[2 * DNS_MAXLABEL + 2]; 
    snprintf(domain, sizeof(domain), "%s.%s", long_label, long_label);

    int qname_len = 0;
    char *result = get_qname_from_line(domain, &qname_len);

    ck_assert_msg(result != NULL, "Result should not be NULL for max label length");
    ck_assert_msg(qname_len > 0, "Qname length should be positive");

    const char *expected[] = {long_label, long_label};
    assert_qname_labels((unsigned char *)result, qname_len, expected, 2);

    free(result);
}
END_TEST

// convert a domain name with a label exceeding the maximum allowed length
START_TEST(test_get_qname_from_line_too_long_label)
{

    char too_long_label[DNS_MAXLABEL + 2];
    memset(too_long_label, 'b', DNS_MAXLABEL + 1);
    too_long_label[DNS_MAXLABEL + 1] = '\0';

    char domain[DNS_MAXLABEL + 3];
    strcpy(domain, too_long_label);

    int qname_len = 0;
    char *result = get_qname_from_line(domain, &qname_len);

    ck_assert_msg(result == NULL, "Result should be NULL for label exceeding max length");
}
END_TEST
// Convert a domain name that is exactly same as max length
START_TEST(test_get_qname_from_line_max_length)
{

    char domain[256];
    char part63[64];
    char part61[62];
    memset(part63, 'x', 63);
    part63[63] = '\0';
    memset(part61, 'y', 61);
    part61[61] = '\0';

    snprintf(domain, sizeof(domain), "%s.%s.%s.%s", part63, part63, part63, part61);

    int qname_len = 0;
    char *result = get_qname_from_line(domain, &qname_len);

    ck_assert_msg(result != NULL, "Result should not be NULL for max total name length");
    ck_assert_msg(qname_len > 0, "Qname length should be positive");

    ck_assert_msg(qname_len == 255, "Qname length should be 255 for max name input");

    const char *expected[] = {part63, part63, part63, part61};
    assert_qname_labels((unsigned char *)result, qname_len, expected, 4);

    free(result);
}
END_TEST
// convert a domain name that bigger that maximum (255 bytes)
START_TEST(test_get_qname_from_line_too_long_total)
{

    char domain[258];
    char part63[64];
    memset(part63, 'z', 63);
    part63[63] = '\0';

    snprintf(domain, sizeof(domain), "%s.%s.%s.%s", part63, part63, part63, part63);

    int qname_len = 0;
    char *result = get_qname_from_line(domain, &qname_len);

    ck_assert_msg(result == NULL, "Result should be NULL for total name exceeding max length");
}
END_TEST
// get_qname_from_line with a NULL input pointer
START_TEST(test_get_qname_from_line_null_input)
{
    int qname_len = 0;
    char *result = get_qname_from_line(NULL, &qname_len);
    ck_assert_msg(result == NULL, "Result should be NULL for NULL input");
}
END_TEST

// process a valid input line to make dns request function and verify that packet was add to list
START_TEST(test_make_dns_request_simple)
{

    list_init();

    char line[] = "A yandex.ru\n";
    make_dns_request(line);

    dns_packet_t *list_head = get_dns_packet_list();
    ck_assert_msg(list_head != NULL, "List should not contain null after push");

    ck_assert_msg(list_head->next == NULL, "List should contain only one packet");

    const struct dns_header *header = (const struct dns_header *)list_head->packet;
    assert_dns_header_query(header);

    deleteList();
}
END_TEST

// Process an input line with invalid format (doesn't have format)
START_TEST(test_make_dns_request_invalid_format_only_domain)
{
    list_init();

    char line[] = "test\n";
    make_dns_request(line);

    dns_packet_t *list_head = get_dns_packet_list();
    ck_assert_msg(list_head == NULL, "List should be empty for invalid format input");

    deleteList();
}
END_TEST
// Call make_dns_request with a NULL input pointer.
START_TEST(test_make_dns_request_null_input)
{
    list_init();

    make_dns_request(NULL);

    dns_packet_t *list_head = get_dns_packet_list();
    ck_assert_msg(list_head == NULL, "List should be empty when input is NULL");

    deleteList();
}
END_TEST

// Call make_dns_request with a empty.
START_TEST(test_make_dns_request_empty_string)
{
    list_init();

    char line[] = "\n";
    make_dns_request(line);

    dns_packet_t *list_head = get_dns_packet_list();
    ck_assert_msg(list_head == NULL, "List should be empty for empty string input");

    deleteList();
}
END_TEST

Suite *dns_module_suite(void)
{
    Suite *s = suite_create("dns_module");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, test_get_qname_from_line_simple);
    tcase_add_test(tc, test_get_qname_from_line_long_label);
    tcase_add_test(tc, test_get_qname_from_line_too_long_label);
    tcase_add_test(tc, test_get_qname_from_line_max_length);
    tcase_add_test(tc, test_get_qname_from_line_too_long_total);
    tcase_add_test(tc, test_get_qname_from_line_null_input);
    tcase_add_test(tc, test_make_dns_request_simple);
    tcase_add_test(tc, test_make_dns_request_invalid_format_only_domain);
    tcase_add_test(tc, test_make_dns_request_null_input);
    tcase_add_test(tc, test_make_dns_request_empty_string);

    suite_add_tcase(s, tc);
    return s;
}
