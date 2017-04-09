#include <stdlib.h>
#include <assert.h>
#include <functional>

const int DEFAULT_STACK_SIZE = 32;

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

	int capacity() const { return m_capacity; }
	int size() const { return m_itemsCount; }

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
		add(T());
		return m_data[m_itemsCount - 1];
	}

	void erase(int index) // easy handmade erase()
	{
		if (index < 0 || index >= m_itemsCount)
		{
			return;
		}
		m_data[index].~T();
		//moveData(m_data + index, m_data + index + 1, m_itemsCount - index - 1);
		for (auto i = index; i < m_itemsCount - 1; i++)
		{
			m_data[i] = m_data[i + 1];
		}
		--m_itemsCount;
	}

	void push_back(const T & value) // for test compatibility
	{
		add(value);
	}

	void erase(const T * item) // for test compatibility
	{
		erase(item - m_data);
	}

	T & operator[](int index)
	{
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
		return m_data + m_itemsCount;
	}

	void clear()
	{
		clearData();
		if (m_data == m_stackData)
		{
			for (auto i = 0; i < m_itemsCount; i++)
			{
				m_stackData[i].~T();
			}
		}
		initDefault();
	}

	void resize(int new_size)
	{
		if (new_size <= DEFAULT_STACK_SIZE)
		{
			clearData();
			m_data = m_stackData;
			m_capacity = DEFAULT_STACK_SIZE;
			m_itemsCount = new_size;
		}
		else
		{
			T* newData = new T[new_size];
			copyData(newData, m_data, new_size < m_itemsCount ? new_size : m_itemsCount);
			m_capacity = new_size;
			m_itemsCount = new_size;
			clearData();
			m_data = newData;
		}
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
		int newCapacity;
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

	static void moveData(T* dst, const T* src, int length)
	{
		memmove(dst, src, length * sizeof(T));
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
	int m_itemsCount;
	int m_capacity;
	T* m_data;
	T m_stackData[DEFAULT_STACK_SIZE];
};

