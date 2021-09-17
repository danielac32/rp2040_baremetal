#pragma once
char mem[80000];
#define KMALLOC_START  (void*)mem
#define KMALLOC_LENGTH sizeof(mem)
void *malloc(int length);
void free(void *ptr);

void malloc_init(void *start, int length);
void malloc_debug();
int malloc_test();


#define KUNIT sizeof(struct kmalloc_chunk)

#define KMALLOC_STATE_FREE 0xa1a1a1a1
#define KMALLOC_STATE_USED 0xbfbfbfbf

struct kmalloc_chunk {
	int state;
	int length;
	struct kmalloc_chunk *next;
	struct kmalloc_chunk *prev;
};

static struct kmalloc_chunk *head = 0;

 