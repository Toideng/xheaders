#include <stdio.h>
#include "xutils.h"





void print_verbose(
	void const *pbuf,
	size_t buflen,
	int multiline,
	FILE *f)
{
	byte *buf = (byte *)pbuf;
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
