#include <stdio.h>

typedef long long llong;
typedef unsigned long long ullong;

#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <typeinfo>
#include <functional>

const int DEFAULT_STACK_SIZE = 16;

template<typename T> class myvector
{
public:
	myvector()
	{
		initDefault();
	}

	~myvector()
	{
		clearData();
	}

	int capacity() { return m_capacity; }
	int size() { return m_itemsCount; }

	void add(const T & value) // because we can
	{
		if (m_itemsCount == m_capacity)
		{
			increaseCapacity();
		}
		m_data[m_itemsCount++] = value;
	}

	T & add() // easy handmade emplace_back()
	{
		auto obj = new T();
		add(*obj);
		return *obj;
	}

	void erase(int index) // easy handmade erase()
	{
		assert(index >= 0 && index < m_itemsCount);

		copyData(m_data + index, m_data + index + 1, m_itemsCount - index);
		m_itemsCount--;
	}

	void push_back(const T & value) // for test compatibility
	{
		add(value);
	}

	void erase(const T * item) // for test compatibility
	{
		int indexToDelete = item - m_data;
		erase(indexToDelete);
	}

	T & operator[](int index)
	{
		assert(index >= 0 && index < m_itemsCount);
		return m_data[index];
	}

	const T & operator[](int index) const
	{
		assert(index >= 0 && index < m_itemsCount);
		return m_data[index];
	}

	T * begin()
	{
		return m_data;
	}

	T * end()
	{
		assert(m_itemsCount > 0);
		return m_data + m_itemsCount;
	}

	void clear()
	{
		clearData();
		initDefault();
	}

	void resize(int new_size)
	{
		T* newData = new T[new_size];
		copyData(newData, m_data, new_size < m_itemsCount ? new_size : m_itemsCount);
		m_capacity = new_size;
		m_itemsCount = new_size;
		clearData();
		m_data = newData;
	}

	void reserve(int min_capacity)
	{
		if (min_capacity <= m_capacity)
		{
			return;
		}

		T* newData = new T[min_capacity];
		copyData(newData, m_data, m_itemsCount);
		m_capacity = min_capacity;
		clearData();
		m_data = newData;
	}

private:
	void initDefault()
	{
		m_capacity = DEFAULT_STACK_SIZE;
		m_itemsCount = 0;
		m_data = m_stackData;
	}

	void increaseCapacity()
	{
		int newCapacity = 0;
		if (m_capacity < 1024)
		{
			newCapacity = m_capacity * 4;
		}
		else if (m_capacity < 256 * 1024)
		{
			newCapacity = m_capacity * 2;
		}
		else if (m_capacity < 1024 * 1024)
		{
			newCapacity = m_capacity + m_capacity * 3 / 4;
		}
		else if (m_capacity < 128 * 1024 * 1024)
		{
			newCapacity = m_capacity + m_capacity / 2;
		}
		else
		{
			newCapacity = m_capacity + m_capacity / 5;
		}

		T* newData = new T[newCapacity];
		copyData(newData, m_data, m_itemsCount);
		m_capacity = newCapacity;
		clearData();
		m_data = newData;
	}

	void copyData(T* dst, const T* src, int length)
	{
		if (!std::is_pod<T>())
		{
			for (int i = 0; i < length; i++)
			{
				dst[i] = src[i];
			}
		}
		else
		{
			memcpy(dst, src, length * sizeof(T));
		}
	}

	void clearData()
	{
		if (m_data != m_stackData)
		{
			delete[] m_data;
		}
	}

private:
	int m_capacity;
	int m_itemsCount;
	T* m_data;
	T m_stackData[DEFAULT_STACK_SIZE];
};

