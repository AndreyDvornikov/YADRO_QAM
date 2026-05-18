#include "modulator.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>

Modulator::Modulator(int M) {
    if (M != 4 && M != 16 && M != 64) {
        throw std::invalid_argument("M должен быть 4, 16, или 64");
    }
    m_ = M;
    k_ = static_cast<int>(std::log2(M));
    
    generateConstellation();
    normalizeConstellation();
    applyGrayMapping();
}

std::complex<double> Modulator::modulate(const std::vector<int>& bits) const {
    if (static_cast<int>(bits.size()) != k_) {
        throw std::invalid_argument("bits.size() должен быть равен k");
    }
    auto it = std::find(mapping_.begin(), mapping_.end(), bits);
    if (it == mapping_.end()) {
        throw std::invalid_argument("биты не найдены");
    }
    int idx = std::distance(mapping_.begin(), it);
    return constellation_[idx];
}

const std::vector<std::complex<double>>& Modulator::constellation() const {
    return constellation_;
}

const std::vector<std::vector<int>>& Modulator::bitMapping() const {
    return mapping_;
}

void Modulator::generateConstellation() {
    int side = static_cast<int>(std::sqrt(m_));
    constellation_.clear();
    constellation_.reserve(m_);
    for(int i = -(side-1); i <= side-1; i+=2){
        for(int j = -(side-1); j <= side-1; j+=2){
            constellation_.emplace_back(i, j);
        }
    }
}

void Modulator::normalizeConstellation() {
    double energy_sum = 0.0;
    for(auto c : constellation_) {
        energy_sum += std::norm(c);
    }
    double avg_power = energy_sum/m_;
    double scale = 1.0 / std::sqrt(avg_power); 
    for(auto& c : constellation_) {
        c *= scale;
    }
}

void Modulator::applyGrayMapping() {
    mapping_.clear();
    mapping_.reserve(m_);
    int side = std::sqrt(m_);
    int bits_per_dim = static_cast<int>(std::log2(side));
    auto gray_bits = [bits_per_dim](int idx) {
        int g = idx ^ (idx >> 1);
        std::vector<int> bits(bits_per_dim);
        for (int b = bits_per_dim - 1; b >= 0; --b) {
            bits[b] = (g >> (bits_per_dim - 1 - b)) & 1;
        }
        return bits;
    };
    for (int i_idx = 0; i_idx < side; ++i_idx) {
        auto i_bits = gray_bits(i_idx);
        for (int q_idx = 0; q_idx < side; ++q_idx) {
            auto q_bits = gray_bits(q_idx);
            auto bits = i_bits;
            bits.insert(bits.end(), q_bits.begin(), q_bits.end());
            mapping_.push_back(bits);
        }
    }
}