/**
 * Autor: UlturgashevVV;
 * StartTime 13.00 26.03.2017 EndTime: 00.00 27.03.2017;
 */

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include <new>

#ifdef TEST
#include "cpu_time.c"

#include <vector>
#include <algorithm>
#endif

#define UNUSED(value) ((void)value)

template<typename T>
class myvector
{
	enum {
		Capacity = 0,
		GrowFactor = 2,
	};

public:
	myvector()
		: capacity_(Capacity)
		, size_(0)
		, values_(nullptr) {
	}

	~myvector() {
		for (int i = 0; i < size_; ++i) {
			values_[i].~T();
		}

		free(values_);
	}

	int capacity() const {
		return capacity_;
	}

	int size() const {
		return size_;
	}

	void add(const T &value) {
		if (size_ != capacity_) {
			new(&values_[size_]) T(value);
			++size_;
		} else {
			reserve(nextCapacity());
			add(value);
		}
	}

	T &add() {
		// NOTE: ºñº¬º¡º º­º²º§º­º¶ï º¡ºº¡ º¦º§º‘ºº’º¦¶÷¶í¶â¶ð º¬¶÷ º¬º’º²¶á¶åº²º‡º ºƒº¬ ¶âº¬º§º‡ºº­º¦¶ï º’¶áº²º®º²º­º­º¬ºº¬ º¬º‘¶ëº²º¡¶÷º º­º ¶â¶÷º²º¡º².
		add(T());
		return values_[size_ - 1];
	}

	void erase(int index) {
		erase(begin() + index);
	}

	void push_back(const T &value) {
		add(value);
	}

	void erase(const T *item) {
		item->~T();
		--size_;

		for (T *pointer = const_cast<T *>(item); pointer != end(); ++pointer) {
			memmove(pointer, pointer + 1, sizeof(T));
		}
	}

	T &operator[](int index) {
		return values_[index];
	}

	const T &operator[](int index) const {
		return values_[index];
	}

	T *begin() {
		return values_;
	}

	T *end() {
		return values_ + size_;
	}

	void clear() {
		for (auto it = begin(); it != end(); ++it) {
			it->~T();
			// *it = 0;
		}

		capacity_ = Capacity;
		size_ = 0;
		values_ = static_cast<T *>(realloc(values_, sizeof(T) * capacity_));
	}

	void resize(int size) {
		if (size > size_) {
			for (int i = 0, count = size - size_; i < count; ++i) {
				add();
			}
		} else {
			for (int i = size_ - 1; i < size; --i) {
				erase(i);
			}
		}
	}

	void reserve(int capacity) {
		assert(capacity >= 0);
		values_ = (values_ == nullptr) ? static_cast<T *>(malloc(sizeof(T) * capacity))
				  : static_cast<T *>(realloc(values_, sizeof(T) * capacity));

		capacity_ = capacity;
	}

private:
	int nextCapacity() {
		return capacity_ == 0 ? 1 : capacity_ * GrowFactor;
	}

private:
	int capacity_;
	int size_;
	T *values_;
};
