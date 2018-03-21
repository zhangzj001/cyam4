#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <stdio.h>
#include <string.h>
#include <map>
#include "token.h"
#include "utils.h"
using namespace std;

namespace yam4{

    struct Macro{
        string name;
        vector<string> param;
        string code;

        /* just for debuging */
        void debug(){
            string x;
            x.reserve(40960);

            x += "name: " + name + "\n";
            x += "param: [";
            if(param.size() > 0)
                x += "\n";
            for(int i=0; i<param.size(); ++i){
                x += " " + toString(i) + " - " + param[i] + "\n";
            }
            x += "]\ncode: " + code + "\n";

            printf("%s", x.c_str());
        }
    };

    typedef map<string, string> MParam;

    class Process{
        public:
            Process(){
                initEmbedMacros();
            }
            void debug(){
                for(map<string, Macro>::iterator it=_macros.begin(); it!=_macros.end(); ++it){
                    it->second.debug();
                }
            }

        public:
            void process_string(const char* str, string& output);
            void process_string(const string& str, string& output);
            void process_file(const string& file, string& output);
            void process_file(const char* file, string& output);

        private:
            void _parse(Tokenizer& t, string& output, const char beg_term = -1, const char end_term = -1, const char end_term2 = -1);
            bool _parseParamVal(Tokenizer& t, vector<string>& v);
            void _replaceCode(string& code, MParam& m_param, string& output);

        private:
            void do_block(Tokenizer& t, string& output);
            void do_define(Tokenizer& t, string& output);
            void do_undef(Tokenizer& t, string& output);
            void do_macro(Tokenizer& t, const string& name, string& output);
            void _do_if(Tokenizer& t, string& output, bool flag, bool macro=true);
            void do_ifdef(Tokenizer& t, string& output);
            void do_ifndef(Tokenizer& t, string& output);
            void do_ifnil(Tokenizer& t, string& output);
            void do_ifnnil(Tokenizer& t, string& output);
            void do_each(Tokenizer& t, string& output);
            void do_include(Tokenizer& t, string& output);
            void do_include_once(Tokenizer& t, string& output);
            void do_str(Tokenizer& t, string& output);
            void do_trim(Tokenizer& t, string& output);
            void do_fread(Tokenizer& t, string& output);
            void do_fwrite(Tokenizer& t, string& output);

            inline bool isMacro(const string& s){
                return _macros.find(s) != _macros.end();
            }
            inline bool isMacro(const vector<string>& v){
                for(int i=0; i<v.size(); ++i){
                    if(!isMacro(v[i]))
                        return false;
                }
                return true;
            }
            inline bool hasParam(const string& s){
                if(_global_param.find(s) != _global_param.end()){
                    return _global_param[s].length() != 0;
                }
                return false;
            }
            inline bool hasParam(const vector<string>& v){
                for(int i=0; i<v.size(); ++i){
                    if(!hasParam(v[i]))
                        return false;
                }
                return true;
            }

        private:
            map<string, Macro> _macros;
            MParam _global_param;
        private:
            /* 预置的宏 */
            void initEmbedMacros();
    };
}

#endif /* end of include guard: __PROCESS_H__ */

