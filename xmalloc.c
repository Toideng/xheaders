#include <stdlib.h>
#include <stdio.h>
#include "xmalloc.h"

size_t xmalloc_alloced = 0;
size_t xmalloc_max = (((1 << 10) << 10) << 5); // 32 MiB

void* _xmalloc(size_t size)
{
	if (xmalloc_alloced + size > xmalloc_max)
	{
		fprintf(stderr, "Can't allocate more than %lu MiB\n", xmalloc_max >> 20);
		exit(EXIT_FAILURE);
	}

	void* result = malloc(size);
	if (!result)
	{
		fprintf(stderr, "[size:%lu]\n", size);
		perror("malloc");
		/* cleanup_pointers_registry(); */
		exit(EXIT_FAILURE);
	}

	/* pointers_registry_add(result); */
	xmalloc_alloced += size;
	return result;
}

void* _xrealloc(void* ptr, size_t new_size)
{
	if (xmalloc_alloced + new_size > xmalloc_max)
	{
		fprintf(stderr, "Can't allocate more than %lu MiB\n", xmalloc_max >> 20);
		exit(EXIT_FAILURE);
	}

	void* result = realloc(ptr, new_size);
	if (!result)
	{
		perror("realloc");
		/* cleanup_pointers_registry(); */
		exit(EXIT_FAILURE);
	}
	/* pointers_registry_update(ptr, result); */
	return result;
}

void _xfree(void* ptr)
{
	free(ptr);
	/*pointers_registry_remove(ptr);*/
}

/* void pointers_registry_add(void* ptr); */
/* void pointers_registry_update(void* old_ptr, void* new_ptr); */
/* void pointers_registry_remove(void* ptr); */
/* void cleanup_pointers_registry(void); */

