#pragma once

#ifndef string_ext_H
#define string_ext_H

#include <string>

#ifdef UNICODE
#define MultiStr	MultiStrW
#define String		std::wstring
#define to_str		std::to_wstring
#else
#define MultiStr  MultiStrA
#define String  std::string
#define to_str std::to_string
#endif // !UNICODE

std::wstring MultiStrW(int count, ...) {

	struct mstr {
		LPCWSTR  data; int size;
		mstr() { size = 0; data = nullptr; };
		mstr(LPCWSTR  data) {
			this->data = data;
			size = lstrlenW(data);
		};
	};
	mstr* sz = new mstr[count];

	va_list vl;
	va_start(vl, count);

	int size = 0;
	for (auto i = 0; i < count; i++) {
		sz[i] = va_arg(vl, LPCWSTR);
		size += sz[i].size;
	}

	std::wstring res;
	res.reserve(size);
	for (auto i = 0; i < count; i++)
		res.append(sz[i].data, sz[i].size);

	return res;
}

std::string MultiStrA(int count, ...) {

	struct mstr {
		LPCSTR  data; int size;
		mstr() { size = 0; data = nullptr; };
		mstr(LPCSTR  data) {
			this->data = data;
			size = lstrlenA(data);
		};
	};
	mstr *sz = new mstr[count];	
	
	va_list vl;
	va_start(vl, count);

	int size = 0;
	for (auto i = 0; i < count; i++) {
		sz[i] = va_arg(vl, LPCSTR);
		size += sz[i].size;
	}

	std::string res;
	res.reserve(size);
	for (auto i = 0; i < count; i++)
		res.append(sz[i].data, sz[i].size);

	return res;
}

String operator+(const String& str, const int ch) { return str + to_str(ch); }
String operator+(const int ch, const String& str) { return to_str(ch) + str; }

#endif // string_ext_H
