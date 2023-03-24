#pragma ones

#include <iostream>
#include <cstring>

//Обработка ошибок
class Exception : public std::exception{
protected:
    char *str;
public:
    Exception(const char* s){
        str = new char[strlen(s) + 1];
        strcpy_s(str, strlen(s) + 1, s);
    }
    Exception(const Exception &e) {
        str = new char[strlen(e.str) + 1];
        strcpy_s(str, strlen(e.str) + 1, e.str);
    }
    ~Exception() { delete[] str; }
    virtual void print() { std::cout << "Exception: " << str << what();}
};

class SampleException : public Exception{
public:
    SampleException(const char* s) : Exception(s) {};
    virtual void print(){
        std::cout << "SampleException: " << str << what(); 
    }
};

class SampleNotSquareException : public SampleException{
public:
    SampleNotSquareException(const char* s) : SampleException(s) {};
    virtual void print(){
        std::cout << "SampleNotSquareException: " << str << what(); 
    }
};

class SampleSizeException : public SampleException{
public:
    SampleSizeException(const char* s) : SampleException(s) {};
    virtual void print(){
        std::cout << "SampleSizeException: " << str << what(); 
    }
};

class ImageException : public Exception{
public:
    ImageException(const char* s) : Exception(s) {};
    virtual void print(){
        std::cout << "ImageException: " << str << what(); 
    }
};

class WrongFormatException : public ImageException{
public:
    WrongFormatException(const char* s) : ImageException(s) {};
    virtual void print(){
        std::cout << "WrongFormatException: " << str << what(); 
    }
};
