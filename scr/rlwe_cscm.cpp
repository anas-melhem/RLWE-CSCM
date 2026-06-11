#include "rlwe_cscm/rlwe_cscm.hpp"
#include <stdexcept>

namespace rlwe_cscm {

RLWECSCM::RLWECSCM(Parameters params, std::uint64_t seed)
    : params_(params), rng_(seed) {
    if (params_.d == 0 || params_.p <= 1 || params_.alpha <= 1) {
        throw std::invalid_argument("invalid parameters");
    }
}

Polynomial RLWECSCM::zero_q() const { return Polynomial(params_.d, params_.q()); }

Polynomial RLWECSCM::one_q() const {
    std::vector<std::int64_t> v(params_.d, 0);
    v[0] = 1;
    return Polynomial(v, params_.q());
}

Polynomial RLWECSCM::lift_to_q(const Polynomial& m) const {
    if (m.degree() != params_.d) throw std::invalid_argument("wrong message degree");
    return m.change_modulus(params_.q());
}

Polynomial RLWECSCM::message(std::initializer_list<std::int64_t> coeffs) const {
    std::vector<std::int64_t> v(params_.d, 0);
    std::size_t i = 0;
    for (auto c : coeffs) {
        if (i >= params_.d) break;
        v[i++] = c;
    }
    return Polynomial(v, params_.p);
}

Polynomial RLWECSCM::random_message() {
    std::uniform_int_distribution<std::int64_t> dist(0, params_.p - 1);
    std::vector<std::int64_t> v(params_.d);
    for (auto& x : v) x = dist(rng_);
    return Polynomial(v, params_.p);
}

Polynomial RLWECSCM::random_poly_q(std::int64_t bound) {
    std::vector<std::int64_t> v(params_.d);
    if (bound < 0) {
        std::uniform_int_distribution<std::int64_t> dist(0, params_.q() - 1);
        for (auto& x : v) x = dist(rng_);
    } else {
        std::uniform_int_distribution<std::int64_t> dist(-bound, bound);
        for (auto& x : v) x = dist(rng_);
    }
    return Polynomial(v, params_.q());
}

Polynomial RLWECSCM::random_small_poly(std::int64_t bound) {
    return random_poly_q(bound);
}

KeyPair RLWECSCM::keygen() {
    Polynomial A = random_poly_q();
    Polynomial s = random_small_poly(1);
    Polynomial e = random_small_poly(1);
    Polynomial pk1 = A.mul(s).add(e.scalar_mul(params_.p));
    Polynomial pk2 = A.scalar_mul(-1);
    return {PublicKey{pk1, pk2}, SecretKey{one_q(), s}};
}

Ciphertext RLWECSCM::encrypt(const PublicKey& pk, const Polynomial& m) {
    Polynomial r = random_small_poly(1);
    Polynomial mq = lift_to_q(m);
    return {pk.pk1.mul(r).add(mq), pk.pk2.mul(r)};
}

Polynomial RLWECSCM::decrypt(const SecretKey& sk, const Ciphertext& c) const {
    Polynomial inner = c.c1.mul(sk.one).add(c.c2.mul(sk.s));
    return inner.change_modulus(params_.p);
}

Ciphertext RLWECSCM::add(const Ciphertext& a, const Ciphertext& b) const {
    return {a.c1.add(b.c1), a.c2.add(b.c2)};
}

Ciphertext RLWECSCM::scalar_mul(const Ciphertext& c, std::int64_t k) const {
    return {c.c1.scalar_mul(k), c.c2.scalar_mul(k)};
}

Ciphertext3 RLWECSCM::multiply_raw(const Ciphertext& a, const Ciphertext& b) const {
    Polynomial m0 = a.c1.mul(b.c1);
    Polynomial m1 = a.c1.mul(b.c2).add(a.c2.mul(b.c1));
    Polynomial m2 = a.c2.mul(b.c2);
    return {m0, m1, m2};
}

Polynomial RLWECSCM::decrypt_mul_raw(const SecretKey& sk, const Ciphertext3& c) const {
    Polynomial s2 = sk.s.mul(sk.s);
    Polynomial inner = c.c0.mul(sk.one).add(c.c1.mul(sk.s)).add(c.c2.mul(s2));
    return inner.change_modulus(params_.p);
}

RelinKey RLWECSCM::make_relin_key(const SecretKey& sk) {
    // Experimental relinearization key. It encrypts s and s^2 under a fresh key rsk.
    KeyPair rk_pair = keygen();
    Polynomial s_msg = sk.s.change_modulus(params_.p);
    Polynomial s2_msg = sk.s.mul(sk.s).change_modulus(params_.p);
    Ciphertext enc_s = encrypt(rk_pair.pk, s_msg);
    Ciphertext enc_s2 = encrypt(rk_pair.pk, s2_msg);
    return {rk_pair.pk, rk_pair.sk, enc_s, enc_s2};
}

Ciphertext RLWECSCM::relinearize(const RelinKey& rk, const Ciphertext3& c) const {
    // Implements the algebraic shape of Eq. 7.54:
    // crs0 = cm0 + cm1*cs0 + cm2*cs20
    // crs1 = cm1*cs1 + cm2*cs21
    Polynomial crs0 = c.c0.add(c.c1.mul(rk.enc_s.c1)).add(c.c2.mul(rk.enc_s2.c1));
    Polynomial crs1 = c.c1.mul(rk.enc_s.c2).add(c.c2.mul(rk.enc_s2.c2));
    return {crs0, crs1};
}

Polynomial RLWECSCM::decrypt_with_relin_secret(const RelinKey& rk, const Ciphertext& c) const {
    return decrypt(rk.rsk, c);
}

} // namespace rlwe_cscm
