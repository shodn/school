#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <new>

#define MYVECTOR_MIN_CAPACITY 8
#define MYVECTOR_CAPACITY_MULTYPLIER 1.5

#define MYVECTOR_SMALL_BUFFER_SIZE 32

template<typename T> class myvector
{
public:
	myvector() : data(small_buffer), _capacity(MYVECTOR_SMALL_BUFFER_SIZE), _size(0) {}
	~myvector()
	{
		for (int i = 0; i < _size; ++i)
			data[i].~T();
		if (data != small_buffer)
			free(data);
	}

	int capacity() const
	{
		return _capacity;
	}

	int size() const
	{
		return _size;
	}

	void add(const T & value)
	{
		extend_if_need();
		data[_size] = value;
		++_size;
	}

	T & add()
	{
		extend_if_need();
		++_size;
		if (data != small_buffer)
			new(&data[_size - 1]) T();
		return data[_size - 1];
	}

	void erase(int index)
	{
		--_size;
		for (int i = index; i < _size; ++i)
			data[i] = data[i + 1];
		data[_size].~T();
	}

	void push_back(const T & value)
	{
		add(value);
	}

	void erase(const T * item)
	{
		for (T* it = (T*)item + 1; it != end(); ++it)
			*(it - 1) = *it;
		--_size;
		data[_size].~T();
	}

	T & operator[](int index)
	{
		assert(index >= 0);
		assert(index < _size);
		return data[index];
	}

	const T & operator[](int index) const
	{
		assert(index >= 0);
		assert(index < _size);
		return data[index];
	}

	T * begin()
	{
		return data;
	}

	T * end()
	{
		return data + _size;
	}

	void clear()
	{
		for (int i = 0; i < _size; ++i)
			data[i].~T();
		_size = 0;
	}

	void resize(int new_size)
	{
		assert(new_size >= 0);

		if (new_size > _capacity)
		{
			_capacity = new_size;
			extend_data_to_capacity();
		}
		if (data != small_buffer && new_size > _size)
		{
			for (int i = _size; i < new_size; i++)
				new(&data[i]) T();
		}
		_size = new_size;
	}

	void reserve(int min_capacity)
	{
		assert(min_capacity >= 0);

		if (min_capacity <= _capacity)
			return;

		_capacity = min_capacity;

		if (data == small_buffer && _capacity <= MYVECTOR_SMALL_BUFFER_SIZE)
			return;

		extend_data_to_capacity();
	}

private:
	inline void extend_if_need()
	{
		if (_size == _capacity)
		{
			double multiplier = 2;
			if (_size > 1000)
				multiplier = 1.5;
			_capacity = int(multiplier * _capacity);
			extend_data_to_capacity();
		}
	}

	inline void extend_data_to_capacity()
	{
		if (data == small_buffer)
		{
			data = (T*)realloc(NULL, _capacity * sizeof(T));
			memcpy(data, small_buffer, sizeof(small_buffer));
		}
		else
			data = (T*)realloc(data, _capacity * sizeof(T));
	}

private:
	int _size;
	int _capacity;
	T *data;
	T small_buffer[MYVECTOR_SMALL_BUFFER_SIZE];
};
