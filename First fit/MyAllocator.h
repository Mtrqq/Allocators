#pragma once

#include <set>
#include "taskAllocator.h"

namespace TaskAllocator
{
	class MyAllocator
	{
	public:
		MyAllocator(size_t i_size);
		~MyAllocator();
		template <typename TValueType>
		TValueType* allocate(size_t i_count = 1)
		{
			void* pointer = m_alloc.Allocate(i_count * sizeof(TValueType));
			return reinterpret_cast<TValueType*>(pointer);
		}
		template <typename TValueType>
		void deallocate(TValueType *ip_pointer)
		{
			m_alloc.Deallocate(ip_pointer);
		}
	private:
		TaskAllocatorImpl::AllocatorImpl m_alloc;
	};

}

