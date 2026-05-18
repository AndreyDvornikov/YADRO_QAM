#pragma once

#include <complex>
#include <vector>

/// Демодулятор M‑QAM по минимуму евклидова расстояния.
class Demodulator {
public:
    /// Конструктор.
    /// @param M Порядок модуляции (4, 16, 64).
    /// @param constellation Нормированное созвездие из Modulator.
    /// @param mapping Битовые метки точек из Modulator.
    /// @throws std::invalid_argument если размеры не совпадают с M.
    Demodulator(int M,
                const std::vector<std::complex<double>>& constellation,
                const std::vector<std::vector<int>>& mapping);

    /// Демодулирует зашумлённый символ в биты.
    /// @param received Принятый комплексный символ.
    /// @return Вектор битов длины log2(M).
    std::vector<int> demodulate(const std::complex<double>& received) const;

private:
    int m_;
    int k_;
    std::vector<std::complex<double>> constellation_; /// Копия созвездия.
    std::vector<std::vector<int>> mapping_;           /// Копия битовых меток.
};