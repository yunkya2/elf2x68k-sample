#include <iostream>
#include <memory>

class Cls {
    const char *msg;
public:
    Cls(const char *m) { msg = m; std::cout << "create" << std::endl; }
    ~Cls() { std::cout << "destroy" << std::endl; }
    void m() { std::cout << "method: " << msg << std::endl; }
};

void func(std::unique_ptr<Cls> p) {
    // ccsmart()からオブジェクトの所有権を渡される
    std::cout << "in func()" << std::endl;
    p->m();
    // 関数終了でスコープを抜けるときにデストラクタが呼ばれる
}

// スマートポインタ
void ccsmart() {
    std::cout << std::endl << "** std::unique_ptr **" << std::endl;

    // オブジェクトを構築する
    auto p = std::make_unique<Cls>("hello");

    // オブジェクトのメソッド呼び出し、ポインタの表示
    p->m();
    std::cout << "p=" << p.get() << std::endl;

    // スマートポインタを関数に渡す (所有権が移動)
    std::cout << "call func()" << std::endl;
    func(std::move(p));
    std::cout << "return from func()" << std::endl;

    // 所有権が移動したためpはnullptrになっている
    std::cout << "p=" << p.get() << std::endl;
}
