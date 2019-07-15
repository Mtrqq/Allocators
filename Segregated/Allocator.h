#pragma once

#include <cmath>

namespace CustomAllocator
{
	constexpr unsigned ByteSize = 8;

	class Allocator
	{
		static constexpr size_t mg_memory_allocated = 1024 * 1024;
		static constexpr size_t mg_memory_per_class = 131072;
		static constexpr size_t mg_minimum_size_class = 8;
	public:
		Allocator();
		~Allocator();
		Allocator(const Allocator&);
		Allocator(Allocator&&);

		void* Allocate(size_t i_size);
		void Deallocate(void* ip_pointer);

	private:
		void* mp_memory_pool_start;
		void* mp_end_of_pool;

		static size_t BytesBetweenPointers(void* ip_start, void* ip_end)
		{
			return static_cast<size_t>(static_cast<char*>(ip_end) - static_cast<char*>(ip_start));
		}

		static void* GetShiftedPointer(void* ip_pointer, long long i_offset)
		{
			return static_cast<char*>(ip_pointer) + i_offset;
		}

		void* GetNthMemoryClass(size_t i_class_index)
		{
			auto p_position = GetShiftedPointer(mp_memory_pool_start, static_cast<long long>(i_class_index) * mg_memory_per_class);
			if (p_position >= mp_end_of_pool) return nullptr;
			return p_position;
		}

		size_t DetermineClassShifting(size_t i_size)
		{
			auto memory_class = static_cast<size_t>(std::ceil(std::log2(i_size)));
			auto base_memory_class = static_cast<size_t>(std::log2(mg_minimum_size_class));
			if (memory_class <= base_memory_class) return 0u;
			return memory_class - base_memory_class;
		}

		size_t GetRealAmountOfBlocks(size_t i_mem)
		{
			auto full_amount = (mg_memory_per_class / i_mem);
			double amount = full_amount - ((double(full_amount) / double(i_mem) / ByteSize));
			return static_cast<size_t>(std::floor(amount));
		}

		static size_t BytesToInteger(unsigned char* ip_pointer, size_t i_count);

		void* GetSuitablePosition(void* ip_memory_class_position, size_t i_class_block_size);
	};

}
