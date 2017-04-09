#include <type_traits>
#include <cstdio>


#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define CLAMP(v, min, max) (MIN((max), MAX((v), (min))))


////////////////////////////////////////////////////////////////
// Declaration
////////////////////////////////////////////////////////////////


template<typename T>
class myvector
{
	static const int BUFFER_STACK_SIZE = 32;
	static const int ELEMENT_SIZE = sizeof(T);
	static const bool IS_POD = std::is_pod<T>::value;

public:

	// Fundamental methods

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

	void check_before_add(int count);
	void check_after_remove();

	void resize_internal(int new_size);

	void free_data();
	void clean_stack_data();

private:

	T m_buffer_stack[BUFFER_STACK_SIZE];

	T * m_data = m_buffer_stack;
	int m_size = 0;
	int m_capacity = BUFFER_STACK_SIZE;
};


////////////////////////////////////////////////////////////////
// Definition
////////////////////////////////////////////////////////////////


template <typename T>
myvector<T>::myvector()
{
	// All members are initialized so do nothing here
}

template <typename T>
myvector<T>::~myvector()
{
	clear();
}

template <typename T>
int myvector<T>::capacity()
{
	return m_capacity;
}

template <typename T>
int myvector<T>::size()
{
	return m_size;
}

template <typename T>
void myvector<T>::add(const T & value)
{
	check_before_add(1);
	new (&m_data[m_size]) T(value);
	++m_size;
}

template <typename T>
T& myvector<T>::add()
{
	check_before_add(1);
	++m_size;
	return m_data[m_size - 1];
}

template <typename T>
void myvector<T>::erase(int index)
{
	if (index >= 0 && index < m_size)
	{
		m_data[index].~T();
		if (index < m_size - 1)
		{
			// Not last element erased so we should move tail of a vector
			if (IS_POD)
			{
				memmove(m_data + index, m_data + index + 1, (m_size - index - 1) * ELEMENT_SIZE);
			}
			else
			{
				while (++index < m_size)
					new (&m_data[index - 1]) T(std::move_if_noexcept(m_data[index]));
			}
		}
		--m_size;
		m_data[m_size].~T(); // probably required if no move semantics supported
		// Disabled for now because adds some a little overhead
		//check_after_remove();
	}
}

template <typename T>
void myvector<T>::push_back(const T & value)
{
	add(value);
}

template <typename T>
void myvector<T>::erase(const T * item)
{
	erase(item - m_data);
}

template <typename T>
T & myvector<T>::operator[](int index)
{
	return *(m_data + CLAMP(index, 0, m_capacity - 1));
}

template <typename T>
const T & myvector<T>::operator[](int index) const
{
	return *(m_data + CLAMP(index, 0, m_capacity - 1));
}

template <typename T>
T * myvector<T>::begin()
{
	return m_data;
}

template <typename T>
T * myvector<T>::end()
{
	return m_data + m_size;
}

template <typename T>
void myvector<T>::clear()
{
	if (m_data != m_buffer_stack)
	{
		free_data();
		m_data = m_buffer_stack;
		m_capacity = BUFFER_STACK_SIZE;
	}
	clean_stack_data();
	m_size = 0;
}

template <typename T>
void myvector<T>::resize(int new_size)
{
	if (new_size < m_size)
	{
		// Remove elements
		if (!IS_POD)
		{
			for (int i = new_size; i < m_size; ++i)
				m_data[i].~T();
		}
	}
	else if (new_size > m_size)
	{
		// Add elements
		resize_internal(new_size);
		if (IS_POD)
		{
			memset(m_data + m_size, 0, (new_size - m_size) * ELEMENT_SIZE);
		}
		else
		{
			for (int i = m_size; i < new_size; ++i)
				new (&m_data[i]) T();
		}
	}
	m_size = new_size;
}

template <typename T>
void myvector<T>::reserve(int min_capacity)
{
	if (min_capacity > m_capacity)
		resize_internal(min_capacity);
}

template <typename T>
void myvector<T>::check_before_add(int count)
{
	if (m_size + count > m_capacity)
		resize_internal(m_size + count);
}

template <typename T>
void myvector<T>::check_after_remove()
{
	if (m_size * 4 < m_capacity)
		resize_internal(m_size);
}

template <typename T>
void myvector<T>::resize_internal(int new_size)
{
	static const float min_factor = 1.2f;
	static const float factor_step = 0.2f;

	if (m_data == m_buffer_stack && new_size <= BUFFER_STACK_SIZE)
		return;

	int chosen_size = BUFFER_STACK_SIZE * 4;
	float factor = 4.0f;
	while (chosen_size - 1 < new_size)
	{
		float new_size_float = chosen_size * factor;
		if (new_size_float >= INT_MAX)
		{
			chosen_size = INT_MAX;
			break;
		}
		// Align allocation memory to 16 bytes JIC
		chosen_size = ((static_cast<int>(new_size_float) >> 4) + 1) << 4;
		factor = MAX(min_factor, factor - factor_step);
	}

	if (chosen_size == m_capacity)
		return;

	if (chosen_size <= BUFFER_STACK_SIZE)
	{
		if (m_data != m_buffer_stack)
		{
			free_data();
			m_data = m_buffer_stack;
			m_capacity = BUFFER_STACK_SIZE;
		}
	}
	else
	{
		bool need_to_copy = false;
		T * new_data = nullptr;
		if (m_data == m_buffer_stack)
		{
			// Create new memory
			new_data = reinterpret_cast<T *>(malloc(chosen_size * ELEMENT_SIZE));
			need_to_copy = true;
		}
		else
		{
			// Try to reallocate memory
			new_data = reinterpret_cast<T *>(realloc(m_data, chosen_size * ELEMENT_SIZE));

			// This can be failed so we should check this and workaround
			if (!new_data)
			{
				new_data = reinterpret_cast<T *>(malloc(chosen_size * ELEMENT_SIZE));
				need_to_copy = true;
			}
		}
		if (need_to_copy)
		{
			int copy_size = chosen_size < m_size ? chosen_size : m_size;
			if (copy_size > 0)
			{
				if (IS_POD)
				{
					memcpy(new_data, m_data, copy_size * ELEMENT_SIZE);
				}
				else
				{
					T * src_data = m_data;
					T * dst_data = new_data;
					for (int i = 0; i < copy_size; ++i, ++src_data, ++dst_data)
						new (dst_data) T(std::move_if_noexcept(*src_data));
				}
			}
			if (m_data == m_buffer_stack)
			{
				// Clean buffer stack
				clean_stack_data();
			}
			else
			{
				free_data();
			}
		}
		m_data = new_data;
		m_capacity = chosen_size;
	}
}

template <typename T>
void myvector<T>::free_data()
{
	if (!IS_POD)
	{
		for (int i = 0; i < m_size; ++i)
			m_data[i].~T();
	}
	free(m_data);
}

template <typename T>
void myvector<T>::clean_stack_data()
{
	if (!IS_POD)
	{
		for (int i = 0; i < BUFFER_STACK_SIZE; ++i)
			m_data[i].~T();
	}
}
