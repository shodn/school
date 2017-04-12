#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <new>

typedef unsigned int uint;

template<typename T, bool POD = false, uint StaticSize = 32>
class myvector
{

public:
    myvector()
        : m_data(nullptr)
        , m_capacity(StaticSize)
        , m_size(0)
    {
        m_data = m_static_data;
    }

    ~myvector()
    {}

    int capacity() const
    {
        return m_capacity;
    }

    int size() const
    {
        return m_size;
    }

    void add(const T & value)
    {
        if (m_size == m_capacity)
            reserve(m_size * growthFactor);
        if (POD)
            m_data[m_size] = value;
        else
        {
            new (m_data + m_size) T(value);
        }

        ++m_size;
    }

    T& add()
    {
        add(T());
        return m_data[m_size - 1];
    }

    void erase(int index)
    {
        assert(index >= 0 && index < m_size && "Index out of range");
        if (!POD)
            (m_data + index)->~T();
        memmove(m_data + index, m_data + index + 1, (m_size - index) * sizeof(T));
        --m_size;

        int thold = m_capacity / growthFactor;
        if (m_size <= thold)
            reserve(thold);
    }

    void push_back(const T & value)
    {
        add(value);
    }

    void erase(const T * item)
    {
        erase(item - m_data);
    }

    T& operator[](int index)
    {
        assert(index >= 0 && index < m_size && "Index out of range");
        return m_data[index];
    }

    const T& operator[](int index) const
    {
        assert(index >= 0 && index < m_size);
        return m_data[index];
    }

    T* begin()
    {
        return m_data;
    }

    T* end()
    {
        return m_data + m_size;
    }

    void clear()
    {
        resize(0);
    }

    void resize(int new_size)
    {
        assert(new_size >= 0 && "Negative size");
        if (new_size > m_size)
        {
            reserve(new_size);
            if (!POD)
                _call_ctor_on_range(m_size, new_size);
        }
        else
        {
            if (!POD)
                _call_dtor_on_range(m_size, new_size);
            reserve(new_size);
        }
        m_size = new_size;
    }

    void reserve(int min_capacity)
    {
        assert(min_capacity >= 0 && "Negative capacity");
        if (min_capacity > StaticSize)
        {
            if (m_data == m_static_data)
            {
                m_data = (T*) malloc(min_capacity * sizeof(T));
                memcpy(m_data, m_static_data, StaticSize * sizeof(T));
            }
            else
                m_data = (T*) realloc(m_data, min_capacity * sizeof(T));

            m_capacity = min_capacity;
        }
        else
        {
            if (m_data != m_static_data)
            {
                memcpy(m_static_data, m_data, StaticSize * sizeof(T));
                free(m_data);
                m_data = m_static_data;
                m_capacity = StaticSize;
            }
        }
    }

private:
    void _call_dtor_on_range(const int& from, const int& to)
    {
        for (int i = from; i < to; ++i)
            (m_data + i)->~T();
    }

    void _call_ctor_on_range(const int& from, const int& to)
    {
        for (int i = from; i < to; ++i)
            new (m_data + i) T();
    }

private:
    T* m_data;
    uint m_capacity;
    uint m_size;
    T m_static_data[StaticSize];
    const float growthFactor = sqrt(2.0f);
};
