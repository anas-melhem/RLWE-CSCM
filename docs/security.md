# Security Notes

This repository is a research prototype and is not production-ready.

## Limitations

- Uses toy parameters by default.
- Uses `std::mt19937_64`, which is not cryptographically secure.
- Uses schoolbook polynomial multiplication.
- Does not implement constant-time arithmetic.
- Does not include parameter validation against known attacks.
- Does not include serialization or authenticated storage.

## Intended Use

- Teaching
- Research prototyping
- Algebraic verification
- Performance exploration
- Comparison experiments

## Not Intended For

- Production encryption
- Real privacy-preserving computation
- Deployment in cloud, IoT, or AI systems without major hardening
