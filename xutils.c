#include <stdio.h>
#include "xtypes.h"
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
					fputs("\x1b[34m<0x0a>\x1b[0m\xa", f);
				}
				else
				{
					ascii = 1;
					fputs(" 0x0a>\x1b[0m\xa", f);
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
		fputs("\xa", f);
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










struct HSVA
rgba_to_HSVA(
	f32 r,
	f32 g,
	f32 b,
	f32 a
	)
{
	struct RGBA in = {r, g, b, a, };
	return RGBA_to_HSVA(in);
}

struct HSVA
RGBA_to_HSVA(
	struct RGBA in
	)
{
	struct HSVA out;
	out.a = in.a;

	assert(in.r >= 0. && in.r <= 1.);
	assert(in.g >= 0. && in.g <= 1.);
	assert(in.b >= 0. && in.b <= 1.);
	assert(in.a >= 0. && in.a <= 1.);

	f32 min = MIN(MIN(in.r, in.g), in.b);
	f32 max = MAX(MAX(in.r, in.g), in.b);
	out.v = max;

	f32 delta = max - min;
	if (delta < 1E-5)
	{
		out.s = 0;
		out.h = 0; // undefined, maybe nan?
		return out;
	}

	if (max < 1E-5)
	{
		// if max is 0, then r = g = b = 0
		// s = 0, h is undefined
		out.s = 0.0;
		out.h = 0.0;
		return out;
	}

	out.s = (delta / max);

	if      (in.r >= max) // > is bogus, just keeps compilor happy
		out.h =       (in.g - in.b) / delta;
	else if (in.g >= max)
		out.h = 2.0 + (in.b - in.r) / delta;
	else // (in.b >= max)
		out.h = 4.0 + (in.r - in.g) / delta;

	out.h *= 60.0;
	if (out.h < 0.0)
		out.h += 360.0;

	return out;
}



struct RGBA
hsva_to_RGBA(
	f32 h,
	f32 s,
	f32 v,
	f32 a
	)
{
	struct HSVA in = {h, s, v, a, };
	return HSVA_to_RGBA(in);
}

struct RGBA
HSVA_to_RGBA(
	struct HSVA in
	)
{
	assert(in.h >= 0. && in.h < 360.);
	assert(in.s >= 0. && in.s <= 1.);
	assert(in.v >= 0. && in.v <= 1.);
	assert(in.a >= 0. && in.a <= 1.);

	struct RGBA out;
	out.a = in.a;

	if (in.s <= 0.) // < is bogus, just shuts up warnings
	{
		out.r = in.v;
		out.g = in.v;
		out.b = in.v;
		return out;
	}

	f32 hh = in.h / 60.;
	int i = (int)hh;
	f32 ff = hh - i;

	f32 p = in.v * (1. - (in.s));
	f32 q = in.v * (1. - (in.s * (ff)));
	f32 t = in.v * (1. - (in.s * (1. - ff)));

	switch (i)
	{
	case 0:
		out.r = in.v;
		out.g = t;
		out.b = p;
		break;

	case 1:
		out.r = q;
		out.g = in.v;
		out.b = p;
		break;

	case 2:
		out.r = p;
		out.g = in.v;
		out.b = t;
		break;

	case 3:
		out.r = p;
		out.g = q;
		out.b = in.v;
		break;

	case 4:
		out.r = t;
		out.g = p;
		out.b = in.v;
		break;

	case 5:
	default:
		out.r = in.v;
		out.g = p;
		out.b = q;
		break;
	}

	return out;
}
