#ifndef MYVECTOR_H
#define MYVECTOR_H

#include <cassert>
#include <cstdlib>
#include <new> // <vector> defines the placement new
/*
inline void *operator new(size_t, void *_Where)
{
	return (_Where);
}

inline void operator delete(void *, void *) 
{	// delete if placement new fails
}
*/

typedef long long llong;
typedef unsigned long long ullong;

template<typename T> struct IsPodType { enum {Value = false}; };

template<> struct IsPodType<char> { enum { Value = true }; };
template<> struct IsPodType<unsigned char> { enum { Value = true }; };
template<> struct IsPodType<short> { enum { Value = true }; };
template<> struct IsPodType<unsigned short> { enum { Value = true }; };
template<> struct IsPodType<int> { enum { Value = true }; };
template<> struct IsPodType<unsigned int> { enum { Value = true }; };
template<> struct IsPodType<long> { enum { Value = true }; };
template<> struct IsPodType<unsigned long> { enum { Value = true }; };
template<> struct IsPodType<long long> { enum { Value = true }; };
template<> struct IsPodType<unsigned long long> { enum { Value = true }; };
template<> struct IsPodType<float> { enum { Value = true }; };
template<> struct IsPodType<double> { enum { Value = true }; };
template<> struct IsPodType<long double> { enum { Value = true }; };

template<typename T, typename SizeType = int> class myvector
{
	enum {
		DEFAULT_SIZE = 4,
	};
	T * m_data;
	SizeType m_cur_length;
	SizeType m_max_capacity;

	void reallocate(SizeType new_size)
	{
		if (IsPodType<T>::Value)
		{
			m_data = (T*)realloc(m_data, sizeof T * new_size);
		}
		else
		{
			T* new_data = (T*)malloc(sizeof T * new_size);

			for (SizeType i = 0; i < m_cur_length; ++i)
			{
				new (&new_data[i])T(m_data[i]);
				m_data[i].~T();
			}
			free(m_data);
			m_data = new_data;
		}
		m_max_capacity = new_size;
	}

	void push_back_impl(const T & value)
	{
		if (m_cur_length >= m_max_capacity)
		{
			reallocate(m_max_capacity * 2);
		}

		new (&m_data[m_cur_length++])T(value);
	}

public:
	myvector(const myvector & orig)
		: m_cur_length(0)
		, m_data(nullptr)
	{
		reallocate(orig.size());
		for (const T & item : orig)
		{
			push_back_impl(item);
		}
	}
	
	myvector& operator=(const myvector& orig)
	{
		clear();
		reallocate(orig.size());
		for (const T & item : orig)
		{
			push_back_impl(item);
		}
		return *this;
	}
	

	myvector()
		: m_cur_length(0)
		, m_data(nullptr)
	{
		reallocate(DEFAULT_SIZE);
	}

	~myvector()
	{
		free(m_data);
		m_data = nullptr;
		m_cur_length = 0;
		m_max_capacity = 0;
	}

	int capacity() const
	{
		return m_max_capacity;
	}

	int size() const
	{
		return m_cur_length;
	}

	void add(const T & value) // because we can
	{
		push_back_impl(value);
	}

	T & add() // easy handmade emplace_back()
	{
		push_back_impl(T());
		return m_data[m_cur_length - 1];
	}

	void erase(int index) // easy handmade erase()
	{
		assert(0 <= index && index < m_cur_length);
		if (IsPodType<T>::Value)
		{
			memmove(&m_data[index], &m_data[index+1], sizeof(T) * (m_cur_length - index - 1));
		}
		else{
			m_data[index].~T();

			for (; index < m_cur_length-1; ++index)
			{
				new (&m_data[index])T(m_data[index+1]);
				m_data[index+1].~T();
			}
		}
		--m_cur_length;
	}

	void push_back(const T & value) // for test compatibility
	{
		push_back_impl(value);
	}

	void erase(const T * item) // for test compatibility
	{
		assert(m_data <= item && item < m_data + m_cur_length);
		const int erased_index = item - m_data;
		erase(erased_index);
	}

	T & operator[](int index)
	{
		assert(0 <= index && index < m_cur_length);
		return m_data[index];
	}

	const T & operator[](int index) const
	{
		assert(0 <= index && index < m_cur_length);
		return m_data[index];
	}

	T * begin()
	{
		return m_data;
	}

	const T * begin() const
	{
		return m_data;
	}

	T * end()
	{
		return &m_data[m_cur_length];
	}

	const T * end() const
	{
		return &m_data[m_cur_length];
	}

	void clear()
	{
		if (!IsPodType<T>::Value)
		{
			for (int index = 0 ; index < m_cur_length; ++index)
			{
				m_data[index].~T();
			}
		}
		m_cur_length = 0;
	}
	
	void resize(int new_size)
	{
		assert(new_size > 0);
		if (new_size > m_cur_length)
		{
			if (new_size > m_max_capacity)
			{
				reallocate(new_size);
				if (IsPodType<T>::Value)
				{
					memset(&m_data[m_cur_length], 0, sizeof(T)* (new_size - m_cur_length));
				}
				else
				{
					for (int createdIndex = m_cur_length; createdIndex < new_size; ++createdIndex)
					{
						new (&m_data[createdIndex])T;
					}
				}
			}
		}
		else
		{
			if (IsPodType<T>::Value)
			{
				// do nothing
			}
			else
			{
				for (int destroyedIndex = m_cur_length-1; destroyedIndex >= new_size; --destroyedIndex)
				{
					m_data[destroyedIndex].~T();
				}
			}
		}
		m_cur_length = new_size;
	}

	void reserve(int min_capacity)
	{
		if (m_cur_length < min_capacity)
		{
			reallocate(min_capacity);
		}
	}
};

#endif // MYVECTOR_H
