#ifndef _ARRAY_H_
#define _ARRAY_H_

#include <algorithm>

template<class T, std::size_t N = 16>
struct array
{
	// Fixed size array of elements of type T.
	T elements[N];

	// Iterator support.
	T* begin() { return elements; }
	const T* begin() const { return elements; }

	T* end() { return elements + N; }
	const T* end() const { return elements + N; }

	// Reverse iterator support.
	std::reverse_iterator<T*> rbegin() { return std::reverse_iterator<T*>(end()); }
	std::reverse_iterator<const T*> rbegin() const { return std::reverse_iterator<const T*>(end()); }
	std::reverse_iterator<const T*> crbegin() const { return std::reverse_iterator<const T*>(end()); }

	std::reverse_iterator<T*> rend() { return std::reverse_iterator<T*>(begin()); }
	std::reverse_iterator<const T*> rend() const { return std::reverse_iterator<const T*>(begin()); }
	std::reverse_iterator<const T*> crend() const { return std::reverse_iterator<const T*>(begin()); }

	T& operator[] (std::size_t i) { return elements[i]; }
	const T& operator[] (std::size_t i) const { return elements[i]; }

	// At performs a range check.
	T& at(std::size_t i)
	{
		if (i >= size())
			std::out_of_range("array<>: index out of range");
		return elements[i];
	}

	const T& at(std::size_t i) const
	{
		if (i >= size())
			std::out_of_range("array<>: index out of range");
		return elements[i];
	}

	T& front() { return elements[0]; }
	const T& front() const { return elements[0]; }

	T& back() { return elements[N - 1]; }
	const T& back() const { return elements[N - 1]; }

	// Size is constant.
	static std::size_t size() { return N; }
	static std::size_t max_size() { return N; }

	static bool empty() { return false; }

	void swap(array<T, N>& a)
	{
		for (std::size_t i = 0; i < N; ++i)
			std::swap(elements[i], a.elements[i]);
	}

	// Assignment with type conversion.
	template <typename T2>
	array<T, N>& operator= (const array<T2, N>& rhs)
	{
		std::copy(rhs.begin(), rhs.end(), begin());
		return *this;
	}

	// Assign one value to all elements.
	void fill(const T& value) { std::fill_n(begin(), size(), value); }
};

// Comparisons.
template<class T, std::size_t N>
bool operator== (const array<T, N>& x, const array<T, N>& y)
{
	return std::equal(x.begin(), x.end(), y.begin());
}

template<class T, std::size_t N>
bool operator!= (const array<T, N>& x, const array<T, N>& y) { return !(x == y); }

template<class T, std::size_t N>
bool operator< (const array<T, N>& x, const array<T, N>& y)
{
	return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template<class T, std::size_t N>
bool operator> (const array<T, N>& x, const array<T, N>& y) { return y < x; }

template<class T, std::size_t N>
bool operator<= (const array<T, N>& x, const array<T, N>& y) { return !(y < x); }

template<class T, std::size_t N>
bool operator>= (const array<T, N>& x, const array<T, N>& y) { return !(x < y); }

#endif