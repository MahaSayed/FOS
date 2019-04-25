#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>

//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)

struct allocated{
	int counter;
	uint32 *address;
}arr[1048576];

int index = 0;

void* kmalloc(unsigned int size)
{
	//TODO: [PROJECT 2019 - MS1 - [1] Kernel Heap] kmalloc()

	int free_space = 0; // Available continuous space of pages
	int end; // Address of end allocation segment
	int count = 0; // Temporary counter for pages number
	int min = 999999999; //Best Fit size counter
	int start; // start address of allocated segment
	int bool = 0; //boolean to indicate if size needed
	int i;


	for(i = KERNEL_HEAP_START; i < KERNEL_HEAP_MAX; i += PAGE_SIZE)
	{
		uint32 *ptr_page_table = NULL;
		struct Frame_Info *frame_info_ptr = get_frame_info(ptr_page_directory, (void*)i, &ptr_page_table);
		if(frame_info_ptr == NULL)
		{
			free_space += PAGE_SIZE;
			count++;
		}
		else
		{
			if(free_space >= size && free_space < min)
			{
				end = i;
				start = end - (count*PAGE_SIZE);
				min = free_space;
				bool = 1;
		     }
			free_space = 0;
			count = 0;
		}
	}

	if(free_space >= size && free_space < min)
	{
		end = i;
		start = end - (count*PAGE_SIZE);
		min = free_space;
		free_space = 0;
		count = 0;
		bool = 1;
	}

    if(bool==0){
    	return NULL;
    }

    //save start address, allocated size
	arr[index].address = (uint32*)start;
	arr[index].counter = ROUNDUP(size, 4096);
	index++;

    int end_address = start + size;

	for(int i = start; i < end_address ; i += PAGE_SIZE)
	{
		struct Frame_Info *frame_info_ptr = NULL;
		allocate_frame(&frame_info_ptr);
		map_frame(ptr_page_directory, frame_info_ptr,(void*)i ,PERM_PRESENT|PERM_WRITEABLE);
	}

	//TODO: [PROJECT 2019 - BONUS1] Implement the FIRST FIT strategy for Kernel allocation
	// Beside the BEST FIT
	// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy
	return (void *)start;
}


void kfree(void* virtual_address)
{
	//TODO: [PROJECT 2019 - MS1 - [1] Kernel Heap] kfree()
	int pages_count = -1; //allocated pages by this address
	int required_index;
	for(int i = 0; i < index; i++)
	{
		if(virtual_address == arr[i].address)
		{
			pages_count = arr[i].counter;
			required_index = i;
			break;
		}
	}

	if(pages_count==-1)
		return;

	for(int i = 0; i < (pages_count/PAGE_SIZE) ; i++)
	{
		unmap_frame(ptr_page_directory, (void *)virtual_address);
		virtual_address += PAGE_SIZE;
	}

	arr[required_index].address = (uint32*)-1;
	arr[required_index].counter = -1;
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	uint32 phy_address;
	uint32 *ptr_page_table = NULL;
	struct Frame_Info* ptr_frame_info = get_frame_info(ptr_page_directory, (void*)virtual_address, &ptr_page_table);
	phy_address = to_physical_address(ptr_frame_info);

     uint32 presentbit=ptr_page_table[PTX(virtual_address)]& PERM_PRESENT;
     if (presentbit==0)
    	 return 0 ;
	return phy_address;
}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	    uint32 b=1,present_bit;
	    int frameNumber=physical_address/PAGE_SIZE;
	    uint32 frame;

		for (uint32 va=KERNEL_HEAP_START; va<KERNEL_HEAP_MAX; va += PAGE_SIZE)
		    {
		    	uint32 *page_table = NULL;
		    	get_page_table(ptr_page_directory, (void *)va, &page_table);

		    	uint32 presentbit=page_table[PTX((void*)va)]& PERM_PRESENT;

		    	uint32 entry = page_table[PTX(va)];
		    	frame=entry>>12;
				  if (frame == frameNumber && (presentbit!=0 ))
					  return va;
		    }

		    //not in kernal heap
	      return 0;

}

//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT 2019 - BONUS2] Kernel Heap Realloc
	// Write your code here, remove the panic and write your code

	return NULL;
	panic("krealloc() is not implemented yet...!!");


}
