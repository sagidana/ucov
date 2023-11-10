#include <stdio.h>

#include "ucov.h"


#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)

// ------------------------------------------------------------------------------------
// barriers
// ------------------------------------------------------------------------------------

#define __scalar_type_to_expr_cases(type)				\
		unsigned type:	(unsigned type)0,			\
		signed type:	(signed type)0

#define __unqual_scalar_typeof(x) typeof(				\
		_Generic((x),						\
			 char:	(char)0,				\
			 __scalar_type_to_expr_cases(char),		\
			 __scalar_type_to_expr_cases(short),		\
			 __scalar_type_to_expr_cases(int),		\
			 __scalar_type_to_expr_cases(long),		\
			 __scalar_type_to_expr_cases(long long),	\
			 default: (x)))

#define __READ_ONCE(x)	(*(const volatile __unqual_scalar_typeof(x) *)&(x))
#define READ_ONCE(x)							\
({									\
	__READ_ONCE(x);							\
})

#define __WRITE_ONCE(x, val)						\
do {									\
	*(volatile typeof(x) *)&(x) = (val);				\
} while (0)

#define WRITE_ONCE(x, val)						\
do {									\
	__WRITE_ONCE(x, val);						\
} while (0)


# define barrier() __asm__ __volatile__("": : :"memory")

#define ia64_mf()	asm volatile ("mf" ::: "memory")
#define mb()		ia64_mf()
#define rmb()		mb()
#define wmb()		mb()

/*
 * IA64 GCC turns volatile stores into st.rel and volatile loads into ld.acq no
 * need for asm trickery!
 */

#define __smp_store_release(p, v)						\
do {									\
	barrier();							\
	WRITE_ONCE(*p, v);						\
} while (0)

#define __smp_load_acquire(p)						\
({									\
	typeof(*p) ___p1 = READ_ONCE(*p);				\
	barrier();							\
	___p1;								\
})

// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------


static ucov_t ucov = {0};

#define TRACED_PC (unsigned long)__builtin_return_address(0)

void notrace __sanitizer_cov_trace_pc(void)
{
	unsigned long pos;
	unsigned long ip = TRACED_PC;
	// not enabled
	if (!__smp_load_acquire(&ucov.enabled)) return;

	// TODO: fix ASLR

	printf("code instrumented: %p\n", (void*)ip);

	pos = READ_ONCE(ucov.buf[0]) + 1;
	if (likely(pos < (ucov.buf_len / sizeof(void*)) - 1)) {
		WRITE_ONCE(ucov.buf[0], pos);
		barrier();
		ucov.buf[pos] = ip;
	}
}

int notrace ucov_init(void *buf, unsigned int buf_len)
{
	ucov.buf = buf;
	ucov.buf_len = buf_len;
	ucov.enabled = 0;
	return 0;
}

int notrace ucov_destroy(void)
{
	ucov.buf = NULL;
	ucov.buf_len = 0;
	ucov.enabled = 0;
	return 0;
}

int notrace ucov_enable(void)
{
	__smp_store_release(&ucov.enabled, 1);
}
int notrace ucov_disable(void)
{
	__smp_store_release(&ucov.enabled, 0);
}
