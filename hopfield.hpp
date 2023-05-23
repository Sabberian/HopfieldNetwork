#pragma ones

#include <vector>
#include "image.hpp"

#ifndef HOPFIELD_HPP
#define HOPFIELD_HPP

#define Matrix_D std::vector<std::vector<double>>

enum n_state{
    passive = -1,
    active = 1
};

class HopfieldNetwork{
private:
    int neuron_count;
    Matrix_D weights;
    std::vector<n_state> neurons_states;
    int sample_count = 0;

protected:
    bool check_network_state(bool* states) const;

    void remove_weights();

public:
    const double N_COEFF = 0.15;

    HopfieldNetwork();
    HopfieldNetwork(int neuron_count);

    HopfieldNetwork(const std::string& path);

    HopfieldNetwork(const HopfieldNetwork& N);

    ~HopfieldNetwork();

    HopfieldNetwork& operator=(const HopfieldNetwork N);

    void clear_weights();

    void reset_network();

    void resize_network(int new_neuron_count);

    void init_weights();

    void init_states();

    void save_network(const std::string& path);

    void load_network(const std::string& path);

    void load_sample(const std::vector<double>& sample, bool check_sample_count=1);

    void load_sample(const std::string& path, bool check_sample_count=1);

    void load_sample(const Matrix_D& sample, bool check_sample_count=1);

    void load_sample(const Image& img, bool check_sample_count=1);

    n_state get_neuron_state(int neuron_index) const;

    std::vector<double> compare(const Matrix_D& img_matrix);

    std::vector<double> compare(const std::string& img_path);

    std::vector<double> compare(const Image& img);

    std::vector<double> compare(const std::vector<double>& img_vector);

    void print_weights();
};
#endif