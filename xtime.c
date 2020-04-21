#include <time.h>
#include <unistd.h>
#include "xtime.h"










struct timestamp
timestamp_get()
{
	struct timestamp result;
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);

	result.secs = (i64)ts.tv_sec - 946684800LL; // seconds since 2000-01-01
	result.frac = (u64)ts.tv_nsec;
	result.frac <<= 32;
	result.frac /= 1000000000;
	result.frac <<= 32; // nanoseconds to 64-pow fracs

	return result;
}










// Returns a - b
struct timestamp
timestamp_diff(
	struct timestamp a,
	struct timestamp b)
{
	struct timestamp result;

	if (a.frac >= b.frac)
	{
		result.secs = a.secs - b.secs;
		result.frac = a.frac - b.frac;
	}
	else
	{
		result.secs = a.secs - b.secs - 1;
		result.frac = a.frac - b.frac;
	}

	return result;
}



u32
timestamp_to_nsecs(
	struct timestamp stamp)
{
	u64 result = stamp.frac;
	result >>= 32;
	result *= 1000000000;
	result >>= 32;
	return ((u32)result);
}


void
sleep_timestamp(
	struct timestamp stamp)
{
	if (stamp.secs < 0)
		return;
	sleep(stamp.secs);
	stamp.secs = 0;
	usleep(timestamp_to_nsecs(stamp)/1000);
}
