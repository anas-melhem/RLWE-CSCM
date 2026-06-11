# RLWE-CSCM Theory Notes

RLWE-CSCM works over the quotient ring:

```text
R_q = Z_q[x] / (x^d + 1)
```

The prototype implements the following operations.

## Parameters

```text
q = alpha * p
```

where `p` is the plaintext modulus and `q` is the ciphertext modulus.

## Key Generation

```text
pk = (A*s + p*e, -A)
sk = (1, s)
```

## Encryption

```text
c = (pk1*r + m, pk2*r)
```

## Decryption

```text
m = [c1 + c2*s mod q] mod p
```

Since:

```text
c1 + c2*s = (A*s + p*e)r + m - A*r*s = m + p*e*r
```

reduction modulo `p` recovers `m`.

## Homomorphic Addition

```text
Enc(m1) + Enc(m2) = Enc(m1 + m2)
```

## Homomorphic Multiplication

For two ciphertexts `c=(c1,c2)` and `d=(d1,d2)`, the raw product is:

```text
M(c,d) = (c1*d1, c1*d2 + c2*d1, c2*d2)
```

It decrypts using the expanded secret key:

```text
sk3 = (1, s, s^2)
```

## Re-linearization

The experimental re-linearization code follows the algebraic shape:

```text
crs0 = cm0 + cm1*cs0 + cm2*cs20
crs1 = cm1*cs1 + cm2*cs21
```

where `cs` and `cs2` are encryptions of `s` and `s^2` under a fresh key.
