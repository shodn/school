//  andrewakh
//  started at: 23:30 23.03.2017 KRSK

#include <stdio.h>
#include <new>

#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>

#define ACCUMULATOR 0x2000
#define CHUNKSIZE 0x2000
#define LINE_FEED 0x0a
#define HASH_TABLE_SIZE 16384

typedef uint32_t hashtype;

namespace shitnsticks {
	template<typename T, int FIX>
	class vector {
	public:
		//		union {
		char raw_fixed[FIX*sizeof(T)];
		//			T small_buffer[FIX];
		//		};
		T* data;
		int max_capacity;
		int cur_length;
		
		vector();
		~vector();
		
		int capacity() const { return max_capacity; };
		int size() const { return cur_length; };
		
		void add( const T& value ); // because we can
		T& add(); // easy handmade emplace_back()
		void erase( int index ); // easy handmade erase()
		
		void push_back( const T& value ) { add( value ); }; // for test compatibility
		void erase( const T* item ) { erase( item - data ); }; // for test compatibility
		
		T& operator[]( int index );
		const T& operator[]( int index ) const;
		T* begin() { return data; };
		T* end() { return data+cur_length; };
		
		void clear();
		void resize( int new_size );
		void reserve( int min_capacity );
	protected:
		int newCap() const;
		void reallocateCheck();
		void reallocate();
	};
	
	template<typename T, int FIX>
	vector<T, FIX>::vector():
	data( reinterpret_cast<T*>(raw_fixed) ),
	max_capacity( FIX ),
	cur_length( 0 )
	{}
	
	template<typename T, int FIX>
	vector<T, FIX>::~vector() {
		if( cur_length ) {
			while( cur_length-- ) {
				data[cur_length].~T();
			}
		}
		if( data != reinterpret_cast<T*>(raw_fixed) ) {
			free( data );
		}
	}
	
	template<typename T, int FIX>
	void vector<T, FIX>::add( const T& value ) {
		reallocateCheck();
		data[cur_length++] = value;
	}
	
	template<typename T, int FIX>
	T& vector<T, FIX>::add() {
		reallocateCheck();
		new( &data[cur_length] ) T;
		return data[cur_length++];
	}
	
	template<typename T, int FIX>
	void vector<T, FIX>::erase( int index ) {
		if( !cur_length ) return;
		index ++;
		for( ; index < cur_length; index ++ ) {
			data[index-1] = data[ index ];
		}
		data[ --cur_length ].~T();
	}
	
	template<typename T, int FIX>
	T& vector<T,FIX>::operator[]( int index ) {
		assert( index >= 0 && index < cur_length );
		return data[index];
	}
	
	template<typename T, int FIX>
	const T& vector<T,FIX>::operator[]( int index ) const {
		assert( index >= 0 && index < cur_length );
		return data[index];
	}
	
	
	template<typename T, int FIX>
	void vector<T,FIX>::clear() {
		if( data != reinterpret_cast<T*>(raw_fixed) ) {
			delete[] data;
		}
	}
	
	template<typename T, int FIX>
	void vector<T,FIX>::resize( int new_size ) {
		if( new_size > max_capacity ) reallocate();
		if( new_size >= cur_length ) {
			for( int i = cur_length; i < new_size; i ++ ) {
				new( data + i ) T;
			}
		}
		else {
			
		}
		
		cur_length = new_size;
	}
	
	template<typename T, int FIX>
	void vector<T,FIX>::reserve( int min_capacity ) {
		
	}
	
	
	template<typename T, int FIX>
	int vector<T,FIX>::newCap() const {
		int result = max_capacity <= 0x00010000 / sizeof( T ) ?
		max_capacity * 2 :
		int( int64_t( max_capacity ) * 6 / 5 );
		result = max_capacity * 2;
		//		std::cout << "realloc: " << std::hex << max_capacity << " to: " << result << '\n';
		
		return result;
	}
	
	template<typename T, int FIX>
	void vector<T,FIX>::reallocateCheck() {
		if( cur_length == max_capacity ) reallocate();
	}
	
	template<typename T, int FIX>
	void vector<T,FIX>::reallocate() {
		T* pData = data;
		int nCap = newCap();
		data = (T*) malloc( nCap * sizeof( T ) );
		for( int i = 0; i != max_capacity; i ++ ) {
			data[i] = pData[i];
		}
		if( pData != reinterpret_cast<T*>(raw_fixed) ) {
			free( pData );
		}
		max_capacity = nCap;
	}
	
}

template<typename T>
class myvector : public shitnsticks::vector<T, 32> {};
