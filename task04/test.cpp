#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <vector>
#include <algorithm>

using namespace std;

///////////////////////////////////////////////////////////////////////////

typedef long long llong;
typedef unsigned int uint;
typedef unsigned char byte;

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#pragma comment(linker, "/defaultlib:psapi.lib")
#pragma message("Automatically linking with psapi.lib")
#endif

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

void die(const char * fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	printf("FATAL: ");
	vprintf(fmt, ap);
	printf("\n");
	va_end(ap);
	exit(1);
}

void load(vector<int> &p, const char *fname)
{
	FILE *fp = fopen(fname, "rb");
	if (!fp)
		die("failed to open %s", fname);

	fseek(fp, 0, SEEK_END);
	int len = ftell(fp); // FIXME? will break on 2gb file
	fseek(fp, 0, SEEK_SET);

	int n = len / 4; // number of ints
	len = n * 4; // rounded length
	p.resize(n);

	if (fread(&p[0], 1, len, fp) != len)
		die("failed to read %s", fname);

	fclose(fp);
}


uint fnv1a(const byte *data, int len)
{
	uint hval = 2166136261UL;
	for (int i = 0; i < len; i++)
	{
		uint c = *data++;
		hval ^= c;
		hval *= 0x1000193;
    }
    return hval;
}


template<typename T>
uint fnv1a(const vector<T> &v)
{
	return fnv1a((const byte*)&v[0], (int)(v.size() * sizeof(T)));
}

void encode(vector<byte> &enc, const vector<int> &raw)
{
	enc.resize(raw.size() * 4);
	memcpy(&enc[0], &raw[0], enc.size());
}


void decode(vector<int> &dec, const vector<byte> &enc)
{
	dec.resize(enc.size() / 4);
	memcpy(&dec[0], &enc[0], enc.size());
}


template<typename T>
int bytes(const vector<T> &v)
{
	return (int)(sizeof(T) * v.size());
}

struct posting
{
	int docid;
	int pos;

	bool operator<(const posting &b) const
	{
		return docid < b.docid || (docid == b.docid && pos < b.pos);
	}
};

struct compare_id
{
	bool operator()(const posting &a, int b) const
	{
		return a.docid < b;
	}
};

void copy_matches(vector<int> &res, posting *r, posting *e, int id)
{
	if (!r || r >= e)
		return;
	while (r < e && r->docid == id)
	{
		res.push_back(r->docid);
		res.push_back(r->pos);
		r++;
	}
}

void lookup(vector<int> &res, const vector<byte> &enc1, const vector<byte> &enc2, int id)
{
	posting *p1 = (posting*)&enc1[0];
	posting *p2 = (posting*)&enc2[0];
	posting *e1 = p1 + enc1.size() / sizeof(posting);
	posting *e2 = p1 + enc2.size() / sizeof(posting);
	
	int from = (int)res.size();
	copy_matches(res, lower_bound(p1, e1, id, compare_id()), e1, id);
	copy_matches(res, lower_bound(p2, e2, id, compare_id()), e2, id);
	if (from == res.size())
		return;

	sort((posting*)&res[from], (posting*)(&res[0] + res.size()));
}

int main(int, char **)
{
	vector<int> raw1, raw2, bench;
	load(raw1, "the.bin");
	load(raw2, "i.bin");
	load(bench, "bench.bin");

	vector<byte> enc1, enc2;
	llong tm1 = microtimer();
	encode(enc1, raw1);
	encode(enc2, raw2);
	tm1 = (microtimer() - tm1) / 1000;

	vector<int> dec1, dec2;
	llong tm2 = microtimer();
	decode(dec1, enc1);
	decode(dec2, enc2);
	tm2 = (microtimer() - tm2) / 1000;

	llong tm3 = microtimer();
	vector<int> lookups;
	for (int id : bench)
		lookup(lookups, enc1, enc2, id);
	tm3 = (microtimer() - tm3) / 1000;

	printf("size, %d\n", bytes(enc1) + bytes(enc2));
	printf("ratio, %.03f\n", double(bytes(enc1) + bytes(enc2)) / (bytes(raw1) + bytes(raw2)));
	printf("encode, %d ms\n", tm1);
	printf("decode, %d ms\n", tm2);
	printf("randread, %d ms\n", tm3);

	FILE *fp = fopen("randread.res", "wb+");
	fwrite(&lookups[0], 1, bytes(lookups), fp);
	fclose(fp);

	printf("raw %08x, %08x\n", fnv1a(raw1), fnv1a(raw2));
	printf("enc %08x, %08x\n", fnv1a(enc1), fnv1a(enc2));
	printf("dec %08x, %08x\n", fnv1a(dec1), fnv1a(dec2));
	printf("lookup %08x\n", fnv1a(lookups));
}
