#include "taskAllocator.h"

namespace TaskAllocatorImpl
{

	AllocatorImpl::AllocatorImpl(size_t i_size)
		: mp_start_buffer(new char[i_size])
		, mp_end_buffer(static_cast<char*>(mp_start_buffer) + i_size)
		, mp_mem_head{ new(mp_start_buffer) MemoryBlock{ nullptr, nullptr, 0} }
	{
	}

	AllocatorImpl::~AllocatorImpl()
	{
		size_t free_memory{};
		while (mp_mem_head->mp_next)
		{
			free_memory += mp_mem_head->m_memoryAvailable + sizeof(MemoryBlock);
			mp_mem_head = mp_mem_head->mp_next;
		}
		std::cout << "\nFree mem :" << free_memory + BytesBetweenPointers(mp_mem_head, mp_end_buffer) + mp_mem_head->m_memoryAvailable + sizeof(MemoryBlock) << "\n";
		delete[] reinterpret_cast<char*>(mp_start_buffer);
	}

	void* AllocatorImpl::Allocate(size_t i_size)
	{
		auto current_node = mp_mem_head;
		while (current_node->mp_next)
		{
			current_node = current_node->mp_next;
			if (current_node->m_memoryAvailable >= (i_size + sizeof(MemoryBlock)))
			{
				current_node->m_memoryAvailable -= i_size + sizeof(MemoryBlock);
				return AllocateAfter(current_node->mp_prev, i_size);
			}
		}
		if (BytesBetweenPointers(current_node, mp_end_buffer) >= i_size)
		{
			return AllocateAfter(current_node, i_size);
		}
		throw std::bad_alloc{};
	}

	void AllocatorImpl::Deallocate(void *ip_pointer)
	{
		if (ip_pointer < mp_start_buffer || ip_pointer >= mp_end_buffer)
			throw std::bad_alloc{};
        MemoryBlock* needed_node = reinterpret_cast<MemoryBlock*>(ShiftPointer(ip_pointer, -static_cast<long long>(sizeof(MemoryBlock))))->mp_next;
		needed_node->m_memoryAvailable = BytesBetweenPointers(needed_node->mp_prev, needed_node);
		//INVALID POINTER POSITION;
		CollapseBlocks(needed_node);
	}

	void* AllocatorImpl::AllocateAfter(MemoryBlock *ip_memory, size_t i_size)
	{
		auto MemoryBlockPos = ShiftPointer(ip_memory, (static_cast<long long>(sizeof(MemoryBlock) + i_size)));
		new(MemoryBlockPos) MemoryBlock{ ip_memory,ip_memory->mp_next, 0 };
		if (ip_memory->mp_next)
			ip_memory->mp_next->mp_prev = static_cast<MemoryBlock*>(MemoryBlockPos);
		ip_memory->mp_next = static_cast<MemoryBlock*>(MemoryBlockPos);
		std::cout << "New memory located at :" << BytesBetweenPointers(mp_mem_head, MemoryBlockPos) << '\n';
		return ShiftPointer(MemoryBlockPos, -static_cast<long long>(i_size));
	}

	void AllocatorImpl::CollapseBlocks(MemoryBlock* ip_selected_block)
	{
		MemoryBlock *start = ip_selected_block, *end = ip_selected_block;
		while (start->mp_prev && start->mp_prev->m_memoryAvailable != 0)
			start = start->mp_prev;
		while (end->mp_next && end->mp_next->m_memoryAvailable != 0)
			end = end->mp_next;
		if (start != end)
		{
			size_t memory_accumulator{};
			MemoryBlock *walker = start;
			while (walker != end)
			{
				memory_accumulator += walker->m_memoryAvailable + sizeof(MemoryBlock);
				walker = walker->mp_next;
			}
			if (start->mp_prev) start->mp_prev->mp_next = end;
			end->mp_prev = start->mp_prev;
			end->m_memoryAvailable += memory_accumulator;
		}
	}

}
