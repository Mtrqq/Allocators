#include "Allocator.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <random>

class AllocatorWrapper
{
public:

	AllocatorWrapper() = default;
	~AllocatorWrapper() = default;
	AllocatorWrapper(CustomAllocator::Allocator&& i_moved_alloc)
		:alloc{ std::move(i_moved_alloc) } {}

	std::vector<void*> TryAllocate(size_t i_memory, size_t i_count = 1);
	std::vector<void*> TryAllocate(const std::vector<size_t>& i_size_vector);
	void DeallocatePointerArray(std::vector<void*>& i_ptrs);
private:
	CustomAllocator::Allocator alloc;
};

std::vector<void*> AllocatorWrapper::TryAllocate(size_t i_memory, size_t i_count)
{
	std::vector<void*> pointers;
	for (unsigned i = 0; i < i_count; ++i)
	{
		pointers.push_back(alloc.Allocate(i_memory));
	}
	return pointers;
}

std::vector<void*> AllocatorWrapper::TryAllocate(const std::vector<size_t>& i_size_vector)
{
	std::vector<void*> pointers;
	for (auto size : i_size_vector)
	{
		std::cout << "Allocation of " << size << " bytes" << std::endl;
		pointers.push_back(alloc.Allocate(size));
	}
	return pointers;
}

void AllocatorWrapper::DeallocatePointerArray(std::vector<void*>& i_ptrs)
{
	while (!i_ptrs.empty())
	{
		alloc.Deallocate(i_ptrs.back());
		i_ptrs.pop_back();
	}
}

int main()
{
	std::ios::sync_with_stdio(false);
	CustomAllocator::Allocator alloc;
	std::vector<void*> pointers;
	for (int i = 0; i < 100000; ++i)
	{
		if (rand() % 2)
		{
			auto memory = (rand() % 256) + 1;
			std::cout << "Allocation of " << memory << " bytes " << std::endl;
			pointers.push_back(alloc.Allocate(memory));
		}
		else if (!pointers.empty())
		{
			auto pos = rand() % pointers.size();
			alloc.Deallocate(pointers[pos]);
			pointers.erase(pointers.begin() + pos);
		}
	}
	std::cout << "\n\nRemoving remaining pointers...\n\n";
	for (auto pointer : pointers)
	{
		alloc.Deallocate(pointer);
	}
	AllocatorWrapper allocator;
	std::vector<size_t> sizes_to_allocate(256);
	std::uniform_int_distribution<int> u_id(1, 512);
	std::mt19937 generator(std::random_device{}());
	auto gen = [&] {return u_id(generator); };
	std::generate(sizes_to_allocate.begin(), sizes_to_allocate.end(), gen);
    pointers = allocator.TryAllocate(sizes_to_allocate);
	allocator.DeallocatePointerArray(pointers);
}