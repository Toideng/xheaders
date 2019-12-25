#ifndef XMEMTOOLS_H
#define XMEMTOOLS_H

#include "xtypes.h"



struct memory_arena
{
	size_t cap;
	size_t len;
	byte *p;
};



struct memory_arena
init_arena(
	size_t new_arena_size,
	void *arena_storage // Specify this to avoid allocation
);

void *
push_object(
	struct memory_arena *arena,
	size_t len
);

void *
push_array(
	struct memory_arena *arena,
	size_t len,
	size_t count
);










struct string_pool_entry
{
	byte *p;
	u32 len;
	u32 hash;
	struct string_pool_entry *next;
};

struct string_pool
{
	struct memory_arena *arena;
	struct string_pool_entry *hash_table[0x100];
};



struct string_pool
init_string_pool(
	struct memory_arena *arena
);

// If string is not interned, returns 0

#define get_interned_string(pool, s, len)\
	__get_interned_string(pool, s, len, 0)

byte *
__get_interned_string(
	struct string_pool *pool,
	byte const *s,
	ssize_t len, // if [len] < 0, length is deduced from null-termd [s]
	_out_ struct string_pool_entry ***pnext // Only set if 0 is returned
);

byte *
intern_string(
	struct string_pool *pool,
	byte const *s,
	ssize_t len // if [len] < 0, length is deduced from null-termd [s]
);



#endif /* XMEMTOOLS_H */
