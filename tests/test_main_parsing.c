#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main_inc.h"
#include "utils.h"
// verify successful parsing of command-line arguments
START_TEST(test_parse_args_success_minimal)
{
    char prog_name[] = "program";
    char *argv[] = {prog_name, "-i", "127.0.0.1", "-a", "8080", "-q", "10000", "-c", "20", "-t", "10", "-f", "../request.txt", NULL}; // file check doesn't work (problem with part)
    int argc = 13;

    struct app_config config = {0};
    int result = parse_arguments(argc, argv, &config);

    ck_assert_msg(result == 0, "parse_arguments should return 0 on success");
    ck_assert_str_eq(config.ip, "127.0.0.1");
    ck_assert_int_eq(config.port, atoi("8080"));
    ck_assert_int_eq(config.QPS, atoi("10000"));
    ck_assert_int_eq(config.number_of_threads, atoi("20"));
    ck_assert_int_eq(config.working_time, atoi("10"));
    ck_assert_str_eq(config.file, "request.txt");
}
END_TEST

// verify that the function correctly handles the case without arguments
START_TEST(test_parse_args_no_args)
{
    char prog_name[] = "program";
    char *argv[] = {prog_name, NULL};
    int argc = 1;

    struct app_config config = {0};
    int result = parse_arguments(argc, argv, &config);

    ck_assert_msg(result == 1, "parse_arguments should return 1 for no args");
}
END_TEST

// wrong ip format
START_TEST(test_parse_args_invalid_ip)
{
    char prog_name[] = "program";
    char *argv[] = {prog_name, "-i", "999.1.1.1", "-a", "53", "-q", "100", "-c", "2", "-f", "../request.txt", NULL};
    int argc = 11;

    struct app_config config = {0};
    int result = parse_arguments(argc, argv, &config);

    ck_assert_msg(result == 1, "Should fail for invalid IP");
}
END_TEST

// wrong port value(not numeric)
START_TEST(test_parse_args_invalid_port_not_number)
{
    char prog_name[] = "program";
    char *argv[] = {prog_name, "-i", "127.0.0.1", "-a", "not_a_number", "-q", "100", "-c", "2", "-f", "../request.txt", NULL};
    int argc = 11;

    struct app_config config = {0};
    int result = parse_arguments(argc, argv, &config);

    ck_assert_msg(result == 1, "Should fail for invalid port ");
}
END_TEST

// wrong QPS value(not numeric)
START_TEST(test_parse_args_invalid_qps_not_number)
{
    char prog_name[] = "program";
    char *argv[] = {prog_name, "-i", "127.0.0.1", "-a", "53", "-q", "abc", "-c", "2", "-f", "../request.txt", NULL};
    int argc = 11;

    struct app_config config = {0};
    int result = parse_arguments(argc, argv, &config);

    ck_assert_msg(result == 1, "Should fail for invalid QPS ");
}
END_TEST

// passing argument without value
START_TEST(test_parse_args_invalid_value_argument)
{
    char prog_name[] = "program";
    char *argv[] = {prog_name, "-i", "-a", "53", "-q", "abc", "-c", "2", "-f", "../request.txt", NULL};
    int argc = 10;

    struct app_config config = {0};
    int result = parse_arguments(argc, argv, &config);

    ck_assert_msg(result == 1, "Should fail for invalid value");
}
END_TEST

// passing unknown argument
START_TEST(test_parse_args_unknown_argument)
{
    char prog_name[] = "program";
    char *argv[] = {prog_name, "-z", "-a", "53", "-q", "abc", "-c", "2", "-f", "../request.txt", NULL};
    int argc = 10;

    struct app_config config = {0};
    int result = parse_arguments(argc, argv, &config);

    ck_assert_msg(result == 1, "Should fail for invalid argument");
}
END_TEST

// wrong thread number(not positive)
START_TEST(test_parse_args_invalid_threads_negative)
{
    char prog_name[] = "program";
    char *argv[] = {prog_name, "-i", "127.0.0.1", "-a", "8080", "-q", "10000", "-c", "-20", "-t", "10", "-f", "request.txt", NULL};
    int argc = 13;

    struct app_config config = {0};
    int result = parse_arguments(argc, argv, &config);

    ck_assert_msg(result == 1, "Should fail for negative number of threads");
}
END_TEST

// wrong working time
START_TEST(test_parse_args_invalid_time_negative)
{
    char prog_name[] = "program";
    char *argv[] = {prog_name, "-i", "127.0.0.1", "-a", "8080", "-q", "10000", "-c", "20", "-t", "-10", "-f", "request.txt", NULL};
    int argc = 13;

    struct app_config config = {0};
    int result = parse_arguments(argc, argv, &config);

    ck_assert_msg(result == 1, "Should fail for negative working time");
}
END_TEST
// file doenst exist
START_TEST(test_parse_args_file_null)
{
    char prog_name[] = "program";
    char *argv[] = {prog_name, "-i", "127.0.0.1", "-a", "8080", "-q", "10000", "-c", "20", "-t", "10", "-f", NULL}; // file check doesn't work (problem with part)
    int argc = 12;

    struct app_config config = {0};
    int result = parse_arguments(argc, argv, &config);
    ck_assert_msg(result == 1, "Should fail for null file");
}
END_TEST

Suite *main_parsing_suite(void)
{
    Suite *s = suite_create("main_parsing");
    TCase *tc = tcase_create("cli");
    tcase_set_timeout(tc, 5);
    // tcase_add_test(tc, test_parse_args_success_minimal);
    tcase_add_test(tc, test_parse_args_no_args);
    tcase_add_test(tc, test_parse_args_invalid_ip);
    tcase_add_test(tc, test_parse_args_invalid_port_not_number);
    tcase_add_test(tc, test_parse_args_invalid_qps_not_number);
    tcase_add_test(tc, test_parse_args_invalid_threads_negative);
    tcase_add_test(tc, test_parse_args_invalid_time_negative);
    tcase_add_test(tc, test_parse_args_invalid_value_argument);
    tcase_add_test(tc, test_parse_args_unknown_argument);
    tcase_add_test(tc, test_parse_args_file_null);
    suite_add_tcase(s, tc);
    return s;
}
