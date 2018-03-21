#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

void explode(char ch, char* str, int len, vector<string>& str_list){
    str_list.clear(); 
    string key = ""; 
    key.reserve(1024);
    int i;
    for(i = 0; i < len; i++) 
    {   
        if(str[i] == '\n')
            break ;
        if(str[i] == ch) 
        {   
            str_list.push_back (key);
            key = ""; 
        }   
        else
        {   
            key.append(1, str[i]);
        }
    }

    str_list.push_back (key);
    return;
}

void explode(char ch, string& str, vector<string>& str_list){
    explode(ch, (char*)str.c_str(), str.length(), str_list);
}

void explode(char ch, const string& str, vector<string>& str_list){
    explode(ch, (char*)str.c_str(), str.length(), str_list);
}


string toString(const bool v)
{
    return v ? "1" : "0";
}

string toString(const int v)
{
    char buf[256];
    sprintf (buf , "%d" , v);
    return buf;
}

std::string toString(const long v)
{
    char buf[256];
    sprintf (buf , "%ld" , v);
    return buf;
}

string toString(const long long v)
{
    char buf[256];
    sprintf (buf , "%lld" , v);
    return buf;
}

string toString(const char* v)
{
    return v;
}

string toString(const std::string v)
{
    return v;
}

string toString(const float v)
{
    char buf[256];
    sprintf (buf , "%f" , v);
    return buf;
}

string toString(const double v)
{
    char buf[256];
    sprintf (buf , "%lF" , v);
    return buf;
}



string toLower(const string& str){
    string tmp;
    tmp.reserve(1024);
    for(int i=0; str[i]!= '\0'; i++) {
        tmp += tolower(str[i]);
    }
    return tmp;
}

string toUpper(const string& str){
    string tmp;
    tmp.reserve(1024);
    for(int i=0; str[i]!= '\0'; i++) {
        tmp += toupper(str[i]);
    }
    return tmp;
}

void str_replace(string& src, const string& match, const string& repl){
    while(true){
        size_t pos = src.find(match);
        if(pos == string::npos)
            break;

        src.replace(pos, match.length(), repl);
    }
}

string trim(string& s){
    int beg = -1;
    int end = s.length();

    char c = -1;
    while(((c = s[++beg])!=-1) && (c == '\n' || c == '\r' || c == '\t' || c == ' '));
    while(((c = s[--end]) != -1) && (c == '\n' || c == '\r' || c == '\t' || c == ' '));

    return s.substr(beg, end-beg+1);
}

string makeString(string& s){
    string out("\"");
    out.reserve(s.length());
    for(int i=0; i<s.length(); ++i){
        char c = s[i];
        if(c == '\\'){
            out += "\\\\";
        }
        else if(c == '\"'){
            out += "\\\"";
        }
        else{
            out += c;
        }
    }
    out += "\"";
    return out;
}

