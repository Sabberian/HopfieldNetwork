#pragma ones

#include <vector>

#include <iostream>
#include <String>
#include <cstring>
#include "errors.cpp"

#define Matrix_D std::vector<std::vector<double>>
#define Matrix_I std::vector<std::vector<int>> 
#define Matrix_T std::vector<std::vector<T>>

//состояния нейрона -1 или 1
enum n_state{
    passive = -1,
    active = 1
};

//функции для работы с матрицами
template <typename T>
Matrix_T operator *(const Matrix_T& M1, const Matrix_T& M2){
    Matrix_T res_matrix(M1.size(), std::vector<T>(M2[0].size()));
    for (int i = 0; i < M1.size(); i++) {
        for (int j = 0; j < M2[0].size(); j++) {
            for (int inner = 0; inner < M2.size(); inner++) {
                res_matrix[i][j] += M1[i][inner] * M2[inner][j];
            }
        }
    }
    return res_matrix;
}

template <typename T> 
Matrix_T transpose_matrix(const Matrix_T& matrix){
    Matrix_T transposed_matrix(matrix.size(), std::vector<T>(matrix.size()));
    for (int i = 0; i < matrix.size(); i++){
        for (int j = i; j < matrix.size(); j++){
            transposed_matrix[i][j] = matrix[j][i];
            transposed_matrix[j][i] = matrix[i][j];
        }
    }
    return transposed_matrix;
};
//------------------------------------------------

//Класс нейронной сети
class HopfieldNetwork{
private:
    int neuron_count;
    Matrix_D weights;

public:
//TODO: оператор копирования | Обработка изображений (img -> matrix, matrix -> img) | все остальное
    HopfieldNetwork(int neuron_count) : neuron_count(neuron_count){
        init_weights();
    };
    ~HopfieldNetwork(){
        weights.clear();
        weights.shrink_to_fit();
    }

    void init_weights(){
        //инициализация матрицы весов нулевыми значениями
        weights.resize(neuron_count);
        for (int i=0; i < neuron_count; i++){
            weights[i].resize(neuron_count);
            for (int j=0; j < neuron_count; j++){
                weights[i][j] = 0;
            }
        }
    }

    void load_sample(Matrix_I sample){
        //загрузить образец из std::vector<std::vector<int>>
        if (sample.size() != sample[0].size()){
            throw SampleNotSquareException("Sample is not square");
        }
        Matrix_I t = (sample * transpose_matrix<int>(sample));
        if (t.size() != weights.size()){
            throw SampleSizeException("Sample size doesn't match weights size");
        }
        for (int i = 0; i < t.size(); i++){
            for (int j = 0; j < t.size(); j++){
                weights[i][j] += t[i][j];
                if (i == j){
                    weights[i][j] = 0;
                }
            }
        }
    }

    void print_weights(){
        for (int i=0; i < neuron_count; i++){
            for (int j=0; j < neuron_count; j++){
                std::cout << weights[i][j] << " ";
            }
            std::cout<<"\n";
        }
    }
};

int main(){
    // std::cout << "Hello Hopfield!\n";
    // HopfieldNetwork net(3);
    // std::vector<std::vector<int>> a = {{1, 0, 1}, {0, 1, 0}, {1, 1, 1}};
    // net.load_sample(a);
    // net.print_weights();
}