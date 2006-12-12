#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <ctime>

using namespace std;

#ifdef __WIN32__
#include <windows.h>
#endif // __WIN32__

#include <utime.h>


char* c_string (string& s);

string s_between (string& line, const string left, const string right);

string s_trim(const string& s);

vector<string> s_split (const string line, string *delim);

time_t getdate (string& s);

string printdate (time_t t);

void normalize_direntry (string& s);

void set_timestamp (string path, time_t date);

void change_directory (string path);

string current_directory ();

void CryptPWD (string* pwd);

#endif // UTILS_H


