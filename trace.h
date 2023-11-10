#ifndef _TRACE_H
#define _TRACE_H

#define notrace __attribute__((no_instrument_function))

void notrace __sanitizer_cov_trace_pc(void);

void notrace __sanitizer_cov_trace_pc(void);

#endif
