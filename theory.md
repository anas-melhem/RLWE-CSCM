#include "rlwe_cscm/rlwe_cscm.hpp"
#include <chrono>
#include <iostream>

using namespace rlwe_cscm;
using clock_type = std::chrono::high_resolution_clock;

template <typename F>
long long time_ns(F&& f) {
    auto start = clock_type::now();
    f();
    auto end = clock_type::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}

int main() {
    Parameters params{16, 257, 16};
    RLWECSCM fhe(params, 2026);
    constexpr int trials = 1000;

    long long keygen_ns = 0, enc_ns = 0, dec_ns = 0, add_ns = 0, mul_ns = 0;
    auto keys = fhe.keygen();

    for (int i = 0; i < trials; ++i) {
        keygen_ns += time_ns([&] { volatile auto k = fhe.keygen(); (void)k; });
        auto m1 = fhe.random_message();
        auto m2 = fhe.random_message();

        Ciphertext c1 = fhe.encrypt(keys.pk, m1);
        Ciphertext c2 = fhe.encrypt(keys.pk, m2);

        enc_ns += time_ns([&] { volatile auto c = fhe.encrypt(keys.pk, m1); (void)c; });
        dec_ns += time_ns([&] { volatile auto m = fhe.decrypt(keys.sk, c1); (void)m; });
        add_ns += time_ns([&] { volatile auto c = fhe.add(c1, c2); (void)c; });
        mul_ns += time_ns([&] { volatile auto c = fhe.multiply_raw(c1, c2); (void)c; });
    }

    std::cout << "operation,avg_time_ns\n";
    std::cout << "keygen," << keygen_ns / trials << "\n";
    std::cout << "encrypt," << enc_ns / trials << "\n";
    std::cout << "decrypt," << dec_ns / trials << "\n";
    std::cout << "hom_add," << add_ns / trials << "\n";
    std::cout << "hom_mul_raw," << mul_ns / trials << "\n";
    return 0;
}
