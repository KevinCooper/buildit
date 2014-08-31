#include "minunit.h"
#include "../src/args.h"
#include "../src/args.c"
#include "../src/definitions.h"
#include "../src/logappend.c"
#include "../src/logappend.h"
#include <unistd.h>
#include <string.h>

char *test_parse_args_basic_1() {
	char *argv[] = { "program_name", "-T", "1", "-K", "secret", "-A", "-E", "Fred",
			"log1", NULL };
	int argc = 9;

	logappend_args args = opt_parser(argc, argv);
	mu_assert(args.returnStatus == 0,
			"Function did not return a good status code");
	mu_assert(args.timestamp == 1, "Timestamp was not converted correctly");
	mu_assert(!strcmp(args.logName, "log1"),
			"Filename did not process correctly");
	return NULL;
}
char *test_parse_args_basic_2() {
	char *argv[] = { "program_name", "-T", "2", "-K", "secret", "-A", "-G", "Jill",
			"log1", NULL };
	int argc = 9;

	logappend_args args = opt_parser(argc, argv);
	mu_assert(args.returnStatus == 0,
			"Function did not return a good status code");
	mu_assert(args.timestamp == 2, "Timestamp was not converted correctly");
	mu_assert(!strcmp(args.logName, "log1"),
			"Filename did not process correctly");
	return NULL;
}
char *test_parse_args_basic_3() {
	char *argv[] = { "program_name", "-T", "3", "-K", "secret", "-A", "-E", "Fred", "-R", "1",
			"log1", NULL };
	int argc = 11;

	logappend_args args = opt_parser(argc, argv);
	mu_assert(args.returnStatus == 0,
			"Function did not return a good status code");
	mu_assert(args.timestamp == 3, "Timestamp was not converted correctly");
	mu_assert(!strcmp(args.logName, "log1"),
			"Filename did not process correctly");
	return NULL;
}
char *test_parse_args_basic_4() {
	char *argv[] = { "program_name", "-T", "4", "-K", "secret", "-A", "-G", "Jill", "-R", "1",
			"log1", NULL };
	int argc = 11;

	logappend_args args = opt_parser(argc, argv);
	mu_assert(args.returnStatus == 0,
			"Function did not return a good status code");
	mu_assert(args.timestamp == 4, "Timestamp was not converted correctly");
	mu_assert(!strcmp(args.logName, "log1"),
			"Filename did not process correctly");
	return NULL;
}
char *test_parse_args_bad_flags() {
	char *argv[] = { "blah", "-A", "-L", NULL };
	int argc = 2;
	logappend_args args = opt_parser(argc, argv);
	mu_assert(args.returnStatus == -1, "Function did not return an error");
	return NULL;
}
char *test_log_append() {
	char *argv[] = { "program_name", "-T", "4", "-K", "secret", "-A", "-G", "Jill", "-R", "1",
			"log1", NULL };
	int argc = 11;

	run_me(argc, argv);
	return NULL;
}

char *all_tests() {
	mu_suite_start();

	mu_run_test(test_parse_args_basic_1);
	mu_run_test(test_parse_args_basic_2);
	mu_run_test(test_parse_args_basic_3);
	mu_run_test(test_parse_args_basic_4);
	mu_run_test(test_parse_args_bad_flags);
	mu_run_test(test_log_append);
	return NULL;
}

RUN_TESTS(all_tests);
