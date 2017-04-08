#include <stdlib.h>
#include <assert.h>

template<typename T> class myvector
{
public:
	myvector();
	~myvector();

	int capacity();
	int size();

	void add(const T & value); // because we can
	T & add(); // easy handmade emplace_back()
	void erase(int index); // easy handmade erase()

	void push_back(const T & value); // for test compatibility
	void erase(const T * item); // for test compatibility

	T & operator[](int index);
	const T & operator[](int index) const;

	T * begin();
	T * end();

	void clear();
	void resize(int new_size);
	void reserve(int min_capacity);

private:
	void deallocate_data();
	int suggest_new_capacity() const;

private:
	T *items_ = nullptr;
	int capacity_ = 0u;
	int size_ = 0u;
	
	static const int small_buffer_size = 32;
	T small_buffer_[small_buffer_size];
};

template<typename T>
myvector<T>::myvector()
	: items_(small_buffer_)
	, capacity_(small_buffer_size)
{
}

template<typename T>
myvector<T>::~myvector()
{
	deallocate_data();
}

template<typename T>
int myvector<T>::capacity()
{
	return capacity_;
}

template<typename T>
int myvector<T>::size()
{
	return size_;
}

template<typename T>
void myvector<T>::add(const T & value)
{
	push_back(value);
}

template<typename T>
T & myvector<T>::add()
{
	assert(size_ <= capacity_);
	if (size_ == capacity_)
	{
		reserve(suggest_new_capacity());
	}
	assert(size_ < capacity_);
	return items_[size_++];
}

template<typename T>
void myvector<T>::erase(const int index)
{
	assert(0 <= index && index < size_);
	if (index + 1 >= size())
	{
		items_[index] = T();
		--size_;
		return;
	}
	for (int i = index + 1; i < size(); ++i)
	{
		items_[i - 1] = (T&&)items_[i];
	}
}

template<typename T>
void myvector<T>::push_back(const T & value)
{
	if (size_ == capacity_)
	{
		reserve(suggest_new_capacity());
	}
	items_[size_++] = value;
}

template<typename T>
void myvector<T>::erase(const T * item)
{
	assert(begin() <= item && item < end());
	erase(item - begin());
}

template<typename T>
T & myvector<T>::operator[](const int index)
{
	assert(0 <= index && index <= size_);
	return items_[index];
}

template<typename T>
const T & myvector<T>::operator[](int index) const
{
	assert(0 <= index && index <= size_);
	return items_[index];
}

template<typename T>
T * myvector<T>::begin()
{
	return items_;
}

template<typename T>
T * myvector<T>::end()
{
	return items_ + size_;
}

template<typename T>
void myvector<T>::clear()
{
	deallocate_data();
	assert(items_ == small_buffer_);
	size_ = 0u;
	capacity_ = small_buffer_size;
}

template<typename T>
void myvector<T>::resize(const int new_size)
{
	assert(0 <= new_size);
	if (new_size <= size_)
	{
		for (int i = new_size; i < size_; ++i)
		{
			items_[i] = T();
		}
		size_ = new_size;
		return;
	}
	if (new_size <= capacity_)
	{
		size_ = new_size;
		return;
	}
	reserve(new_size);
	size_ = new_size;
}

template<typename T>
void myvector<T>::reserve(const int new_capacity)
{
	assert(0 <= new_capacity);
	if (new_capacity <= capacity_)
	{
		return;
	}
	T *new_data = new T[new_capacity];
	for (int i = 0; i < size(); ++i)
	{
		new_data[i] = (T&&)items_[i];
	}
	deallocate_data();
	items_ = new_data;
	capacity_ = new_capacity;
}

template<typename T>
void myvector<T>::deallocate_data()
{
	if (items_ != small_buffer_)
	{
		delete[] items_;
		items_ = small_buffer_;
		return;
	}
	for (int i = 0; i < small_buffer_size; ++i)
	{
		items_[i] = T();
	}
}

template<typename T>
int myvector<T>::suggest_new_capacity() const
{
	const auto multiplier = capacity_ * sizeof(T) >= 1024 * 1024 
		? 1.5
		: 2.0;
	return static_cast<int>(capacity_ * multiplier);
}
