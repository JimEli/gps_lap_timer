// Simple pair class.
// 9.26.2019 JME

#ifndef _PAIR_H_
#define _PAIR_H_

template <class T1, class T2>
struct pair
{
	T1 first;
	T2 second;

	pair() { }
	pair(const T1& a, const T2& b) : first(a), second(b) { }
};

template <class T1, class T2>
inline bool operator== (const pair<T1, T2>& x, const pair<T1, T2>& y) { return x.first == y.first && x.second == y.second; }

template <class T1, class T2>
inline bool operator< (const pair<T1, T2>& x, const pair<T1, T2>& y) { return x.first < y.first || (!(y.first < x.first) && x.second < y.second); }

template <class T1, class T2>
inline pair<T1, T2> make_pair(const T1& x, const T2& y) { return pair<T1, T2>(x, y); }

#endif
