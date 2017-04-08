#include <stdlib.h>
#include <assert.h>

static const int staticBufferSize = 32;

template<typename T> class myvector
{
private:
  int _capacity, _size;
  T * data;
  T buffer[staticBufferSize];
  void expandCapacity(int newCapacity) {
    _capacity = newCapacity;
    T * oldData = (data == NULL) ? buffer : data;
    data = new T[newCapacity];
    for (int i=0; i<_size; i++) {
      data[i] = oldData[i];
    }
//    if (oldData != buffer) {
//      free(oldData);
//    }
  }
  void checkSize() {
    if (_size == _capacity ) {
      assert(_capacity != INT_MAX);
      double k = 1.0 - 1.0/(double)(INT_MAX);
      int d = (double)_capacity*k;
      int newCapacity;
      if (d > INT_MAX - _capacity) {
        newCapacity = INT_MAX;
      } else {
        newCapacity = _capacity + d;
      }
      expandCapacity(newCapacity);
    }
  }
public:
  myvector() {
    data = NULL;
    _capacity = staticBufferSize;
    _size = 0;
  }
  ~myvector() {
//    if (data != NULL) {
//      free(data);
//    }
    data = NULL;
  }
  
  int capacity() { return _capacity; }
  int size() { return _size; }
  
  void add(const T & value) {
    checkSize();
    T * curData = (data == NULL) ? buffer : data;
    curData[_size++] = value;
  }
  T & add() {
    checkSize();
    T * curData = (data == NULL) ? buffer : data;
    curData[_size] = T();
    return curData[_size++];
  }
  void erase(int index) {
    if (index >=0 && index < _size) {
      T * curData = (data == NULL) ? buffer : data;
      for (int i=index+1; i<_size; i++) {
        curData[i-1] = curData[i];
      }
      _size--;
    }
  }
  
  void push_back(const T & value) { add(value); }
  void erase(const T * item) {
    for (int i=0; i<_size; i++) {
      T * curData = (data == NULL) ? buffer : data;
      if (*item == curData[i]) {
        erase(i);
        return;
      }
    }
  }
  
  T & operator[](int index) {
    assert(index >=0 && index < _size);
    T * curData = (data == NULL) ? buffer : data;
    return curData[index];
  }
  const T & operator[](int index) const {
    assert(index >=0 && index < _size);
    const T * curData = (data == NULL) ? buffer : data;
    return curData[index];
  }
  
  T * begin() { return (data == NULL) ? buffer : data;; }
  T * end() { return &((data == NULL) ? buffer[_size-1] : data[_size-1]); }
  
  void clear() {
    data = NULL;
    _size = 0;
    _capacity = staticBufferSize;
  }
  void resize(int new_size) {
    if (new_size > _capacity) {
      expandCapacity(new_size);
    } else if (new_size < _size){
      _size = new_size;
    }
  }
  void reserve(int min_capacity) {
    if (min_capacity > _capacity) {
      expandCapacity(min_capacity);
    }
  }
  
};
