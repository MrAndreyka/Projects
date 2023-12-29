#pragma once

#ifndef MyRectPoint_H
#define MyRectPoint_H

#include <windows.h>

struct MyPOINT :POINT {
	MyPOINT(const LONG _x, const LONG _y) { SET(_x, _y); }
	explicit MyPOINT(const POINT& val) { SET(val); }

	void SET(const LONG _x, const LONG _y) { x = _x; y = _y; }
	void SET(const POINT& val) { x = val.x; y = val.y; }

	MyPOINT operator- (const POINT& val) { return MyPOINT(x - val.x, y - val.y); }
	MyPOINT& operator-= (const POINT& val) { x -= val.x; y -= val.y; return *this; }
	MyPOINT& operator/= (const LONG val) { x /= val; y /= val; return *this; }
	MyPOINT& operator=(const POINT& val) { SET(val); return *this; }
};

#define MyPOINT_(val) static_cast<MyPOINT&>(val)
POINT& operator+= (POINT& val1, POINT& val2) { val1.x += val2.x; val1.y += val2.y; return val1; }
POINT operator- (POINT& val1, POINT& val2) { return MyPOINT(val1.x - val2.x, val1.y - val2.y); }

struct MyRECT :RECT {
	MyRECT(const LONG l, const LONG t, const LONG r, const LONG b) { SET(l, t, r, b); }
	explicit MyRECT(const RECT& val) { SET(val); }

	void Calc(const LONG left, const LONG top, const LONG width, const LONG height) { SET(left, top, left + width, top + height); }
	void SET(const LONG l, const LONG t, const LONG r, const LONG b) { SetRect(this, l, t, r, b); }
	void SET(const RECT& val) { CopyMemory(this, &val, sizeof(RECT)); }

	LONG GetWidth() { return right - left; }
	LONG GetHeight() { return bottom - top; }

	MyRECT& SetWidthFromCenter(LONG width) {
		left += (GetWidth() - width) / 2;
		right = left + width;
		return *this;
	}
	MyRECT& SetHeightFromCenter(LONG height) {
		top = (GetHeight() - height) / 2;
		bottom = top + height;
		return *this;
	}

	MyRECT& ShiftH(const int val) { left += val; right += val; return *this; };
	MyRECT& ShiftV(const int val) { top += val; bottom += val; return *this; };

	MyRECT Copy() { return MyRECT(*this); }
	MyRECT Copy(LONG dleft, LONG dtop, LONG dright, LONG dbottom) { return MyRECT(left + dleft, top + dtop, right + dright, bottom + dbottom); }

	MyRECT& operator=(const RECT& val) { SET(val); return *this; }
};

#define MyRECT_(val) static_cast<MyRECT&>(val)

#endif
