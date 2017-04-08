#include <stdio.h>
#include <assert.h>

#include <vector>

#define MAXINT INT_MAX

typedef long long llong;
typedef unsigned long long ullong;

//-----------------------------------------------------------------------------------------------------------------
template<typename T>
class myvector
{
public:
     myvector()
     {
          m_capacity = 4;
          m_data     = (T*)malloc(m_capacity * sizeof(T));
     }

    ~myvector()
     {
         clear();
         free(m_data);
     }

     const int  capacity() const { return m_capacity; }
     const int  size    () const { return m_size;     }
     const bool empty   () const { return m_size == 0; }

     void add( const T &val )
     {
          push_back( val );
     }

     T& add()
     {
          if ( m_size == m_capacity )
               reallocate( expand_capacity( m_capacity ));

          new (m_data + m_size) T();
          ++m_size;

          return m_data[m_size-1];
     }

     void push_back( const T &val )
     {
          if ( m_size == m_capacity )
               reallocate( expand_capacity( m_capacity ));

          new (m_data + m_size) T( val );
          ++m_size;
     }

     void erase( int idx )
     {
          assert(idx >= 0 && idx < m_size);

          erase( &m_data[idx] );
     }

     void erase( const T *elem )
     {
          assert( (m_data <= elem) && (elem < m_data+m_size));

          T tmp = *elem;                     // to call dtor for copy of deleting element 

          for ( T *p = const_cast<T*>(elem),  *end = m_data + (m_size - 1); p != end; ++p )
               p[0] = p[1];

          --m_size;

          if ( m_size > 0 )
               (m_data + m_size)->~T();      // total: 2 dtors, 1 copy ctor, (size-1) copy
     }

     T& operator[] ( int idx )
     {
          assert( idx >= 0 && idx < m_size );
          return m_data[idx];
     }
     const T& operator[] ( int idx ) const
     {
          assert( idx >= 0 && idx < m_size );
          return m_data[idx];
     }

           T* begin() const { return m_data; }
     const T* begin()       { return m_data; }
           T* end  ()       { return m_data + m_size; }
     const T* end  () const { return m_data + m_size; }

     T& front()
     {
          assert( m_size > 0 );
          return &m_data[0];
     }
     const T& front() const
     {
          assert( m_size > 0 );
          return &m_data[0];
     }

     T& back()
     {
          assert( m_size > 0 );
          return &m_data[m_size-1];
     }
     const T& back() const
     {
          assert( m_size > 0 );
          return &m_data[m_size-1];
     }

     void clear()
     {
         for ( T *p = m_data, *end = m_data+m_size; p != end; ++p )
              p->~T();

          m_size = 0; 
     }

     void resize( int new_size )
     {
          assert( new_size >= 0 );

          if ( new_size < m_size )
          {
               for (T *p = m_data + new_size, *end = m_data + m_size; p != end; ++p)
                    p->~T();

               m_size = new_size;
          }
          else
          if ( new_size > m_size )
          {
               reserve(new_size);

               for (T* p = m_data + m_size, *end = m_data + new_size; p != end; ++p)
                    new (p) T();

               m_size = new_size;
          }     
     }

     void reserve( int min_capacity )
     {
          if (m_capacity < min_capacity)
          {
               int new_capacity = expand_capacity(m_capacity);

               while (new_capacity < min_capacity)
                    new_capacity = expand_capacity(new_capacity);

               reallocate(new_capacity);
          }
     }

private:
     T*  m_data     = nullptr;
     int m_size     = 0;
     int m_capacity = 0;

     void reallocate( int capacity )
     {
          //{
          //     T* data = (T*)malloc(capacity * sizeof(T));

          //     assert (data != nullptr);

          //     for ( T *src = m_data, *dest = data, *end = m_data+m_size; src != end; ++src, ++dest)
          //     {
          //          *dest = *src;
          //          src->~T();
          //     }

          //     free(m_data);

          //     m_capacity = capacity;
          //     m_data     = data;
          //}

          T *data = (T*)realloc( (void*)m_data, capacity * sizeof(T));

          assert (data != nullptr);

          m_capacity = capacity;
          m_data     = data;
     }

     int expand_capacity( int capacity ) const
     {          
          //const int c = capacity < 32 ? capacity << 1 : static_cast<int>(capacity * 1.2f);
          //const int c = capacity < 32 ? capacity << 1 : static_cast<int>(capacity * 1.41f);

          const int c = static_cast<int>(capacity * 1.5f);

          //const int c = capacity << 1;

          return c > 0 ? c : MAXINT;
     }
};
