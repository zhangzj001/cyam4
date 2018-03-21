#ifndef __FILE_H__
#define __FILE_H__

#include <fstream>
#include <string>
using namespace std;

class File{
    public:
        File(const char* file, ios_base::openmode mode = ios_base::in | ios_base::out | ios_base::binary);
        File(const string& file, ios_base::openmode mode = ios_base::in | ios_base::out | ios_base::binary);
        File();
        ~File();

    public:
        int readChar();
        void readLine(string& out);
        int read(string& out, int n);
        int read(char* out, int n);
        int read2End(char* out, int len);
        int read2End(string& out);

    public:
        void writeChar(char c);
        void write(string& str, bool flush = false);
        void write(const char* str, int len, bool flush = false);
        void flush();

    public:
        void close();
        void open(const char* file, ios_base::openmode mode = ios_base::in | ios_base::out | ios_base::binary);
        void open(const string& file, ios_base::openmode mode = ios_base::in | ios_base::out | ios_base::binary);
        bool isOpen();

    private:
        fstream fs;
};

#endif
