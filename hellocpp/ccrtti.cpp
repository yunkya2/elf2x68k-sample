#include <iostream>
#include <typeinfo>

class Base {
public:
    virtual ~Base() {}
};
class Derived : public Base {};

// 実行時型情報 (RTTI)
void ccrtti() {
    std::cout << std::endl << "** RTTI **" << std::endl;

    Base b;

    Derived d;
    Base* p1 = &b;
    Base* p2 = &d;

    // オブジェクトの型情報を表示
    std::cout << typeid(b).name() << "\n"           // 基底クラス
              << typeid(d).name() << "\n"           // 派生クラス
              << typeid(p1).name() << "\n"          // 基底クラスへのポインタ
              << typeid(*p1).name() << "\n"         // ポインタの指すオブジェクトは基底クラス
              << typeid(p2).name() << "\n"          // 派生クラスへのポインタ
              << typeid(*p2).name() << std::endl;   // ポインタの指すオブジェクトは派生クラス
}
