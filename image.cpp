#include "image.hpp"
#include "errors.cpp"

Image::Image(const Image &img)
{
    iheader_ = img.iheader_;
    fheader_ = img.fheader_;
    matrix_ = img.matrix_;
}

Image &Image::operator=(const Image &img)
{
    Clear();
    iheader_ = img.iheader_;
    fheader_ = img.fheader_;
    matrix_ = img.matrix_;
    return *this;
}

Image::Image(const std::string &path)
{
    *this = ReadImg(path);
}

Image ReadImg(const std::string &path)
{
    Image img;
    std::ifstream f;

    f.open(path, std::ios_base::in | std::ios_base::binary);
    if (!f.is_open())
    {
        throw std::invalid_argument("ReadImg: Can't open file");
    }
    f.read(reinterpret_cast<char *>(&img.fheader_), sizeof(File_header));
    f.read(reinterpret_cast<char *>(&img.iheader_), sizeof(Info_header));

    if (img.fheader_.name[0] != 'B' || img.fheader_.name[1] != 'M')
    {
        throw WrongFormatException("ReadImg: Image not in BMP format");
    }

    if (img.iheader_.bitsPerPixel != 24)
    {
        throw WrongFormatException("ReadImg: Image not in 24 bit per pixel format");
    }

    resize_matrix(img.matrix_, img.iheader_.height, img.iheader_.width);

    const std::streamsize padding = img.iheader_.width % 4;

    RGB pixel;
    for (int i = 0; i < img.iheader_.height; ++i)
    {
        for (int j = 0; j < img.iheader_.width; ++j)
        {
            f.read(reinterpret_cast<char *>(&pixel), sizeof(RGB));
            if (pixel.r != pixel.b || pixel.b != pixel.g)
            {
                throw WrongFormatException("ReadImg: Image not in grayscale");
            }
            img.matrix_[img.iheader_.height - 1 - i][j] = pixel.r;
        }
        f.ignore(padding);
    }
    return img;
}

void WriteImg(const std::vector<std::vector<double>> &matrix, const std::string &path)
{
    // Write new bmp 24 bit image from double matrix
    std::vector<std::vector<uint8_t>> new_m(matrix.size(), std::vector<uint8_t>(matrix[0].size()));

    if (matrix.size() < 2)
    {
        throw MatrixSizeException("WriteImg: Size of matrix is invalid");
    }

    for (int i = 0; i < new_m.size(); i++)
    {
        for (int j = 0; j < new_m[0].size(); j++)
        {
            new_m[i][j] = static_cast<uint8_t>(matrix[i][j] * 255);
        }
    }

    WriteImg(new_m, path);
}
void WriteImg(const std::vector<std::vector<uint8_t>> &matrix, const std::string &path)
{
    // Write new bmp 24 bit image from uint8_t matrix
    std::ofstream stream;

    stream.open(path, std::ios_base::out | std::ios_base::binary);

    Image img;
    File_header fheader;
    Info_header iheader;

    if (matrix.size() == 0)
    {
        throw MatrixSizeException("WriteImg: Size of matrix is invalid");
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

    stream.write(reinterpret_cast<char *>(&fheader), sizeof(File_header));
    stream.write(reinterpret_cast<char *>(&iheader), sizeof(Info_header));

    const std::streamsize garb_between_lines = iheader.width % 4;
    RGB pixel;

    for (size_t i = 0; i < iheader.height; ++i)
    {
        for (size_t j = 0; j < iheader.width; ++j)
        {
            uint8_t tmp = matrix[iheader.height - 1 - i][j];
            pixel = RGB{tmp, tmp, tmp};
            stream.write(reinterpret_cast<char *>(&pixel), sizeof(RGB));
        }
        char garbage[garb_between_lines];
        stream.write(reinterpret_cast<char *>(&garbage), garb_between_lines);
    }
}

void WriteImg(const Image &img, const std::string &path)
{
    // Write new bmp 24 bit image from Image
    std::ofstream stream;

    stream.open(path, std::ios_base::out | std::ios_base::binary);
    ;

    stream << img.get_file_header();
    stream << img.get_info_header();

    // resize_matrix(img.matrix_, img.get_height(), img.get_width());

    const std::streamsize garb_between_lines = img.get_width() % 4;

    RGB pixel;

    for (size_t i = 0; i < img.get_height(); ++i)
    {
        for (size_t j = 0; j < img.get_width(); ++j)
        {
            uint8_t tmp = img.matrix_[img.get_height() - 1 - i][j];
            pixel = RGB{tmp, tmp, tmp};
            stream.write(reinterpret_cast<char *>(&pixel), sizeof(RGB));
        }
        char garbage[garb_between_lines];
        stream.write(reinterpret_cast<char *>(&garbage), garb_between_lines);
    }
}

Image negative(const Image &img)
{
    Image res = img;
    for (int i = 0; i < res.get_height(); i++)
    {
        for (int j = 0; j < res.get_width(); j++)
        {
            res.matrix_[i][j] = 255 - res.matrix_[i][j];
        }
    }
    return res;
}

std::vector<std::vector<uint8_t>> negative(const std::vector<std::vector<uint8_t>> &m)
{
    std::vector<std::vector<uint8_t>> res = m;
    for (int i = 0; i < res.size(); i++)
    {
        for (int j = 0; j < res[0].size(); j++)
        {
            res[i][j] = 255 - res[i][j];
        }
    }
    return res;
}

std::vector<std::vector<double>> negative(const std::vector<std::vector<double>> &m)
{
    std::vector<std::vector<double>> res = m;
    for (int i = 0; i < res.size(); i++)
    {
        for (int j = 0; j < res[0].size(); j++)
        {
            res[i][j] = 1 - res[i][j];
        }
    }
    return res;
}

std::vector<std::vector<double>> ImageToMatrix(const Image &img, bool reversed)
{
    // convert Image to matrix double black -> 1, white -> 0 if reversed is true
    std::vector<std::vector<double>> res;
    resize_matrix(res, img.matrix_[0].size(), img.matrix_.size());
    for (int i = 0; i < img.matrix_.size(); ++i)
    {
        for (int j = 0; j < img.matrix_[0].size(); ++j)
        {
            res[i][j] = reversed ? (255 - img.matrix_[i][j]) / 255. : img.matrix_[i][j] / 255.;
        }
    }
    return res;
}

template <typename T>
void resize_matrix(std::vector<std::vector<T>> &matrix, int w, int h)
{
    matrix.resize(h);
    for (auto &row : matrix)
    {
        row.resize(w);
    }
}