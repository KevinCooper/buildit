#ifndef ARGS_H_
#define ARGS_H_


#include "definitions.h"
#include "args.c"
#include <stdint.h>

//int opt_parser(int32_t argc, char **argv, error_t*  status);

logappend_args opt_parser(int32_t argc, char **argv, error_t*  status);

#endif /* ARGS_H_ */
