#include "common.h"

#include <vector>

struct MatchWord{
	WORDID word;
	WORD count;
	WORD doc_count;
	WORD total_count;
};

struct MatchDoc{
	std::vector<MatchWord> match_words;
	DocInfo* doc_info;
	double bm25;

	MatchDoc(){
		doc_info = NULL;
	}
};

void parse_query(char *search_query, std::vector<WORDID> &words);
void search(Index *index, std::vector<WORDID> &words, BYTE *file_data);


