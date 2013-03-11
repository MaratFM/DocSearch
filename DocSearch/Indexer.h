#include <vector>
#include <fstream>
#include <unordered_map>

#include "common.h"


struct DocInfo {
	WORD words_count;
	DWORD file_offset;
	DWORD length;
};


struct WordInDoc {
	DOCID doc_id;
	WORD count;
};

struct IndexEntry {
	std::vector<WordInDoc> in_docs;
	WORD total_count;
	WORD doc_count;

	IndexEntry();
	IndexEntry(DOCID doc_id,  WORD count );
	void serialize(const WORDID &word_id, std::ofstream &ofs);
};

class crc32Hash{
	public:
		size_t operator()(const DWORD &k) const{
			return k;
		}
};

class Index{
public:
	DocInfo *  add_doc(DOCID doc_id, DocInfo* doc_info);
	DocInfo *  add_doc(DOCID doc_id, DWORD offset, DWORD length, WORD words_count);
	void add_entry(DOCID doc_id, WORDID word_id, unsigned short count );

	DocInfo* get_doc(DOCID doc_id) {return doc_meta[doc_id]; }
	IndexEntry* get_entry(WORDID word_id) { return entries[word_id]; };
	DWORD get_total_docs() { return doc_meta.size(); }
	DWORD get_total_words() { return total_words; }

	void save_to_file(const char *file_path);
	void clear();

private:	
	DWORD total_words;
	std::unordered_map<WORDID, IndexEntry*, crc32Hash> entries;
	std::unordered_map<DOCID, DocInfo*> doc_meta; //TODO use vector because docid incrementally grows
};

class Indexer{
public:
	Indexer(Index *index, BYTE* file_data);
	void index_document(StatCallback callback);

private:
	Index *index;
	BYTE *file_data;
	SimpleCounter<WORDID> counter;
};
