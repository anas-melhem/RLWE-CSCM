#pragma once

#include "rlwe_cscm/polynomial.hpp"
#include <random>
#include <vector>

namespace rlwe_cscm {

struct Parameters {
    std::size_t d{8};
    std::int64_t p{17};
    std::int64_t alpha{16};
    std::int64_t q() const { return p * alpha; }
};

struct PublicKey {
    Polynomial pk1;
    Polynomial pk2;
};

struct SecretKey {
    Polynomial one;
    Polynomial s;
};

struct Ciphertext {
    Polynomial c1;
    Polynomial c2;
};

struct Ciphertext3 {
    Polynomial c0;
    Polynomial c1;
    Polynomial c2;
};

struct KeyPair {
    PublicKey pk;
    SecretKey sk;
};

struct RelinKey {
    PublicKey rpk;
    SecretKey rsk;
    Ciphertext enc_s;
    Ciphertext enc_s2;
};

class RLWECSCM {
public:
    explicit RLWECSCM(Parameters params, std::uint64_t seed = std::random_device{}());

    const Parameters& params() const { return params_; }
    KeyPair keygen();
    Ciphertext encrypt(const PublicKey& pk, const Polynomial& m);
    Polynomial decrypt(const SecretKey& sk, const Ciphertext& c) const;

    Ciphertext add(const Ciphertext& a, const Ciphertext& b) const;
    Ciphertext scalar_mul(const Ciphertext& c, std::int64_t k) const;
    Ciphertext3 multiply_raw(const Ciphertext& a, const Ciphertext& b) const;
    Polynomial decrypt_mul_raw(const SecretKey& sk, const Ciphertext3& c) const;

    RelinKey make_relin_key(const SecretKey& sk);
    Ciphertext relinearize(const RelinKey& rk, const Ciphertext3& c) const;
    Polynomial decrypt_with_relin_secret(const RelinKey& rk, const Ciphertext& c) const;

    Polynomial message(std::initializer_list<std::int64_t> coeffs) const;
    Polynomial random_message();
    Polynomial random_poly_q(std::int64_t bound = -1);
    Polynomial random_small_poly(std::int64_t bound = 1);

private:
    Parameters params_;
    mutable std::mt19937_64 rng_;

    Polynomial zero_q() const;
    Polynomial one_q() const;
    Polynomial lift_to_q(const Polynomial& m) const;
};

} // namespace rlwe_cscm
