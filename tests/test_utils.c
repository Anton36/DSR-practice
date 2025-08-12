#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
// positive ip check
START_TEST(test_check_valid_ip_valid_examples)
{
    ck_assert_int_eq(check_valid_ip("127.0.0.1"), 1);
    ck_assert_int_eq(check_valid_ip("0.0.0.0"), 1);
    ck_assert_int_eq(check_valid_ip("255.255.255.255"), 1);
    ck_assert_int_eq(check_valid_ip("192.168.1.42"), 1);
}
END_TEST
//negative ip check 
START_TEST(test_check_valid_ip_invalid_examples)
{
    ck_assert_int_eq(check_valid_ip(""), 0);
    ck_assert_int_eq(check_valid_ip("999.0.0.1"), 0);
    ck_assert_int_eq(check_valid_ip("256.256.256.256"), 0);
    ck_assert_int_eq(check_valid_ip("abc.def.ghi.jkl"), 0);
    ck_assert_int_eq(check_valid_ip("192.168.1."), 0);
    ck_assert_int_eq(check_valid_ip("192.168.1.1.1"), 0);
    ck_assert_int_eq(check_valid_ip(" 127.0.0.1"), 0);
    ck_assert_int_eq(check_valid_ip("127.0.0.1 "), 0);
    ck_assert_int_eq(check_valid_ip("..127.0.0.1 "), 0);
}
END_TEST

Suite* utils_suite(void)
{
    Suite *s = suite_create("utils");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, test_check_valid_ip_valid_examples);
    tcase_add_test(tc, test_check_valid_ip_invalid_examples);
    suite_add_tcase(s, tc);
    return s;
}
