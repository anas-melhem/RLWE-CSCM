#include "rlwe_cscm/rlwe_cscm.hpp"
#include <iostream>

using namespace rlwe_cscm;

int main() {
    Parameters params{8, 17, 16};  // d, p, alpha; q = alpha*p
    RLWECSCM fhe(params, 42);
    auto keys = fhe.keygen();

    auto m1 = fhe.message({3, 1});       // 3 + x in R_p
    auto m2 = fhe.message({4, 2});       // 4 + 2x in R_p

    auto c1 = fhe.encrypt(keys.pk, m1);
    auto c2 = fhe.encrypt(keys.pk, m2);

    auto d1 = fhe.decrypt(keys.sk, c1);
    auto d2 = fhe.decrypt(keys.sk, c2);

    auto c_add = fhe.add(c1, c2);
    auto m_add = fhe.decrypt(keys.sk, c_add);

    auto c_mul3 = fhe.multiply_raw(c1, c2);
    auto m_mul = fhe.decrypt_mul_raw(keys.sk, c_mul3);

    std::cout << "m1        = " << m1.str() << "\n";
    std::cout << "Dec(c1)   = " << d1.str() << "\n";
    std::cout << "m2        = " << m2.str() << "\n";
    std::cout << "Dec(c2)   = " << d2.str() << "\n";
    std::cout << "Dec(c1+c2)= " << m_add.str() << "\n";
    std::cout << "Dec(c1*c2)= " << m_mul.str() << "\n";

    auto expected_add = m1.add(m2);
    auto expected_mul = m1.mul(m2);
    std::cout << "Expected add = " << expected_add.str() << "\n";
    std::cout << "Expected mul = " << expected_mul.str() << "\n";

    auto rk = fhe.make_relin_key(keys.sk);
    auto c_relin = fhe.relinearize(rk, c_mul3);
    auto m_relin = fhe.decrypt_with_relin_secret(rk, c_relin);
    std::cout << "Dec(relinearized c1*c2) = " << m_relin.str() << "\n";

    return 0;
}
