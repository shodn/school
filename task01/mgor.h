#include <stdlib.h>
#include <assert.h>

typedef long long llong;
typedef unsigned long long ullong;

const long COUNT = 100000000L;

llong new_vector_size(llong size) {
    return size + size / 2;
}

template<typename T>
class HybridVector
{
public:
    enum { DEFAULT_SIZE = 32 };
    
	HybridVector() { }
	~HybridVector() { delete [] m_bigBuffer; }

	int capacity() { return m_capacity; }
	int size() { return m_size; }

     // because we can
	void add(const T &value)
    {
        if (m_size >= m_capacity) {
            reallocate(new_vector_size(m_capacity));
        }
        m_data[m_size++] = value;
    }
    
    // easy handmade emplace_back()
	T & add()
    {
        add(T{ });
        return *(end() - 1);
    }
    
    // easy handmade erase()
	void erase(int index)
    {
        for (llong i = index; i < m_size - 1; ++i) {
            m_data[i] = m_data[i+1];
        }
        --m_size;
    }

    // for test compatibility
	void push_back(const T &value) { add(value); }
    
    // for test compatibility
	void erase(const T *item)
    {
        erase(item - m_data);
    }

	T & operator[](int index) { return m_data[index]; }
	const T & operator[](int index) const { return m_data[index]; }

	T * begin() { return m_data; }
	T * end() { return m_data + m_size; }

	void clear()
    {
        if (m_data == m_smallBuffer) {
            clearSmallBuffer();
        } else {
            delete [] m_bigBuffer;
            m_data = m_smallBuffer;
            m_capacity = DEFAULT_SIZE;
        }
        m_size = 0LL;
    }
    
	void resize(int new_size)
    {
        if (new_size < m_size) {
            for (llong i = new_size; i < m_size; ++i) {
                m_data[i] = T{ };
            }
            m_size = new_size;
        } else if (new_size > m_size) {
            if (new_size > m_capacity) {
                reallocate(new_size);
            }
            for (llong i = m_size; i < new_size; ++i) {
                m_data[i] = T{ };
            }
            m_size = new_size;
        }
    }

	void reserve(int min_capacity)
    {
        if (min_capacity > m_capacity) {
            reallocate(min_capacity);
        }
    }
    
private:
    void clearSmallBuffer()
    {
        for (auto &it : m_smallBuffer) {
            it = T{ };
        }
    }
    
    void reallocate(int new_size)
    {
        T *new_data = new T[new_size];
        for (llong i = 0; i < m_size; ++i) {
            new_data[i] = m_data[i];
        }
        if (m_data == m_bigBuffer) {
            delete [] m_data;
        } else {
            clearSmallBuffer();
        }
        m_data = m_bigBuffer = new_data;
        m_capacity = new_size;
    }
    
private:
    llong m_capacity = DEFAULT_SIZE;
    llong m_size = 0LL;
    T *m_bigBuffer = nullptr;
    T *m_data = m_smallBuffer;
    T m_smallBuffer[DEFAULT_SIZE];
};

#define myvector HybridVector
