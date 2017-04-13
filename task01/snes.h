#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <climits>
#include <cmath>
#include <vector>
#include <string>

#include <iostream>

const int startSize = 32;
const int bigSize = 1e5;
const double mediumK = 2;
const double bigK = 1.2;

typedef long long llong;
typedef unsigned long long ullong;

template<typename T> bool isPOD(T &t)
{
	return false;
}

template<> bool isPOD<int>(int &a)
{
	return true;
}

template<> bool isPOD<char>(char &a)
{
	return true;
}

template<> bool isPOD<float>(float &a)
{
	return true;
}

template<> bool isPOD<double>(double &a)
{
	return true;
}
template<typename T> class myvector
{
public:
	myvector() :
		data(small),
		maxLen(startSize),
		len(0) {}
	~myvector()
	{
		clear();
	}

	int capacity()
	{
		return maxLen;
	}
	int size()
	{
		return len;
	}

	void add(const T & value) // because we can
	{
		if (len + 1 == maxLen)
		{
			if( maxLen < bigSize )
				reserve( maxLen * mediumK );
			else
				reserve( maxLen * bigK );
		}
		*(data + len) = value;
		++len;
	}
	T & add() // easy handmade emplace_back()
	{
		add( T() );
		return *(data + len - 1);
	}
	void erase(int index) // easy handmade erase()
	{
	    assert( 0 <= index && index < len );
		if( !isPOD(*data))
			(data + index)->~T();
        shiftleft(data+index+1, data+len, 1);
		--len;
	}

	void push_back(const T & value) // for test compatibility
	{
		add(value);
	}
	void erase(const T * item) // for test compatibility
	{
		erase( item - data );
	}

	T & operator[](int index)
	{
		assert( 0 <= index && index < len );
		return data[index];
	}
	const T & operator[](int index) const
	{
		assert( 0 <= index && index < len );
		return data[index];
	}

	T * begin()
	{
		return data;
	}
	T * end()
	{
		return data+len;
	}

	void clear()
	{
        if( !isPOD(*data) )
        {
            if( data == small )
            {
                for(int i=0; i<maxLen; ++i)
                    (data+i)->~T();
            }
            else
                delete[] (data);
	    }
        data = small;
		len = 0;
		maxLen = startSize;
	}
	void resize(int new_size)
	{
	    assert( 0 <= new_size && new_size < INT_MAX );
		if( new_size > len )
		{
			if( new_size > maxLen )
				reserve(new_size);
		}
		if( new_size < len )
		{
		    if( !isPOD(*data) )
			{
				for(int i=new_size; i<len; ++i)
					(data+i)->~T();
			}
		}
		len = new_size;
	}
	void reserve(int min_capacity)
	{
	    assert( 0 <= min_capacity && min_capacity < INT_MAX );
		if( min_capacity > maxLen )
		{
			maxLen = min_capacity;
			if( data == small)
			{
				data = new T[maxLen];
				copy(small, small+len, data);
			}
			else
			{
				T * new_data = new T[maxLen];
				copy(data, data+len, new_data);
				delete[] (data);
				data = new_data;
			}
		}
	}
private:
	void shiftleft(T *first, T *last, int shift)
	{
	    T *result = first - shift;
		while( first != last )
		{
			*result = *first;
			++first;
			++result;
		}
	}
	T * data;
	int maxLen;
	int len;
	T small[startSize];
};
