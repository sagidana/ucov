#include <stdio.h>

#include "trace.h"

#define TRACED_PC (unsigned long)__builtin_return_address(0)
void notrace __sanitizer_cov_trace_pc(void)
{
    printf("code instrumented: %p\n", TRACED_PC);
}
