#include <stdio.h>
#include "xutils.h"





void print_verbose(
	void const *buffer,
	size_t buflen,
	int multiline,
	FILE *f)
{
	byte *buf = (byte *)buffer;
	int ascii = 1;
	int hasnewline = 0;
	for (size_t i = 0;  i < buflen;  i++)
	{
		char c = buf[i];
		hasnewline = 0;
		if (c > 0x1F && c < 0x7F)
		{
			if (ascii)
				fputc(c, f);
			else
			{
				fputs(">\x1b[0m", f);
				ascii = 1;
				fputc(c, f);
			}
		}
		else if (c == 0x0a)
		{
			hasnewline = 1;
			if (!multiline)
			{
				if(ascii)
				{
					ascii = 0;
					fputs("\x1b[34m<0x0a", f);
				}
				else
					fputs(" 0x0a", f);
			}
			else
			{
				if (ascii)
				{
					fputs("\x1b[34m<0x0a>\x1b[0m\n", f);
				}
				else
				{
					ascii = 1;
					fputs(" 0x0a>\x1b[0m\n", f);
				}
			}
		}
		else
		{
			if(ascii)
			{
				ascii = 0;
				fputs("\x1b[34m<", f);
				fprintf(f, "0x%02x", (byte)c);
			}
			else
				fprintf(f, " 0x%02x", (byte)c);
		}
	}
	if (!ascii)
		fputs(">\x1b[0m", f);
	if (multiline && !hasnewline)
		fputs("\n", f);
}










size_t split_buf_in_lines(
	void *buffer,
	size_t buflen,
	struct buf *lines,
	size_t maxlines,
	int crop_0xd)
{
	assert(buffer);
	assert(lines);
	assert(maxlines);

	byte *buf = (byte*)buffer;

	size_t n = 0;
	lines[n].p = buf;
	lines[n].len = 0;
	lines[n].cap = 0;

	if (!buflen)
		return 1;

	if (!crop_0xd)
	{
		size_t i;
		for (i = 0;  i < buflen;  i++)
		{
			if (buf[i] == 0xa)
			{
				n++;
				if (n == maxlines)
					break;
				lines[n].p = buf + i + 1;
				lines[n].len = 0;
				lines[n].cap = 0;
				continue;
			}
			lines[n].len++;
		}

		if (n == maxlines || lines[n].len == 0)
			return n;
		return n + 1;
	}



	enum
	{
		st_new,
		st_semi,
		st_normal,
	} state = st_new;

	for (size_t i = 0;  i < buflen;  i++)
	{
		if (buf[i] == 0xa)
		{
			n++;

			if (n == maxlines)
				return maxlines;

			lines[n].p = buf + i + 1;
			lines[n].len = 0;
			lines[n].cap = 0;

			state = st_new;
			continue;
		}

		if (state == st_semi)
		{
			if (buf[i] != 0xd)
			{
				state = st_normal;
				lines[n].len++;
			}
			lines[n].len++;
			continue;
		}

		if (buf[i] == 0xd)
		{
			state = st_semi;
			continue;
		}

		lines[n].len++;
		if (state != st_normal)
			state = st_normal;
	}

	if (state == st_semi)
		lines[n].len++;

	if (state == st_new)
	{
		return n;
	}
	return n + 1;
}










int
is_hexadigit(char c)
{
	if ((c >= '0' && c <= '9')
	 || (c >= 'a' && c <= 'f')
	 || (c >= 'A' && c <= 'F'))
		return 1;
	return 0;
}
int
hexadigit_decode(char digit)
{
	if (digit >= '0' && digit <= '9')
		return digit - '0';
	if (digit >= 'a' && digit <= 'f')
		return digit - 'a' + 10;
	if (digit >= 'A' && digit <= 'F')
		return digit - 'A' + 10;
	return 0;
}










size_t
GCD__size_t(
	size_t a,
	size_t b)
{
	if (a == b)
		return a;

	if (a == 0)
		return b;

	if (b == 0)
		return a;

	if (a < b)
	{
		size_t temp = a;
		a = b;
		b = temp;
	}

	size_t r = a % b;
	while (r)
	{
		a = b;
		b = r;
		r = a % b;
	}

	return b;
}










i64
get_timestamp()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (1000000000 * (i64)ts.tv_sec + (i64)ts.tv_nsec);
}
