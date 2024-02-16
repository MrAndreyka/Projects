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
	class iterator {
		friend ForwardList;
	private:
		Cell<T>* pos;
		iterator(Cell<T>* _pos) : pos(_pos) {}
	public:
		iterator() {}
		iterator(const iterator& val) :pos(val.pos) {}

		bool operator!=(iterator const& other) const noexcept { return pos != other.pos; };
		bool operator==(iterator const& other) const noexcept { return pos == other.pos; };
		To& operator*() const noexcept { return pos->at(); };
		To& operator->() const noexcept { return pos->at(); };
		iterator& operator++() { pos = pos->next; return *this; };
		bool isEnd() const noexcept { return pos == nullptr; }
	};

	ForwardList() { _begin = _end = nullptr; }
	ForwardList(const T& val) { _begin = _end = new Cell<T>(val); }
	ForwardList(const ForwardList& val) { _begin = _end = nullptr; *this = val; }
	ForwardList(ForwardList&& val) {_begin = _end = nullptr; *this = val; }

	~ForwardList();

	iterator<T, T> begin() const noexcept { return _begin; }
	iterator<T, T> end() const noexcept { return nullptr; }
	iterator<T, const T> cbegin() const noexcept { return _begin; }
	iterator<T, const T> cend() const noexcept { return nullptr; }
	iterator<T, T> before_end() const noexcept { return _end; }
	iterator<T, const T> cbefore_end() const noexcept { return _end; }

	void push_back(const T& val) { _end = (_end == nullptr) ? _begin = new Cell<T>(val) : (_end->next = new Cell<T>(val)); }
	void push_begin(const T& val);

	void push_after(const T& val, const iterator<T, T>& elem) { push_after(val, elem.pos); }

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


