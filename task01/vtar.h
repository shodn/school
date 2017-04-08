#include <stdio.h>
#include <iostream>
#include <vector>
#include <cassert>

typedef long long llong;
typedef unsigned long long ullong;

class standart_vector_policy
{
protected:
	size_t start_capacity() const
	{
		return 4;
	}

	size_t next_size(size_t size) const
	{
		return static_cast<size_t>(size * 1.5);
	}
};

class save_memory_policy
{
protected:
	size_t start_capacity() const
	{
		return 4;
	}

	size_t next_size(size_t size) const
	{
		return (size_t)ceil(size * 1.2);
	}
};

class save_performance_policy
{
protected:
	size_t start_capacity() const
	{
		return 8;
	}

	size_t next_size(size_t size) const
	{
		return size * 2;
	}
};

template<typename T_VALUE, typename T_POLICY = standart_vector_policy, typename T_SIZE = size_t>
class myvector : private T_POLICY
{
private:
	using T_POLICY::start_capacity;
	using T_POLICY::next_size;

	T_VALUE *_data = nullptr;
	T_SIZE _capacity = start_capacity();
	T_SIZE _size = 0;

public:
	typedef T_VALUE  value_type;
	typedef T_SIZE   size_type;
	typedef T_POLICY policy_type;

	myvector() 
		: _data(new T_VALUE[start_capacity()])
		, _capacity(start_capacity())
		, _size(0)
	{
	}

	explicit myvector(int capacity) 
		: _data(new value_type[capacity])
		, _capacity(capacity)
		, _size(0)
	{
	}

	myvector(const std::initializer_list<T_VALUE> &list) 
		: _data(new value_type[next_size(list.size())])
		, _capacity(next_size(list.size()))
		, _size(list.size())
	{
		size_type i = 0;
		for (auto &item : list)
		{
			_data[i++] = item;
		}
	}

	myvector(const myvector<T_VALUE, T_POLICY, T_SIZE> &other)
		: _data(new value_type[other._capacity])
		, _capacity(other._capacity)
		, _size(other._size)
	{
		for (size_type i = 0; i < other._size; i++)
		{
			_data[i] = other._data[i];
		}
	}

	myvector<T_VALUE, T_POLICY, T_SIZE> &operator=(const myvector<T_VALUE, T_POLICY, T_SIZE> &other)
	{
		if (&other == this)
			return *this;

		_data = new value_type[other._capacity];
		_capacity = other._capacity;
		_size = other._size;
		
		for (size_type i = 0; i < other._size; i++)
		{
			_data[i] = other._data[i];
		}

		return *this;
	}

	~myvector()
	{
		delete[] _data;
	}

	size_type capacity() const
	{
		return _capacity;
	}

	size_type size() const
	{
		return _size;
	}

	void add(const value_type & value)
	{
		if (_size == _capacity)
		{
			resize(next_size(_capacity));
		}

		_data[_size] = value;
		_size++;
	}

	value_type &add()
	{
		add(value_type());
		return _data[_size - 1];
	}

	void erase(size_type index)
	{
		erase(_data + index);
	}

	void push_back(const value_type & value)
	{
		add(value);
	}

	void erase(const value_type * item)
	{
		_size--;
		for (auto i = const_cast<value_type*>(item); i != _data + _size; i++)
		{
			*i = *(i + 1);
		}
	}

	value_type & operator[](size_type index)
	{
		assert(index < _size);
		return _data[index];
	}

	const value_type & operator[](size_type index) const
	{
		assert(index < _size);
		return _data[index];
	}

	value_type * begin() const
	{
		return _data;
	}

	value_type * end() const
	{
		return _data + _size;
	}

	void clear()
	{
		delete[] _data;

		_data = new value_type[start_capacity()];
		_capacity = start_capacity();
		_size = 0;
	}

	void resize(size_type new_size, const value_type &value = value_type())
	{
		auto n_size = (new_size > _size) ? _size : new_size;
		auto moved_data = new value_type[new_size];
		for (size_type i = 0; i < n_size; i++)
		{
			moved_data[i] = _data[i];
		}
		delete[] _data;

		_data = moved_data;

		_capacity = new_size;
	}

	void reserve(size_type min_capacity)
	{
		if (min_capacity > _capacity)
			resize(min_capacity);
	}
};

