#ifndef XMALLOC_H
#define XMALLOC_H



#define xmalloc(count, single_size) _xmalloc((count) * (single_size))
void* _xmalloc(size_t size);

#define xrealloc(ptr, count, single_size) _xrealloc((ptr), (count) * (single_size));
void* _xrealloc(void* ptr, size_t new_size);

#define xfree(ptr) _xfree(ptr)
void _xfree(void* ptr);

/* void pointers_registry_add(void* ptr); */
/* void pointers_registry_update(void* old_ptr, void* new_ptr); */
/* void pointers_registry_remove(void* ptr); */
/* void cleanup_pointers_registry(void); */

#endif /* XMALLOC_H */
