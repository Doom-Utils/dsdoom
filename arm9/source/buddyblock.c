#include "buddyblock.h"

typedef struct tag_blockheader_t {
	u8 size;
	u8 free;
	u16 tag;
	struct tag_blockheader_t *nextblock;
	struct tag_blockheader_t *prevblock;
} blockheader_t;


blockheader_t *buckets[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int blocksize[9] = { 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072 };
void *memBase;
void *startMem;
int topBlock;

void initBuddyBlocks(int startBlock) {
	startMem = malloc(blocksize[startBlock] + blocksize[startBlock] - 1);

	blockheader_t *alignMem = (blockheader_t*)(((u32)startMem + blocksize[startBlock] - 1) & -blocksize[startBlock]);
	
	alignMem->size = startBlock;
	alignMem->free = 1;
	alignMem->tag = 0xbeef;
	alignMem->nextblock = 0;
	alignMem->prevblock = 0;
	buckets[startBlock] = alignMem;
	
	memBase = alignMem; 
	topBlock = startBlock;
}

void *blockAlloc( u32 size) {

	int bucket = 0;
	blockheader_t *block1, *block2;

	while ( buckets[bucket] == 0 && bucket < topBlock ) bucket++;
	while ( blocksize[bucket] < (size + sizeof(blockheader_t)) && bucket < 7) bucket++;	

	if ( buckets[bucket] == 0 ) {
		I_Error ("buddyblock: Failure trying to allocate %u bytes\n",size);
		return 0;
	}
	while (1) {
		if ( size < (blocksize[bucket]/2) - sizeof(blockheader_t) && bucket >0) {
		
			block1 = buckets[bucket];
			buckets[bucket] = block1->nextblock;
			
			if ( buckets[bucket] )
				buckets[bucket]->prevblock = 0;

			bucket--;
			block2 = ((void *)block1) + blocksize[bucket];	

			block1->nextblock = block2;
			block1->prevblock = 0;
			block1->size = bucket;
			block1->tag = 0xbeef;

			block2->prevblock = block1;
			block2->size = 	bucket;
			block2->nextblock = buckets[bucket];
			block2->tag = 0xbeef;
			
			buckets[bucket] = block1;
			
			if ( 0 != block2->nextblock )
				block2->nextblock->prevblock = block2;

		} else {
			block1 = buckets[bucket];
			buckets[bucket] = block1->nextblock;
			if ( buckets[bucket] )
				buckets[bucket]->prevblock = 0;

			block1->free = 0;
			block1->tag = 0xdead;
			return (void*)(block1+1);			
		}
	}
}

void blockFree( void * mem) {

	if ( mem == 0 ) return;

	blockheader_t *buddy;

	blockheader_t *block = (blockheader_t *)(mem - sizeof(blockheader_t));
	
	if ( block->tag != 0xdead )
		I_Error ("buddyblock: freeing corrupted block\n");

	
	while(1) {
		if ( block->size < topBlock ) {
			u32 offset = (void *)block - (void *)memBase;

			if ( (offset / blocksize[block->size]) & 1)
				buddy = (blockheader_t *)(((void *)block) - blocksize[block->size]);
			else
				buddy = (blockheader_t *)(((void *)block) + blocksize[block->size]);
		
			if ( buddy->free && buddy->size == block->size) {
				if ( buddy == buckets[buddy->size] ) {
					buckets[buddy->size] = buddy->nextblock;
					if ( buckets[buddy->size] )
						buckets[buddy->size]->prevblock = 0;
				}
				
				if (block > buddy ) {
					blockheader_t *temp = block;
					block = buddy;
					buddy = temp;
				}
				block->size++;
			} else {
				break;
			}
		} else {
			break;
		}
	}
	block->nextblock = buckets[block->size];
	block->tag = 0xbeef;
	block->free = 1;
	buckets[block->size] = block;
	
	if ( block->nextblock )
		block->nextblock->prevblock = block;
	
}
