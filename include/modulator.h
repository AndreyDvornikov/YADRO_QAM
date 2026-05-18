#pragma once

#include <complex>
#include <vector>

/// Класс квадратурного модулятора M‑QAM (QPSK, 16‑QAM, 64‑QAM).
/// Генерирует созвездие с нормированной средней энергией и кодом Грея.
class Modulator {
public:
    /// Конструктор модулятора.
    /// @param M Порядок модуляции: 4 (QPSK), 16 или 64.
    /// @throws std::invalid_argument если M не 4, 16 или 64.
    explicit Modulator(int M);

    /// Преобразует блок битов в комплексный символ созвездия.
    /// @param bits Вектор битов длины k = log2(M).
    /// @return Комплексный символ (нормированное созвездие).
    /// @throws std::invalid_argument если bits.size() != k или биты не найдены.
    std::complex<double> modulate(const std::vector<int>& bits) const;

    /// Возвращает нормированное созвездие.
    /// @return Константная ссылка на вектор точек созвездия.
    const std::vector<std::complex<double>>& constellation() const;

    /// Возвращает таблицу битовых меток (код Грея).
    /// @return Константная ссылка на вектор векторов битов, где mapping_[i] соответствует constellation_[i].
    const std::vector<std::vector<int>>& bitMapping() const;

private:
    int m_;                                  /// Порядок модуляции (4, 16 или 64).
    int k_;                                  /// Число бит на символ (log2(M)).
    std::vector<std::complex<double>> constellation_; /// Точки созвездия (нормированы).
    std::vector<std::vector<int>> mapping_;           /// Битовые метки точек созвездия.

    /// Генерирует ненормированные точки созвездия в квадратной сетке.
    void generateConstellation();

    /// Нормирует созвездие так, чтобы средняя энергия символа стала равна 1.
    void normalizeConstellation();

    /// Заполняет mapping_ битовыми метками согласно коду Грея (BRGC).
    void applyGrayMapping();
};