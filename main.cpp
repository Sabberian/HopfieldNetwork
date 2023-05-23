#pragma ones

#include <vector>

#include <math.h>
#include "image.hpp"
#include "hopfield.hpp"
#include "errors.cpp"
#include "gui.hpp"
#define Matrix_D std::vector<std::vector<double>>
#define Matrix_I std::vector<std::vector<int>> 
#define Matrix_T std::vector<std::vector<T>>

template <typename T>
Matrix_T conv_array(const std::vector<T>& v, int w, int h){
    //array (1 dim) -> matrix (2 dims)

    if (h*w != v.size()){
        throw MatrixSizeException("Can't convert vector to matrix with given size");
    }

    Matrix_T m(h, std::vector<T>(w));
    for (int i = 0; i < h; i++){
        for (int j = 0; j < w; j++){
            m[i][j] = v[i*w + j];
        }
    }
    return m;
}

int main(){
    // std::cout << "Hello Hopfield!\n";
    // HopfieldNetwork net(25);
    // net.load_sample("./samples/1.bmp");
    // std::vector<double> a = net.compare("./samples/smile.bmp");
    // std::vector<std::vector<double>> b = conv_array(a, 5, 5);
    // WriteImg(negative(static_cast<std::vector<std::vector<double>>&>(b)), "new.bmp");
    // net.save_network("./net.hop");
    drawPixelArt();
}
