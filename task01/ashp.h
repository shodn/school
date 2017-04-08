#include <string.h>
#include <stdlib.h>
#include <new>

namespace
{
    const float factor = 2.f;
    class NonPod
    {
        int _val = 0;
        int *_bigdata = nullptr;
    public:
        NonPod() : _val(42) 
		{
			_bigdata = new int[128];
		}
		
		~NonPod()
		{
			delete[] _bigdata;
			_bigdata = nullptr;
		}
		NonPod(const NonPod &other)
		{
			this->_val = other._val;
			this->_bigdata = new int[128];
			memcpy(this->_bigdata, other._bigdata, sizeof(int) * 128);
		}

		NonPod &operator=(const NonPod &other)
		{
			this->_val = other._val;
			memcpy(this->_bigdata, other._bigdata, sizeof(int) * 128);
			return *this;
		}
    };
#define DEFAULT_CAPACITY 64

    template<typename T> class myvector
    {
        T *_buffer = nullptr;
        int _capacity = DEFAULT_CAPACITY;
        int _size = 0;
        T _preallocated[DEFAULT_CAPACITY];

        void reallocate(int new_capacity)
        {
			// here, new_capacity is guaranteed to be greater than current capacity
			// (and thus, greater than current size)
            _capacity = new_capacity;

            T *newdata = static_cast<T*>(malloc(_capacity * sizeof(T)));
            for (int i = 0; i < _size; ++i)
			{
				new(newdata + i) T();
                newdata[i] = _buffer[i];
                (_buffer + i)->~T();
            }

            _buffer = newdata;
        }

    public:
        myvector()
        {
			reallocate(64);
			//_buffer = _preallocated;
        }

        ~myvector()
        {
            for (int i = 0; i < _size; ++i)
                (_buffer + i)->~T();

            if (_buffer != _preallocated)
                free(_buffer);

        }

        int capacity() const
        {
            return _capacity;
        }

        int size() const
        {
            return _size;
        }

        // because we can
        void add(const T & value)
        {
            if (_capacity == _size)
                reallocate(_capacity * factor);

            new(_buffer + _size) T();
			_buffer[_size] = value;
            ++_size;
        }

        // easy handmade emplace_back()
        T & add()
        {
			if (_capacity == _size)
				reallocate(_capacity * factor);

            new (_buffer + _size) T();
            ++_size;
            return _buffer[_size - 1];
        }

        // easy handmade erase()
        void erase(int index)
        {
            (_buffer + index)->~T();

            for (int i = index; i <_size; ++i)
            {
                _buffer[i] = _buffer[i+1];
                (_buffer + i + 1)->~T();
            }
            --_size;
        }

        // for test compatibility
        void push_back(const T & value)
        {
			add(value);
        }

        // for test compatibility
        void erase(const T * item)
        {
			erase(item - _buffer);
        }

        T & operator[](int index)
        {
			return _buffer[index];
        }

        const T & operator[](int index) const
        {
			return _buffer[index];
        }

        T * begin()
        {
            return _buffer;
        }

        T * end()
        {
            return _buffer + _size;
        }

        void clear()
        {
			for (int i = 0; i < _size; ++i)
                (_buffer + i)->~T();
            _size = 0;
        }

        void resize(int new_size)
        {
            if (new_size > _capacity)
                reallocate(new_size);

            if (new_size > _size)
            {
                for (int i = _size; i < new_size; ++i)
                        new (_buffer + i) T();

                _size = new_size;
            }
            else if (new_size < _size)
            {
                for (int i = new_size; i < _size; ++i)
                    (_buffer + i)->~T();
                _size = new_size;
            }
        }

        void reserve(int min_capacity)
        {
			if (min_capacity <= _capacity)
				return;

            reallocate(min_capacity);
        }
    };
}
