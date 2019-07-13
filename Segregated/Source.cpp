#include "Allocator.h"

#include <iostream>
#include <vector>

int main()
{
	std::ios::sync_with_stdio(false);
	CustomAllocator::Allocator alloc;
	std::vector<std::pair<void*, size_t>> pointers;
	for (int i = 0; i < 100000; ++i)
	{
		if (rand() % 2)
		{
			auto memory = (rand() % 200) + 1;
			std::cout << "Allocation of " << memory << " bytes " << std::endl;
			pointers.push_back(std::make_pair(alloc.Allocate(memory), memory));
		}
		else if (!pointers.empty())
		{
			auto pos = rand() % pointers.size();
			std::cout << "Deallocation of " << pointers[pos].second << " bytes " << std::endl;
			alloc.Deallocate(pointers[pos].first);
			pointers.erase(pointers.begin() + pos);
		}
	}
	std::cout << "\n\nRemoving remaining pointers : \n\n";
	for (auto pointer : pointers)
	{
		alloc.Deallocate(pointer.first);
		std::cout << "Deallocation of " << pointer.second << " bytes " << std::endl;
	}
}