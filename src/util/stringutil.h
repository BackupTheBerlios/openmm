#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include <string>
#include <vector>
#include <ctime>
using namespace std;


class StringUtil {
public:
    static string s_trim(const string& s);
    static int    s_wildcmp(const char *wild, const char *str, bool ignoreNonAscii = false);
    static int    s_split(const string& input, const string& delimiter, vector<string>& results,
                          bool includeEmpties = true);
    static string s_join(vector<string> strings, const string& delim);
    static string s_replace(const string& input, const char c, const char r);
    static string s_time(string format, time_t t);
//    static void stringVectorConcat(vector<string>& result, const vector<string>& vec1, const vector<string>& vec2);
};

#endif
