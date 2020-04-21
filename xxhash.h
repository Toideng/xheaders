#ifndef XXHASH64_H
#define XXHASH64_H

#include "xtypes.h"
#include "xutils.h"



enum xxhash64_constant
{
	XXHASH64_BLOCKBYTES    = 32,
	XXHASH64_OUTBYTES      = 8,
	XXHASH64_SEEDBYTES     = 8,
};

struct xxhash64_state
{
	u64 state[4];
	byte buf[XXHASH64_BLOCKBYTES];
	size_t buflen;
	size_t total_length;
};



int
xxhash64_init(
	struct xxhash64_state *S,
	u64 seed
);

int
xxhash64_update(
	struct xxhash64_state *S,
	void const* pin,
	size_t inlen
);

int
xxhash64_final(
	struct xxhash64_state *S,
	u64 *out
);

u64
xxhash64(
	void const *in,
	size_t inlen,
	u64 seed
);










#ifdef XXHASH64_IMPLEMENTATION
#include <string.h>

u64 const xxhash64_prime1 = 0x9E3779B185EBCA87ull;
u64 const xxhash64_prime2 = 0xC2B2AE3D27D4EB4Full;
u64 const xxhash64_prime3 = 0x165667B19E3779F9ull;
u64 const xxhash64_prime4 = 0x85EBCA77C2B2AE63ull;
u64 const xxhash64_prime5 = 0x27D4EB2F165667C5ull;

static inline u64
xxhash64_internal_rotl64(
	u64 const w,
	u32 const c)
{
	return ( w >> (64 - c) ) | ( w << c );
}

#define PROCESS_SINGLE(prev, in)\
	(xxhash64_internal_rotl64((prev) + (in) * xxhash64_prime2, 0x1f) * xxhash64_prime1)

// data should be (u64*)
#define PROCESS_BLOCK(data, s0, s1, s2, s3)\
	(s0) = PROCESS_SINGLE((s0), (data)[0]);\
	(s1) = PROCESS_SINGLE((s1), (data)[1]);\
	(s2) = PROCESS_SINGLE((s2), (data)[2]);\
	(s3) = PROCESS_SINGLE((s3), (data)[3]);



int
xxhash64_init(
	struct xxhash64_state *S,
	u64 seed)
{
//	memset(S, 0, sizeof(struct xxhash64_state));
	S->buflen = 0;
	S->total_length = 0;

	S->state[0] = seed + xxhash64_prime1 + xxhash64_prime2;
	S->state[1] = seed + xxhash64_prime2;
	S->state[2] = seed;
	S->state[3] = seed - xxhash64_prime1;

	return 0;
}



int
xxhash64_update(
	struct xxhash64_state *S,
	void const* pin,
	size_t inlen)
{
	if (inlen > 0)
	{
		S->total_length += inlen;
		byte const* in = pin;

		size_t left = S->buflen;
		size_t fill = XXHASH64_BLOCKBYTES - left;

#if 1
		u64 s0 = S->state[0]
		  , s1 = S->state[1]
		  , s2 = S->state[2]
		  , s3 = S->state[3];

		if (inlen >= fill)
		{
			S->buflen = 0;
			memcpy(S->buf + left, in, fill);
			PROCESS_BLOCK(((u64*)(S->buf)), s0, s1, s2, s3);
			in += fill;
			inlen -= fill;
			while (inlen >= XXHASH64_BLOCKBYTES)
			{
				PROCESS_BLOCK(((u64*)(in)), s0, s1, s2, s3);
				in += XXHASH64_BLOCKBYTES;
				inlen -= XXHASH64_BLOCKBYTES;
			}
		}
		memcpy(S->buf + S->buflen, in, inlen);
		S->buflen += inlen;
		S->state[0] = s0;
		S->state[1] = s1;
		S->state[2] = s2;
		S->state[3] = s3;
#else
		if (inlen >= fill)
		{
			S->buflen = 0;
			memcpy(S->buf + left, in, fill);
			PROCESS_BLOCK(((u64*)(S->buf)), S->state[0], S->state[1], S->state[2], S->state[3]);
			in += fill;
			inlen -= fill;
			while (inlen >= XXHASH64_BLOCKBYTES)
			{
				PROCESS_BLOCK(((u64*)(in)), S->state[0], S->state[1], S->state[2], S->state[3]);
				in += XXHASH64_BLOCKBYTES;
				inlen -= XXHASH64_BLOCKBYTES;
			}
		}
		memcpy(S->buf + S->buflen, in, inlen);
		S->buflen += inlen;
#endif
	}
	return 0;
}



int
xxhash64_final(
	struct xxhash64_state *S,
	u64 *out)
{
	u64 result;

	if (S->total_length >= XXHASH64_BLOCKBYTES)
	{
		result = xxhash64_internal_rotl64(S->state[0], 1)
		       + xxhash64_internal_rotl64(S->state[1], 7)
		       + xxhash64_internal_rotl64(S->state[2], 12)
		       + xxhash64_internal_rotl64(S->state[3], 18);
		result = (result ^ PROCESS_SINGLE(0, S->state[0])) * xxhash64_prime1 + xxhash64_prime4;
		result = (result ^ PROCESS_SINGLE(0, S->state[1])) * xxhash64_prime1 + xxhash64_prime4;
		result = (result ^ PROCESS_SINGLE(0, S->state[2])) * xxhash64_prime1 + xxhash64_prime4;
		result = (result ^ PROCESS_SINGLE(0, S->state[3])) * xxhash64_prime1 + xxhash64_prime4;
	}
	else
	{
		result = S->state[2] + xxhash64_prime5;
	}

	result += S->total_length;
	byte *data = S->buf;
	byte *stop = data + S->buflen;

	for (;  data + 8 <= stop;  data += 8)
		result = xxhash64_internal_rotl64(result ^ PROCESS_SINGLE(0, *(u64*)data), 27) * xxhash64_prime1 + xxhash64_prime4;

	if (data + 4 <= stop)
	{
		result = xxhash64_internal_rotl64(result ^ (*(u32*)data) * xxhash64_prime1, 23) * xxhash64_prime2 + xxhash64_prime3;
		data += 4;
	}

	while (data != stop)
		result = xxhash64_internal_rotl64(result ^ (*data++) * xxhash64_prime5, 11) * xxhash64_prime1;

	result ^= result >> 33;
	result *= xxhash64_prime2;
	result ^= result >> 29;
	result *= xxhash64_prime3;
	result ^= result >> 32;

	*out = result;

	return 0;
}



u64
xxhash64(
	void const *in,
	size_t inlen,
	u64 seed)
{
	struct xxhash64_state S;
	u64 result;
	xxhash64_init(&S, seed);
	xxhash64_update(&S, in, inlen);
	xxhash64_final(&S, &result);
	return result;
}



#undef PROCESS_SINGLE
#undef PROCESS_BLOCK

#endif /* XXHASH64_IMPLEMENTATION */

#endif /* XXHASH64_H */
