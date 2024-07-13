#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>

//  C++文字列 std::string
void ccstring() {
    std::cout << std::endl << "** std::string **" << std::endl;

    // 文字列の初期化
    std::string s = "test string";
    std::cout << s << std::endl;

    // 文字列の加算
    s += " + string 2";
    std::cout << s << std::endl;
    std::cout << "length = " << s.length() << std::endl;

    // C文字列 への変換
    std::printf("c_str = %s\n", s.c_str());
    std::printf("strlen() = %d\n", std::strlen(s.c_str()));
}
