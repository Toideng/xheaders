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

#define print_verbose_buffer(b, multiline, fout) print_verbose((b).p, (b).len, multiline, fout)
#define print_verbose_fixedbuffer(b, multiline, fout) print_verbose((b).p, (b).len, multiline, fout)




int is_hexadigit(char c);
int hexadigit_decode(char digit);



// https://stackoverflow.com/questions/3018313/

struct RGBA
{
	f32 r; // a fraction in [0, 1]
	f32 g; // a fraction in [0, 1]
	f32 b; // a fraction in [0, 1]
	f32 a; // a fraction in [0, 1]
};

struct HSVA
{
	f32 h; // angle in [0, 360)
	f32 s; // a fraction in [0, 1]
	f32 v; // a fraction in [0, 1]
	f32 a; // a fraction in [0, 1]
};

struct HSVA rgba_to_HSVA(f32 r, f32 g, f32 b, f32 a);
struct HSVA RGBA_to_HSVA(struct RGBA in);

struct RGBA hsva_to_RGBA(f32 h, f32 s, f32 v, f32 a);
struct RGBA HSVA_to_RGBA(struct HSVA in);










#endif /* XUTILS_H */
