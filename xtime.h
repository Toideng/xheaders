#ifndef XTIME_H
#define XTIME_H

#include <time.h>
#include "xtypes.h"


typedef union
{
	// Technically it is a 128-bit integer number in two's complement
	// interpreted as 128.64 fixed-point fraction
	struct
	{
		u64 frac; // Fractions of a second since last integer second
		i64 secs; // Seconds since 2000-01-01
	};
	byte data[16];
} uts_t; // "Universal TimeStamp"

typedef struct
{
	/* A real timestamp corresponding to an actual point in time */
	uts_t stamp;
	/* `adjust` is added (sign-awarily) to the raw hardware clock to get the stamp */
	uts_t adjust;
} utsa_t; // "Universal TimeStamp with Adjustment"



utsa_t
utsa_createnew(
);

utsa_t *
utsa_get(
	utsa_t *astamp
);

/*

How `struct tm` is used here:

```
struct tm {
	int tm_sec;    // Seconds (0-59)
	int tm_min;    // Minutes (0-59)
	int tm_hour;   // Hours (0-23)
	int tm_mday;   // Day of the month (0-30)
	int tm_mon;    // Month (0-11)
	int tm_year;   // Year - 2000
	int tm_wday;   // (ignored on input) Day of the week (0-6, Monday = 0)
	int tm_yday;   // Day in the year (0-365, 1 Jan = 0)
	int tm_isdst;  // Nanoseconds (it's the only place to put them)
};
```

Notice change from leap seconds, mondays, origin at 2000, DoM from 0 and
nanoseconds

*/

struct tm *
uts_gmtime_r(
	uts_t *stamp,
	struct tm *result
);

uts_t
uts_mktime(
	struct tm *tm
);



// Both uts_sub and uts_add guarantee correct behaviour if result coincides with
// either a or b or both

uts_t *
uts_sub(
	uts_t *result, // = a - b
	uts_t *a,
	uts_t *b
);

uts_t *
uts_add(
	uts_t *result, // = a + b
	uts_t *a,
	uts_t *b
);



void
uts_sleep(
	uts_t *uts
);



#endif /* XTIME_H */
