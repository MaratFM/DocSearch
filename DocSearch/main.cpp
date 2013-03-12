#include "Indexer.h"
#include "Searcher.h"
#include "sphinxstem.h"

#include <iostream>
#include <vector>

DWORD64 t1, t2;

void PrintStat(DOCID doc_id){
	if (doc_id % 10000==0) {
		std::cout << doc_id << ": ";
		t2 = GetTimeMs64();
		std::cout << (t2 - t1) / 1000 << "ms\n";
		t1 = t2;
	}
}

int main(int argc, const char * argv[])
{
	setlocale(LC_ALL, "Russian");
	stem_en_init();
	
	BYTE *file_data = read_file("/Users/marat/TMP/wp.txt");
	
	Index index;
	Indexer indexer(&index, file_data);
    
	DWORD64 tstart = t1 = GetTimeMs64();
	indexer.index_document(&PrintStat);
	t2 = GetTimeMs64();
	std::cout << "total: " << (t2 - tstart)/1000 << "ms\nsave\n";
    
	index.save_to_file("/Users/marat/TMP/index.bin");
    
	std::vector<WORDID> words;
	parse_query("light fault", words);
	search(&index, words, file_data);
    
    close_file(file_data);
    return 0;
}

