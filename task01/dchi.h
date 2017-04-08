#include <stdlib.h>
#include <assert.h>
#include <type_traits>
#include <string.h>

template<typename Type, unsigned char staticSize = 5>
class myvector {
		
		Type * data;
		Type staticData[staticSize + 1];
		
		int length;
		int reserved;
		
	public:
		
		myvector() : data(staticData), reserved(staticSize + 1), length(0) {
			
		}
		//copy constructor
		~myvector() {
			if(data != staticData) delete data;
		}
		
		inline int capacity() const {
			return reserved;
		}
		
		inline int size() const {
			return length;
		}
		
		inline void add(const Type & value) {
			if(reserved <= (length + 1)) {
				realloc();
			}
			data[length++] = value;
		}
		
		inline Type & add() {
			if(reserved <= (length + 1)) {
				realloc();
			}
			return data[length++];
		}
		
		inline void erase(int index) {
			assert((unsigned int)index < (unsigned int)length && "myvector::erase(): bad index");
			copy(data + index, data + index + 1, --length - index);
		}
		
		inline void push_back(const Type & value) {
			if(reserved <= (length + 1)) {
				realloc();
			}
			data[length++] = value;
		}
		
		inline void erase(const Type * item) {
			erase(int(item - data));
		}
		
		inline Type & operator[](int index) {
			assert((unsigned int)index < (unsigned int)length && "myvector::operator[]: bad index");
			return data[index];
		}
		
		inline const Type & operator[](int index) const {
			assert((unsigned int)index < (unsigned int)length && "myvector::operator[] const: bad index");
			return data[index];
		}
		
		inline Type * begin() {
			return &data[0];
		}
		
		inline Type * end() {
			return &data[length];
		}
		
		inline void clear() {
			length = 0;
		}
		
		inline void destroy() {
			if(data != staticData) delete [] data;
			reserved = staticSize + 1;
			length = 0;
			data = staticData;
		}
		
		inline void resize(int size) {
			if(size == length) return;
			reserved = size + 1;
			Type * newData = new Type[reserved];
			copy(newData, data, size > length ? length : size);
			if(data != staticData) delete [] data;
			data = newData;
			length = size;
		}
		
		inline void reserve(int capacity) {
			if(capacity <= reserved) return;
			reserved = capacity + capacity / 2 + 1;
			Type * newData = new Type[reserved];
			copy(newData, data, length);
			if(data != staticData) delete [] data;
			data = newData;
		}
		
	private:
		
		inline void realloc() {
			reserved = length * 2;
			Type * newData = new Type[reserved];
			
			copy(newData, data, length);
			
			if(data != staticData) delete [] data;
			data = newData;
		}
		
		inline static void copy(Type * dst, const Type * src, int length) {
			if(length == 0) return;
			if(std::is_pod<Type>()) {
				memcpy(dst, src, length * sizeof(Type));
			} else {
				for(int i = 0; i < length; i++) {
					dst[i] = src[i];
				}
			}
		}
		
};
