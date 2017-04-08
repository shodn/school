#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <vector>
#include <cassert>


//===================================================================
// School code
//===================================================================

template<typename T> class myvector {

public:
	myvector()
		: data(data_on_stack)
		, used(0)
		, total(stacked_items)
		, data_offset(0)
	{}

	~myvector()
	{
		if (data != data_on_stack)
		{
			for (int i = 0; i < used; i++)
			{
				data[i].~T();
			}
			free(data - data_offset);
		}
	}

	int size()
	{
		return used;
	}

	int capacity()
	{
		return total;
	}

	void resize(int new_size)
	{
		while (used < new_size)
		{
			add();
		}

		while (used > new_size)
		{
			data[used--].~T();
		}

		try_shrink();
	}

	void erase(int index)
	{
		assert(index >= 0);
		assert(index < used);
		
		if (data == data_on_stack || index > used / 4)
		{
			memmove(&data[index], &data[index + 1], (used - index - 1) * sizeof(T));

			if (data != data_on_stack)
				data[used - 1].~T();
		}
		else
		{
			memmove(&data[1], &data[0], index * sizeof(T));

			data[0].~T();
			data++;
			data_offset++;
			total--;
		}

		used--;

		try_shrink();
	}

	void erase(const T * item)
	{
		assert(item >= begin() && item < end());
		erase(item - begin());
	}

	void reserve(int min_capacity)
	{
		if (min_capacity > total)
		{
			alloc_more(min_capacity);
		}
	}

	void emplace_back(const T & value)
	{
		add(value);
	}

	void add(const T & value)
	{
		if (used == total)
		{
			if (total < 2 * 1024 * 1024)
				alloc_more(total * 2);
			else
				alloc_more((int)(total * 1.6));
		}

		if (data == data_on_stack)
			data[used] = value;
		else
			new (&data[used]) T(value);

		used++;
	}

	T & add()
	{
		add(T());
		return data[used - 1];
	}

	void push_back(const T & value)
	{
		add(value);
	}

	T & operator[](int index)
	{
		assert(index >= 0);
		assert(index < used);
		return data[index];
	}

	const T & operator[](int index) const
	{
		assert(index >= 0);
		assert(index < used);
		return data[index];
	}

	T * begin()
	{
		return &data[0];
	}

	T * end()
	{
		// Not dereferencable
		return &data[used];
	}

	T & front() // To test begin
	{
		return *begin();
	}

	T & back() // To test begin
	{
		return *(end() - 1);
	}

	void clear()
	{

		if (data != data_on_stack)
		{
			for (int i = 0; i < used; i++)
			{
				data[i].~T();
			}

			free(data - data_offset);
			data_offset = 0;
		}

		data = data_on_stack;
		used = 0;
		total = stacked_items;
	}


private:
	T * data;
	int used;
	int total;
	int data_offset;
	static const int stacked_items = 2;
	T data_on_stack[stacked_items];

	void alloc_more(int new_total)
	{
		if (new_total <= total)
			return;

		total = new_total;

		if (data == data_on_stack)
		{
			data = (T *)malloc(sizeof(T)* total);
			for (int i = 0; i < used; i++)
			{
				new (&data[i]) T(data_on_stack[i]);
			}
		}
		else
		{
			T * new_data = (T *)malloc(sizeof(T)* total);
			for (int i = 0; i < used; i++)
			{
				new (&new_data[i]) T(data[i]);
				data[i].~T();
			}
			free(data - data_offset);
			data_offset = 0;
			data = new_data;
		}
	}

	void try_shrink()
	{
		int new_count = total / 4;

		if (used > new_count / 2) // Good usage
			return;

		if (data_on_stack) // Cannot shrink
			return;

		T * new_data;

		if (new_count < stacked_items / 2)
		{
			new_data = data_on_stack;

			for (int i = 0; i < used; i++)
			{
				data_on_stack[i] = data[i];
			}
		} 
		else
		{
			T * new_data = (T *)malloc(new_count * sizeof(T));
			memcpy(new_data, data, used * sizeof(T));
		}

		free(data - data_offset);
		data_offset = 0;
		data = new_data;

		total = new_count;
	}
};
