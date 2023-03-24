#pragma ones

#include "image.h"
#include "errors.cpp"

Image ReadImg(const std::string& path){
    Image img;
    std::fstream f;

    f.open(path, std::ios_base::in | std::ios_base::binary);
    if (!f.is_open()) {
        throw std::invalid_argument("Can't open file");
    }

    f.read(reinterpret_cast<char*> (&img.fheader_), sizeof(File_header));
    f.read(reinterpret_cast<char*> (&img.iheader_), sizeof(Info_header));

    if (img.fheader_.name[0] != 'B' || img.fheader_.name[1] != 'M') {
        throw WrongFormatException("Image not in BMP format");
    }

    if (img.iheader_.bitsPerPixel != 24) {
        throw WrongFormatException("Image not in 24 bit per pixel format");
    }

    resize_matrix(img.matrix_ , img.iheader_.height, img.iheader_.width);

    const std::streamsize padding = img.iheader_.width % 4;

    RGB pixel;
    for (int i = 0; i < img.iheader_.height; ++i) {
        for (int j = 0; j < img.iheader_.width; ++j) {
            f.read(reinterpret_cast<char*>(&pixel), sizeof(RGB));
            if (pixel.r != pixel.b || pixel.b != pixel.g){
                throw WrongFormatException("Image not in grayscale");
            }
            img.matrix_[img.iheader_.height - 1 - i][j] = pixel.r;
        }
        f.ignore(padding);
    }
    return img;
}

void WriteImg(const std::vector<std::vector<uint8_t>>& matrix, const std::string& path){
    //Write new bmp 24 bit image from uint8_t matrix
    std::fstream stream;

    stream.open(path, std::ios_base::out | std::ios_base::binary);;

    Image img;
    File_header fheader;
    Info_header iheader;

    if (matrix.size() == 0){
        throw WrongFormatException("In WriteImg: wrong matrix size");
    }
    if (matrix.size() != matrix[0].size()){
        throw WrongFormatException("In WriteImg: matrix must be square");
    }

    fheader.name[0] = (char)66;
    fheader.name[1] = (char)77;
    fheader.offset = sizeof(File_header) + sizeof(Info_header);
    fheader.size = fheader.offset + (matrix.size() * 3 + matrix[0].size() % 4) * matrix.size();
    fheader.garbage = 0;

    iheader.header_size = sizeof(Info_header);
    iheader.height = matrix.size();
    iheader.width = matrix[0].size();
    iheader.colorplanes = 1;
    iheader.bitsPerPixel = 24;
    iheader.compression = 0;
    iheader.bmp_byte_size = 0;
    iheader.hres = 3780;
    iheader.vres = 3780;
    iheader.num_colors = 0;
    iheader.num_important_colors = 0;

    std::cout<<fheader.name[0] << fheader.name[1] << " "<<fheader.offset << " " << fheader.size << " " <<sizeof(File_header) << " " << sizeof(Info_header);
    
    stream.write(reinterpret_cast<char*> (&fheader), sizeof(File_header));
    stream.write(reinterpret_cast<char*> (&iheader), sizeof(Info_header));

    const std::streamsize garb_between_lines = iheader.width % 4;
    RGB pixel;

    for (size_t i = 0; i < iheader.height; ++i) {
        for (size_t j = 0; j < iheader.width; ++j) {
            uint8_t tmp = matrix[iheader.height - 1 - i][j];
            pixel = RGB{tmp, tmp, tmp};
            stream.write(reinterpret_cast<char*> (&pixel), sizeof(RGB));

        }
        char garbage[garb_between_lines];
        stream.write(reinterpret_cast<char*> (&garbage), garb_between_lines);
    }

}

void WriteImg(Image& img, const std::string& path){
    //Write new bmp 24 bit image from Image
    std::fstream stream;

    stream.open(path, std::ios_base::out | std::ios_base::binary);;

    stream.write(reinterpret_cast<char*>(&img.fheader_), sizeof(File_header));
    stream.write(reinterpret_cast<char*>(&img.iheader_), sizeof(Info_header));


    resize_matrix(img.matrix_, img.iheader_.height, img.iheader_.width);

    const std::streamsize garb_between_lines = img.iheader_.width % 4;

    RGB pixel;

    for (size_t i = 0; i < img.iheader_.height; ++i) {
        for (size_t j = 0; j < img.iheader_.width; ++j) {
            uint8_t tmp = img.matrix_[img.iheader_.height - 1 - i][j];
            pixel = RGB{tmp, tmp, tmp};
            stream.write(reinterpret_cast<char*> (&pixel), sizeof(RGB));

        }
        char garbage[garb_between_lines];
        stream.write(reinterpret_cast<char*> (&garbage), garb_between_lines);
    }
}

int main(){
    std::vector<std::vector<uint8_t>> v = {{255, 0, 255, 0, 255}, {128, 30, 0, 0, 0}, {128, 60, 60, 128, 0}, {128, 60, 60, 128, 0}, {128, 60, 60, 128, 0}};
    WriteImg(v, "./new.bmp");

}