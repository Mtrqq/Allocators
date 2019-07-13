#include "MyAllocator.h"
#include <iostream>
#include <vector>

int main()
{
	std::ios::sync_with_stdio(false);
	TaskAllocatorImpl::AllocatorImpl alloc(1024 * 1024);
	std::vector<std::pair<void*, size_t>> pointers;
	int countOfCurrentAlloc{};
	for (int i = 0; i < 1000; ++i)
	{
		if (rand() % 2)
		{
			auto memory = rand() % 1000 + 1;
			pointers.push_back(std::make_pair(alloc.Allocate(memory), memory));
			std::cout << "Allocation of " << memory << " memory bytes " << std::endl;
		}
		else if (!pointers.empty())
		{
			auto pos = rand() % pointers.size();
			std::cout << "Deallocation of " << pointers[pos].second << std::endl;
			alloc.Deallocate(pointers[pos].first);
			pointers.erase(pointers.begin() + pos);
		}
	}
	for (auto pointer : pointers)
	{
		alloc.Deallocate(pointer.first);
	}
}
