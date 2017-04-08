#include <stdio.h>

#include <string>
using namespace std;

#define S1(arg) #arg
#define S2(arg) S1(arg)
#pragma message("compiling " S2(WHO) ".h")

///////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

typedef long long llong;
typedef unsigned long long ullong;

llong microtimer()
{
#ifdef _MSC_VER
	// Windows time query
	static llong iBase = 0;
	static llong iStart = 0;
	static llong iFreq = 0;

	LARGE_INTEGER iLarge;
	if (!iBase)
	{
		// get start QPC value
		QueryPerformanceFrequency(&iLarge); iFreq = iLarge.QuadPart;
		QueryPerformanceCounter(&iLarge); iStart = iLarge.QuadPart;

		// get start UTC timestamp
		// assuming it's still approximately the same moment as iStart, give or take a msec or three
		FILETIME ft;
		GetSystemTimeAsFileTime(&ft);

		iBase = (llong(ft.dwHighDateTime)<<32) + llong(ft.dwLowDateTime);
		iBase = (iBase - 116444736000000000ULL) / 10; // rebase from 01 Jan 1601 to 01 Jan 1970, and rescale to 1 usec from 100 ns
	}

	// we can't easily drag iBase into parens because iBase*iFreq/1000000 overflows 64bit int!
	QueryPerformanceCounter(&iLarge);
	return iBase + (iLarge.QuadPart - iStart) * 1000000 / iFreq;

#else
	// UNIX time query
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return llong(tv.tv_sec) * llong(1000000) + llong(tv.tv_usec);
#endif
}

///////////////////////////////////////////////////////////////////////////

struct Test1
{
	int a;
	int b;
	Test1() : a(123), b(456) {}
	Test1(int aa, int bb) : a(aa), b(bb) {}
};

struct Test2
{
private:
	int gen; // gen == generation

public:
	Test2() : gen(0) {}
	Test2(const Test2 & rhs) : gen(rhs.gen + 1000000) {}
	Test2 & operator=(const Test2 & rhs) { gen = rhs.gen + 1; return *this; } // curiously, NOT required for std::vector
	int get() { return gen; }
};

int g_ctor3 = 0;
int g_dtor3 = 0;

struct Test3
{
	string s;
	int x;
	float f;

	Test3() { g_ctor3++; }
	~Test3() { g_dtor3++; }
};

int RUNS = 10*1000*1000;

void main()
{
	llong tm;
	volatile int r = 0;
	int testno = 0;

	// some warmup
	for (int i = 0; i < 100*1000*1000; i++)
		r += i * i;

	printf("name, " S2(WHO) "\n");

	////////////////////
	// compilation test
	////////////////////

	{
		myvector<int> t;
		r += t.capacity();
		r += t.size();
#ifndef _TESTING_STD
		t.add(1);
		t.add(2);
		t.add() = 3;
		t.add() = 4;
		t.erase(1);
#else
		t.emplace_back(1);
		t.emplace_back(2);
		t.push_back(3);
		t.push_back(4);
		t.erase(t.begin() + 1);
#endif
		t.push_back(5);
		t.erase(t.begin() + 1);
		r += t[0];
		const myvector<int> & rt = t;
		r += rt[1];
		r += (t.begin() != t.end());
		t.clear();
		t.resize(12);
		t.reserve(34);
	}

	///////////////////
	// functional test
	///////////////////

	{
		bool ok;

		myvector<Test1> t1;
		ok = true;
		for (int i = 0; i < 10000; i++)
			t1.push_back(Test1(i, i + 100));
		for (int i = 0; i < 10000 && ok; i++)
			if (t1[i].a != i || t1[i].b != i + 100)
				ok = false;
		printf("t01, %s\n", ok ? "ok" : "FAIL");

		myvector<Test2> t2;
		for (int i = 0; i <= 10000; i++)
			t2.push_back(Test2());
		printf("t02a, %d\n", t2[0].get());
		printf("t02b, %d\n", t2[0].get());

		myvector<Test3> t3;
		printf("t03a, %d / %d\n", g_ctor3, g_dtor3);
		t3.reserve(32);
		printf("t03b, %d / %d\n", g_ctor3, g_dtor3);
		t3.resize(16);
		printf("t03c, %d / %d\n", g_ctor3, g_dtor3);
		t3.clear();
		printf("t03d, %d / %d\n", g_ctor3, g_dtor3);
	}

	/////////////
	// benchmark
	/////////////

#define BEGIN(_name) printf("b%02d %s, ", ++testno, _name); r = 0; tm = microtimer();
#define TIMES(_n) for (int i = 0; i < _n; i++)
#define LOOP TIMES(RUNS)
#define END printf("%d\n", int(microtimer() - tm) / 1000);

	BEGIN("ctor/dtor")
	LOOP
	{
		myvector<int> t;
		r += t.size();
	}
	END

	BEGIN("ctor/push_back/dtor")
	LOOP
	{
		myvector<int> t;
		t.push_back(1);
		r += t.size();
	}
	END

	BEGIN("size")
	{
		myvector<int> t;
		LOOP r += t.size();
	}
	END

	BEGIN("push_back")
	{
		myvector<int> t;
		LOOP t.push_back(i);
		r += t.size();
	}
	END

	BEGIN("add")
	{
		myvector<int> t;
#ifdef _TESTING_STD
		LOOP t.emplace_back(i);
#else
		LOOP t.add() = i;
#endif
		r += t.size();
	}
	END

	{
		myvector<int> t;
		TIMES(RUNS) t.push_back(i);

		BEGIN("operator[]")
		LOOP r += t[i];
		END

		const myvector<int> & rt = t;
		BEGIN("const operator[]")
		LOOP r += rt[i];
		END

		BEGIN("ptr erase(3)")
		TIMES(100)
		{
			t.erase(t.begin() + 3);
			r += t.size();
		}
		END

		BEGIN("idx erase(3)")
		TIMES(100)
		{
#ifdef _TESTING_STD
			t.erase(t.begin() + 3);
#else
			t.erase(3);
#endif
			r += t.size();
		}
		END

		BEGIN("ptr erase(mid)")
		TIMES(100)
		{
			t.erase(t.begin() + t.size() / 2);
			r += t.size();
		}
		END

		BEGIN("idx erase(mid)")
		TIMES(100)
		{
#ifdef _TESTING_STD
			t.erase(t.begin() + t.size() / 2);
#else
			t.erase(t.size() / 2);
#endif
			r += t.size();
		}
		END

		BEGIN("ptr erase(end)")
		TIMES(RUNS / 2)
		{
			t.erase(t.end() - 1);
			r += t.size();
		}
		END

		BEGIN("idx erase(end)")
		TIMES(RUNS / 2)
		{
#ifdef _TESTING_STD
			t.erase(t.end() - 1);
#else
			t.erase(t.size() - 1);
#endif
			r += t.size();
		}
		END
	}

	BEGIN("push_back(2int)")
	{
		myvector<Test1> t;
		LOOP t.push_back(Test1(1,1));
		r += t.size();
	}
	END

	BEGIN("push_back(string)")
	{
		myvector<string> t;
		LOOP t.push_back("abc");
		r += t.size();
	}
	END
}
