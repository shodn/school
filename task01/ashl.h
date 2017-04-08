#include <assert.h>
#include <math.h>

template<typename T> class myvector
{
public:
	myvector()
	{
		_data = new T[_max_capacity];
	}

	~myvector()
	{
		clear();
	}

	int capacity() const
	{
		return _max_capacity;
	}

	int size() const
	{
		return _current_size;
	}

	void add(const T & value)
	{
		if (_current_size == _max_capacity)
			reallocate();

		_data[_current_size] = value;
		++_current_size;
	}

	T & add()
	{
		add(T());
		return _data[_current_size];
	}

	void erase(int index)
	{
		assert(index >= 0 && index < _current_size);
		
		for (int i = index; i < (_current_size - 1); ++i)
			_data[i] = _data[i + 1];

		--_current_size;
	}

	void push_back(const T & value)
	{
		add(value);
	}

	void erase(const T * item)
	{
		erase(item - begin());
	}

	T & operator[](int index)
	{
		assert(index >= 0 && index < _current_size);
		return _data[index];
	}

	const T & operator[](int index) const
	{
		assert(index >= 0 && index < _current_size);
		return _data[index];
	}

	T * begin()
	{
		assert(_current_size != 0);
		return &_data[0];
	}

	T * end()
	{
		assert(_current_size != 0);
		return &_data[_current_size - 1];
	}

	void clear()
	{
		delete [] _data;
		_data = nullptr;
		_max_capacity = 8;
		_current_size = 0;
	}
	
	void resize(int new_size)
	{
		assert(new_size > 0);

		if (new_size < _current_size)
			_current_size = new_size;

		auto oldData = _data;
		_data = new T[new_size];

		for (int i = 0; i < _current_size; ++i)
			_data[i] = oldData[i];

		delete[] oldData;

		_max_capacity = new_size;
	}

	void reserve(int min_capacity)
	{
		assert(min_capacity > 0);

		if (min_capacity < _current_size)
			min_capacity = _current_size;

		auto oldData = _data;
		_data = new T[min_capacity];

		for (int i = 0; i < _current_size; ++i)
			_data[i] = oldData[i];

		delete[] oldData;

		_max_capacity = min_capacity;
	}

	bool empty() const
	{
		return _current_size == 0;
	}

	int _reallocation_count = 0;

private:
	T *_data = nullptr;
	int _max_capacity = 16;
	int _current_size = 0;

	void reallocate()
	{
		_max_capacity = static_cast<int>(_max_capacity * sqrt(2.0f));

		auto oldData = _data;
		_data = new T[_max_capacity];

		for (int i = 0; i < _current_size; ++i)
			_data[i] = oldData[i];

		delete[] oldData;

		++_reallocation_count;
	}
};
