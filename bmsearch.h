#ifndef BMSEARCH_H
#define BMSEARCH_H
#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

class BMsearch
{
public:
    BMsearch();
    BMsearch(const string& pattern);
    void bulidBadCharacterTable();
    vector<int> search(const string& text);

private:
    string pattern;
    unordered_map<char, int> badCharacterTable;
};

#endif // BMSEARCH_H
