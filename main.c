#include <stdio.h>
#include <dlfcn.h>
#include <errno.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>


int (*ucov_init)(void *, unsigned int) = NULL;
int (*ucov_destroy)(void) = NULL;
int (*ucov_enable)(void) = NULL;
int (*ucov_disable)(void) = NULL;
unsigned long base = 0;

int (*sub)(int,int) = NULL;
int (*add)(int,int) = NULL;

size_t  get_library_address(const char*  libname)
{
    char path[256];
    char buff[256];
    int len_libname = strlen(libname);
    FILE* file;
    size_t  addr = 0;

    snprintf(path, sizeof path, "/proc/%d/smaps", getpid());
    file = fopen(path, "rt");
    if (file == NULL)
        return 0;

    while (fgets(buff, sizeof buff, file) != NULL) {
        int  len = strlen(buff);
        if (len > 0 && buff[len-1] == '\n') {
            buff[--len] = '\0';
        }
        if (len <= len_libname || memcmp(buff + len - len_libname, libname, len_libname)) {
            continue;
        }
        size_t start, end, offset;
        char flags[4];
        if (sscanf(buff, "%zx-%zx %c%c%c%c %zx", &start, &end,
                   &flags[0], &flags[1], &flags[2], &flags[3], &offset) != 7) {
            continue;
        }
        if (flags[0] != 'r' || flags[2] != 'x') {
            continue;
        }
        addr = start - offset;
        break;
    }
    fclose(file);
    return addr;
}
int init_load_lib(void)
{
	void *handle;
	handle = dlopen("./lib.so", RTLD_NOW);
	if (!handle){
		printf("failed to load lib.so: %s\n", dlerror());
		return 1;
	}
	base = get_library_address("lib.so");

	ucov_init = dlsym(handle, "ucov_init");
	if (!ucov_init){printf("dlsym failed: %s\n", dlerror()); return 1;}
	ucov_destroy = dlsym(handle, "ucov_destroy");;
	if (!ucov_destroy){printf("dlsym failed: %s\n", dlerror()); return 1;}
	ucov_enable = dlsym(handle, "ucov_enable");;
	if (!ucov_enable){printf("dlsym failed: %s\n", dlerror()); return 1;}
	ucov_disable = dlsym(handle, "ucov_disable");;
	if (!ucov_disable){printf("dlsym failed: %s\n", dlerror()); return 1;}

	sub = dlsym(handle, "sub");;
	if (!sub){printf("dlsym failed: %s\n", dlerror()); return 1;}
	add = dlsym(handle, "add");;
	if (!add){printf("dlsym failed: %s\n", dlerror()); return 1;}

	return 0;
}

#define UCOV_BUF_SIZE (1024*1024*5)
int main()
{
	if (init_load_lib()) return 1;
	unsigned long *ucov = mmap( NULL,
								UCOV_BUF_SIZE,
								PROT_READ | PROT_WRITE,
								MAP_PRIVATE|MAP_ANONYMOUS,
								-1, 0);
	if (ucov == MAP_FAILED){
		printf("mmap failed: %d\n", errno);
		return 1;
	}
	ucov_init(ucov, UCOV_BUF_SIZE);

	ucov_enable();
	printf("%d\n", sub(1,6));
	ucov_disable();
	printf("%d\n", sub(1,6));

	for (unsigned long i = 0; i < ucov[0]; i++){
		printf("%p\n", ucov[i+1]);
		printf("%p\n", ucov[i+1] - base);
	}

	ucov_destroy();
	munmap(ucov, UCOV_BUF_SIZE);


	return 0;
}
