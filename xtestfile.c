#include <stdio.h>
#include <string.h>
#include "xmalloc.h"
#include "xtypes.h"
#include "xutils.h"

#include "xtestfile.h"










int
read_test_suite(
	char *name,
	struct fixedbuffer *in,
	struct fixedbuffer *out
	)
{
	if (!in && !out)
		return -1;

	size_t len = strlen(name);
	len += 0x10;
	char *catbuf = xmalloc(len, 1);

	in->p = 0;

	if (in)
	{
		sprintf(catbuf, "%s.in", name);
		FILE *f = fopen(catbuf, "rb");
		if (!f)
		{
			// No such file
			fprintf(stderr, "%s: file not found, stop\xa", catbuf);
			return -4;
		}
		fseek(f, 0, SEEK_END);
		ssize_t size = ftell(f);
		if (size <= 0)
		{
			// Must have something
			fclose(f);
			return -2;
		}
		rewind(f);

		in->p = xmalloc((size_t)size, 1);
		in->len = size;
		fread(in->p, 1, size, f);
		fclose(f);

		int res = strip_comments(in);
		if (res)
		{
			fprintf(stderr, " * Failed to strip comments in input: code [%d]\xa", res);
			xfree(in->p);
			return -3;
		}
	}

	if (out)
	{
		sprintf(catbuf, "%s.out", name);
		FILE *f = fopen(catbuf, "rb");
		if (!f)
		{
			// No such file
			fprintf(stderr, "%s: file not found, stop\xa", catbuf);
			if (in->p)
				xfree(in->p);
			return -4;
		}
		fseek(f, 0, SEEK_END);
		ssize_t size = ftell(f);
		if (size <= 0)
		{
			// Must have something
			if (in->p)
				xfree(in->p);
			fclose(f);
			return -2;
		}
		rewind(f);

		out->p = xmalloc((size_t)size, 1);
		out->len = size;
		fread(out->p, 1, size, f);
		fclose(f);

		int res = strip_comments(out);
		if (res)
		{
			fprintf(stderr, " * Failed to strip comments in output: code [%d]\xa", res);
			xfree(in->p);
			xfree(out->p);
			return -3;
		}
	}

	return 0;
}










int
strip_comments(
	struct fixedbuffer *buf
	)
{
	int has_b2sum = 0;
	byte b2sum[BLAKE2B_OUTBYTES] = {0};

	if (*buf->text != '*'
	 && *buf->text != '#')
	{
		// Must begin with a star or a hash sign
		return -1;
	}

	char *p = buf->p;
	char *end = p + buf->len;

	while (1) // Iterate over lines
	{
		// [p] point to first symbol of line

		if (p >= end)
		{
			// Must have an xaxa before EOF
			return -2;
		}

		if (*p == 0xa)
		{
			// Finally, xaxa found
			p++;
			break;
		}

		if (*p != '#'
		 && *p != '*')
		{
			return -3;
		}

		if (*p == '#')
		{
			if (has_b2sum)
			{
				// Only one hash can be in the file
				return -5;
			}
			p++;
			size_t i = 0;

			while (p < end && *p != 0xa)
			{
				if (!is_hexadigit(*p))
					return -4;
				if (i >= 2 * BLAKE2B_OUTBYTES)
					return -4;

				byte b = hexadigit_decode(*p);
				b2sum[i / 2] |= b << (4 * (1 - (i % 2)));

				i++;
				p++;
			}
			
			if (p == end)
				return -2;
			if (i < 2 * BLAKE2B_OUTBYTES)
				return -4;

			has_b2sum = 1;

			p++; // [p] can only point to a newline character
			continue;
		}

		// Only '*' can now be pointed by [p]. Advance to next line.
		while (p < end && *p != 0xa)
			p++;
		if (p == end)
			return -2;
		p++; // Advance to start of next line
	}

	if ((buf->len = end - p))
		memmove(buf->p, p, buf->len);

	if (has_b2sum)
	{
		byte b2sum_derived[BLAKE2B_OUTBYTES];
		blake2b(
			b2sum_derived, BLAKE2B_OUTBYTES,
			buf->p, buf->len,
			0, 0, 0);

		if (memcmp(b2sum, b2sum_derived, BLAKE2B_OUTBYTES))
			return -6;
	}

	return 0;
}
