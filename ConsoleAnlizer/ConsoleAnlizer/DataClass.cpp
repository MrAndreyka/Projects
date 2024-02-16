
#ifndef DataClass
#define DataClass

#include "DataClass.h"

template <class T>
void ForwardList<T>::push_after(const T& val, ForwardList<T>::Cell<T>* elem) {
	auto t = new Cell<T>(val);
	t->next = elem->next;
	elem->next = t;
}

template <class T>
ForwardList<T>::~ForwardList() {
	if (_begin == nullptr) return;
	for (Cell<T>* p, *i = p = _begin; i != nullptr; p = i)
	{
		i = p->next;
		delete p;
	}
}

template <class T>
void ForwardList<T>::push_begin(const T& val) {
	auto t = new Cell<T>(val);
	t->next = _begin;
	_begin = t;
	if (!_end)
		_end = _begin;
}

template <class T>
ForwardList<T>& ForwardList<T>::operator <<(const T& val) {
	push_back(val);
	return *this;
}

template <class T>
ForwardList<T>& ForwardList<T>::operator >>(const T& val) {
	push_begin(val);
	return *this;
}


/*template <class T>
ForwardList<T>& ForwardList<T>::operator =(ForwardList<T>&& val)
{
	this->~ForwardList();
	if (this == &val)
		return *this;
	_begin = val._begin; 
	_end = val._end; 
	val._begin = val._end = nullptr; 
	return *this; 
}

template <class T>
ForwardList<T>& ForwardList<T>::operator =(const ForwardList<T>& val)
{
	this->~ForwardList();
	//if (this != &val)
	//	for (T &a : val) push_begin(a);
	return *this;
}*/

#endif // DataClass