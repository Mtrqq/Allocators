#include "MyAllocator.h"

TaskAllocator::MyAllocator::MyAllocator(size_t i_size)
	:m_alloc{i_size}
{
}

TaskAllocator::MyAllocator::~MyAllocator()
{
}

