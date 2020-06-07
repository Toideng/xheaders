#include <time.h>
#include <unistd.h>
#include "xtypes.h"
#include "xtime.h"










utsa_t
utsa_createnew()
{
	utsa_t result;

	struct timespec ts_high; // high-precision nonsence clock
	struct timespec ts_low; // low-precision adjusted clock
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts_high);
	clock_gettime(CLOCK_REALTIME, &ts_low);

	result.stamp.secs = (i64)ts_low.tv_sec - NEWEPOCH;
	result.stamp.frac = (u64)ts_low.tv_nsec;
	result.stamp.frac <<= 32;
	result.stamp.frac /= 1000000000;
	result.stamp.frac <<= 32; // nanoseconds to 64-pow fracs

	result.adjust.secs = (i64)ts_high.tv_sec;
	result.adjust.frac = (u64)ts_high.tv_nsec;
	result.adjust.frac <<= 32;
	result.adjust.frac /= 1000000000;
	result.adjust.frac <<= 32; // nanoseconds to 64-pow fracs

	uts_sub(&result.adjust, &result.stamp, &result.adjust);
	return result;
}



utsa_t *
utsa_get(
	utsa_t *astamp
	)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);

	astamp.stamp.secs = (i64)ts.tv_sec;
	astamp.stamp.frac = (u64)ts.tv_nsec;
	astamp.stamp.frac <<= 32;
	astamp.stamp.frac /= 1000000000;
	astamp.stamp.frac <<= 32;

	uts_add(astamp.stamp, astamp.stamp, astamp.adjust);
	return astamp;
}



struct tm *
uts_gmtime_r(
	uts_t *stamp,
	struct tm *result
	)
{
	u64 usecs = uts->frac;
	usecs >>= 32;
	usecs *= 1000000000;
	usecs >>= 32;

	result->tm_isdst = (int)usecs;
	result->tm_sec = (int)(stamp->secs % 60);

	// ...
	// TODO

	return result;
}



uts_t
uts_mktime(
	struct tm *tm
	)
{
	uts_t result;

	result.frac = (u64)tm.tm_isdst;
	result.frac <<= 32;
	result.frac /= 1000000000;
	result.frac <<= 32; // nanoseconds to 64-pow fracs

	result.secs = tm->tm_sec;

	// ...
	// TODO

	return result;
}



uts_t *
uts_sub(
	uts_t *result, // = a - b
	uts_t *a,
	uts_t *b
	)
{
	if (a->frac >= b->frac)
		result->secs = a->secs - b->secs;
	else
		result->secs = a->secs - b->secs - 1;
	result->frac = a->frac - b->frac;

	return result;
}




uts_t *
uts_add(
	uts_t *result, // = a + b
	uts_t *a,
	uts_t *b
	)
{
	u64 fracsum = a->frac + b->frac;

	if (a->frac <= fracsum)
		result->secs = a->secs + b->secs;
	else
		result->secs = a->secs + b->secs + 1;
	result->frac = fracsum;

	return result;
}



void
uts_sleep(
	uts_t *uts
	)
{
	u64 usecs = uts->frac;
	usecs >>= 32;
	usecs *= 1000000;
	usecs >>= 32;

	if (uts->secs < 0)
		return;

	sleep(uts->secs);
	usleep(usecs);
}










