#pragma once

#include <stdexcept>
#include <iostream>

namespace TaskAllocatorImpl
{

	class AllocatorImpl
	{
		struct MemoryBlock
		{
			MemoryBlock *mp_prev, *mp_next;
			size_t m_memoryAvailable;
		};
	public:
		AllocatorImpl(size_t i_size);

		~AllocatorImpl();

		void* Allocate(size_t i_size);

		void Deallocate(void* ip_pointer);
	private:
		void* mp_start_buffer;
		void* mp_end_buffer;
		MemoryBlock *mp_mem_head;

		void* AllocateAfter(MemoryBlock *ip_memory, size_t i_size);

		void CollapseBlocks(MemoryBlock* ip_selected_block);

		static size_t BytesBetweenPointers(void* ip_start, void * ip_end)
		{
			return static_cast<size_t>(static_cast<char*>(ip_end) - (static_cast<char*>(ip_start) + sizeof(MemoryBlock)));
		}

		static void* ShiftPointer(void* ip_pointer, long long i_bytesAmount)
		{
			return static_cast<char*>(ip_pointer) + i_bytesAmount;
		}
	};

}