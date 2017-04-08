#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <new>
#include <cassert>

template<typename T, bool POD = false> 
class myvector
{
    using size_type = long long;
public:
	myvector(){}
	~myvector()
    {
        if(data_ != nullptr && data_ != reinterpret_cast<T*>(stack_data_))
            free(data_);
    }

	int capacity()
    {
        return static_cast<int>(capacity_);
    }
	int size()
    {
        return static_cast<int>(size_);
    }
	void add(const T & value)
    {
        add();
        data_[size_ - 1] = value;
    }
	T & add()
    {
        if(size_ == capacity_)
            grow();
        ++size_;
        new (data_ + size_ - 1) T();
        return data_[size_ - 1];
    }
	void erase(int index)
    {
        assert(index >= 0 && index < size_);
        if(POD)
        {
            std::memcpy(data_ + index, data_ + index + 1, size_ - index - 1);
        }
        else
        {
            data_[index].~T();
            if(index != size_ - 1)
            {
            new (data_ + index) T();
            for(int i = index; i < size_ - 1; ++i)
                data_[i] = data_[i + 1];
            data_[size_ - 1].~T();
            }
        }
        --size_;
    }

	void push_back(const T & value)
    {
        add();
        data_[size_ - 1] = value;
    }
	void erase(const T * item)
    {
        assert(item >= data_ && item < data_ + size_);
        erase(static_cast<int>(item - data_));
    }

	T & operator[](int index)
    {
        assert(index >= 0 && index < size_);
        return data_[index];
    }
	const T & operator[](int index) const
    {
        assert(index >= 0 && index < size_);
        return data_[index];
    }

	T * begin()
    {
        return data_;
    }
	T * end()
    {
        return data_ + size_;
    }

	void clear()
    {
        for(int i = 0; i < size_; ++i)
            data_[i].~T();
        size_ = 0;
    }
	void resize(int new_size)
    {
        assert(new_size >= 0);
        if(new_size >= capacity_)
        {
            reserve(new_size);
            for(int i = 0; i < size_; ++i)
            {
                new (&data_[i]) T();
            }
        }
        else if(new_size < size_)
        {
           for(int i = 0; i < size_; ++i)
           {
               data_[i].~T();
           }
        }
        size_ = new_size;
    }
	void reserve(int min_capacity)
    {
        assert(min_capacity >= 0);
        T* tmp = nullptr;
        if(min_capacity <= capacity_)
            return;
        if(POD)
        {
            tmp = static_cast<T*>(malloc(min_capacity * sizeof(T)));
            if(tmp == nullptr)
                throw "err";
            std::memcpy(tmp, data_, size_);
        }
        else
        {
           tmp = static_cast<T*>(malloc(sizeof(T) * min_capacity));
            if(tmp == nullptr)
                throw "err";
            for(int i = 0; i < size_; ++i)
            {
                new (tmp + i) T();
                tmp[i] = data_[i];
                data_[i].~T();
            }
        }
        if(data_ != reinterpret_cast<T*>(stack_data_))
            free(data_);
        data_ = tmp;
        capacity_ = min_capacity;
    }
    T* data()
    {
        return data_;
    }
private:
    void grow()
    {
        double growth_factor;
        
        if(data_ == reinterpret_cast<T*>(stack_data_))
        {
            growth_factor = 2;
        }
        else if(capacity_ * sizeof(T) > 512 && capacity_ * sizeof(T) < 4096)
        {
            growth_factor = 1.5;
        }
        else
        {
            growth_factor = 1.2;
        }
        
        T* tmp = nullptr;

        if(POD && data_ != reinterpret_cast<T*>(stack_data_))
        {
            tmp = static_cast<T*>(realloc(data_, capacity_ * growth_factor * sizeof(T)));
            assert(tmp != nullptr);
        }
        else if(POD && data_ == reinterpret_cast<T*>(stack_data_))
        {
            tmp = static_cast<T*>(malloc(capacity_ * growth_factor * sizeof(T)));
            assert(tmp != nullptr);
            std::memcpy(tmp, data_, size_);
            if(data_ != reinterpret_cast<T*>(stack_data_))
                free(data_);
        }
        else
        {
            tmp = static_cast<T*>(malloc(sizeof(T) * capacity_ * growth_factor));
            assert(tmp != nullptr);
            for(int i = 0; i < size_; ++i)
            {
                new (tmp + i) T();
                tmp[i] = data_[i];
                data_[i].~T();
            }
            if(data_ != reinterpret_cast<T*>(stack_data_))
                free(data_);
        }
        data_ = tmp;
        capacity_ *= growth_factor;
    }

    size_type capacity_ = 32;
    size_type size_ = 0;
    char stack_data_[32 * sizeof(T)];
    T* data_ = reinterpret_cast<T*>(stack_data_);
};