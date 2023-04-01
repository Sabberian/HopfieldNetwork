#pragma ones

#include <vector>

#include <math.h>
#include "image.hpp"
#include "hopfield.hpp"
#include "errors.cpp"
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
    std::cout << "Hello Hopfield!\n";
    HopfieldNetwork net(16);
    std::vector<std::vector<double>> a = {{1, 0, 0, 1}, {0, 1, 1, 0}, {0, 1, 1, 0}, {1, 0, 0, 1}};
    net.load_sample(a, false);
    a = {{0, 1, 0, 1}, {1, 0, 1, 0}, {0, 1, 0, 1}, {1, 0, 1, 0}};
    net.load_sample(a, false);
    // a = {{1, 1, 0, 0}, {0, 0, 1, 1}, {1, 1, 0, 0}, {0, 0, 1, 1}};
    // net.load_sample(a, true);
    // std::vector<std::vector<double>> b = {{1, 1, 1, 1}, {1, 1, 1, 0}, {0, 1, 1, 0}, {1, 1, 0, 1}};
    std::vector<std::vector<double>> b = {{1, 1, 0, 1}, {1, 0, 1, 0}, {1, 1, 0, 0}, {0, 0, 1, 0}};
    std::vector<double> c = net.compare(b);
    std::vector<std::vector<double>> k = conv_array(c, 4, 4);
    for (auto i: k){
        for (auto j : i){
            std::cout << j << " ";
        }
        std::cout<<"\n";
    }
    WriteImg(k, "new.bmp");
    net.save_network("./net.hop");
    net.print_weights();
    std::vector<std::vector<double>> a = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
    WriteImg(a, "base.bmp");
}
