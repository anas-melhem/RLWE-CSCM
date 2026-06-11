# RLWE-CSCM C++ Prototype

A compact C++ research prototype of **RLWE-CSCM**: an RLWE-based fully homomorphic encryption construction supporting encryption, decryption, homomorphic addition, raw homomorphic multiplication, and experimental ciphertext re-linearization.

> **Status:** research prototype. This repository is intended for experimentation, teaching, and reproducibility. It is **not production-ready** and must not be used to protect real data.

## Background

RLWE-CSCM was proposed as a fully homomorphic encryption scheme over the ring

```text
R_q = Z_q[x] / (x^d + 1)
```

with modulus relation:

```text
q = alpha * p
```

The construction encrypts a message `m in R_p` as a two-component ciphertext:

```text
c = (c1, c2) = (pk1 * r + m, pk2 * r) in R_q^2
```

where the public key is:

```text
pk = (pk1, pk2) = (A*s + p*e, -A)
```

and the secret key is:

```text
sk = (1, s)
```

Decryption is performed by:

```text
m = [ <c, sk> mod q ] mod p
```

Because `q` is an integer multiple of `p`, the `p*e*r` term disappears after reduction modulo `p`.

## Implemented Features

- Polynomial arithmetic in `Z_q[x] / (x^d + 1)`
- Toy RLWE-CSCM parameter setup
- Key generation
- Encryption
- Decryption
- Homomorphic addition
- Homomorphic scalar multiplication
- Raw homomorphic multiplication producing a 3-component ciphertext
- Decryption of raw multiplication using `(1, s, s^2)`
- Experimental re-linearization back to a 2-component ciphertext under a fresh key
- Correctness tests
- Simple benchmarking utility

## Repository Structure

```text
RLWE-CSCM-cpp/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── CITATION.cff
├── include/
│   └── rlwe_cscm/
│       ├── polynomial.hpp
│       └── rlwe_cscm.hpp
├── src/
│   ├── polynomial.cpp
│   └── rlwe_cscm.cpp
├── examples/
│   └── basic_demo.cpp
├── tests/
│   └── test_correctness.cpp
├── benchmarks/
│   └── benchmark_ops.cpp
└── docs/
    ├── theory.md
    └── security.md
```

## Build

### Requirements

- C++17 compiler
- CMake 3.16+

No external cryptographic library is required for this first prototype.

### Compile

```bash
git clone https://github.com/anas-melhem/RLWE-CSCM-cpp.git
cd RLWE-CSCM-cpp
cmake -S . -B build
cmake --build build
```

## Run Example

```bash
./build/example_basic
```

Expected output includes plaintext messages, decrypted ciphertexts, homomorphic addition, raw homomorphic multiplication, and re-linearized multiplication.

## Run Tests

```bash
ctest --test-dir build --output-on-failure
```

The test suite checks:

- encryption/decryption correctness
- homomorphic addition correctness
- raw homomorphic multiplication correctness
- experimental re-linearization correctness

## Run Benchmarks

```bash
./build/benchmark_ops
```

Example output format:

```csv
operation,avg_time_ns
keygen,15420
encrypt,6210
decrypt,2910
hom_add,820
hom_mul_raw,7340
```

Actual numbers depend on compiler, CPU, parameters, and build settings.

## Minimal C++ Example

```cpp
#include "rlwe_cscm/rlwe_cscm.hpp"
#include <iostream>

using namespace rlwe_cscm;

int main() {
    Parameters params{8, 17, 16}; // d, p, alpha; q = alpha*p
    RLWECSCM fhe(params, 42);
    auto keys = fhe.keygen();

    auto m1 = fhe.message({3, 1});
    auto m2 = fhe.message({4, 2});

    auto c1 = fhe.encrypt(keys.pk, m1);
    auto c2 = fhe.encrypt(keys.pk, m2);

    auto c_add = fhe.add(c1, c2);
    auto c_mul = fhe.multiply_raw(c1, c2);

    std::cout << fhe.decrypt(keys.sk, c_add).str() << "\n";
    std::cout << fhe.decrypt_mul_raw(keys.sk, c_mul).str() << "\n";
}
```

## Important Security Notice

This implementation uses small toy parameters by default and simple coefficient sampling. It is designed to verify the algebraic behavior of RLWE-CSCM, not to provide real-world security.

Before any production or serious security use, the implementation would need:

- cryptographically secure randomness
- large parameter sets
- validated RLWE parameter selection
- constant-time implementation review
- side-channel analysis
- independent cryptanalysis
- comparison with standard FHE libraries such as OpenFHE, Microsoft SEAL, and TFHE

## Roadmap

- [ ] Add big-integer / multiprecision backend
- [ ] Add secure parameter profiles
- [ ] Add command-line interface
- [ ] Add serialization for keys and ciphertexts
- [ ] Add NTT-based polynomial multiplication
- [ ] Add OpenFHE / SEAL benchmark comparison
- [ ] Add privacy-preserving ML examples
- [ ] Add Dockerfile and GitHub Actions

## Citation

If this software helps your research, please cite the related RLWE-CSCM dissertation contribution and this repository.

```bibtex
@phdthesis{melhem2021rlwecscm,
  title={Analysis and Development of Ciphers Homomorphic on Addition and Multiplication},
  author={Melhem, Anas},
  school={Eastern Mediterranean University},
  year={2021}
}
```

## Author

**Dr. Anas Melhem**  
Assistant Professor, Computer Systems Engineering Department  
Palestine Technical University

Research areas: Fully Homomorphic Encryption, Post-Quantum Cryptography, Lattice-Based Cryptography, Privacy-Preserving Machine Learning, and Cryptanalysis.
