#include "file.h"
#include "string.h"

File::File(const char* file, ios_base::openmode mode){
    this->open(file, mode);
}

File::File(const string& file, ios_base::openmode mode){
    this->open(file, mode);
}

File::File(){
    //用于延迟打开文件
}

File::~File(){
    this->close();
}

bool File::isOpen(){
    return this->fs.is_open();
}

void File::open(const char* file, ios_base::openmode mode){
    //先关闭当前已打开文件流
    this->close();
    //再打开新的文件流
    this->fs.open(file, mode);
}

void File::open(const string& file, ios_base::openmode mode){
    //先关闭当前已打开文件流
    this->close();
    //再打开新的文件流
    this->fs.open(file.c_str(), mode);
}

void File::close(){
    if(this->fs.is_open())
        this->fs.close();
}

int File::readChar(){
    if(this->fs.good())
        return this->fs.get();
    return -1;
}

void File::readLine(string& out){
    out.clear();
    static const int BUF_LEN = 4096;
    while(this->fs.good()){
        char buf[BUF_LEN];
        memset(buf, 0, BUF_LEN);

        this->fs.getline(buf, BUF_LEN);
        out.append(buf);

        if(this->fs.gcount() < BUF_LEN)
            break;
    }
}

int File::read(string& out, int n){
    out.clear();
    if(this->fs.good()){
        char buf[n + 1];
        memset(buf, 0, n+1);

        this->fs.read(buf, n);
        out.append(buf);
        return fs.gcount();
    }
    return 0;
}

int File::read(char* out, int n){
    if(this->fs.good()){
        fs.read(out, n);
        return fs.gcount();
    }
    return 0;
}

int File::read2End(char* out, int len){
    int BUF_LEN = 4096;

    int size = 0;
    memset(out, 0, len);
    while(true){
        if(size + BUF_LEN >= len)
            BUF_LEN = len - size;
        if(BUF_LEN <= 0)
            break;

        int n = read(out + size, BUF_LEN);
        size += n;
        if(n == 0)
            break;
    }
    return size;
}

int File::read2End(string& out){
    static const int BUF_LEN = 4096 + 1;

    out.clear();
    int size = 0;
    while(this->fs.good()){
        char buf[BUF_LEN];
        memset(buf, 0, BUF_LEN);

        this->fs.read(buf, BUF_LEN - 1);
        out.append(buf);
        size += fs.gcount();
    }
    return size;
}

void File::writeChar(char c){
    if(this->fs.good()){
        this->fs.put(c);
    }
}

void File::write(string& str, bool flush){
    if(this->fs.good()){
        this->fs.write(str.c_str(), str.length());
        if(flush)
            this->flush();
    }
}

void File::write(const char* str, int len, bool flush){
    if(this->fs.good()){
        this->fs.write(str, len);
        if(flush)
            this->flush();
    }
}

void File::flush(){
    this->fs.flush();
}

