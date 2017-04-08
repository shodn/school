# ifndef H_RDUS_MYVEC_H
# define H_RDUS_MYVEC_H

# include <cstdlib>
# include <stdexcept>
# include <climits>
# include <cassert>

# if __cplusplus <= 199711L
# define nullptr_C11 NULL
# else
# define nullptr_C11 nullptr
# endif

template<typename T>
struct DefaultAllocator12;

template<typename T,
         typename AllocatorT=DefaultAllocator12<T> > class myvector {
public:
    typedef myvector<T, AllocatorT> Self;
    typedef T * iterator;
private:
    T * _data,
      * _end,
      * _reservedBgn,
      * _reservedEnd;
public:
    // Required interface:
	myvector() : _data(nullptr_C11),
                 _end(nullptr_C11),
                 _reservedBgn(nullptr_C11),
                 _reservedEnd(nullptr_C11) {}
	~myvector() { clear(); }

	int capacity() { return _reservedEnd - _data; }
	int size() { return _end - _data; }

	void add(const T & value); // because we can
	T & add(); // easy handmade emplace_back()
	void erase(int index); // easy handmade erase()

	void push_back(const T & value); // for test compatibility
	void erase(const T * item); // for test compatibility

	T & operator[](int index) {
            return at(index); }
	const T & operator[](int index) const {
            return at(index); }

	T * begin() { return _data; }
	T * end() { return _end; }

	void clear();
	void resize(int new_size);
	void reserve(int min_capacity);
    // ^^^ end of required interface
public:
    // sugar/helpers/aux...
    const T & at( int index ) const;
    T & at( int index ) {
        const Self * cSelf = this;
        return const_cast<T&>( cSelf->at(index) ); }
	int size() const { return _end - _data; }
};  // myvector

template<typename T, typename AllocatorT> void
myvector<T, AllocatorT>::add(const T & value) {
    push_back(value);
}

template<typename T, typename AllocatorT> T &
myvector<T, AllocatorT>::add() {
    add( T() );
    return *(end()-1);
}

template<typename T, typename AllocatorT> void
myvector<T, AllocatorT>::erase( int index ) {
    erase( &(at(index)) );
}

template<typename T, typename AllocatorT> void
myvector<T, AllocatorT>::push_back(const T & value) {
    if( !(_end < _reservedEnd) ) {
        reserve( size() + 1 );
    }
    *(_end++) = value;
}

// meaningful stuff:

template<typename T, typename AllocatorT> const T &
myvector<T, AllocatorT>::at( int index ) const {
    assert( _data );
    if(!(size() > index)) {
        throw std::out_of_range( "Custom vector index overflow." );
    }
    return *(_data + index);
}

template<typename T, typename AllocatorT> void
myvector<T, AllocatorT>::erase(const T * item) {
    if( item > _end || item < _data ) {
        throw std::out_of_range( "Invalid element ptr to be erased." );
    }
    AllocatorT::erase_one( _data, const_cast<T*>(item), _end );
}

template<typename T, typename AllocatorT> void
myvector<T, AllocatorT>::clear() {
    AllocatorT::clear( _reservedBgn );
    _data = _end = _reservedBgn = _reservedEnd = nullptr_C11;
}

template<typename T, typename AllocatorT> void
myvector<T, AllocatorT>::resize(int new_size) {
    AllocatorT::reallocate( _data, _end, _reservedBgn, _reservedEnd, new_size );
}

template<typename T, typename AllocatorT> void
myvector<T, AllocatorT>::reserve(int min_capacity) {
    AllocatorT::reallocate( _data, _end, _reservedBgn, _reservedEnd, min_capacity );
}


//
// Dumb allocation strategy with increasing factor 1.2

template<typename T>
struct DefaultAllocator12 {
    static void erase_one( T *& bgn, T * item, T *& dEnd ) {
        // Deletes a particular element inside a block of memory
        // preserving the integrity of the remained sequence.
        T * cc = item;
        if( item - bgn <= dEnd - bgn ) {
            for( T * c = item + 1; c != dEnd; ++c, ++cc ) {
                *cc = *c;
            }
            --dEnd;
        } else {
            for( T * c = item - 1; c >= bgn; --c, --cc ) {
                *cc = *c;
            }
            ++bgn;
        }
    }
    
    static void clear( T * dat ) {
        delete [] dat;
    }

    static void reallocate( T *& bgn, T *& dEnd, T *& rBgn, T *& rEnd, int n ) {
        if(!(rEnd - bgn < n)) return;
        int nn;
        T * newdat = new T [nn = fine_block_size(rEnd - rBgn, n)];
        if( bgn ) {
            //memcpy( newdat, bgn, sizeof(T)*(dEnd - bgn) );  // good only for POD
            # if 1
            std::copy( bgn, dEnd, newdat );
            # else
            T * cc = newdat;
            for( T * c = bgn; c != dEnd; ++c, ++cc ) {
                *cc = *c;
            }
            # endif
            // FIXME: according to STL, new objects at reserved space has to
            // be _inserted_ apparently meaning that for POD type they have to
            // be initially set to zero.
            clear( rBgn );
        }
        rBgn = bgn = newdat;
        rEnd = bgn + nn;
        dEnd = bgn + n - 1;
    }

    static size_t fine_block_size( int oldSize, size_t newSize ) {
        if( newSize >= INT_MAX ) {
            throw std::bad_alloc();  // Bad reallocation block size requested.
        }
        // FIXME: precious magic numbers:
        if( newSize < 32 ) {
            return 32;
        } else {
            newSize *= 2;
            return (newSize-newSize%8)+8;
        }
    }
};

# endif  // H_RDUS_MYVEC_H
