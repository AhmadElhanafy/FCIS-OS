#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>

//2022: NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
int32 kheap_array[(KERNEL_HEAP_MAX - KERNEL_HEAP_START) / PAGE_SIZE];
uint32 lastalloc = KERNEL_HEAP_START ;

void* kmalloc(unsigned int size)
{
	size = ROUNDUP(size , PAGE_SIZE) ;
	unsigned int temp = 0;
	int num_pages = size / PAGE_SIZE ;
	int counter = 0;
	int flag = 0;

	struct Frame_Info* frame_info = NULL;
	uint32* ptr_page_table = NULL;

	if (KERNEL_HEAP_MAX - KERNEL_HEAP_START < size)
	{
		return NULL;
	}



	for(int addr = lastalloc; addr < KERNEL_HEAP_MAX; addr += PAGE_SIZE)
	{
		frame_info = NULL;
		ptr_page_table = NULL;
		get_page_table(ptr_page_directory, (void*)(addr), &ptr_page_table);
		frame_info = get_frame_info(ptr_page_directory, (void*)addr, &ptr_page_table);

		if(frame_info == NULL)
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

	loop:
	counter = 0;
	for(int addr2 = KERNEL_HEAP_START; addr2 < lastalloc; addr2 += PAGE_SIZE)
	{
		frame_info = NULL;
		ptr_page_table = NULL;
		get_page_table(ptr_page_directory, (void*)(addr2), &ptr_page_table);
		frame_info = get_frame_info(ptr_page_directory, (void*)addr2, &ptr_page_table);

		if(frame_info == NULL)
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
			temp = addr2 - ((num_pages - 1) * PAGE_SIZE);
			goto here;
		}
	}

	here:
	if(flag == 1)
	{
		flag = 0;
		counter = 0;
		lastalloc = temp + (num_pages * PAGE_SIZE);
		for(int i = 0; i < num_pages; i++)
		{
			get_page_table(ptr_page_directory, (void*)(temp + (i * PAGE_SIZE)), &ptr_page_table);
			frame_info = get_frame_info(ptr_page_directory, (void*)(temp + (i * PAGE_SIZE)), &ptr_page_table);
			int k = allocate_frame(&frame_info);
			map_frame(ptr_page_directory, frame_info, (void *)(temp + (i * PAGE_SIZE)), PERM_WRITEABLE);
		}

		int place =  (temp - KERNEL_HEAP_START)/ PAGE_SIZE ;
		kheap_array[place] = num_pages;
		return (void*)temp;
	}

	return NULL;
}

void kfree(void* virtual_address)
{
	//TODO: [PROJECT 2022 - [2] Kernel Heap] kfree()
	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details
	int place =  ((int32 ) virtual_address - KERNEL_HEAP_START)/ PAGE_SIZE ;

	for(int i = 0 ; i < kheap_array[place] ; i++)
	{
		struct Frame_Info* frame_info = NULL;
		uint32* ptr_page_table = NULL;
		uint32 va = ((int32)virtual_address + (i * PAGE_SIZE));

		get_page_table(ptr_page_directory, (void*)(va + (i * PAGE_SIZE)), &ptr_page_table);
		frame_info = get_frame_info(ptr_page_directory, (void*)(va + (i * PAGE_SIZE)), &ptr_page_table);
		unmap_frame(ptr_page_directory, (void*) va);
	}
	kheap_array[place] = 0;
}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT 2022 - [3] Kernel Heap] kheap_virtual_address()
	// Write your code here, remove the panic and write your code
	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details
	//change this "return" according to your answer
	for(uint32 i = KERNEL_HEAP_START ; i < KERNEL_HEAP_MAX ; i += PAGE_SIZE)
	{
		struct Frame_Info* frame_info = NULL;
		uint32* ptr_page_table = NULL;

		frame_info = get_frame_info(ptr_page_directory, (void*)i, &ptr_page_table);
		if(frame_info != NULL)
		{
			uint32 pa = to_physical_address(frame_info);
			if(pa == physical_address)
			{
				return i;
			}
		}
	}
	return 0;
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT 2022 - [4] Kernel Heap] kheap_physical_address()
	// Write your code here, remove the panic and write your code
	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details
	//change this "return" according to your answer
	uint32* ptr_page_table = NULL;
	get_page_table(ptr_page_directory, (void*)virtual_address, &ptr_page_table);
	unsigned int add = (ptr_page_table[PTX(virtual_address)] >> 12) * PAGE_SIZE;
	return add;
}
