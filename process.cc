#include "process.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <iostream>
#include <set>
#include "file.h"
using namespace std;

namespace yam4{
    
    void Process::initEmbedMacros(){
    }

    void Process::process_string(const char* str, string& output){
        string s(str);
        this->process_string(s, output);
    }

    void Process::process_string(const string& str, string& output){
        Tokenizer t(str);
        _parse(t, output);
    }

    void Process::process_file(const string& file, string& output){
        File f(file);
        string s;
        f.read2End(s);

        this->process_string(s, output);
    }

    void Process::process_file(const char* file, string& output){
        string f(file);
        this->process_file(f, output);
    }

    void Process::_parse(Tokenizer& t, string& output, const char beg_term, const char end_term, const char end_term2){
        static int depth = 0;
        depth++;
        if(depth >= 100){
            printf("存在太深的宏递归栈，请检查是否存在无限递归\n");
            exit(1);
        }

        t.skipWhiteSpace();

        int count = 0;
        if(beg_term != -1 && !t.readNext(beg_term))
            return;

        string _output;
        _output.reserve(1024);

        count++;
        char c = -1;
        while((c = t.getChar())!=-1){
            if(isChar(c, beg_term)) count++;
            if(isChar(c, end_term)) count--;
            if(isChar(c, end_term2)) count--;
            if(count == 0){
                break;
            }

            if(isChar(c, '#')){
                //宏调用
                bool b = t.readNext("{");
                string w = t.readWord();

                if(w == "define"){
                    do_define(t, _output);
                }
                else if(w == "undef"){
                    do_undef(t, _output);
                }
                else if(w == "ifdef"){
                    do_ifdef(t, _output);
                }
                else if(w == "ifndef"){
                    do_ifndef(t, _output);
                }
                else if(w == "include"){
                    do_include(t, _output);
                }
                else if(w == "include_once"){
                    do_include_once(t, _output);
                }
                else if(w == "each"){
                    do_each(t, _output);
                }
                else if(w == "str"){
                    do_str(t, _output);
                }
                else if(w == "trim"){
                    do_trim(t, _output);
                }
                else if(w == "ifnil"){
                    do_ifnil(t, output);
                }
                else if(w == "ifnnil"){
                    do_ifnnil(t, output);
                }
                /***** 扩展的一些指令 ******/
                else if(w == "dnl"){
                    t.skipWhiteSpace();
                }
                else if(w == "nl"){
                    _output += "\n";
                }
                else if(w == "fread"){
                    do_fread(t, _output);
                }
                else if(w == "fwrite"){
                    do_fwrite(t, _output);
                }
                /*****************************/
                else if(isMacro(w)){
                    do_macro(t, w, _output);
                }
                else{
                    //nothing can be reliazed, output original string
                    _output += "#";
                    _output += w;
                }

                if(b)
                    t.readNext("}");

            }
            /* 
             * FIXME 不需要直接输出的东西
            */
            else if(isChar(c, '-')){
                c = t.getChar();
                //遇到 -[ 了
                if(isChar(c, '[')){
                    string s;

                    while(true){
                        //遇到第一个 [ 号
                        while((c=t.getChar())!=-1 && !isChar(c, ']')){
                            s += c;
                        }
                        c = t.getChar();
                        if(c==-1 || isChar(c, '-')){
                            break;
                        }else{
                            s += ']';
                            s += c;
                        }
                    }

                    _output += s;
                }else{
                    _output += '-';
                    _output += c;
                }
            }
            //FIXME 不要转义字符，可以使用[[]]代替，否则在嵌入代码时容易将代码中的转义字符给替换了
            //else if(isChar(c, '\\')){
            //    c = t.getChar();
            //    if(c != -1) _output += c;
            //}
            else{
                _output += c;
            }
        }

        //替换一次参数
        string s;
        s.reserve(1024);
        _replaceCode(_output, _global_param, s);

        output += s;

        depth--;
    }

    void Process::do_block(Tokenizer& t, string& output){
        _parse(t, output, '{', '}');
    }

    void Process::do_define(Tokenizer& t, string& output){
        string name = t.readWord();

        Macro& m = _macros[name];
        m.name = name;
        t.readParam(m.param);

        //XXX 延迟处理
        string s = t.readBlock();
        //do_block(t, s);
        m.code = trim(s);
    }

    void Process::do_undef(Tokenizer& t, string& output){
        string name = t.readWord();
        _macros.erase(name);
    }

    bool Process::_parseParamVal(Tokenizer& t, vector<string>& v){
        t.skipWhiteSpace();
        char c = -1;

        while((c = t.getChar())!=-1 && !isChar(c, ')') && !isChar(c, ',')){
            //字符串
            if(isChar(c, '\'') || isChar(c, '"')){
                t.getChar(-1);
                string s = t.readString();
                v.push_back(s);

                //把后面的空白符吸收掉
                t.skipWhiteSpace();
            }
            //block
            else if(isChar(c, '{')){
                t.getChar(-1);
                string s;
                do_block(t, s);
                v.push_back(s);

                //把后面的空白符吸收掉
                t.skipWhiteSpace();
            }
            //其它字面量
            else{
                t.getChar(-1);
                string s;
                _parse(t, s, -1, ',', ')');
                t.getChar(-1);
                v.push_back(s);
            }
        }

        return c==-1 || isChar(c, ')');
    }

    void Process::do_macro(Tokenizer& t, const string& name, string& output){
        if(_macros.find(name) == _macros.end()){
            //没有这个宏，将原文输出
            output += "#";
            output += name;
            return;
        }

        Macro& m = _macros[name];

        vector<string> v;
        if(t.readNext("(")){
            while(!_parseParamVal(t, v));
        }

        //将参数压入参数栈
        for(int i=0; i<m.param.size(); ++i){
            if(i < v.size()){
                _global_param[m.param[i]] = v[i];
            }
        }

        //处理一次宏代码
        string s;
        s.reserve(1024);
        Tokenizer code_t(m.code);
        _parse(code_t, s);
        output += s;

        //弹出参数
        for(int i=0; i<m.param.size(); ++i){
            if(i < v.size()){
                _global_param.erase(m.param[i]);
            }
        }
    }

    void Process::_replaceCode(string& code, MParam& m_param, string& output){
        //必须复制一份，不要污染定义
        output = code;

        for(MParam::iterator it=m_param.begin(); it!=m_param.end(); ++it){
            const string& s1 = it->first;
            const string& s2 = it->second;
            str_replace(output, s1, s2);
        }
    }


    void Process::_do_if(Tokenizer& t, string& output, bool flag, bool macro){
        if(!t.readNext("("))
            return;

        vector<string> v;
        while(!_parseParamVal(t, v));

        bool b;

        if(macro){
            if(flag) b = isMacro(v);
            else b = !isMacro(v);
        }else{
            if(flag) b = hasParam(v);
            else b = !hasParam(v);
        }

        string code;
        string ws;
        //解析#if块
        if(b){
            do_block(t, code);

            //跳过#else部分
            if(t.readNext("#else")){
                //FIXME 直接读一个块可能会导致一些解析上的问题
                t.readBlock();
                ws = "";
            }
        }
        //跳过#if，解析#else块
        else{
            //FIXME 直接读一个块可能会导致一些解析上的问题
            t.readBlock();
            ws = t.skipWhiteSpace();
            if(t.readNext("#else")){
                do_block(t, code);
                ws = "";
            }
        }
        
        output += code;
        output += ws;
    }

    void Process::do_ifdef(Tokenizer& t, string& output){
        _do_if(t, output, true);
    }

    void Process::do_ifndef(Tokenizer& t, string& output){
        _do_if(t, output, false);
    }

    void Process::do_each(Tokenizer& t, string& output){
        if(t.readNext("(")){
            string code = t.readBlock();

            t.readNext(",");
            vector<string> v;
            while(!_parseParamVal(t, v));

            string key = "$$";
            for(int i=0; i<v.size(); ++i){
                MParam m_param;
                m_param[key] = v[i];

                //先替换$$符号
                string s;
                _replaceCode(code, m_param, s);

                //再进行解析
                string s2;
                process_string(s, s2);

                output += s2;
            }
        }
    }

    void Process::do_include(Tokenizer& t, string& output){
        static int depth = 0;

        depth++;
        if(depth >= 100){
            printf("include too many files...\n");
            exit(1);
        }

        if(t.readNext("(")){
            vector<string> v;
            while(!_parseParamVal(t, v));

            for(int i=0; i<v.size(); ++i){
                string _out;
                process_file(v[i], _out);

                output += _out;
            }
        }
    }

    void Process::do_include_once(Tokenizer& t, string& output){
        static set<string> cache;

        if(t.readNext("(")){
            vector<string> v;
            while(!_parseParamVal(t, v));

            for(int i=0; i<v.size(); ++i){
                //FIXME 诡异的情况，如果在栈内开辟一段内存空间，传入realpath后会崩溃，原因还未知
                char* buf = NULL;
                if((buf = realpath(v[i].c_str(), buf)) == NULL){
                    continue;
                }

                //查看cache中是否已经有这个路径，如果有说明已经处理过，如果没有则宏处理这个文件
                string s(buf);
                if(cache.find(s) != cache.end())
                    continue;

                //宏解析
                string _out;
                process_file(v[i], _out);
                output += _out;

                //加入cache中，标记已经处理过
                cache.insert(s);

                //释放掉内存
                free(buf);
            }
        }
    }

    void Process::do_str(Tokenizer& t, string& output){
        if(t.readNext("(")){
            vector<string> v;
            while(!_parseParamVal(t, v));

            string s;
            for(int i=0; i<v.size(); ++i){
                s += v[i];
            }

            output += makeString(s);
        }
    }

    void Process::do_trim(Tokenizer& t, string& output){
        if(t.readNext("(")){
            vector<string> v;
            while(!_parseParamVal(t, v));

            for(int i=0; i<v.size(); ++i){
                output += trim(v[i]);
            }
        }
    }

    void Process::do_fread(Tokenizer& t, string& output){
        if(t.readNext("(")){
            vector<string> v;
            while(!_parseParamVal(t, v));

            for(int i=0; i<v.size(); ++i){
                File f(v[i]);
                string s;
                s.reserve(1024);
                f.read2End(s);
                output += s;
            }
        }
    }

    void Process::do_fwrite(Tokenizer& t, string& output){
        if(t.readNext("(")){
            vector<string> v;
            while(!_parseParamVal(t, v));

            if(v.size() != 0){
                string s;
                s.reserve(1024);
                for(int i=1; i<v.size(); ++i){
                    s += v[i];
                }

                File f(v[0], ios_base::out);
                f.write(s);
            }
        }
    }

    void Process::do_ifnil(Tokenizer& t, string& output){
        _do_if(t, output, true, false);
    }

    void Process::do_ifnnil(Tokenizer& t, string& output){
        _do_if(t, output, false, false);
    }
}

