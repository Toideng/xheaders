#ifndef UTF8IO_H
#define UTF8IO_H

#include "xtypes.h"





// count_upperbits(0b'1101'0101) = 2

int
count_upperbits(
	byte b
);

// lowerbits(0b'1101'0101) = 0b'0001'0101

byte
lowerbits(
	byte b
);



// Counts number of valid UTF-8 codepoints in [input]
// Returns -1 if [input] is not a valid UTF-8 string

ssize_t
utf8_count_codepoints(
	fixedbuffer input
);

// Puts at most [count] codepoints from [input] to [output]
// Returns number of codepoints decoded

size_t
utf8_decode(
	fixedbuffer input,
	size_t count,
	u32 *output
);








#if 0
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



#endif
