#include "demodulator.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

Demodulator::Demodulator(int M,
                         const std::vector<std::complex<double>>& constellation,
                         const std::vector<std::vector<int>>&)
{
    if (M != 4 && M != 16 && M != 64)
        throw std::invalid_argument("M должно быть 4, 16 или 64");

    m_ = M;
    k_ = static_cast<int>(std::log2(M));
    side_ = static_cast<int>(std::sqrt(M));
    bits_per_dim_ = k_ / 2;

    double max_abs = 0.0;
    for (const auto& pt : constellation) {
        max_abs = std::max(max_abs, std::abs(pt.real()));
        max_abs = std::max(max_abs, std::abs(pt.imag()));
    }
    d_ = max_abs / (side_ - 1);

    thresholds_.reserve(side_ - 1);
    for (int i = 1; i < side_; ++i) {
        double boundary = d_ * (-side_ + 2 * i);
        thresholds_.push_back(boundary);
    }

    level_bits_.resize(side_);
    for (int idx = 0; idx < side_; ++idx) {
        int gray = idx ^ (idx >> 1);
        std::vector<int> bits(bits_per_dim_);
        for (int b = 0; b < bits_per_dim_; ++b)
            bits[bits_per_dim_ - 1 - b] = (gray >> b) & 1;
        level_bits_[idx] = bits;
    }
}

int Demodulator::find_level(double value) const {
    int level = 0;
    while (level < side_ - 1 && value >= thresholds_[level])
        ++level;
    return level;
}

std::vector<int> Demodulator::demodulate(const std::complex<double>& received) const {
    int i_idx = find_level(received.real());
    int q_idx = find_level(received.imag());

    std::vector<int> bits = level_bits_[i_idx];
    const auto& q_bits = level_bits_[q_idx];
    bits.insert(bits.end(), q_bits.begin(), q_bits.end());
    return bits;
}