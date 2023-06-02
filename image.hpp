#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#ifndef IMAGE_HPP
#define IMAGE_HPP

using std::ostream;
struct __attribute__((packed)) File_header
{
    char name[2];
    unsigned int size;
    int garbage;
    unsigned int offset;

    void Clear()
    {
        *this = File_header({});
    }
};

struct __attribute__((packed)) Info_header
{
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

    void Clear()
    {
        *this = Info_header({});
    }
};

inline ostream &operator<<(ostream &os, const Info_header &header)
{
    os << header.header_size << header.width << header.height << header.colorplanes << header.bitsPerPixel << header.compression << header.bmp_byte_size << header.hres << header.vres << header.num_colors << header.num_important_colors;
    return os;
}

inline ostream &operator<<(ostream &os, const File_header &header)
{
    os << header.name[0] << header.name[1] << header.size << header.garbage << header.offset;
    return os;
}

struct __attribute__((packed)) RGB
{
    uint8_t b, g, r;
};

class Image
{
private:
    Info_header iheader_;
    File_header fheader_;

public:
    std::vector<std::vector<uint8_t>> matrix_;

    Image() {}

    Image(const std::string &path);

    Image(const Image &);

    Image &operator=(const Image &);

    void Clear()
    {
        matrix_.clear();
        matrix_.shrink_to_fit();
        iheader_.Clear();
        fheader_.Clear();
    }

    const Info_header &get_info_header() const
    {
        return iheader_;
    }

    const File_header &get_file_header() const
    {
        return fheader_;
    }

    int get_width() const
    {
        return iheader_.width;
    }

    int get_height() const
    {
        return iheader_.height;
    }

    friend Image ReadImg(const std::string &path);
    friend void WriteImg(Image &, const std::string &path);
};

Image ReadImg(const std::string &path);

void WriteImg(const std::vector<std::vector<uint8_t>> &matrix, const std::string &path);

void WriteImg(const std::vector<std::vector<double>> &matrix, const std::string &path);

void WriteImg(const Image &img, const std::string &path);

Image negative(const Image &img);

std::vector<std::vector<double>> negative(const std::vector<std::vector<double>> &m);

std::vector<std::vector<uint8_t>> negative(const std::vector<std::vector<uint8_t>> &m);

std::vector<std::vector<double>> ImageToMatrix(const Image &img, bool reversed = true);

template <typename T>
void resize_matrix(std::vector<std::vector<T>> &matrix, int w, int h);

#endif