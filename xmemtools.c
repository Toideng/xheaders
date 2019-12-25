#include <string.h>
#include "xutils.h"
#include "xmalloc.h"
#include "xxhash.h"
#include "xmemtools.h"










struct memory_arena
init_arena(
	size_t new_arena_size,
	void *arena_storage) // Specify this to avoid allocation
{
	assert(new_arena_size);
	struct memory_arena result;

	result.len = 0;
	result.cap = new_arena_size;
	if (arena_storage)
		result.p = arena_storage;
	else
	{
		result.p = xmalloc(new_arena_size, 1);
		if (!result.p)
		{
			fprintf(stderr,
				"Failed to allocate memory for new arena\xa");
			assert(0);
		}
	}
	memset(result.p, 0, result.cap);

	return result;
}



void *
push_object(
	struct memory_arena *arena,
	size_t len)
{
	if (arena->len + len > arena->cap)
	{
		// Fail: ran out of arena
		fprintf(stderr, "[push_object]: ran out of arena\xa");
		return 0;
	}

	void *result = arena->p + arena->len;
	arena->len += len;
	return result;
}



void *
push_array(
	struct memory_arena *arena,
	size_t len,
	size_t count)
{
	return push_object(arena, len * count);
}










struct string_pool
init_string_pool(
	struct memory_arena *arena)
{
	assert(arena);

	struct string_pool result;

	result.arena = arena;
	memset(result.hash_table, 0, sizeof result.hash_table);

	return result;
}



// If string is not interned, returns 0
byte *
__get_interned_string(
	struct string_pool *pool,
	byte const *s,
	ssize_t s_len, // if [len] < 0, length is deduced from null-termd [s]
	_out_ struct string_pool_entry ***_pnext)

{
	assert(s);

	size_t len;
	if (s_len < 0)
		len = strlen(s);
	else
		len = (size_t)s_len;

	size_t off = 0;
	byte *result = 0;
	u32 hash = (u32)xxhash64(s, len, 0);

	struct string_pool_entry **pnext = pool->hash_table + (hash & 0xff);
	struct string_pool_entry *next = *pnext;

	while (next)
	{
		if (next->hash == hash
		 && next->len  == len
		 && !memcmp(next->p, s, len))
		{
			return next->p;
		}
		pnext = &next->next;
		next = *pnext;
	}

	if (_pnext)
		*_pnext = pnext;
	return 0;
}



byte *
intern_string(
	struct string_pool *pool,
	byte const *s,
	ssize_t s_len) // if [len] < 0, length is deduced from null-termd [s]
{
	assert(s);

	size_t len;
	if (s_len < 0)
		len = strlen(s);
	else
		len = (size_t)s_len;

	struct string_pool_entry **pnext;
	byte *result = __get_interned_string(pool, s, len, &pnext);
	if (result)
		return result;

	*pnext = push_object(pool->arena, sizeof (struct string_pool_entry));
	result = push_object(pool->arena, len + 1);
	if (!(*pnext && result))
	{
		fprintf(stderr, "[intern_string]: ran out of arena\xa");
		return 0;
	}
	next = *pnext;

	next->p = result;
	next->len = (u32)len;
	next->hash = hash;
	next->next = 0;

	memcpy(result, s, len);
	*(result + len) = 0;

	return result;
}
