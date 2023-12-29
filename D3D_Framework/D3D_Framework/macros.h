#pragma once

#define to_null(p) (p, p = nullptr)

#ifdef _DEBUG
#define _debug(x)	(x)
#else
#define _debug(x)
#endif

#define Not_null(x, y)	if(x != nullptr) x##y
#define returnError(mes)	{Log::Get()->Err(mes); return false;}

#define _DELETE(p)		{ if(p)delete to_null(p); }
#define _DELETE_ARRAY(p)	{ if(p)delete[] to_null(p); }
#define _RELEASE(p)		{ if(p){(p)->Release(); (p)=nullptr;} }
#define _CLOSE(p)		{ if(p){(p)->Close(); delete to_null(p);} }



// Текущая версия фреймворка
#define _VERSION_FRAMEWORK 3