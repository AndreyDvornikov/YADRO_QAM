#pragma once

#include <complex>
#include <vector>

/// Демодулятор квадратной M‑QAM по пороговым решениям для I и Q.
class Demodulator {
public:
    /// Конструктор.
    /// @param M Порядок модуляции (4, 16, 64).
    /// @param constellation Нормированное созвездие (любая точка, чтобы определить масштаб).
    /// @param mapping Не используется, оставлен для совместимости.
    /// @throws std::invalid_argument если M не 4, 16 или 64.
    Demodulator(int M,
                const std::vector<std::complex<double>>& constellation,
                const std::vector<std::vector<int>>& mapping);

    /// Демодулирует зашумлённый символ в биты (пороговый метод).
    /// @param received Принятый комплексный символ.
    /// @return Вектор битов длины log2(M).
    std::vector<int> demodulate(const std::complex<double>& received) const;

private:
    int m_;
    int k_;
    int side_;                // корень из M (2, 4 или 8)
    int bits_per_dim_;        // бит на одну координату
    double d_;                // расстояние между соседними уровнями (после нормировки)
    std::vector<double> thresholds_; // границы между уровнями для I и Q (size = side_-1)
    std::vector<std::vector<int>> level_bits_; // код Грея для каждого уровня (0..side_-1)

    /// Вычисляет индекс уровня по вещественной координате.
    int find_level(double value) const;
};