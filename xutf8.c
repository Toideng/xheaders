#include <stdlib.h>
#include <stdio.h>

#include "xtupes.h"
#include "utf8io.h"
#include "xmalloc.h"



int
count_upperbits(
	byte b)
{
	byte mask = (byte)0x80;
	int result = 0;
	while (mask & b)
	{
		mask >>= 1;
		result++;
	}
	return result;
}



byte
lowerbits(
	byte b)
{
#if 0
	byte mask = (byte)0x80;
	while (mask & b)
	{
		b &= ~mask;
		mask >>= 1;
	}
	return b;
#else
	int n = count_upperbits(b);
	byte mask = (~((~0u)<<(8-n)))&0xff;
	return b & mask;
#endif
}










// Returns negative if [*s] points to illegal sequence
i32
utf8_nextchar_strict(
	_inout_ byte **s,
	byte *end)
{
	if (*s >= end)
		return -1;

	int n = count_upperbits(**s);
	if (!n)
		return (*(*s)++);
	if (n == 1 || n == 7 || n == 8)
		return -1;
	if (*s + n > end)
		return -1;

	i32 result = lowerbits(**s);
	for (int i = 1;  i < n;  i++)
	{
		if (count_upperbits((*s)[i]) != 1)
			return -1;
		result <<= 6;
		result |= lowerbits((*s)[i]);
	}

	*s += n;
	return result;
}



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
	byte *end)
{
	if (*s >= end)
		return 0;

	if (!(**s & 0x80))
		return (*(*s)++);

	u32 result = lowerbits(*(*s)++);
	while (1)
	{
		if (*s == end)
			break;
		byte b = **s;
		if (!(b & 0x80))
			break;
		if ((b & 0x40))
			break;

		result <<= 6;
		result |= lowerbits(b);
		++*s;
	}

	return result;
}



int
utf8_is_valid_str(
	byte *s,
	size_t len)
{
	int n = 0;
	for (size_t i = 0;  i < len;  i++)
	{
		int prefix = count_upperbits(s[i]);
		if (!n)
		{
			if (prefix == 1)
				return 0;
			if (prefix > 1)
				n = prefix;
		}
		else
		{
			if (prefix != 1)
				return 0;
		}

		if (n)
			n--;
	}
	if (!n)
		return 0;
	return 1;
}



#if 0
size_t
utf8_count_valid_characters(
	byte *s,
	size_t len)
{
	size_t result = 0;
	int n = 0;
	for (size_t i = 0;  i < len;  i++)
	{
		int prefix = count_upperbits(s[i]);
		if (!n)
		{
			if (prefix == 1)
				return 0;
			if (prefix > 1)
				n = prefix;

			if (prefix == 0)
			{
				result++;
				continue;
			}
		}
		else
		{
			if (prefix != 1)
				return 0;
		}

		if (n)
			n--;
	}
	if (!n)
		return 0;
	return 1;
}
#endif










#if 0
/* Converting string to data and back always returns the original string if it
 * had no characters with value < 0.
 *
 * Converting data to string and back may return data that signifitanctly
 * differs from the original data.
 */

string convert_data_to_string(data const *d)
{
	string result = {0, 0};
	size_t i;
	size_t result_length = 0;

	int first = 1;

	/* ESTIMATE REQUIRED MEMORY */

	for (i = 0;  i < d->length;  i++)
	{
		uint8_t c = d->bytes[i];

		if (c & 0x80)
		{
			if (first)
			{
				first = 0;
				result_length++;
			}
			else if (c & 0x40) result_length++;
		}
		else
		{
			result_length++;
			first = 1;
		}
	}

	result.length = result_length;
	result.symbols = xmalloc_(result.length * sizeof(uchar), "cdts");

	/* FILL IN */

	size_t x = 0;
	uchar accum = 0;
	first = 1;

	for (i = 0;  i < d->length;  i++)
	{
		uint8_t c = d->bytes[i];

		if (c & 0x80)
		{
			if (first)
			{
				first = 0;
				accum = (uchar)lowerbits(c);
			}
			else
			{
				if (c & 0x40)
				{
					result.symbols[x++] = (accum & 0x7FffFFff);
					accum = (uchar)lowerbits(c);
				}
				else
					accum = (accum << 6) | lowerbits(c);
			}
		}
		else
		{
			if (!first)
				result.symbols[x++] = (accum & 0x7FffFFff);
			first = 1;
			result.symbols[x++] = c;
		}
	}

	if (!first)
		result.symbols[x] = (accum & 0x7FffFFff);

	return result;
}










data
convert_string_to_data(
	string const *s)
{
	data result = {0, 0};
	size_t i;
	size_t result_length = 0;

	/* ESTIMATE REQUIRED MEMORY */

	for (i = 0;  i < s->length;  i++)
	{
		uchar c = s->symbols[i];
		if (c < 0) continue;
		if (c < 0x80)
		{
			result_length++;
			continue;
		}

		int j;
		for (j = 2;  j < 6;  j++)
			if (c < (1 << (1 + 5*j)))
			{
				result_length += j;
				break;
			}

		if (j == 6)
			result_length += j;
	}

	result.length = result_length;
	result.bytes = xmalloc_(result.length, "cstd");

	/* FILL IN THE MEMORY */

	size_t x = 0;

	for (i = 0;  i < s->length;  i++)
	{
		u32 c = s->symbols[i];
		if (c < 0x80)
		{
			result.bytes[x++] = c;
			continue;
		}

		if (c < (1 << (1 + 5*2)))
		{
			result.bytes[x++] = (0xc0 | ((c >> 6) & 0x3F));
			result.bytes[x++] = (0x80 | ((c >> 0) & 0x3F));
			continue;
		}

		if (c < (1 << (1 + 5*3)))
		{
			result.bytes[x++] = (0xe0 | ((c >> 12) & 0x3F));
			result.bytes[x++] = (0x80 | ((c >>  6) & 0x3F));
			result.bytes[x++] = (0x80 | ((c >>  0) & 0x3F));
			continue;
		}

		if (c < (1 << (1 + 5*4)))
		{
			result.bytes[x++] = (0xf0 | ((c >> 18) & 0x3F));
			result.bytes[x++] = (0x80 | ((c >> 12) & 0x3F));
			result.bytes[x++] = (0x80 | ((c >>  6) & 0x3F));
			result.bytes[x++] = (0x80 | ((c >>  0) & 0x3F));
			continue;
		}

		if (c < (1 << (1 + 5*5)))
		{
			result.bytes[x++] = (0xf8 | ((c >> 24) & 0x3F));
			result.bytes[x++] = (0x80 | ((c >> 18) & 0x3F));
			result.bytes[x++] = (0x80 | ((c >> 12) & 0x3F));
			result.bytes[x++] = (0x80 | ((c >>  6) & 0x3F));
			result.bytes[x++] = (0x80 | ((c >>  0) & 0x3F));
			continue;
		}

		result.bytes[x++] = (0xfc | ((c >> 30) & 0x01));
		result.bytes[x++] = (0x80 | ((c >> 24) & 0x3F));
		result.bytes[x++] = (0x80 | ((c >> 18) & 0x3F));
		result.bytes[x++] = (0x80 | ((c >> 12) & 0x3F));
		result.bytes[x++] = (0x80 | ((c >>  6) & 0x3F));
		result.bytes[x++] = (0x80 | ((c >>  0) & 0x3F));
	}

	return result;
}
#endif

