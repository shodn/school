#include <stdlib.h>

#include <string.h>
#include <assert.h>

typedef long long llong;
typedef unsigned long long ullong;

template<typename T> class myvector
{
public:
    myvector()
    {
        m_data = allocate(DEFAULT_CAPACITY);
        m_capacity = DEFAULT_CAPACITY;
        m_size = 0;
    }
    ~myvector()
    {
        delete[] m_data;
    }

    int capacity()
    {
        return (int)m_capacity;
    }
    int size()
    {
        return (int)m_size;
    }

    void add(const T & value)
    {
        if (m_size < m_capacity)
        {
            memcpy(m_data + m_size, &value, sizeof(T));
            m_size++;
        }
        else
        {
            reserve(m_size != 0 ? (int)m_size * 2 : DEFAULT_CAPACITY);
            add(value);
        }
    }
    T & add()
    {
        add(T());
        return m_data[m_size - 1];
    }
    void erase(int index)
    {
        assert(index >= 0 && index < m_size);
        m_data[index]->T::~T();
        memcpy(m_data + index, m_data + index + 1, (size_t)m_size - index);
        m_size--;
    }

    void push_back(const T & value)
    {
        add(value);
    }
    void erase(const T * item)
    {
        llong i = 0;
        while (m_data[i] != *item && i != m_size)
        {
            i++;
        }
        if (i == m_size)
        {
            return;
        }
        erase((int)i);
    }

    T & operator[](int index)
    {
        T* current = m_data + index;
        return *current;
    }

    const T & operator[](int index) const
    {
        const T* current = m_data + index;
        return *current;
    }

    T * begin()
    {
        return m_data;
    }
    T * end()
    {
        return m_data + m_size;
    }

    void clear()
    {
        delete[] m_data;
        m_data = NULL;
        m_capacity = 0;
        m_size = 0;
    }
    void resize(int new_size)
    {
        if (new_size == m_size)
        {
            return;
        }
        T * tmp = NULL;
        try
        {
            tmp = new T[new_size];
        }
        catch (...)
        {
            printf("Cannot allocate memory for vector, size %d", new_size);
            exit(1);
        }
        llong minSize = myvector::min(new_size, m_size);
        memcpy(tmp, m_data, (size_t)minSize * sizeof(T));
        delete[] m_data;
        m_data = tmp;
        m_size = new_size;
        m_capacity = new_size;
    }
    void reserve(int min_capacity)
    {
        if (min_capacity > m_capacity)
        {
            T * tmp = allocate(min_capacity);
            memcpy(tmp, m_data, (size_t)m_size * sizeof(T));
            delete[] m_data;
            m_data = tmp;
            m_capacity = min_capacity;
        }
    }

private:
    T* allocate(size_t size)
    {
        T * tmp = NULL;
        try
        {
            tmp = (T*)malloc(sizeof(T)*size);
        }
        catch (...)
        {
            printf("Cannot allocate memory for vector, size %d", size);
            exit(1);
        }
        return tmp;
    }
    const llong& min(const llong& a, const llong& b)
    {
        return !(b < a) ? a : b;
    }
private:
    T *     m_data;
    ullong  m_capacity;
    ullong  m_size;

    static const int DEFAULT_CAPACITY = 32;
};
