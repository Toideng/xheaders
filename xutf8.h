#ifndef UTF8IO_H
#define UTF8IO_H

#include "xtypes.h"



// Returns negative if [*s] points to illegal sequence
i32
utf8_nextchar_strict(
	_inout_ byte **s,
	byte *end
);

// A non-strict char is either:
//   --- An 0XXX'XXXX byte
//   --- A sequence like 1XXX'XXXX {10XX'XXXX} (i.e. sequence is either
//       terminated by byte 0XXX'XXXX, byte 11XX'XXXX or end of input, thusly
//       interpretation of two strings may be different for them separatly and
//       for result of their concatenation), where lower bits of every byte are
//       concatenated in a special way

u32
utf8_nextchar(
	byte **s,
	byte *end
);



int
utf8_is_valid_str(
	byte *s,
	size_t len
);



#endif
