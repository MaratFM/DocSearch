#include "Indexer.h"
#include "Searcher.h"

#include <vector>
#include <algorithm>
#include <iostream>


void parse_query(char * search_query, std::vector<WORDID> &words){
	BYTE buffer[1024];
	BYTE *end, *start = (BYTE*)search_query;
	for(end=start; *end; end++);

	while(get_token(&start, end, buffer, 1024)){
		words.push_back(get_word_id(buffer));		
	}	
}

	bool myfunction (std::pair<double, DocInfo*> i, std::pair<double, DocInfo*> j)	{
		return i.first > j.first;
	}

void search(Index *index, std::vector<WORDID> &words, BYTE *file_data){
	std::unordered_map<DOCID, MatchDoc> results;
	for(auto it = words.begin(); it != words.end(); ++it) {
		IndexEntry* entry = index->get_entry(*it);
		if (entry){
			for(auto i = entry->in_docs.begin(); i != entry->in_docs.end(); ++i) {
				MatchDoc match_doc = results[i->doc_id];
				if (match_doc.doc_info==NULL){
					match_doc.doc_info = index->get_doc(i->doc_id);
				}
				
				MatchWord match_word = {*it, i->count, entry->doc_count, entry->total_count};
				match_doc.match_words.push_back(match_word);
				results[i->doc_id] = match_doc;
			}
		}
	}
	std::vector<std::pair<double, DocInfo*>> ranked;
	ranked.reserve(results.size());

	DWORD N = index->get_total_docs();
	double avgD = double(index->get_total_words()) / N;
	for(auto dit = results.begin(); dit != results.end(); ++dit) {
		MatchDoc match_doc = dit->second;

		double bm25 = 0;
		for(auto wit = match_doc.match_words.begin(); wit != match_doc.match_words.end(); ++wit) {
			double IDF = log(double(N) / wit->doc_count);
			double freqD = double(wit->count) / match_doc.doc_info->words_count;
			bm25 += IDF * ( (freqD * (BM25_k1+1)) / (freqD+BM25_k1*(1-BM25_b+BM25_b*match_doc.doc_info->words_count/avgD) ) );
            bm25 += 1000; // add 1000 for each word, so total relevance will be bm25+1000*mathed_words
		
		}
		match_doc.bm25 = bm25;
		results[dit->first] = match_doc;
		ranked.push_back(std::pair<double, DocInfo*>(bm25, match_doc.doc_info));
	}

	std::sort(ranked.begin(), ranked.end(), myfunction);
	char buffer[MAX_DOC_LEN];
	int i = 0;
	for(auto rit = ranked.begin(); rit != ranked.end(); ++rit) {
		i++;
		if (i>25) break;
        if (rit->second->length >= MAX_DOC_LEN-1) {
            std::cout << rit->first << ":" << "***document too large***";
        }else{
            strncpy(buffer, (char *)file_data + rit->second->file_offset, rit->second->length);
            buffer[rit->second->length] = '\0';
            std::cout << rit->first << ":" << buffer;
        }
	}
}


