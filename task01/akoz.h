#include <assert.h>
#include <new>

template<class T>
struct IsPod
{
#ifdef _MSC_VER
	static const bool value = __is_pod(T) && __has_trivial_constructor(T);
#else
	static const bool value = __is_pod(T);
#endif
};

struct TrivialTag {};
struct NontrivialTag{};
template <bool is_pod>
struct Dispatch{ typedef TrivialTag tag; };
template<> struct Dispatch<true> { typedef TrivialTag tag; };
template<> struct Dispatch<false> { typedef NontrivialTag tag; };


template<class T>
T& move(T& value)
{
	return value;
}

template<class T>
T min_value(const T& a, const T&b)
{
	return a < b ?  a : b;
}


template<class T>
class myvector final
{
public:
	static const auto min_capacity = 16;

public:
	myvector() = default;
	~myvector()
	{
		destroy_elements(begin_, end_, typename Dispatch<IsPod<T>::value>::tag());
		deallocate(begin_);
	}

public:
	int capacity() const { return capacity_end_ - begin_; }
	int size() const { return end_ - begin_; }

	void add(const T& value)
	{
		assert(size() < INT_MAX);
		reserve(size() + 1);
		new (end_)T(value);
		++end_;
	}
	T & add()
	{
		assert(size() < INT_MAX);
		reserve(size() + 1);
		new (end_)T();
		++end_;
		return *(end_ - 1);
	};
	void erase(const int index)
	{
		assert(index >= 0 && index < size());
		(begin_ + index)->~T();
		move_elements(begin_ + index, begin_ + index + 1, size() - index - 1, typename Dispatch<IsPod<T>::value>::tag());
		--end_;
	}

	// STL compatibility
	void push_back(const T& value) { add(value); }
	T & emplace_back() { return add(); }
	void erase(const T * const iterator) { assert(iterator >= begin_); erase(iterator - begin_); }

	T & operator[](const int index)
	{
		assert(index >= 0 && index < size());
		return begin_[index];
	}
	const T & operator[](const int index) const
	{
		assert(index >= 0 && index < size());
		return begin_[index];
	}

	T * begin() { return begin_; }
	T * end() { return end_; }

	void clear()
	{
		destroy_elements(begin_, end_, typename Dispatch<IsPod<T>::value>::tag());
		end_ = begin_;
	}
	void resize(const int desirable_size)
	{
		assert(desirable_size >= 0 && desirable_size <= INT_MAX);
		const auto current_size = size();
		if (desirable_size < current_size)
		{
			const auto new_end = end_ - (size() - desirable_size);
			destroy_elements(new_end, end_, typename Dispatch<IsPod<T>::value>::tag());
			end_ = new_end;
			return;
		}
		else if (desirable_size > current_size)
		{
			reserve(desirable_size);

			const auto new_end = begin_ + desirable_size;
			for (auto iterator = end_; iterator != new_end; ++iterator)
			{
				new (iterator) T();
			}
			end_ = new_end;
		}
	}
	void reserve(const int desirable_size)
	{
		assert(desirable_size >= 0 && desirable_size <= INT_MAX);
		if (capacity() < desirable_size)
		{
			grow(desirable_size);
		}
	}
		
private:
	void move_elements(T *destination, T *source, const int source_size, NontrivialTag)
	{
		auto dest_iterator = destination;
		auto source_iterator = source;
		const auto source_end = source + source_size;
		while (source_iterator != source_end)
		{
			*dest_iterator = move(*source_iterator);
			++dest_iterator;
			++source_iterator;
		}
	}
	void move_elements(T * const destination, T * const source, const int source_size, TrivialTag)
	{
		memmove(destination, source, source_size * sizeof(T));
	}
	void copy_elements(T * const destination, T * const source, const int source_size, NontrivialTag)
	{
		auto dest_iterator = destination;
		auto source_iterator = source;
		const auto source_end = source + source_size;
		while (source_iterator != source_end)
		{
			new (dest_iterator)T(move(*source_iterator));
			++dest_iterator;
			++source_iterator;
		}
	}
	void copy_elements(T * const destination, T * const source, const int source_size, TrivialTag)
	{
		memcpy(destination, source, source_size * sizeof(T));
	}
	void destroy_elements(T * const, T * const, TrivialTag)
	{
	}
	void destroy_elements(T * const elements_begin, T * const elements_end, NontrivialTag)
	{
		assert(elements_begin <= elements_end);
		for (auto iterator = elements_begin; iterator != elements_end; ++iterator)
		{
			iterator->~T();
		}
	}
	T * allocate(const int elements_count)
	{
		return static_cast<T*>(operator new[](elements_count * sizeof(T)));
	}
	void deallocate(T * const it)
	{
		operator delete[](it);
	}
	double grow_factor()
	{
		const auto change_factor_capacity = 100000;
		const auto small_capacity_grow_factor = 2.;
		const auto big_capacity_grow_factor = 1.2;
		if (capacity() < change_factor_capacity)
		{
			return small_capacity_grow_factor;
		}
		else
		{
			return big_capacity_grow_factor;
		}
	}
	void grow(const int desirable_size)
	{
		const auto current_size = size();
		unsigned new_size = current_size;
		while (new_size < static_cast<unsigned>(desirable_size))
		{
			new_size = new_size ?
				min_value<unsigned>(static_cast<unsigned>(static_cast<double>(new_size) * grow_factor()), INT_MAX)
				: min_capacity;
		}
		const auto new_begin = allocate(new_size);
		const auto new_end = new_begin + current_size;
		const auto new_capacity_end = new_begin + new_size;

		copy_elements(new_begin, begin_, current_size, typename Dispatch<IsPod<T>::value>::tag());
		destroy_elements(begin_, end_, typename Dispatch<IsPod<T>::value>::tag());
		deallocate(begin_);

		begin_ = new_begin;
		end_ = new_end;
		capacity_end_ = new_capacity_end;
	}

private:
	T *begin_ = nullptr;
	T *end_ = nullptr;
	T *capacity_end_ = nullptr;
};
