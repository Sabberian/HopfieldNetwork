#define SFML_STATIC

#include <vector>
#include <math.h>
#include "image.hpp"
#include "hopfield.hpp"
#include "errors.cpp"
#include "gui.hpp"


int main()
{
    // std::cout << "Hello Hopfield!\n";
    // HopfieldNetwork net(49);
    // net.load_sample("./samples/1.bmp");
    // net.load_network("./network/numbers.hop")
    // std::vector<double> a = net.compare("./samples/smile.bmp");
    // std::vector<std::vector<uint8_t>> b = conv_array(a, 5, 5);
    // WriteImg(negative(b), "new.bmp");
    // net.save_network("./net.hop");
    make_gui();
}
