
#include "Allocator.h"

#include <iostream>
#include <cstring>
#include <stdexcept>
#include <limits>

CustomAllocator::Allocator::Allocator()
	:mp_memory_pool_start{ new char[mg_memory_allocated] },
	mp_end_of_pool{GetShiftedPointer(mp_memory_pool_start,mg_memory_allocated)}
{
	auto steps_count = mg_memory_allocated / mg_memory_per_class;
	auto memory_class_token_size = mg_minimum_size_class;
	auto p_walker = mp_memory_pool_start;
	for (unsigned i = 0; i < steps_count; ++i)
	{
		auto amount_of_bits = GetRealAmountOfBlocks(memory_class_token_size);
		if (amount_of_bits < 8) amount_of_bits = 8;
		memset(p_walker, std::numeric_limits<unsigned char>::max(), 
			static_cast<size_t>(std::ceil((double)amount_of_bits / ByteSize)));
		p_walker = GetShiftedPointer(p_walker, mg_memory_per_class);
		memory_class_token_size <<= 1;
	}
}

CustomAllocator::Allocator::~Allocator()
{
	delete[] reinterpret_cast<unsigned char*>(mp_memory_pool_start);
}

CustomAllocator::Allocator::Allocator(const Allocator& i_another_alloc)
	:mp_memory_pool_start{i_another_alloc.mp_memory_pool_start},
	mp_end_of_pool{i_another_alloc.mp_end_of_pool}
{
}

CustomAllocator::Allocator::Allocator(Allocator&& i_another_alloc)
	: mp_memory_pool_start{ std::move(i_another_alloc.mp_memory_pool_start) },
	mp_end_of_pool{ std::move(i_another_alloc.mp_end_of_pool) }
{
}

void* CustomAllocator::Allocator::Allocate(size_t i_size)
{
	auto class_shifting = DetermineClassShifting(i_size);
	void* p_memory_class_pointer = GetNthMemoryClass(static_cast<size_t>(class_shifting));
	auto suitable_position = GetSuitablePosition(p_memory_class_pointer, mg_minimum_size_class << class_shifting);
	if (suitable_position) return suitable_position;
	throw std::bad_alloc{};
}

void CustomAllocator::Allocator::Deallocate(void* ip_pointer)
{
	if (ip_pointer < mp_memory_pool_start && ip_pointer >= mp_end_of_pool)
		throw std::bad_alloc{}; // alloc ?
	auto distance = BytesBetweenPointers(mp_memory_pool_start, ip_pointer);

	auto class_index = distance / mg_memory_per_class;
	auto memory_class_start = GetNthMemoryClass(class_index);
	auto memory_for_token = mg_minimum_size_class << class_index;
	distance = BytesBetweenPointers(memory_class_start, ip_pointer);
	if (distance % memory_for_token != 0)
	{
		std::cout << "Error :" << distance % memory_for_token;
		throw std::bad_alloc{};
	}
	auto skipped_system_blocks = (mg_memory_per_class / memory_for_token) - GetRealAmountOfBlocks(memory_for_token);
	distance = distance / memory_for_token - skipped_system_blocks;
	auto bit_field = static_cast<unsigned char*>(GetShiftedPointer(memory_class_start, distance / ByteSize));
	*bit_field |= (1 << (ByteSize - (distance % ByteSize)- 1));
}

//Converts first n bytes to integer format
size_t CustomAllocator::Allocator::BytesToInteger(unsigned char* ip_pointer, size_t i_bytes_count)
{
	size_t result{};
	auto p_pointer_copy = ip_pointer;
	for (unsigned i = 0; i < i_bytes_count; ++i, ++p_pointer_copy)
	{
		result <<= sizeof(unsigned char) * ByteSize;
		result += *p_pointer_copy;
	}
	return result;
}

//Returns pointer to free memory in a certain memory class
void* CustomAllocator::Allocator::GetSuitablePosition(void* ip_memory_class_position, size_t i_class_block_size)
{
	if (ip_memory_class_position == nullptr) 
		throw std::bad_alloc{};
	long long bits_to_observe = static_cast<long long>(GetRealAmountOfBlocks(i_class_block_size));
	auto step_in_bytes = bits_to_observe / ByteSize;
	if (step_in_bytes > sizeof(unsigned long long)) 
		step_in_bytes = sizeof(unsigned long long);
	auto p_walker = ip_memory_class_position;
	unsigned position_from_start{};
	while (step_in_bytes > 0 && bits_to_observe > 0)
	{
		if (BytesToInteger(static_cast<unsigned char*>(p_walker), step_in_bytes) != 0)
		{
			step_in_bytes >>= 1;
		}
		else
		{
			p_walker = GetShiftedPointer(p_walker, step_in_bytes);
			bits_to_observe -= step_in_bytes * ByteSize;
			position_from_start += step_in_bytes * ByteSize;
		}
	}
	if (bits_to_observe <= 0) return nullptr;
	unsigned char* bit_field = static_cast<unsigned char*>(p_walker);
	auto checked_bit = 1 << (ByteSize - 1);
	while (bits_to_observe--)
	{
		std::cout << ((*bit_field) & checked_bit);
		if ((*bit_field) & checked_bit)
		{
			*bit_field &= ~checked_bit;
			std::cout << std::endl;
			auto offset = static_cast<long long>(i_class_block_size) * position_from_start;
			return GetShiftedPointer(ip_memory_class_position,
				offset + ((mg_memory_per_class / i_class_block_size) - GetRealAmountOfBlocks(i_class_block_size)) * i_class_block_size);
		}
		checked_bit >>= 1;
		++position_from_start;
	}
	return nullptr;
}
