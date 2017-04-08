#include <stdio.h>
#include <vector>
#include <time.h>
#include <cassert>
#include <random>

/////// SHODAN-CODE AHEAD 
typedef long long llong;
typedef unsigned long long ullong;

/////// PAVEL-CODE AHEAD. Abandon hope all ye who enter here

template<typename T> class myvector
{
public:
	myvector()
	{
		mResizeFactor = 1.2f;
		mSize = 0;
		mCapacity = 0;
		mData = NULL;
	}

	~myvector()
	{
		clear();
	}

	myvector(float resizeFactor) : mResizeFactor(resizeFactor), mSize(0), mCapacity(0), mData(NULL)
	{
	}

	__forceinline int capacity()
	{
		return mCapacity;
	}

	__forceinline int size()
	{
		return mSize;
	}

	void add(const T & value)
	{
		if (mSize >= mCapacity)
		{
			reserve((int)((mCapacity + 1) * mResizeFactor));
		}

		mData[mSize++] = std::move(value);
	}

	T & add()
	{
		if (mSize >= mCapacity)
		{
			reserve((int)((mCapacity+1) * mResizeFactor));
		}

		return mData[mSize++];
	}

	void push_back(const T & value)
	{
		add(value);
	}

	void erase(int index)
	{
		assert((index >= 0) && (index < mSize) && "Index out of range");

		for (int i = index; i < (mSize - 1); ++i)
		{
			mData[i] = std::move(mData[i + 1]);
		}
		--mSize;

		if (!__is_pod(T))
		{
			mData[mSize].~T();
		}
	}

	void erase(const T * item)
	{
		erase((int)(item - mData));
	}

	__forceinline T & operator[](int index)
	{
		assert((index >= 0) && (index < mSize) && "Index out of range");
		return mData[index];
	}

	__forceinline const T & operator[](int index) const
	{
		assert((index >= 0) && (index < mSize) && "Index out of range");
		return mData[index];
	}

	__forceinline T * begin()
	{
		return mData;
	}

	__forceinline T * end()
	{
		return mData + mSize;
	}

	void clear()
	{
		if (!__is_pod(T))
		{
			for (int i = 0; i < mSize; ++i)
			{
				mData[i].~T();
			}
		}
		mSize = 0;
	}

	void resize(int new_size)
	{
		assert((new_size >= 0) && "Index out of range");
		if (new_size < mCapacity)
		{
			delete[] & mData[new_size];
		}
		else
		{
			reserve(new_size);
		}
		mCapacity = new_size;
		mSize = mSize < new_size ? mSize : new_size;
		if (!mCapacity)
			mData = NULL;
	}

	void reserve(int min_capacity)
	{
		if (min_capacity <= mCapacity)
			return;

		T* tempData = new T[min_capacity];
		for (int i = 0; i < mSize; ++i)
		{
			tempData[i] = std::move(mData[i]);
		}

		delete[] mData;
		mData = tempData;
		mCapacity = min_capacity;
	}

protected:
	int			mSize;
	int			mCapacity;
	float		mResizeFactor;
	T*			mData;
};

