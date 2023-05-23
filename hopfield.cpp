#pragma ones

#include <vector>

#include <iostream>
#include <math.h>
#include "image.hpp"
#include "errors.cpp"
#include "hopfield.hpp"
#define Matrix_D std::vector<std::vector<double>>
#define Matrix_I std::vector<std::vector<int>> 
#define Matrix_T std::vector<std::vector<T>>

//Matrix functions
template <typename T, typename U>
Matrix_T operator *(const Matrix_T& M, const std::vector<U>& V){
    //transposed vector * vector
    if (M[0].size() != 1){
        throw MatrixSizeException("Can't multiply matrix with not 1 columns matrix on vector");
    }
    Matrix_T res_matrix(M.size(), std::vector<T>(V.size()));
    for (int i = 0; i < res_matrix.size(); i++) {
        for (int j = 0; j < res_matrix[0].size(); j++) {
            res_matrix[i][j] += M[i][0] * V[j];
        }
    }
    return res_matrix;
}

template <typename T>
Matrix_T operator *(const Matrix_T& M1, const Matrix_T& M2){
    if (M1.size() != M2[0].size()){
        throw MatrixSizeException("Invalid matrix multiplication");
    }
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

template <typename T> 
Matrix_T transpose_vector(const std::vector<T>& vec){
    Matrix_T transposed_vec(vec.size(), std::vector<T>(1));
    for (int i = 0; i < vec.size(); i++){
        transposed_vec[i][0] = vec[i];
    }
    return transposed_vec;
};

template <typename T>
void print_matrix(const Matrix_T& M){
    for (auto i:M){
        for (auto j:i){
            std::cout << std::to_string(j) << " ";
        }
        std::cout<< "\n";
    }
}

template <typename T>
std::vector<T> expand_matrix(const Matrix_T& M){
    //matrix (2 dims) -> array(1 dim)
    std::vector<T> s(M.size() * M[0].size());
    for (int i = 0; i < M.size(); i++){
        for (int j = 0; j < M[0].size(); j++){
            s[j+i*M.size()] = M[i][j];
        }
    }
    return s;
}

//------------------------------------------------

//Network

bool HopfieldNetwork::check_network_state(bool* states) const{
    for (int i = 0; i < neuron_count; i++){
        if (!states[i])
            return 0;
    }
    return 1;
}

void HopfieldNetwork::remove_weights(){
    weights.clear();
    weights.shrink_to_fit();
}

HopfieldNetwork::HopfieldNetwork() : neuron_count(0){}
HopfieldNetwork::HopfieldNetwork(int neuron_count) : neuron_count(neuron_count){
    sample_count = 0;
    init_weights();
    init_states();
}
HopfieldNetwork::HopfieldNetwork(const std::string& path){
    load_network(path);
}
HopfieldNetwork::HopfieldNetwork(const HopfieldNetwork& N){
    neuron_count = N.neuron_count;
    weights = N.weights;
    neurons_states = N.neurons_states;
    sample_count = N.sample_count;
}

HopfieldNetwork::~HopfieldNetwork(){
    weights.clear();
    weights.shrink_to_fit();
}

HopfieldNetwork& HopfieldNetwork::operator=(const HopfieldNetwork N){
    reset_network();
    neuron_count = N.neuron_count;
    sample_count = N.sample_count;
    weights = N.weights;
    neurons_states = N.neurons_states;
    return *this;
}

void HopfieldNetwork::clear_weights(){
    for (int i = 0; i < weights.size(); i++){
        for (int j = 0; j < weights[0].size(); j++){
            weights[0][i] = 0;
        }
    }
    sample_count = 0;
}

void HopfieldNetwork::reset_network(){
    remove_weights();
    neurons_states.clear();
    sample_count = 0;
    neuron_count = 0;
}

void HopfieldNetwork::resize_network(int new_neuron_count){
    neuron_count = new_neuron_count;

    weights.clear();
    weights.shrink_to_fit();
    neurons_states.clear();

    sample_count = 0;
    init_weights();
    init_states();
}

void HopfieldNetwork::init_weights(){
    //matrix init with zero weights
    weights.resize(neuron_count);
    for (int i=0; i < neuron_count; i++){
        weights[i].resize(neuron_count);
        for (int j=0; j < neuron_count; j++){
            weights[i][j] = 0;
        }
    }
}

void HopfieldNetwork::init_states(){
    neurons_states.resize(neuron_count);
    for (int i=0; i < neuron_count; i++){
        neurons_states[i] = n_state::passive;
    }
}

void HopfieldNetwork::save_network(const std::string& path){
    std::ofstream f;
    f.open(path, std::ios_base::out);
    if (neuron_count != weights.size()){
        throw MatrixSizeException("Weight's matrix size doesn't match number of neurons");
    }
    f << neuron_count << " " << sample_count << " ";
    for (int i = 0; i < neuron_count; i++){
        for (int j = 0; j < neuron_count; j++){
            f << weights[i][j] << " ";
        }
    }
}

void HopfieldNetwork::load_network(const std::string& path){
    std::ifstream f;
    f.open(path, std::ios_base::in);

    if (!f.is_open()) {
        throw std::invalid_argument("Can't open file");
    }

    f >> neuron_count >> sample_count;
    resize_matrix(weights, neuron_count, neuron_count);

    for (int i = 0; i < neuron_count; i++){
        for (int j = 0; j < neuron_count; j++){
            f >> weights[i][j];
        }
    }
    init_states();
}

void HopfieldNetwork::load_sample(const std::vector<double>& sample, bool check_sample_count){
    //Load sample from std::vector<double> (sample values must be 0 to 1)
    if (check_sample_count){
        if (neuron_count * N_COEFF < sample_count + 1){
            throw SampleCountException("Too many samples. Samples count must be not more then neuron count * N_COEFF");
        }
    }
    std::vector<double> mut_sample(sample.size());

    if (sample.size() != neuron_count){
        throw SampleSizeException("Sample size doesn't match neurons count");
    }

    for (int i = 0; i < neuron_count; i++){
        if (sample[i] < 0 || sample[i] > 1) throw SampleFormatException("Sample values must be 0 to 1");
        mut_sample[i] = sample[i] >= 0.5 ? 1 : -1;
    }
    Matrix_D t = transpose_vector(mut_sample) * mut_sample;

    for (int i = 0; i < t.size(); i++){
        for (int j = 0; j < t[0].size(); j++){
            weights[i][j] += t[i][j];
            if (i == j){
                weights[i][j] = 0;
            }
        }
    }

    sample_count++;
}

void HopfieldNetwork::load_sample(const std::string& path, bool check_sample_count){
    //Load sample from bmp file
    load_sample(ImageToMatrix(ReadImg(path)), check_sample_count);
}

void HopfieldNetwork::load_sample(const Matrix_D& sample, bool check_sample_count){
    //Load sample from double matrix (matrix values must be 0 to 1)
    std::vector<double> s = expand_matrix(sample);
    load_sample(s, check_sample_count);
}

void HopfieldNetwork::load_sample(const Image& img, bool check_sample_count=1){
    //Load sample from Image
    load_sample(ImageToMatrix(img), check_sample_count);
}

n_state HopfieldNetwork::get_neuron_state(int neuron_index) const{
    int sum = 0;
    for (int i=0; i < neuron_count; i++){
        sum += neurons_states[i] * weights[neuron_index][i];
    }
    return (sum >= 0 ? n_state::active : n_state::passive);
}

std::vector<double> HopfieldNetwork::compare(const Matrix_D& img_matrix){
    //Clear noise from img by matrix
    return compare(expand_matrix(img_matrix));
}

std::vector<double> HopfieldNetwork::compare(const std::string& img_path){
    //Clear noise from img by path
    return compare(ImageToMatrix(Image(img_path)));
}

std::vector<double> HopfieldNetwork::compare(const Image& img){
    //Clear noise from img by path
    return compare(ImageToMatrix(img));
}

std::vector<double> HopfieldNetwork::compare(const std::vector<double>& img_vector){
    //Asynchronous noise clearing from template converted to vector (template values must be 0 to 1, 0 - white 1 - black)
    n_state sprev, snew;
    bool is_unchanged[neuron_count];

    if (img_vector.size() != neuron_count){
        throw SampleSizeException("Sample to compare doesn't match neuron count");
    }

    for (int i = 0; i < neuron_count; i++){
        neurons_states[i] = img_vector[i] >= 0.5 ? n_state::active : n_state::passive;
    }
    while (!check_network_state(is_unchanged)){
        for (int i = 0; i < neuron_count; i++){
            sprev = neurons_states[i];
            snew = get_neuron_state(i);
            if (sprev == snew){
                is_unchanged[i] = 1;
            } else{
                is_unchanged[i] = 0;
                neurons_states[i] = snew;
            }
        }
    }
    std::vector<double> res(neuron_count);
    for (int i = 0; i < neuron_count; i++){
        res[i] = neurons_states[i] > 0 ? 1 : 0;
    }
    return res;
}

void HopfieldNetwork::print_weights(){
    for (int i=0; i < neuron_count; i++){
        for (int j=0; j < neuron_count; j++){
            std::cout << weights[i][j] << " ";
        }
        std::cout<<"\n";
    }
}


