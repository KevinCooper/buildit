#include "minunit.h"
#include "../src/args.h"
#include "../src/definitions.h"
#include <unistd.h>
#include <string.h>
char *test_parse_args_1()
{
	char *argv[] = {"blah", "-T", "1234", "filename", NULL};
	int argc = 4;
	error_t *  status = NULL;

	logappend_args args = opt_parser( argc, argv, status);
	mu_assert(status == FUNC_OK, "Function did not return a good status code");
	mu_assert(args.timestamp == 1234, "Timestamp was not converted correctly");
	mu_assert(strcmp(args.logName, "filename"), "Filename did not process correctly");
    return NULL;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_parse_args_1);

    return NULL;
}

RUN_TESTS(all_tests);
