//---------------------------------------------------------------------------

#ifndef myexcH
#define myexcH

#include <cstddef>

//---------------------------------------------------------------------------

class Aut{
private:
	char *data;
	int *cs;

public:
	Aut(){ data = NULL; cs = NULL; };
	Aut(const Aut &val){ data = val.data; cs = val.cs; *cs += 1; };
	~Aut(){ Clear(); }
	void Clear();
	void SetS(char *s);
	const char* c_str(){ return data; };
	void operator =(const Aut &val);
	void operator =(char* val){ SetS(val); };
	};

class TMyEx	{
	private:
		int *ss;
		void *Data;
	public:
		int Code;
		char*Str;
		char*ClName;
		template <class T> T*Get()
			{ return (T*)Data; };
		TMyEx(TMyEx &val);
		TMyEx(int eCode, char* vStr, char* ClassName, void *vData = NULL, int Dsz = -1);
		__CLR_OR_THIS_CALL ~TMyEx();
	};

/*try
		{throw TEx(1301);}
		catch(TEx Ex)
		{if(Ex.fCode==1301)
		ShowMessage("Exception!");};  */

#include "MyExcept.cpp"
#endif
