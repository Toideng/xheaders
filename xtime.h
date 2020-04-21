#ifndef XTIME_H
#define XTIME_H

#include "xtypes.h"


struct timestamp
{
	i64 secs; // Seconds since 2000-01-01
	u64 frac; // Fractions of a second since last integral second
};



struct timestamp
timestamp_get();



// Returns a - b
struct timestamp
timestamp_diff(
	struct timestamp a,
	struct timestamp b
);



u32
timestamp_to_nsecs(
	struct timestamp stamp
);



void
sleep_timestamp(
	struct timestamp stamp
);



#endif /* XTIME_H */
