#include "stringutil.h"
#include "debug.h"

#include <sys/time.h>
#include <ctype.h>

/*
    Some utility functions for strings.
    Partially inspired by some code-snippets from the net (public domain, no special licences identifiable).
    */

/*
    s_trim ()
    strip spaces, newlines and carriage returns from both ends:
    */

string
StringUtil::s_trim(const string& s)
{
    if (s.length() == 0) {
        return s;
    }
    int b = s.find_first_not_of(" \t\n\r");
    int e = s.find_last_not_of(" \t\n\r");	
    if (b == -1) { // No non-spaces
        return "";
    }
    return string(s, b, e - b + 1);
}


/*
    If ignoreNonAscii is true, all non ascii characters are ignored until the first
    occurence of a wildcard (this should be enough for our application, but is not
    general enough).
*/

int
StringUtil::s_wildcmp(const char *wild, const char *str, bool ignoreNonAscii) {
//     TRACE("StringUtil::wildcmp(), pattern: %s, str: %s", wild, str);
    const char *cp = NULL, *mp = NULL;

    while ((*str) && (*wild != '*')) {
        if (ignoreNonAscii && !isascii(*wild)) {
//         if (ignoreNonAscii && !isalnum(*wild)) {
            wild++;
            continue;
        }
        if (ignoreNonAscii && !isascii(*str)) {
//         if (ignoreNonAscii && !isalnum(*str)) {
            str++;
            continue;
        }
        if ((*wild != *str) && (*wild != '?')) {
            return 0;
        }
        wild++;
        str++;
    }

    while (*str) {
        if (*wild == '*') {
            if (!*++wild) {
                return 1;
            }
            mp = wild;
            cp = str+1;
        } else if ((*wild == *str) || (*wild == '?')) {
            wild++;
            str++;
        } else {
            wild = mp;
            str = cp++;
        }
    }

    while (*wild == '*') {
        wild++;
    }
    return !*wild;
}

//   stringSplit() can also be implemented using the <cstring> function strtok():
//   see: http://www.cplusplus.com/ref/cstring/strtok.html
//        http://www.codeguru.com/columns/DotNetTips/article.php/c9279/
//   char str[] ="This is a sample string,just testing.";
//   char * pch;
//   printf ("Splitting string \"%s\" in tokens:\n",str);
//   pch = strtok (str," ");
//   while (pch != NULL)
//   {
//     printf ("%s\n",pch);
//     pch = strtok (NULL, " ,.");
//   }

int
StringUtil::s_split(const string& input, const string& delimiter, vector<string>& results,
                    bool includeEmpties)
{
    int iPos = 0;
    int newPos = -1;
    int sizeS2 = (int)delimiter.size();
    int isize = (int)input.size();

    if(( isize == 0 ) || ( sizeS2 == 0 )) {
        return 0;
    }

    vector<int> positions;
    newPos = input.find (delimiter, 0);
    if (newPos < 0) { 
        return 0; 
    }

    int numFound = 0;
    while (newPos >= iPos) {
        numFound++;
        positions.push_back (newPos);
        iPos = newPos;
        newPos = input.find (delimiter, iPos+sizeS2);
    }

    if (numFound == 0) {
        return 0;
    }

    for (int i=0; i <= (int)positions.size(); ++i) {
        string s("");
        if (i == 0) { 
            s = input.substr (i, positions[i]); 
        }
        int offset = positions[i-1] + sizeS2;
        if (offset < isize) {
            if (i == positions.size()) {
                s = input.substr(offset);
            }
            else if (i > 0) {
                s = input.substr( positions[i-1] + sizeS2, 
                      positions[i] - positions[i-1] - sizeS2 );
            }
        }
        if (includeEmpties || ( s.size() > 0 )) {
            results.push_back(s);
        }
    }
    return numFound;
}


string
StringUtil::s_join(vector<string> strings, const string& delim)
{
    string res;
    for (vector<string>::iterator i = strings.begin(); i != strings.end(); i++) {
        res.append((*i) + delim);
    }
    return res.substr(0, res.length() - delim.length());
}


string
StringUtil::s_replace(const string& input, const char c, const char r)
{
    string res(input);
    int i = -1;
    while ((i = input.find(c, i + 1)) != string::npos) {
        res[i] = r;
    }
    return res;
}


string
StringUtil::s_time(string format, time_t t)
{
    tm *brokenDownTime;
    const int bufSize = 128;
    char res[bufSize];

    brokenDownTime = localtime(&t);
    strftime(res, bufSize, format.c_str(), brokenDownTime);
    return res;
}


// void
// StringUtil::stringVectorConcat(vector<string>& result, const vector<string>& vec1, const vector<string>& vec2)
// {
//     result = vector<string>(vec1);
//     for(vector<string>::iterator i = vec2.begin(); i != vec2.end(); i++) {
//         result.push_back(*i);
//     }
// }
