#pragma ones

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

struct __attribute__((packed)) File_header {
    char name[2];
    unsigned int size;
    int garbage;
    unsigned int offset;

    void Clear() {
        *this = File_header({});
    }
};

struct __attribute__((packed)) Info_header {
    unsigned int header_size;
    unsigned int width;
    unsigned int height;
    unsigned short int colorplanes;
    unsigned short int bitsPerPixel;
    unsigned int compression;
    unsigned int bmp_byte_size;
    unsigned int hres;
    unsigned int vres;
    unsigned int num_colors;
    unsigned int num_important_colors;

    void Clear() {
        *this = Info_header({});
    }
};

struct __attribute__((packed)) RGB{
    uint8_t b, g, r;
};

class Image{
private:
    Info_header iheader_;
    File_header fheader_;
public:
    std::vector<std::vector<uint8_t>> matrix_;

    Image(){}

    Image(const std::string& path);

    Image(const Image&);

    Image& operator=(const Image&);

    void Clear(){
        matrix_.clear();
        matrix_.shrink_to_fit();
        iheader_.Clear();
        fheader_.Clear();
    }

    Info_header& get_info_header(){
        return iheader_;
    }

    File_header& get_file_header(){
        return fheader_;
    }

    int get_width(){
        return iheader_.width;
    }

    int get_height(){
        return iheader_.height;
    }

    friend Image ReadImg(const std::string&);
    friend void WriteImg(Image&, const std::string&);
};

Image ReadImg(const std::string&);

void WriteImg(const std::vector<std::vector<uint8_t>>&, const std::string&);

void WriteImg(const std::vector<std::vector<double>>&, const std::string&);

void WriteImg(Image&, const std::string&);

Image& negative(Image&);

std::vector<std::vector<double>>& negative(std::vector<std::vector<double>>& m);

std::vector<std::vector<uint8_t>>& negative(std::vector<std::vector<uint8_t>>& m);

std::vector<std::vector<double>> ImageToMatrix(const Image& img, bool reversed=true);

template <typename T>
void resize_matrix(std::vector<std::vector<T>>& matrix, int w, int h){
    matrix.resize(h);
    for (auto& row: matrix){
        row.resize(w);
    }
}
