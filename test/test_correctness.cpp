#include "rlwe_cscm/rlwe_cscm.hpp"
#include <cassert>
#include <iostream>

using namespace rlwe_cscm;

int main() {
    Parameters params{8, 17, 16};
    RLWECSCM fhe(params, 12345);
    auto keys = fhe.keygen();

    for (int i = 0; i < 200; ++i) {
        auto m1 = fhe.random_message();
        auto m2 = fhe.random_message();
        auto c1 = fhe.encrypt(keys.pk, m1);
        auto c2 = fhe.encrypt(keys.pk, m2);

        auto dec1 = fhe.decrypt(keys.sk, c1);
        assert(dec1.equals_mod(m1, params.p));

        auto c_add = fhe.add(c1, c2);
        auto dec_add = fhe.decrypt(keys.sk, c_add);
        assert(dec_add.equals_mod(m1.add(m2), params.p));

        auto c_mul3 = fhe.multiply_raw(c1, c2);
        auto dec_mul = fhe.decrypt_mul_raw(keys.sk, c_mul3);
        assert(dec_mul.equals_mod(m1.mul(m2), params.p));

        auto rk = fhe.make_relin_key(keys.sk);
        auto c_relin = fhe.relinearize(rk, c_mul3);
        auto dec_relin = fhe.decrypt_with_relin_secret(rk, c_relin);
        assert(dec_relin.equals_mod(m1.mul(m2), params.p));
    }

    std::cout << "All RLWE-CSCM correctness tests passed.\n";
    return 0;
}
