template<typename T>
class myvector {
    typedef long long llong;

public:
    myvector() :
        _size(0),
        _capacity(8),
        _array(new T[_capacity])
    {
    }

    ~myvector() {
        _size = 0;
        _capacity = 0;
        delete[] _array;
    }

    int capacity() {
        return _capacity;
    }

    int size() {
        return _size;
    }

    void add(const T & value) {
        _add(value);
    }

    T & add() {
        return _add(T());
    }

    void erase(int index) {
        _array[index].~T();
        if (index < _size) {
            for (llong i = index; i < _size-2; ++i) {
                _array[i] = _array[i+1];
            }
        }
        --_size;
    }

    void push_back(const T & value) {
        add(value);
    }

    void erase(const T * item) {
        erase(item - _array);
    }

    T & operator[](int index) {
        return _array[index];
    }

    const T & operator[](int index) const {
        return static_cast<const T&>(_array[index]);
    }

    T * begin() {
        return _array;
    }

    T * end() {
        return _array + _size - 1;
    }

    void clear() {
        for (llong i = _size-1; i >= 0; --i) {
            _array[i].~T();
        }
        _size = 0;
    }

    void resize(int new_size) {
        llong tmp = _size;
        if (new_size < _size) {
            for (llong i = _size-1; i >= new_size-1; --i) {
                _array[i].~T();
            }
            _size = new_size;
        }
        _reserve( _min_capacity(new_size) );
        if (new_size > tmp) {
            for (llong i = tmp; i < new_size; ++i) {
                _array[i] = T();
            }
        }
        _size = new_size;
    }

    void reserve(int min_capacity) {
        _reserve(_min_capacity(min_capacity));
    }

private:
    llong _size;
    llong _capacity;
    T* _array;

    T & _add(const T & value) {
        if (_size >= _capacity) {
            _reserve(2*_capacity);
        }
        return _array[_size++] = value;
    }

    llong _min_capacity(llong value) {
        llong result;
        for (result = 8; result < value; result *= 2);
        return result;
    }

    void _reserve(llong new_capacity) {
        if (_capacity == new_capacity) {
            return;
        }
        T* _new_array = new T[new_capacity];
        for (llong i = 0; i < _size; ++i) {
            _new_array[i] = _array[i];
        }
        delete[] _array;
        _array = _new_array;
        _capacity = new_capacity;
    }
};
