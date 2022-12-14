
#include <inc/lib.h>

// malloc()
//	This function use NEXT FIT strategy to allocate space in heap
//  with the given size and return void pointer to the start of the allocated space

//	To do this, we need to switch to the kernel, allocate the required space
//	in Page File then switch back to the user again.
//
//	We can use sys_allocateMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls allocateMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the allocateMem function is empty, make sure to implement it.


//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
unsigned int* next_fit_strategy(uint32 size);

int32 user_array[(KERNEL_HEAP_MAX - KERNEL_HEAP_START) / PAGE_SIZE];
uint32 lastalloc = USER_HEAP_MAX;

void* malloc(uint32 size)
{
	//TODO: [PROJECT 2022 - [9] User Heap malloc()] [User Side]
	// Write your code here, remove the panic and write your code
	//panic("malloc() is not implemented yet...!!");

	// Steps:
	//	1) Implement NEXT FIT strategy to search the heap for suitable space
	//		to the required allocation size (space should be on 4 KB BOUNDARY)
	unsigned int* ret_address = NULL;
	if (sys_isUHeapPlacementStrategyNEXTFIT())
		ret_address = next_fit_strategy(size);
	//	2) if no suitable space found, return NULL
	if (ret_address == NULL)
		return NULL;
	//	 Else,
	//	3) Call sys_allocateMem to invoke the Kernel for allocation
	sys_allocateMem((uint32)ret_address, size);
	// 	4) Return pointer containing the virtual address of allocated space,
	//
	return (void*) ret_address;

	//This function should find the space of the required range
	// *** ON 4KB BOUNDARY ******** //

	//Use sys_isUHeapPlacementStrategyNEXTFIT() and
	//sys_isUHeapPlacementStrategyBESTFIT() for the bonus
	//to check the current strategy
}

void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	panic("smalloc() is not required ..!!");
	return NULL;
}

void* sget(int32 ownerEnvID, char *sharedVarName)
{
	panic("sget() is not required ..!!");
	return 0;
}

// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from page file and main memory then switch back to the user again.
//
//	We can use sys_freeMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls freeMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the freeMem function is empty, make sure to implement it.

void free(void* virtual_address)
{
	//TODO: [PROJECT 2022 - [11] User Heap free()] [User Side]
	// Write your code here, remove the panic and write your code
	//panic("free() is not implemented yet...!!");
	uint32 va = ROUNDDOWN((uint32)virtual_address, PAGE_SIZE);
	int place =  ((int32)virtual_address - USER_HEAP_START)/ PAGE_SIZE ;
	sys_freeMem((unsigned int)va, PAGE_SIZE * user_array[place]);
	user_array[place] = 0;
	//you shold get the size of the given allocation using its address
	//you need to call sys_freeMem()
	//refer to the project presentation and documentation for details
}


void sfree(void* virtual_address)
{
	panic("sfree() is not requried ..!!");
}


//===============
// [2] realloc():
//===============

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_moveMem(uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		which switches to the kernel mode, calls moveMem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		in "memory_manager.c", then switch back to the user mode here
//	the moveMem function is empty, make sure to implement it.

void *realloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT 2022 - BONUS3] User Heap Realloc [User Side]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");

	return NULL;
}

unsigned int* next_fit_strategy(uint32 size)
{
	size = ROUNDUP(size , PAGE_SIZE) ;
	int num_pages = size / PAGE_SIZE ;

	unsigned int temp = 0;
	int counter = 0;
	int flag = 0;

	if (USER_HEAP_MAX - USER_HEAP_START < size)
	{
		return NULL;
	}


	for(int addr = lastalloc; addr < USER_HEAP_MAX; addr += PAGE_SIZE)
	{
		int ind = (addr - USER_HEAP_START) / PAGE_SIZE;
		if (user_array[ind] == 0)
		{
			counter++;
		}
		else
		{
			counter = 0;
		}

		if(counter == num_pages)
		{
			flag = 1;
			temp = addr - ((num_pages - 1) * PAGE_SIZE);

			goto here;
		}
	}

	counter = 0;
	for(int addr2 = USER_HEAP_START; addr2 < lastalloc; addr2 += PAGE_SIZE)
	{
		int ind = (addr2 - USER_HEAP_START) / PAGE_SIZE;
		if (user_array[ind] == 0)
		{
			counter++;
		}
		else
		{
			counter=0;
		}

		if(counter == num_pages)
		{
			flag = 1;
			temp = addr2 - ((num_pages - 1) * PAGE_SIZE);
			goto here;
		}
	}

	here:
	if(flag == 1)
	{
		lastalloc = temp + (num_pages * PAGE_SIZE);
		flag = 0;
		counter = 0;
		int place =  (temp - USER_HEAP_START)/ PAGE_SIZE ;
		user_array[place] = num_pages;
		return (void*)temp;
	}
	return NULL;
}
