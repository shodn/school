#pragma once

#include <cstdint>
#include <ratio>
#include <memory>
#include <algorithm>

template<typename T, size_t InitialCapacity = 16, typename GrowthFactor = std::ratio<2, 1> > 
class myvector
{
public:
    using value_type = T;
    using allocator_type = std::allocator<value_type>;
    using size_type = std::int32_t;
    using reference = value_type&;
    using const_referece = const value_type&;
    using pointer = value_type*;
    using const_pointer = const pointer;

    using initial_capacity = std::integral_constant<size_type, InitialCapacity>;
    using growth_factor = GrowthFactor;

    myvector()
    {
    }

    ~myvector()
    {
        clear();
        mAllocator.deallocate(mData, mCapacity);
    }

    size_type capacity() const
    {
        return mCapacity;
    }

    size_type size() const
    {
        return mSize;
    }

    // because we can
    void add(const_referece value)
    {
        push_back(value);
    }

    // easy handmade emplace_back()
    reference add()
    {
        if (mSize == mCapacity)
        {
            grow(mSize + 1);
        }
        std::allocator_traits<allocator_type>::construct(mAllocator, mData + mSize);
        return *(mData + mSize++);
    }

    // easy handmade erase()
    void erase(size_type index)
    {
        erase(mData + index);
    }


    // for test compatibility
    void push_back(const T& value)
    {
        if (mSize == mCapacity)
        {
            grow(mSize + 1);
        }
        std::allocator_traits<allocator_type>::construct(mAllocator, mData + mSize, value);
        mSize++;
    }

    void push_back(T&& value)
    {
        if (mSize == mCapacity)
        {
            grow(mSize + 1);
        }
        std::allocator_traits<allocator_type>::construct(mAllocator, mData + mSize, std::forward<T>(value));
        mSize++;
    }

    // for test compatibility
    void erase(const_pointer item)
    {
        std::allocator_traits<allocator_type>::destroy(mAllocator, item);
        std::move(item + 1, mData + mSize, item);
        mSize--;
    }


    reference operator[](size_type index)
    {
        return mData[index];
    }

    const_referece operator[](size_type index) const
    {
        return mData[index];
    }

    pointer begin()
    {
        return mData;
    }

    pointer end()
    {
        return mData + mSize;
    }

    void clear()
    {
        std::for_each(mData, mData + mSize, [this](reference ref)
        {
            std::allocator_traits<allocator_type>::destroy(mAllocator, &ref);
        });
        mSize = 0;
    }

    void resize(size_type count)
    {
        if (count > mSize)
        {
            if (count > mCapacity)
            {
                grow(count);
            }

            std::for_each(mData + mSize, mData + count, [this](reference ref)
            {
                std::allocator_traits<allocator_type>::construct(mAllocator, &ref);
            });

            mSize = count;
        }
        else if (count < mSize)
        {
            std::for_each(mData + count, mData + mSize, [this](reference ref)
            {
                std::allocator_traits<allocator_type>::destroy(mAllocator, &ref);
            });
            mSize = count;
        }
    }

    void reserve(size_type min_cap)
    {
        if (min_cap > mCapacity)
        {
            grow(min_cap);
        }
    }
private:

    void grow(const size_type desired_size)
    {
        // calc new capacity
        size_type new_capacity = mCapacity;
        while (new_capacity < desired_size)
        {
            new_capacity *= growth_factor::num / growth_factor::den;
        }

        // allocate memory
        pointer buffer = std::allocator_traits<allocator_type>::allocate(mAllocator, new_capacity);
        std::move(mData, mData + mSize, buffer);

        std::swap(mData, buffer);
        std::swap(mCapacity, new_capacity);

        // release memory
        std::for_each(buffer, buffer + mSize, [this](reference ref)
        {
            std::allocator_traits<allocator_type>::destroy(mAllocator, &ref);
        });
        mAllocator.deallocate(buffer, new_capacity);
    }


    allocator_type mAllocator;
    pointer mData = std::allocator_traits<allocator_type>::allocate(mAllocator, initial_capacity::value);
    size_type mCapacity = initial_capacity::value;
    size_type mSize = 0;
};
