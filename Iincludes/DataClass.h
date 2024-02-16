#pragma once
template <class T>
class ForwardList {
private:
	template <class T>
	class Cell {
		friend ForwardList;
	private:
		T value;
		Cell<T>* next = nullptr;
	public:
		Cell() {};
		Cell(const T& val) :value(val) {};
		T& operator*() const noexcept { return value; };
		T& at() { return value; }
	};

	void push_after(const T& val, Cell<T>* elem);
	Cell<T>* _begin, * _end;
public:
	template <class T, class To>
	class iteratorFL {
		friend ForwardList;
	private:
		Cell<T>* pos;
		iteratorFL(Cell<T>* _pos) : pos(_pos) {}
	public:
		iteratorFL() {}
		iteratorFL(const iteratorFL& val) :pos(val.pos) {}

		bool operator!=(iteratorFL const& other) const noexcept { return pos != other.pos; };
		bool operator==(iteratorFL const& other) const noexcept { return pos == other.pos; };
		To& operator*() const noexcept { return pos != nullptr ? pos->at(): T(NULL); };
		To& operator->() const noexcept { return pos->at(); };
		iteratorFL& operator++() { pos = pos->next; return *this; };
		iteratorFL next() { return pos->next; }
		bool isEnd() const noexcept { return pos == nullptr; }
		To& at() { return pos->at(); }

		size_t operator-(const iteratorFL& b) const {
			auto c = b;
			size_t res(0);
			while (c != *this)
				res++, ++c;
			return res;
		}
	};

	template <class T, class To>
	class iteratorFLP :public iteratorFL<T, To> {
		friend ForwardList;
	};

	typedef iteratorFL<T, const T> const_iterator;
	typedef iteratorFL<T, T> iterator;

	ForwardList() { _begin = _end = nullptr; }
	ForwardList(const T& val) { _begin = _end = new Cell<T>(val); }
	ForwardList(const ForwardList& val) { _begin = _end = nullptr; *this = val; }
	ForwardList(ForwardList&& val) {_begin = _end = nullptr; *this = val; }

	~ForwardList();

	iterator begin() const noexcept { return _begin; }
	iterator end() const noexcept { return nullptr; }
	const_iterator cbegin() const noexcept { return _begin; }
	const_iterator cend() const noexcept { return nullptr; }
	iterator before_end() const noexcept { return _end; }
	const_iterator cbefore_end() const noexcept { return _end; }

	void push_back(const T& val) { _end = (_end == nullptr) ? _begin = new Cell<T>(val) : (_end->next = new Cell<T>(val)); }
	void push_begin(const T& val);
	void push_after(const T& val, const iterator& elem) { push_after(val, elem.pos); }
	
	void clear();

	T& back() const { return before_end().at(); }
	T& front() const { return begin().at(); }
	
	//Удаляем все элементы между beg, и end
	void erase_from(const const_iterator& beg, const const_iterator& end);
	//Удаляем элемент после beg
	void erase_after(const const_iterator& beg);
	T cut_after(const const_iterator& beg);
	void erase_begin();
	T cut_begin();

	//рассчитывает размер списка
	size_t get_size() const { return end() - begin(); }

	ForwardList<T>& operator <<(const T& val);
	ForwardList<T>& operator >>(const T& val);

	//ForwardList<T>& operator =(ForwardList<T>&& val);
	//ForwardList<T>& operator =(const ForwardList<T>& val);

	ForwardList<T>& operator =(const ForwardList<T>& val)
	{
		if (this == &val) *this;
		this->~ForwardList();
		auto b = val.cbegin();
		while (b != val.cend())
		{
			push_back(*b);
			++b;
		}
		//for (T &a : val) push_begin(a);
		return *this;
	};

	ForwardList<T>& operator =(ForwardList<T>&& val)
	{
		if (this == &val)
			return *this;
		this->~ForwardList();
		_begin = val._begin;
		_end = val._end;
		val._begin = val._end = nullptr;
		return *this;
	};

};


