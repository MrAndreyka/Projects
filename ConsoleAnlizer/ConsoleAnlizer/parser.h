#pragma once
#include <string>
#include <string>

#include "DataClass.h"



struct token
{
	enum class Type	{ Operator, Number, Var, Other };
	const Type type;
	const std::string Val;
	token(Type type, const std::string Val) :type(type), Val(Val) {};
};

class parser
{
	using type_item = typename std::string;
	using u_char = typename unsigned char;

	struct ResultParse 
	{
		const u_char error;
		const size_t pos;
		ResultParse(const u_char error, const size_t pos) :error(error), pos(pos) {};
	};
public:

	parser() {};
	~parser();

	ResultParse parse(const std::string& str);
	const ForwardList<type_item>& Items() const noexcept { return items; };

	static constexpr auto npos{ static_cast<size_t>(-1) };
	static std::string ErrorToStr(u_char error);

private:
	ForwardList<type_item> items;

	bool AddItem(const char* str, size_t count);

	u_char load_expression(char*& str);
	size_t load_operator(char*& str);
	size_t load_word(char*& str);
	size_t load_number(char*& str);
	void skip_space(char*& str);
};
