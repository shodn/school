#include <stdlib.h>
#include <assert.h>

template<typename T>
class myvector
{
	T *data_;
	int size_;
	int cursor_;

public:
	myvector() : myvector(16)
	{
	}

	myvector(int size) : data_(nullptr), size_(0), cursor_(0)
	{
		data_ = new T[size];
		size_ = size;
	}

	myvector(const myvector<T> &other)
	{
		data_ = new T[other.size_];
		size_ = other.size_;
		cursor_ = other.cursor_;

		for (size_t i = 0; i < other.size(); i++)
		{
			data_[i] = other.data_[i];
		}
	}

	myvector<T> &operator=(myvector<T> other)
	{
		swap(*this, other);
		return *this;
	}

	~myvector()
	{
		if (data_ != nullptr)
		{
			delete[] data_;
		}
	}

	int capacity() const
	{
		return 0;
	}

	int size() const
	{
		return cursor_;
	}

	void add(const T &value)
	{
		push_back(value);
	}

	T &add()
	{
		check_size();
		cursor_ = cursor_ + 1;
		return data_[cursor_ - 1];
	}

	void erase(int index)
	{
		assert(index >= 0 && index < cursor_);
		if (cursor_ == 1)
		{
			data_[0] = T();
		}
		else
		{
			for (int i = index; i < cursor_ - 1; i++)
			{
				data_[i] = data_[i + 1];
			}
		}
		cursor_ = cursor_ - 1;
	}

	void push_back(const T &value)
	{
		check_size();
		data_[cursor_] = value;
		cursor_ = cursor_ + 1;
	}

	void erase(const T *item)
	{
		assert(item >= begin() && item < end());
		erase(item - begin());
	}

	T &operator[](int index)
	{
		assert(index >= 0 && index < cursor_);
		return data_[index];
	}

	const T &operator[](int index) const
	{
		assert(index >= 0 && index < cursor_);
		return data_[index];
	}

	T *begin()
	{
		return &data_[0];
	}

	T *end()
	{
		return &data_[cursor_];
	}

	void clear()
	{
		for (int i = 0; i < cursor_; i++)
		{
			data_[i] = T();
		}
		cursor_ = 0;
	}

	void resize(int new_size)
	{
		myvector<T> t(new_size);
		for (size_t i = 0; i < size_; i++)
		{
			t.data_[i] = data_[i];
		}
		t.cursor_ = cursor_;
		swap(*this, t);
	}

	void reserve(int min_capacity)
	{
		// not implemented
	}

private:
	void swap(myvector<T> &a, myvector<T> &b)
	{
		{
			T *t = a.data_;
			a.data_ = b.data_;
			b.data_ = t;
		}

		{
			int t = a.size_;
			a.size_ = b.size_;
			b.size_ = t;
		}

		{
			int t = a.cursor_;
			a.cursor_ = b.cursor_;
			b.cursor_ = t;
		}
	}

	void check_size()
	{
		if (cursor_ == size_ - 1)
		{
			resize(int(1.2 * size_));
		}
	}
};
