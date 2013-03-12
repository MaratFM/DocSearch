#include <fstream>
#include <time.h>

#include "common.h"
#include "crc32.h"
#include "sphinxstem.h"


DWORD64 GetTimeMs64(){
	return clock();
}

BYTE* read_file(const char *file_path){
	std::ifstream file (file_path);
	if (file.is_open())
	{
		file.seekg(0, std::ios::end);
		long size = file.tellg();
		BYTE *file_data = new BYTE [size];
		file.seekg (0, std::ios::beg);
		file.read ((char *)file_data, size);
		file.close();	
		return file_data;
	}
	return NULL;
}
void close_file(BYTE * file_data){
    delete[] file_data;
}

DWORD get_word_id(BYTE* cstr){
	stem_en(cstr);
	return CRC32( cstr);
}


bool get_token(BYTE ** start, const BYTE * end, BYTE * buffer, const int max_len){
	static const char cp1251_charmap[] =
		"                " "                "
		"                " "0123456789      "
		" abcdefghijklmno" "pqrstuvwxyz     "
		" abcdefghijklmno" "pqrstuvwxyz     "
        "                " "                "
        "                " "                "
        "                " "                "
		"                " "                ";
	if (*start==end)
		return false;

	BYTE pos = 0;
	bool in_token = false;

	while(*start!=end){
		BYTE ch = cp1251_charmap[**start];
		(*start)++;
		if (ch==' '){
			if (in_token){
				if (pos >= MIN_TERM_LEN){
					buffer[pos] = '\0';
					return true;
				}else{
					in_token = false;
					pos=0;
				}
			}
		} else {
			in_token = true;
			if (pos >= MAX_TERM_LEN-1){				
				in_token = false;
				pos = 0;
			}else{
				buffer[pos] = ch;
				pos++;
			}
		}
	}
	buffer[pos] = '\0';
	return in_token;
}