#ifndef _UCOV_H
#define _UCOV_H

#define notrace __attribute__((no_instrument_function))

void notrace __sanitizer_cov_trace_pc(void);

typedef struct {
	unsigned long *buf;
	unsigned int buf_len;
	volatile unsigned int enabled;
}ucov_t;

int notrace ucov_init(void *buf, unsigned int buf_len);
int notrace ucov_destroy(void);
int notrace ucov_enable(void);
int notrace ucov_disable(void);

#endif
