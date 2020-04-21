#ifndef XUTILS_H
#define XUTILS_H

#include "xtypes.h"
#include <stdio.h>
#include <time.h>

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
	void const *buf,
	size_t buflen,
	int multiline,
	FILE *fout
);




int is_hexadigit(char c);
int hexadigit_decode(char digit);



#endif /* XUTILS_H */
