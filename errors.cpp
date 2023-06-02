#include <iostream>
#include <cstring>

//Обработка ошибок
class Exception : public std::exception{
protected:
    char *str;
public:
    explicit Exception(const char* s){
        str = new char[strlen(s) + 1];
        strcpy_s(str, strlen(s) + 1, s);
    }
    ~Exception() { delete[] str; }
    const char* what() const noexcept override { return str; }
    virtual void print() { std::cout << "Exception: " << what();}
};

class SampleException : public Exception{
public:
    SampleException(const char* s) : Exception(s) {};
    virtual void print(){
        std::cout << "SampleException: " << what(); 
    }
};

class SampleSizeException : public SampleException{
public:
    SampleSizeException(const char* s) : SampleException(s) {};
    virtual void print(){
        std::cout << "SampleSizeException: " << what(); 
    }
};

class SampleFormatException : public SampleException{
public:
    SampleFormatException(const char* s) : SampleException(s) {};
    virtual void print(){
        std::cout << "SampleFormatException: " << what(); 
    }
};

class SampleCountException : public SampleException{
public:
    SampleCountException(const char* s) : SampleException(s) {};
    virtual void print(){
        std::cout << "SampleCountException: " << what(); 
    }
};

class ImageException : public Exception{
public:
    ImageException(const char* s) : Exception(s) {};
    virtual void print(){
        std::cout << "ImageException: " << what(); 
    }
};

class WrongFormatException : public ImageException{
public:
    WrongFormatException(const char* s) : ImageException(s) {};
    virtual void print(){
        std::cout << "WrongFormatException: " << what(); 
    }
};

class MatrixException : public Exception{
public:
    MatrixException(const char* s) : Exception(s) {};
    virtual void print(){
        std::cout << "MatrixException: " << what(); 
    }
};

class MatrixSizeException : public MatrixException{
public:
    MatrixSizeException(const char* s) : MatrixException(s) {};
    virtual void print(){
        std::cout << "MatrixSizeException: " << what(); 
    }
};