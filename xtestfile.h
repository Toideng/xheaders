#ifndef TESTFILE_H
#define TESTFILE_H

#include "xtypes.h"
#include "blake2b.h"



/*
 * Reads in <name>.in and/or <name>.out and strips preambles
 * Set [in] or [out] to 0 if there's no need to read corresponding file
 */

int
read_test_suite(
	char *name,             // path, as in fopen, but w/o ".in" or ".out"
	struct fixedbuffer *in, // malloced
	struct fixedbuffer *out // malloced
);

// Checks b2sum (if any) and mommoves over comment/sum
int
strip_comments(
	struct fixedbuffer *buf
);



#endif /* TESTFILE_H */
