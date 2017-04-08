#define _CRT_SECURE_NO_WARNINGS 1

//#include "stdafx.h"

#include <stdio.h>
#include <vector>
#include <time.h>
#include <cassert>
#include <random>


typedef          long long  llong;
typedef unsigned long long ullong;


//#pragma endregion asd  


template<typename T>
class myvector
{

private:

    /// <summary>       †“ó“Ÿ¿ ﬂ¿‹ô»›‹»≥“ (“∆≥»£ ‹òŸ»‘¿’¿ ùŸ»ÿ»‹ô“)  </summary>
    T*	    m_Data;
    /// <summary>       ∑»ﬂòï“ö »ÿﬂ¿£ôø ﬂ¿‹ô»›‹»≥“ (¿”û»ÿ ‘ñ∆»Ÿ»‹‹¿› —“ÿöô€ ‘ ≥“ﬁÿ»≥“õ ¿∆‹¿’¿ ùŸ»ÿ»‹ô“)  </summary>
    int		m_Capacity;
    /// <summary>       ∑»ﬂòï»» ﬂ¿Ÿ€ó»£ô‘¿ ùŸ»ÿ»‹ô¿‘ ‘ ﬂ¿‹ô»›‹»≥» (¿”û»ÿ ﬁ“∆»›£ô‘¿‘“‹‹¿› ‘ñ∆»Ÿ»‹‹¿› —“ÿöô€ ‘ ≥“ﬁÿ»≥“õ ¿∆‹¿’¿ ùŸ»ÿ»‹ô“)  </summary>
    int		m_Size;
    /// <summary>       ª¿ùìì€ü€»‹ô —¿Ÿ€ô€ﬂ€ ≥¿£ô“  </summary>
    int		m_Policy;

    /// <summary>       †“ó“Ÿø‹“ö »ÿﬂ¿£ôø ﬂ¿‹ô»›‹»≥“  </summary>
    static const int    INITIAL_CAPACITY    = 16;


public:

    myvector()
    {
        m_Capacity      = INITIAL_CAPACITY;
        m_Data          = (T*)malloc( (INITIAL_CAPACITY * sizeof(T)) );
        m_Size          = 0;
        m_Policy        = 1.2;
    }
    myvector( float policy )
    {
        m_Capacity      = INITIAL_CAPACITY;
        m_Data          = (T*)malloc( (INITIAL_CAPACITY * sizeof(T)) );
        m_Size          = 0;
        m_Policy        = policy;
    }
    ~myvector()
    {
        if( !__is_pod(T) )
            for( int i = 0;  i < m_Size;  i++ )
                m_Data[i].~T();

        free( m_Data );
    }

    /// <summary> π¿Ÿòó€ôø ô»ﬂòïòî »ÿﬂ¿£ôø ﬂ¿‹ô»›‹»≥“ </summary>
    /// <returns>          ∑»ﬂòï“ö »ÿﬂ¿£ôø ﬂ¿‹ô»›‹»≥“ </returns>
    int capacity() { return m_Capacity; }
    /// <summary> π¿Ÿòó€ôø ô»ﬂòï»» ﬂ¿Ÿ€ó»£ô‘¿ ùŸ»ÿ»‹ô¿‘ ‘ ﬂ¿‹ô»›‹»≥» </summary>
    /// <returns>          ∑»ﬂòï»» ﬂ¿Ÿ€ó»£ô‘¿ ùŸ»ÿ»‹ô¿‘ ‘ ﬂ¿‹ô»›‹»≥» </returns>
    int size() { return m_Size; }

    // because we can

    /// <summary> Ä¿”“‘€ôø ùŸ»ÿ»‹ô ‘ ﬂ¿‹»ü ﬂ¿‹ô»›‹»≥“ </summary>
    /// <param name="value">    ùŸ»ÿ»‹ô </param>
    void add( const T& value )
    {
        if( m_Size >= m_Capacity )
            reserve( (m_Capacity * m_Policy) );

        memcpy( (m_Data + m_Size), &value, sizeof(T) );
        m_Size++;
    }
    /// <summary> Ä¿”“‘€ôø ùŸ»ÿ»‹ô ‘ ﬂ¿‹»ü ﬂ¿‹ô»›‹»≥“ </summary>
    /// <param name="value">    ùŸ»ÿ»‹ô </param>
    void push_back( const T& value ) { add( value ); }

    // easy handmade emplace_back()

    /// <summary> Ä¿”“‘€ôø ùŸ»ÿ»‹ô ‘ ﬂ¿‹»ü ﬂ¿‹ô»›‹»≥“ </summary>
    /// <returns>       ££ñŸﬂ“ ‹“ ∆¿”“‘Ÿ»‹‹ñ› ùŸ»ÿ»‹ô </returns>
    T& add()
    {
        if( m_Size >= m_Capacity )
            reserve( (m_Capacity * m_Policy) + 1 );

        int pos     = m_Size++;

        memset( (m_Data + pos), 0x00, sizeof(T) );

        return *(m_Data + pos);
    }

    // easy handmade erase()

    /// <summary> ™∆“Ÿ€ôø ùŸ»ÿ»‹ôñ ﬂ¿‹ô»›‹»≥“, ‹“ó€‹“ö £ òﬂ“ﬁ“‹‹¿’¿ </summary>
    /// <param name="index">    €‹∆»ﬂ£ ùŸ»ÿ»‹ô“ </param>
    void erase( int index )
    {
        // !!! ‘¿ô ¿‹“, ≥“∆¿£ôø-ô¿! (‹ò €Ÿ€ —¿Ÿ¿‘€‹“)
        assert( index >= 0  &&  index < m_Size  &&  "Index out of range" );

        if( !__is_pod(T) )
            m_Data[index].~T();

        for( int i = index;  i < (m_Size - 1);  i++ )
            memcpy( (m_Data + i), (m_Data + i + 1), sizeof(T) );

        m_Size--;
    }

    /// <summary> ™∆“Ÿ€ôø ùŸ»ÿ»‹ôñ ﬂ¿‹ô»›‹»≥“, ‹“ó€‹“ö £ òﬂ“ﬁ“‹‹¿’¿ </summary>
    /// <param name="item">     ùŸ»ÿ»‹ô </param>
    void erase( const T* item ) { erase( item - m_Data ); }


    /// <summary> π¿Ÿòó€ôø ùŸ»ÿ»‹ô ﬂ¿‹ô»›‹»≥“ </summary>
    /// <param name="index">    €‹∆»ﬂ£ ùŸ»ÿ»‹ô“ ﬂ¿‹ô»›‹»≥“ </param>
    /// <returns>               ££ñŸﬂ“ ‹“ ùŸ»ÿ»‹ô ﬂ¿‹ô»›‹»≥“ </returns>
    T& operator[]( int index )
    {
        // !!! ‘¿ô ¿‹“, ≥“∆¿£ôø-ô¿! (‹ò €Ÿ€ —¿Ÿ¿‘€‹“)
        assert( index >= 0  &&  index < m_Size  &&  "Index out of range" );

        T retVal;

        memcpy( &retVal, (m_Data + index), sizeof(T) );

        return retVal;
    }
    /// <summary> π¿Ÿòó€ôø ùŸ»ÿ»‹ô ﬂ¿‹ô»›‹»≥“ </summary>
    /// <param name="index">    €‹∆»ﬂ£ ùŸ»ÿ»‹ô“ ﬂ¿‹ô»›‹»≥“ </param>
    /// <returns>               ﬂ¿‹£ô“‹ô‹“ö ££ñŸﬂ“ ‹“ ùŸ»ÿ»‹ô ﬂ¿‹ô»›‹»≥“ </returns>
    const T& operator[]( int index ) const
    {
        // !!! ‘¿ô ¿‹“, ≥“∆¿£ôø-ô¿! (‹ò €Ÿ€ —¿Ÿ¿‘€‹“)
        assert( index >= 0  &&  index < m_Size  &&  "Index out of range" );

        T retVal;

        memcpy( &retVal, (m_Data + index), sizeof(T) );

        return retVal;
    }

    /// <summary> π¿Ÿòó€ôø òﬂ“ﬁ“ô»Ÿø ‹“ ‹“ó“Ÿ¿ ﬂ¿‹ô»›‹»≥“ </summary>
    /// <returns>       “∆≥»£ ‹òŸ»‘¿’¿ ùŸ»ÿ»‹ô“ ﬂ¿‹ô»›‹»≥“ </returns>
    T* begin() { return m_Data; }
    /// <summary> π¿Ÿòó€ôø òﬂ“ﬁ“ô»Ÿø ‹“ ﬂ¿‹»ü ﬂ¿‹ô»›‹»≥“ </summary>
    /// <returns>       “∆≥»£ —¿£Ÿ»∆‹»’¿ ùŸ»ÿ»‹ô“ ﬂ¿‹ô»›‹»≥“ </returns>
    T* end() { return m_Data + m_Size; }

    /// <summary> æó€£ô€ôø ﬂ¿‹ô»›‹»≥, ¿£‘¿”¿∆€ôø —“ÿöôø (»£Ÿ€ ‹» POD) </summary>
    void clear()
    {
        if( !__is_pod(T) )
            for( int i = m_Size - 1;  i >= 0;  i-- )
                m_Data[i].~T();

        m_Capacity  = INITIAL_CAPACITY;
        m_Data      = (T*)realloc( m_Data, (INITIAL_CAPACITY * sizeof(T)) );
        m_Size      = 0;
    }

    /// <summary> ™£ô“‹¿‘€ôø ﬂ¿Ÿ€ó»£ô‘¿ ùŸ»ÿ»‹ô¿‘ ﬂ¿‹ô»›‹»≥“ </summary>
    /// <param name="index">    ﬂ¿Ÿ€ó»£ô‘¿ ùŸ»ÿ»‹ô¿‘ ﬂ¿‹ô»›‹»≥“ </param>
    void resize( int new_size )
    {
        // !!! ‘¿ô ¿‹“, ≥“∆¿£ôø-ô¿! (‹ò €Ÿ€ —¿Ÿ¿‘€‹“)
        assert( new_size >= 0  &&  "Index out of range" );

        if( new_size < m_Capacity )
        {
            if( !__is_pod(T) )
                for( int i = new_size;  i < m_Capacity;  i++ )
                    m_Data[i].~T();
        }
        else
            reserve( new_size );

        m_Size      = m_Size < new_size?  m_Size:  new_size;
    }
        
    /// <summary> æ”»£—»ó€ôø »ÿﬂ¿£ôø ﬂ¿‹ô»›‹»≥“ </summary>
    /// <param name="index">    »ÿﬂ¿£ôø ﬂ¿‹ô»›‹»≥“ </param>
    void reserve( int min_capacity )
    {
        if( min_capacity <= m_Capacity )
            return;


        m_Data          = (T*)realloc( m_Data, (min_capacity * sizeof(T)) );

        m_Capacity      = min_capacity;
    }
};
