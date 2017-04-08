#include <cstdlib>
#include <cassert>
#include <cstring>
#include <cstdio>

#include <iostream>
#include <vector>

typedef long long llong;
typedef unsigned long long ullong;

#define RESIZE_POLICY 1.2121212121
#define START_SIZE 16

template<typename T>
class myvector
{
private:
	T* _data;
	int _size;
	int _capacity;

	void swap_data(int new_capacity) {
		T * new_data = static_cast<T*>(malloc(new_capacity * sizeof(T)));
		for (int i = 0; i < _size; ++i) {
			new (new_data + i) T(_data[i]);
			_data[i].~T();
		}
		free(_data);
		_data = new_data;
		_capacity = new_capacity;
	}

public:
	myvector() : _size(0), _capacity(START_SIZE) {
		_data = static_cast<T*>(malloc(_capacity * sizeof(T)));
	}

	myvector(const myvector & other) : _size(other._size), _capacity(other._capacity) {
		_data = static_cast<T*>(malloc(_data, _capacity * sizeof(T)));

		for (int i = 0; i < _size; ++i)
			new (_data + i) T(other._data[i]);
	}

	~myvector() {
		for (int i = 0; i < _size; ++i)
			_data[i].~T();
		free(_data);
	}


	int capacity() {
		return _capacity;
	}

	int size() {
		return _size;
	}


	void add(const T & value) {
		new (_data + _size++) T(value);
		if (_size == _capacity) {
			_capacity = static_cast<int>(_capacity * RESIZE_POLICY);
			swap_data(_capacity);
		}
	}

	T& add() {
		add(T());
		return _data[_size - 1];
	}

	void erase(int index) {
		assert(index >= 0 && index < _size);
		for (int i = index; i < _size - 1; ++i) {
			_data[i].~T();
			new (_data + i) T(_data[i + 1]);
		}
		_data[_size - 1].~T();
		--_size;
	}


	void push_back(const T & value) {
		add(value);
	}

	void erase(const T * item) {
		erase(item - _data);
	}


	T & operator[](int index) {
		assert(index >= 0 && index < _size);
		return _data[index];
	}

	const T& operator[](int index) const {
		assert(index >= 0 && index < _size);
		return _data[index];
	}

	myvector<T>& operator=(const myvector<T>& other) {
		clear();

		_size = other._size;
		_capacity = other._capacity;
		_data = static_cast<T*>(malloc(_data, _capacity * sizeof(T)));

		for (int i = 0; i < _size; ++i)
			new (_data + i) T(other._data[i]);
		return *this;
	}


	T* begin() {
		return _data;
	}

	T* end() {
		return _data + _size;
	}


	void clear() {
		for (int i = 0; i < _size; ++i)
			_data[i].~T();

		_size = 0;
	}

	void resize(int new_size) {
		if (new_size == _size) return;

		if (new_size < _size) {
			for (int i = new_size; i < _size; ++i)
				_data[i].~T();
		}
		else {
			int new_capacity = _capacity;
			while (new_capacity < _size)
				new_capacity *= RESIZE_POLICY;

			if (_capacity != new_capacity)
				swap_data(new_capacity);

			for (int i = _size; i < new_size; ++i)
				new (_data + i) T();
		}
		_size = new_size;
	}

	void reserve(int min_capacity) {
		if (_capacity == min_capacity) return;

		while (_capacity < min_capacity)
			_capacity *= RESIZE_POLICY;

		swap_data(_capacity);
	}
};
