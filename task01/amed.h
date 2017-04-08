#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef int size_type;

template<typename T> class myvector {
public:
  static void deleteRange(T* begin, T* end) {
    while (begin != end) {
      begin->~T();
      begin++;
    }
  }

private:
  size_type _size;
  size_type _capacity;
  T* _data;

public:
  myvector();
  ~myvector();

  size_type capacity();
  size_type size();

  void add(const T&);
  T& add();
  void erase(size_type);

  void push_back(const T&);
  void erase(const T*);

  T& operator[](size_type);
  const T& operator[](size_type) const;

  T* begin();
  T* end();
  T& back();

  void clear();
  void resize(size_type);
  void reserve(size_type);
};

template<typename T>
myvector<T>::myvector() {
  _capacity = 8;
  _size = 0;
  _data = static_cast<T*>(malloc(sizeof(T) * _capacity));
}

template<typename T>
myvector<T>::~myvector() {
  deleteRange(_data, _data + _size);
  free(_data);
}

template<typename T>
size_type myvector<T>::capacity() {
  return _capacity;
}

template<typename T>
size_type myvector<T>::size() {
  return _size;
}

template<typename T>
void myvector<T>::add(const T& value) {
  push_back(value);
}

template<typename T>
T& myvector<T>::add() {
  T* item = new T();
  push_back(*item);
  delete item;
  return back();
}

template<typename T>
void myvector<T>::erase(size_type index) {
  assert(index >= 0 && index < _size);
  if (_size == 1) {
    clear();
  } else {
    for (size_type i = index; i < _size - 1; i++) {
      memcpy(_data + i, _data + i + 1, sizeof(T));
    }

    _size--;
  }
}

template<typename T>
void myvector<T>::push_back(const T& value) {
  T* item = new T(value);

  if (_size == _capacity) {
    // Resize policy
    size_type newCapacity = _capacity < 512 ? _capacity * 2 : _capacity * 1.2;

    _data = static_cast<T*>(realloc(_data, sizeof(T) * newCapacity));
    _capacity = newCapacity;
  }

  memcpy(_data + _size, item, sizeof(T));
  delete item;
  _size++;
}

template<typename T>
void myvector<T>::erase(const T* item) {
  size_type index = item - _data;
  assert(index >= 0 && index < _size);
  erase(index);
}

template<typename T>
T& myvector<T>::operator[](size_type index) {
  assert(index >= 0 && index < _size);
  return _data[index];
}

template<typename T>
const T& myvector<T>::operator[](size_type index) const {
  assert(index >= 0 && index < _size);
  return _data[index];
}

template<typename T>
T* myvector<T>::begin() {
  return _data;
}

template<typename T>
T* myvector<T>::end() {
  return _data + _size;
}

template<typename T>
T& myvector<T>::back() {
  return *(_data + _size - 1);
}

template<typename T>
void myvector<T>::clear() {
  deleteRange(_data, _data + _size);
  _size = 0;
  _capacity = 8;
  _data = static_cast<T*>(realloc(_data, sizeof(T) * _capacity));
}

template<typename T>
void myvector<T>::resize(size_type new_size) {
  if (new_size != 0) {
    if ((new_size > _capacity) || (new_size < _capacity/2)) {
      _capacity = new_size;
      _data = static_cast<T*>(realloc(_data, sizeof(T) * new_size));
    }
  } else {
    clear();
  }

  _size = new_size;
}

template<typename T>
void myvector<T>::reserve(size_type min_capacity) {
  if (min_capacity > _capacity) {
    _capacity = min_capacity < 512 ? min_capacity * 2 : min_capacity * 1.2;
    _data = static_cast<T*>(realloc(_data, sizeof(T) * _capacity));
  }
}
