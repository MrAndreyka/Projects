
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
ForwardList<T>::~ForwardList<T>() {
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

template <class T>
void ForwardList<T>::clear() {
	auto beg = begin();
	auto a = beg;
	while (!beg.isEnd()) {
		++beg;
		delete a.pos;
		a = beg;
	}
	_begin = _end = nullptr;
}

template <class T>
void ForwardList<T>::erase_from(const const_iterator& beg, const const_iterator& end)
{
	auto b = beg.next();
	beg.pos->next = nullptr;

	if (end == _end)
		_end = beg;

	const_iterator t;
	while (b != end) {
		t = b;
		b++;
		delete t;
	}
	beg.pos->next = b;
}

template <class T>
void ForwardList<T>::erase_after(const const_iterator& beg)
{
	auto b = beg.pos->next;
	beg.pos->next = b->next;
	if (b == _end)
		_end = beg.pos;
	delete b;
}

template <class T>
T ForwardList<T>::cut_after(const const_iterator& beg)
{
	auto b = beg.pos->next->at();
	erase_after(beg);
	return b;
}

template <class T>
void ForwardList<T>::erase_begin()
{
	auto b = _begin;
	_begin = _begin->next;
	delete b;
}

template <class T>
T ForwardList<T>::cut_begin()
{
	auto b = _begin->at();
	erase_begin();
	return b;
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