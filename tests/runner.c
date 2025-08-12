#include <check.h>
#include <stdlib.h>
#include <stdio.h>

Suite *utils_suite(void);
Suite *linked_list_suite(void);
Suite *dns_module_suite(void);
Suite *main_parsing_suite(void);

int main(void)
{
    int number_failed = 0;
    SRunner *sr = srunner_create(utils_suite());
    srunner_add_suite(sr, linked_list_suite());
    srunner_add_suite(sr, dns_module_suite());
    srunner_add_suite(sr, main_parsing_suite());
    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    if (number_failed == 0)
    {
        printf("\nAll tests passed!\n");
        return EXIT_SUCCESS;
    }
    else
    {
        printf("\nTests failed: %d\n", number_failed);
        return EXIT_FAILURE;
    }
}
