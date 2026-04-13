#include "bmsearch.h"
#include<set>
BMsearch::BMsearch() {}

//哈利波特检索
//1.检索算法：BM查找算法
//核心思想：坏字符和好后缀
//BM算法的两个主要部分：
//坏字符规则：
//在模式匹配过程中，如果遇到不匹配的字符，使用该字符在模式中最后一次出现的位置来决定滑动的步长。
//好后缀规则：
//当部分匹配成功后，匹配失败时，根据已经匹配的部分进行滑动。

BMsearch::BMsearch(const string& pattern) :pattern(pattern)
{
    bulidBadCharacterTable();
}

//建立坏字符跳跃表
void BMsearch::bulidBadCharacterTable()
{
    int m = pattern.size();
    for (int i = 0; i < m; i++)
    {
        badCharacterTable[pattern[i]] = i;
    }
}

//搜索算法
vector<int> BMsearch::search(const string& text)
{
    vector<int> result;
    int n = text.size();
    int m = pattern.size();
    if (m == 0) return result;

    int shift = 0;
    while (shift <= n - m)
    {
        int j = m - 1;
        while (j >= 0 && pattern[j] == text[shift + j])
            j--;

        if (j < 0)
        {
            result.push_back(shift);
            //使用好后缀规则跳转
            shift += (shift + m < n) ? m - badCharacterTable[text[shift + m]] : 1;
        } else
        {
            //坏字符跳转
            int bcShift = j - badCharacterTable[text[shift + j]];
            shift += max(1, bcShift);
        }
    }
    return result;
}

