#include <iostream>
#include <map>
#include <string>

// 連想配列 std::map
void ccmap() {
    std::cout << std::endl << "** std::map **" << std::endl;

    // 連想配列を初期化
    std::map<std::string, int> mp {{"X68000", 1},{"MZ-80C", 2},{"MZ-2521", 3}};

    // 連想配列の全要素を表示
    for (const auto &[key, value] : mp) {
        std::cout << "key=" << key << " value=" << value << std::endl;
    }

    // 存在しないキーをの値を得ようとしてstd::out_of_range例外を起こす
    try {
        std::cout << mp.at("unknown") << std::endl;
    }
    catch (std::out_of_range& e) {                  // 例外をキャッチ
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

    // std::cout << mp.at("unknown") << std::endl;     // 例外発生で終了
}
