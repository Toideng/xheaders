#ifndef XUTILS_H
#define XUTILS_H

#include "xtypes.h"
#include <stdio.h>
#include <time.h>

#define MAKEUSED(x) ((void)(x))
#define MIN(a, b) (((a)<(b))?(a):(b))
#define MAX(a, b) (((a)>(b))?(a):(b))
#define FMOD(a, b)\
	(((a) >= 0) ? fmod((a), (b)) : (fabs(b) + fmod((a), (b))))

#ifndef offsetof
#define offsetof(type, member)  __builtin_offsetof (type, member)
#endif

#define internal static



#ifndef RELEASE
#  define assert(cond)\
((cond)?0:_assert_failed(__FILE__, __LINE__));

#pragma GCC diagnostic ignored "-Wunused-function"
static int _assert_failed(char const *file, int line)
{
	fprintf(stderr, "Assert in file `%s`, line %d failed\xa", file, line);
	*((char*)0) = 0;
	return 0;
}
#pragma GCC diagnostic pop

#else
#  define assert(cond)
#endif



#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif



void print_verbose(
	void const *buffer,
	size_t buflen,
	int multiline,
	FILE *fout
);



// Return # of lines (guaranteed to be > 0)
size_t split_buf_in_lines(
	void *buffer,
	size_t buflen,
	struct buf *lines, // Preallocated buffer for line headers
	size_t maxlines,    // How many headers can [lines] fit or less
	int crop_0xd
);




int is_hexadigit(char c);
int hexadigit_decode(char digit);



size_t
GCD__size_t(
	size_t a,
	size_t b
);



i64
get_timestamp();



#endif /* XUTILS_H */
