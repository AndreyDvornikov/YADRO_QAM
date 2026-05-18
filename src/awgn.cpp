#include "awgn.h"
#include <cmath>

AwgnChannel::AwgnChannel(double noise_variance, unsigned int seed)
    : variance_(noise_variance),
      rng_(seed),
      noise_dist_(0.0, 1.0)
{}

std::complex<double> AwgnChannel::addNoise(const std::complex<double>& symbol) {
    double sigma = std::sqrt(variance_ / 2.0);
    double ni = noise_dist_(rng_) * sigma;
    double nq = noise_dist_(rng_) * sigma;
    return symbol + std::complex<double>(ni, nq);
}