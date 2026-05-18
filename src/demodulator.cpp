#include "demodulator.h"
#include <cmath>
#include <algorithm>

Demodulator::Demodulator(int M,
                         const std::vector<std::complex<double>>& constellation,
                         const std::vector<std::vector<int>>& mapping)
{
    if (M != 4 && M != 16 && M != 64)
        throw std::invalid_argument("Демодулятор: M должен быть 4, 16 или 64");
    if (static_cast<int>(constellation.size()) != M ||
        static_cast<int>(mapping.size()) != M)
        throw std::invalid_argument("Демодулятор: constellation/mapping size must match M");

    m_ = M;
    k_ = static_cast<int>(std::log2(M));
    constellation_ = constellation;
    mapping_ = mapping;
}

std::vector<int> Demodulator::demodulate(const std::complex<double>& received) const
{
    auto it = std::min_element(constellation_.begin(), constellation_.end(),
        [&received](const std::complex<double>& a, const std::complex<double>& b) {
            return std::norm(received - a) < std::norm(received - b);
        });
    int idx = std::distance(constellation_.begin(), it);
    return mapping_[idx];
}