#include "parser.h"
#include "DataClass.cpp"

parser::~parser(){}

std::string parser::ErrorToStr(u_char error)
{
	std::string res;

	switch (error)
	{
	case 1: res = "Ожидается выражение";
		break;
	case 2: res = "Ожидается ')'";
		break;
	case 3: res = "Ожидается значение";
		break;
	case 4: res = "Ожидается оператор";
		break;
	default:
		res = "Неизвестная ошибка";
		break;
	}
	return res;
}

bool parser::AddItem(const char* str, size_t count)
{
	if (count == 0) return false;
	items.push_back(std::string(str- count, count));
	return true;
}

parser::ResultParse parser::parse(const std::string& str) {
	auto st = const_cast<char*>(str.c_str());
	auto r = load_expression(st);
	return ResultParse(r, st - str.c_str());
}

parser::u_char parser::load_expression(char*& st)
{
	skip_space(st);
	size_t res = 0;
	auto bg = st;

	while (*st != 0 && *st != ')') {

		if (*st == '(')
		{
			AddItem(++st, 1);
			res = load_expression(st);
			if(res!=0) 
				return res;

			if(*st != ')')
				return 2;
			AddItem(++st, 1);
		}
		else {
			res = load_word(st);
			if (!AddItem(st, res))
				return 3;
		}

		res = load_operator(st);
		if (!AddItem(st, res)) {
			if (*st == 0 || *st == ')')
				break;
			else 
				return 4;
		}

		skip_space(st);
	}

	return bg==st?1:0;
}

size_t parser::load_operator(char*& st)
{
	skip_space(st);

	if (*st == '+' || *st == '-' ||
		*st == '*' || *st == '/' ||
		*st == '^' || *st == '%')
		return st++, 1;

	return 0;
}

size_t parser::load_word(char*& st)
{
	skip_space(st);
	if (*st >= 48 && *st <= 57)
		return load_number(st);
		
	auto bg = st;

	//48-57 65-90 97-122
	while (*st >= 65 && *st <= 90 ||
		*st >= 97 && *st <= 122)
		st++;

	return st - bg;
}

size_t parser::load_number(char*& st)
{
	auto bg = st;
	u_char tck(0), e(0);

	//48-57 65-90 97-122
	while (*st >= 48 && *st <= 57 || *st == '.' || *st == '-' || *st == '+' || *st == 'E' || *st == 'e')
	{ 
		if (*st == '.')
			if (!tck) tck = 1;
			else break;
		else if (*st == '-' ||*st == '+'){
			if (bg != st && st[-1] != 'e' && st[-1] != 'E') break;
		}
		else if (*st > 57)
			if (e) break;
			else e = 1;
		st++;
	}

	return st - bg;
}

void parser::skip_space(char*& str)
{
	while (*str == ' ' || *str == '\t' || *str == '\n')
		str++;

}
