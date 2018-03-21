#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <string>
#include <vector>
using namespace std;

namespace yam4{

    inline bool isAlpha(char c){
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    inline bool isNumber(char c){
        return (c >= '0' && c <= '9');
    }

    inline bool isWhiteSpace(char c){
        return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
    }

    inline bool isChar(char c, char c2){
        return c == c2;
    }

    inline bool isKeyChar(char c){
        return isChar(c, '#') || isChar(c, '{') || isChar(c, '}') || isChar(c, '(') || isChar(c, ')') || isChar(c, '\\');
    }

    class Tokenizer{
        public:
            Tokenizer(const string& str):idx(-1), in(str){}

        public:
            inline char getChar(int step = 1){
                idx += step;
                if(idx < 0 || idx >= in.length()){
                    return -1;
                }
                return in[idx];
            }

            inline char currChar(){
                if(idx < 0 || idx >= in.length()){
                    return -1;
                }
                return in[idx];
            }

            inline string skipWhiteSpace(){
                string w;
                char c = -1;
                while((c = getChar())!=-1 && isWhiteSpace(c)){
                    w += c;
                }
                getChar(-1);
                return w;
            }

            string getLeftString();
            bool guess(const string& s, bool forward = false);
            bool guess(const char* s, bool forward = false);
            bool readNext(const string& s);
            bool readNext(const char* s);
            bool readNext(const char c);

        public:
            string readNumber();
            string readString();
            string readWord();
            string readBlock(char beg='{', char end='}');
            void readParam(vector<string>& v);
            bool readSingleParamVal(vector<string>& j);
            void readParamVal(vector<string>& j);

        private:
            string in;
            int idx;

    };

}

#endif /* end of include guard: __TOKEN_H__ */
