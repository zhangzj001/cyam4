#ifndef __UTILS_H__
#define __UTILS_H__

#include <vector>
#include <string>
using namespace std;

void explode(char ch, char* str, int len, vector<string>& str_list);
void explode(char ch, string& str, vector<string>& str_list);
void explode(char ch, const string& str, vector<string>& str_list);

string toString(const bool v); 
string toString(const char* v); 
string toString(const std::string v); 
string toString(const int v); 
string toString(const long v); 
string toString(const long long v); 
string toString(const float v); 
string toString(const double v);

string toLower(const string& str);
string toUpper(const string& str);

void str_replace(string& src, const string& match, const string& repl);

string trim(string &s);
string makeString(string& s);

#endif /* end of include guard: __UTILS_H__ */
