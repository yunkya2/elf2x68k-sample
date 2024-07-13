#include <iostream>

//  ラムダ式
void cclambda() {
    std::cout << std::endl << "** lambda **" << std::endl;

    auto lambda = [](int x) {
        std::cout << "lambda x = " << x << std::endl;
    };

    lambda(1234);
    lambda(5678);
}
