#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cmath>

template<typename T> class myvector
{

public:

	myvector()
	{
		_allocated = 16;
		_used = 0;
		_array = (T*)malloc(sizeof(T) * _allocated);
	}

	~myvector()
	{
		clear();
		free(_array);
	}

	int capacity()
	{
		return _allocated;
	}

	int size()
	{
		return _used;
	}

	void add(const T & value) // because we can
	{
		if (_used + 1 > _allocated)
		{
			grow();
		}
		_array[_used] = value;
		_used++;
	}

	T & add() // easy handmade emplace_back()
	{
		if (_used + 1 > _allocated)
		{
			grow();
		}
		add(T());
		return _array[_used - 1];
	}

	void erase(int index) // easy handmade erase()
	{
		_array[index].~T();
		for (size_t i = index; i < _used - 1; i++)
		{
			_array[i] = _array[i + 1];
		}
		_used--;
	}

	void push_back(const T & value) // for test compatibility
	{
		add(value);
	}

	void erase(const T * item) // for test compatibility
	{
		int index = -1;
		for (size_t i = 0; i < _used; i++)
		{
			if (_array[i] == *item)
			{
				index = i;
				break;
			}
		}
		if (index != -1)
		{
			erase(index);
		}
	}

	T & operator[](int index)
	{
		assert(index > -1);
		size_t lIndex = index;
		assert(lIndex < _used);
		return _array[index];
	}

	const T & operator[](int index) const
	{
		assert(index > -1);
		size_t lIndex = index;
		assert(lIndex < _used);
		return _array[index];
	}

	T * begin()
	{
		return _array;
	}

	T * end()
	{
		return &_array[_used - 1];
	}

	void clear()
	{
		for (size_t i = 0; i < _used; i++)
		{
			_array[i].~T();
		}
		_used = 0;
	}

	void resize(int new_size)
	{
		reserve(new_size);
		size_t nSize = new_size;
		if (nSize > _used)
		{
			for (size_t i = _used; i < nSize; i++)
			{
				_array[i] = T();
			}
		}
		else
		{
			for (size_t i = _used - 1; i > nSize; i--)
			{
				_array[i].~T();
			}
		}
		_used = nSize;
	}

	void reserve(int min_capacity)
	{
		size_t minC = min_capacity;
		if (minC > _allocated)
		{
			_allocated = minC;
			_array = (T*)realloc(_array, sizeof(T) * _allocated);
		}
	}

private:
	T *_array;
	size_t _allocated;
	size_t _used;

	const size_t growMark = (size_t)1.049e+8;

	void grow()
	{
		size_t prediction = sizeof(T) * _allocated * 2;

		double multiplier = prediction > growMark ? 1.2 : 2;

		_allocated = (size_t)(round(_allocated * multiplier));
		_array = (T*)realloc(_array, sizeof(T) * _allocated);
	}
};