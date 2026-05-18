#pragma once

#include <complex>
#include <random>

/// Модель канала с аддитивным белым гауссовским шумом (AWGN).
class AwgnChannel {
public:
    /// Конструктор канала.
    /// @param noise_variance Полная дисперсия комплексного шума (E[|n|²]).
    /// @param seed Начальное состояние генератора (по умолчанию std::random_device{}).
    explicit AwgnChannel(double noise_variance, unsigned int seed = std::random_device{}());

    /// Добавляет комплексный гауссовский шум к символу.
    /// @param symbol Комплексный символ на входе канала.
    /// @return Зашумлённый комплексный символ.
    std::complex<double> addNoise(const std::complex<double>& symbol);

private:
    double variance_;                                 /// Полная дисперсия комплексного шума.
    std::mt19937 rng_;                                /// Генератор псевдослучайных чисел.
    std::normal_distribution<double> noise_dist_;     /// Стандартное нормальное распределение N(0,1).
};