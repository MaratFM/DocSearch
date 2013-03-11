
#include <algorithm>
#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <unordered_map>
#include <forward_list>
#include <fstream>
#include <sstream>

#include "Indexer.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////
IndexEntry::IndexEntry(){
	in_docs.reserve(1024);
	total_count = 0;
	doc_count = 0;
}

IndexEntry::IndexEntry(DOCID doc_id,  WORD count ){
	in_docs.reserve(1024);
	WordInDoc in_doc = {doc_id, count};
	in_docs.push_back(in_doc);
	total_count = count;
	doc_count = 1;
}

void IndexEntry::serialize(const WORDID &word_id, ofstream &ofs){
	if (ofs.good()){
		ofs.write(reinterpret_cast<const char*>(&word_id), sizeof(word_id));
		ofs.write(reinterpret_cast<const char*>(&doc_count), sizeof(doc_count));
		ofs.write(reinterpret_cast<const char*>(&total_count), sizeof(total_count));
		for(auto it = in_docs.begin(); it != in_docs.end(); it++) {
			WordInDoc& doc = *it;
			ofs.write(reinterpret_cast<const char*>(&doc.doc_id), sizeof(doc.doc_id));
			ofs.write(reinterpret_cast<const char*>(&doc.count), sizeof(doc.count));
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////
DocInfo *  Index::add_doc(DOCID doc_id, DocInfo* doc_info){
	doc_meta[doc_id] = doc_info;
	total_words += doc_info->words_count;
    return doc_info;
}

DocInfo * Index::add_doc(DOCID doc_id, DWORD offset, DWORD length, WORD words_count){
	DocInfo *doc_info = new DocInfo();
	doc_info->file_offset = offset;
	doc_info->length = length;
	doc_info->words_count = words_count;
	add_doc(doc_id, doc_info);
    return doc_info;
}

void Index::add_entry(DOCID doc_id, WORDID word_id, unsigned short count ){
	IndexEntry*& entry = entries[word_id];
	if (entry == NULL){
		entry = new IndexEntry(doc_id, count);
	}else{
		entry->total_count += count;
		entry->doc_count++;
		if (entry->doc_count <= 1000){	
			WordInDoc in_doc = {doc_id, count};
			entry->in_docs.push_back(in_doc);
		}else{
			entry->in_docs.clear();
		}
	}
}

void Index::save_to_file(const char *file_path){
	ofstream bin_file(file_path);
	for(auto it = entries.begin(); it != entries.end(); it++) 
		it->second->serialize(it->first, bin_file);
		
	bin_file.close();


	ofstream index_file("/Users/marat/TMP/index.txt");

	for(auto it = entries.begin(); it != entries.end(); it++) {
		auto index_entry = it->second;
		index_file << it->first << " " << index_entry->total_count << " ";
		for(auto i = index_entry->in_docs.begin(); i != index_entry->in_docs.end(); i++) {
			index_file << (*i).doc_id << " " << (*i).count << " ";
		}
		index_file << "\n";
	}
	index_file.close();

}

void Index::clear(){
	entries.clear();
	doc_meta.clear();
	total_words = 0;
}
//////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////
Indexer::Indexer(Index *i, BYTE* f){
	index = i;
	file_data = f;
}

void Indexer::index_document(StatCallback callback){
	//index->clear();

	DOCID doc_id = 0;
	BYTE *end, *start = file_data;
	while(*start){
		for(end=start; *end && *end!='\n'; end++);
		doc_id++;	
         DocInfo *doc_info = new DocInfo();
         doc_info->file_offset = start-file_data;
         doc_info->length = end-start+1;
         doc_info->words_count = 0;
			
		callback(doc_id);
		counter.flush();
	
		WORD words_in_doc = 0;
		BYTE buffer[1024];
		while(get_token(&start, end, buffer, 1024)){
			words_in_doc++;
			counter.push(get_word_id(buffer));
		}
         doc_info->words_count = words_in_doc;
         index->add_doc(doc_id, doc_info);


		WORDID word;
		WORD count;
		while(counter.pop(word, count)){
			index->add_entry(doc_id, word, count);
		}

		start = ++end;
	}		
}


//////////////////////////////////////////////////////////////////////////////////////////

