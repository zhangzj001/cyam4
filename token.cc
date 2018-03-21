#include "token.h"
#include <list>
#include <stdio.h>

namespace yam4{

    string Tokenizer::getLeftString(){
        return in.substr(idx);
    }

    bool Tokenizer::guess(const string& s, bool forward){
        skipWhiteSpace();

        size_t len = s.length();
        string cmp;
        for(int i=0; i<len; ++i){
            char c = getChar();
            if(c != -1)
                cmp += c;
        }

        bool r = (s == cmp);

        //如果比较失败，或者不希望向前移动，回滚回去
        if(!r || !forward){
            getChar(-len);
        }

        return r;
    }

    bool Tokenizer::guess(const char* s, bool forward){
        string str(s);
        return guess(str, forward);
    }

    bool Tokenizer::readNext(const string& s){
        return guess(s, true);
    }

    bool Tokenizer::readNext(const char* s){
        string str(s);
        return readNext(str);
    }

    bool Tokenizer::readNext(const char c){
        string s;
        s += c;
        return readNext(s);
    }

    string Tokenizer::readNumber(){
        skipWhiteSpace();

        string s;
        char c = -1;
        while((c = getChar())!=-1 && isNumber(c)){
            s += c;
        }
        getChar(-1);

        return s;
    }

    string Tokenizer::readString(){
        skipWhiteSpace();

        string s;
        //一般情况下deli="|’，除非是有目的性的调用，选用了其它字符作为分隔
        char deli = getChar();
        if(!isChar(deli, '"') && !isChar(deli, '\'')){
            getChar(-1);
            return s;
        }

        s += deli;
        char c = -1;
        while((c=getChar())!=-1 && !isChar(c, deli)){
            s += c;
        }
        s += deli;

        return s;
    }

    string Tokenizer::readWord(){
        skipWhiteSpace();

        string s;
        char c = getChar();
        //必须以字母或者下划线开始
        if(isAlpha(c) || isChar(c, '_')){
            do{
                s += c;
            }while((c = getChar())!=-1 && (isAlpha(c) || isNumber(c) || isChar(c, '_')));
        }
        getChar(-1);

        return s;
    }

    string Tokenizer::readBlock(char beg, char end){
        skipWhiteSpace();

        string s;
        int count = 0;
        char c = getChar();
        if(isChar(c, beg)){
            count++;
            while((c = getChar())!=-1){
                if(isChar(c, beg)) count++;
                if(isChar(c, end)) count--;
                if(count == 0) break;
                s += c;
            }
        }else{
            getChar(-1);
        }

        return s;
    }

    void Tokenizer::readParam(vector<string>& v){
        skipWhiteSpace();

        if(readNext("(")){
            while(!readNext(")")){
                string p = readWord();
                v.push_back(p);
                readNext(",");
            }
        }
    }

    bool Tokenizer::readSingleParamVal(vector<string>& v){
        skipWhiteSpace();
        char c = -1;
        while((c = getChar())!=-1 && !isChar(c, ')') && !isChar(c, ',')){
            //字符串
            if(isChar(c, '\'' || isChar(c, '"'))){
                getChar(-1);
                string s = readString();
                v.push_back(s);
            }
            //block
            else if(isChar(c, '{')){
                getChar(-1);
                string s = readBlock();
                v.push_back(s);
            }
            //其它字面量
            else{
                getChar(-1);
                string s;
                while((c=getChar())!=-1 && !isChar(c, ')') && !isChar(c, ',')){
                    s += c;
                }
                v.push_back(s);
            }

            if(isChar(c, ')'))
                break;
        }
        skipWhiteSpace();

        return isChar(c, ')');
    }

    void Tokenizer::readParamVal(vector<string>& v){
        skipWhiteSpace();

        if(readNext("(")){
            while(!readSingleParamVal(v));
        }
    }

}

