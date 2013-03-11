#include <string>

#ifndef _common_
#define _common_

typedef unsigned long long DWORD64;
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;

//#define DEBUG 0
#define BM25_k1 2.0
#define BM25_b 0.75

typedef DWORD WORDID;
typedef DWORD DOCID;
typedef void (*StatCallback)(DOCID doc_id);


#define MIN_TERM_LEN 3
#define MAX_TERM_LEN 1024

bool get_token(BYTE ** start, const BYTE * end, BYTE * buffer, const int max_len);
DWORD get_word_id(BYTE * s);
DWORD64 GetTimeMs64();
BYTE* read_file(const char *file_path);

template<typename T> class SimpleCounter{
public:
	SimpleCounter(){
		flush();
	}
	void flush(){
		size = 0;
	}
	void push(T value){
		for(int i=0; i<size; ++i){
			if (buffer[i].value == value){
				buffer[i].count++;
				return;
			}
		}
		Pair p = {value, 1};
		buffer[size] = p;
		size++;
	}
	bool pop(T &value, unsigned short &count){
		if (!size) return false;
		size--;
		Pair p = buffer[size];
		value = p.value;
		count = p.count;
		
		return true;
	}
	
private:
	struct Pair {
		T value;
		unsigned short count;
	};
	Pair buffer[1024];
	unsigned short size;
};

#endif // _common_