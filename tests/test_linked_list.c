#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "linked_list_api.h"

static int count_list_elements(dns_packet_t *head)
{
    int count = 0;
    dns_packet_t *current = head;
    while (current != NULL)
    {
        count++;
        current = current->next;
    }
    return count;
}

// pushing a single packet to the list
START_TEST(test_list_push_single)
{
    list_init();

    unsigned char test_data[] = {1, 2, 3, 4};
    int data_len = sizeof(test_data);

    bool result = list_push(test_data, data_len);
    ck_assert_msg(result == true, "list_push should succeed");

    dns_packet_t *list_head = get_dns_packet_list();
    ck_assert_msg(list_head != NULL, "List head should not be NULL");
    ck_assert_msg(list_head->next == NULL, "List should contain only one element");
    ck_assert_msg(list_head->packet_length == data_len, "Packet length mismatch");
    ck_assert_msg(memcmp(list_head->packet, test_data, data_len) == 0, "Packet data mismatch");

    deleteList();
}
END_TEST
// pushing multiple packets to the list
START_TEST(test_list_push_multiple)
{
    list_init();

    unsigned char data1[] = {1, 2};
    unsigned char data2[] = {3, 4, 5};
    unsigned char data3[] = {6};

    list_push(data1, sizeof(data1));
    list_push(data2, sizeof(data2));
    list_push(data3, sizeof(data3));

    dns_packet_t *list_head = get_dns_packet_list();
    int count = count_list_elements(list_head);
    ck_assert_msg(count == 3, "List should contain 3 elements, got %d", count);

    ck_assert_msg(list_head->packet_length == sizeof(data3), "First element length mismatch");
    ck_assert_msg(memcmp(list_head->packet, data3, sizeof(data3)) == 0, "First element data mismatch");

    list_head = list_head->next;
    ck_assert_msg(list_head->packet_length == sizeof(data2), "Second element length mismatch");
    ck_assert_msg(memcmp(list_head->packet, data2, sizeof(data2)) == 0, "Second element data mismatch");

    list_head = list_head->next;
    ck_assert_msg(list_head->packet_length == sizeof(data1), "Third element length mismatch");
    ck_assert_msg(memcmp(list_head->packet, data1, sizeof(data1)) == 0, "Third element data mismatch");

    list_head = list_head->next;
    ck_assert_msg(list_head == NULL, "List should end after third element");

    deleteList();
}
END_TEST

// verify behavior when poll is full
START_TEST(test_list_push_full_pool)
{
    list_init();

    int pushed_count = 0;
    unsigned char test_byte = 1;
    bool result = true;

    while (result && pushed_count < MAX_LIST_SIZE + 5)
    {
        unsigned char data[MAX_UDP_MESSAGE_SIZE];
        for (int i = 0; i < MAX_UDP_MESSAGE_SIZE; i++)
        {
            data[i] = test_byte;
        }
        result = list_push(data, 5);
        if (result)
        {
            pushed_count++;
        }
        test_byte++;
    }

    ck_assert_msg(pushed_count == MAX_LIST_SIZE, "Should be able to push exactly MAX_LIST_SIZE (%d) elements, pushed %d", MAX_LIST_SIZE, pushed_count);

    unsigned char dummy_data[5] = {50};
    bool final_result = list_push(dummy_data, sizeof(dummy_data));
    ck_assert_msg(final_result == false, "Push after pool exhaustion should fail");

    int list_count = count_list_elements(get_dns_packet_list());
    ck_assert_msg(list_count == MAX_LIST_SIZE, "Active list should contain MAX_LIST_SIZE elements");

    deleteList();

    ck_assert_msg(get_dns_packet_list() == NULL, "Active list should be NULL after deleteList");

    unsigned char data_after_clear[] = {10};
    bool result_after_clear = list_push(data_after_clear, sizeof(data_after_clear));
    ck_assert_msg(result_after_clear == true, "Push should succeed after deleteList");
    ck_assert_msg(get_dns_packet_list() != NULL, "List should not be NULL after push post-deleteList");
    ck_assert_msg(get_dns_packet_list()->packet_length == sizeof(data_after_clear), "Packet length mismatch after push post-deleteList");
    ck_assert_msg(memcmp(get_dns_packet_list()->packet, data_after_clear, sizeof(data_after_clear)) == 0, "Packet data mismatch after push post-deleteList");

    deleteList();
}
END_TEST

// the basic functionality of the list iterator
START_TEST(test_iterator)
{
    list_init();

    unsigned char data1[] = {1};
    unsigned char data2[] = {2, 3};
    unsigned char data3[] = {4, 5, 6};

    list_push(data1, sizeof(data1));
    list_push(data2, sizeof(data2));
    list_push(data3, sizeof(data3));

    dns_packet_t *list_head = get_dns_packet_list();
    iterator_t iterator;
    create_iterator(&iterator, list_head);

    dns_packet_t *pkt = iterator_next(&iterator);
    ck_assert_msg(pkt != NULL, "First iterator_next should not return NULL");
    ck_assert_msg(pkt->packet_length == sizeof(data3), "First packet length mismatch");
    ck_assert_msg(memcmp(pkt->packet, data3, sizeof(data3)) == 0, "First packet data mismatch");

    pkt = iterator_next(&iterator);
    ck_assert_msg(pkt != NULL, "Second iterator_next should not return NULL");
    ck_assert_msg(pkt->packet_length == sizeof(data2), "Second packet length mismatch");
    ck_assert_msg(memcmp(pkt->packet, data2, sizeof(data2)) == 0, "Second packet data mismatch");

    pkt = iterator_next(&iterator);
    ck_assert_msg(pkt != NULL, "Third iterator_next should not return NULL");
    ck_assert_msg(pkt->packet_length == sizeof(data1), "Third packet length mismatch");
    ck_assert_msg(memcmp(pkt->packet, data1, sizeof(data1)) == 0, "Third packet data mismatch");

    deleteList();
}
END_TEST

Suite *linked_list_suite(void)
{
    Suite *s = suite_create("linked_list");
    TCase *tc = tcase_create("core");

    tcase_add_test(tc, test_list_push_single);
    tcase_add_test(tc, test_list_push_multiple);
    tcase_add_test(tc, test_list_push_full_pool);

    tcase_add_test(tc, test_iterator);

    suite_add_tcase(s, tc);
    return s;
}
