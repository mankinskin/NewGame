#pragma once
#include<vector>
#include <functional>

template<typename T>
class managed_vector {

public:
	managed_vector();
	managed_vector(std::size_t size, std::size_t max_capacity = size);
	T& operator[](size_t index) {
		return data[index];
	}
	
	void resize(size_t size);
	void reserve(size_t size);
	void push_back(T& el);
	size_t size() { return _size; }
	void force_insert(T* pPos, T* pBlock, size_t pCount);
	T& last();
	T& first();
	T** first_ptr();
	T** last_ptr();
	T** index_ptr(size_t index);
	void revalidate();
	void clear();
	bool is_valid() {
		return valid;
	}
private:
	std::vector<T> data;
	std::vector<T*> access;
	size_t _size;
	size_t _capacity;
	int valid;
};

template<typename T>
managed_vector<T>::managed_vector()
{
}

template<typename T>
managed_vector<T>::managed_vector(std::size_t size, std::size_t max_capacity)
	: _size(size), _capacity(max_capacity), data(std::vector<T>(size)), access(std::vector<T*>(max_capacity))
{
	revalidate();
}

template<typename T>
void managed_vector<T>::resize(size_t size)
{
	_size = size;
	data.resize(_size);
	access.resize(_size);
	revalidate();
}

template<typename T>
void managed_vector<T>::reserve(size_t capacity)
{
	_capacity = capacity;
	data.reserve(_capacity);
	access.reserve(_capacity);
}

template<typename T>
void managed_vector<T>::push_back(T & el)
{
	_size += 1;
	data.push_back(el);
	access.resize(_size);
	valid = 0;
}

template<typename T>
inline void managed_vector<T>::force_insert(T * pPos, T * pBlock, size_t pCount)
{
	_size += pCount;
	unsigned int posIt = pPos - &data[0];
	access.resize(_size);
	data.resize(_size);
	T* posPtr = &data[0] + posIt;
	std::memcpy(posPtr + pCount, posPtr, sizeof(T)*pCount);
	std::memcpy(posPtr, pBlock, sizeof(T)*pCount);
	
	revalidate();
}

template<typename T>
inline T & managed_vector<T>::last()
{
	return data[_size-1];
}

template<typename T>
inline T & managed_vector<T>::first()
{
	return data[0];
}

template<typename T>
T** managed_vector<T>::last_ptr()
{
	return &access[_size-1];
}

template<typename T>
inline T ** managed_vector<T>::index_ptr(size_t index)
{
	return &access[index];
}

template<typename T>
T** managed_vector<T>::first_ptr()
{
	return &access[0];
}

template<typename T>
void managed_vector<T>::revalidate()
{
	for (unsigned int i = 0; i < _size; ++i) {
		access[i] = &data[i];
	}
	valid = 1;
}

template<typename T>
inline void managed_vector<T>::clear()
{
	_size = 0;
	_capacity = 0;
	data.clear();
	access.clear();
}

