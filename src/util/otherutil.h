#ifndef OTHERUTIL_H
#define OTHERUTIL_H

#include <string>
#include <vector>
#include <ctime>

using namespace std;

#include <utime.h>

class OtherUtil {
public:
    static string s_between(string& line, const string left, const string right);
    static string s_trim(const string& s);
    static vector<string> s_split(const string line, string *delim);
    static int wildcmp(const char *wild, const char *str);
    static int stringSplit(const string& input, const string& delimiter, vector<string>& results,
                           bool includeEmpties = true);
    static string s_join(vector<string> strings, const string& delim);
    static string s_replace(const string& input, const char c, const char r);
//    static void stringVectorConcat(vector<string>& result, const vector<string>& vec1, const vector<string>& vec2);
    static string printdate(time_t t);
    static void normalize_direntry(string& s);
    static void set_timestamp(string path, time_t date);
};

#endif


