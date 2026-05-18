"""
Построение графика BER для симуляции M‑QAM в AWGN канале.
Сравнение экспериментальных данных с теоретической кривой.
"""

import numpy as np
import matplotlib.pyplot as plt
from scipy.special import erfc
import sys
from pathlib import Path


class TheoreticalBER:
    """Теоретическая оценка BER для квадратной QAM с кодом Грея в AWGN."""

    def __init__(self, M: int):
        """
        Args:
            M: порядок модуляции (4, 16, 64).
        """
        if M not in {4, 16, 64}:
            raise ValueError("M должен быть 4, 16 или 64")
        self.M = M
        self.k = int(np.log2(M))

    def _q_function(self, x: np.ndarray) -> np.ndarray:
        """Q-функция: Q(x) = 0.5 * erfc(x / sqrt(2))."""
        return 0.5 * erfc(x / np.sqrt(2))

    def compute(self, variance: np.ndarray) -> np.ndarray:
        """
        Рассчитать теоретический BER для заданных дисперсий шума.

        Args:
            variance: массив значений дисперсии комплексного шума (N0).

        Returns:
            Массив BER той же длины.
        """
        Es_N0 = 1.0 / variance

        if self.M == 4:
            return self._q_function(np.sqrt(2 * Es_N0))
        else:
            sqrt_M = np.sqrt(self.M)
            ser = (4.0 * (1.0 - 1.0 / sqrt_M) *
                   self._q_function(np.sqrt(3.0 * Es_N0 / (self.M - 1))))
            return ser / self.k


def load_simulation_data(file_path: str = "ber_results.txt"):
    """
    Загрузить экспериментальные результаты из текстового файла.

    Args:
        file_path: путь к файлу с колонками variance BER.

    Returns:
        Кортеж (variance, ber) – numpy массивы.
    """
    if not Path(file_path).exists():
        raise FileNotFoundError(
            f"Файл {file_path} не найден. Сначала запустите qam_sim."
        )
    data = np.loadtxt(file_path)
    if data.ndim != 2 or data.shape[1] < 2:
        raise ValueError("Файл должен содержать минимум два столбца")
    return data[:, 0], data[:, 1]


class BERPlotter:
    """Построение графика BER: эксперимент + теория."""

    def __init__(self, M: int):
        self.M = M
        self.theory = TheoreticalBER(M)

    def plot(self, var_sim: np.ndarray, ber_sim: np.ndarray,
             save_path: str = "ber_plot.png"):
        """
        Построить график и сохранить в файл.

        Args:
            var_sim: дисперсии из симуляции.
            ber_sim: BER из симуляции.
            save_path: путь для сохранения рисунка.
        """
        plt.figure(figsize=(8, 6))

        plt.semilogy(var_sim, ber_sim, 'bo-',
                     label=f'Моделирование ({self.M}-QAM)')

        var_theory = np.logspace(
            np.log10(np.min(var_sim)),
            np.log10(np.max(var_sim)),
            200
        )
        ber_theory = self.theory.compute(var_theory)
        plt.semilogy(var_theory, ber_theory, 'r-',
                     label=f'Теория ({self.M}-QAM)')

        plt.xlabel('Дисперсия шума')
        plt.ylabel('BER')
        plt.title(f'BER {self.M}-QAM в AWGN канале')
        plt.legend()
        plt.grid(True, which='both', linestyle='--', alpha=0.7)

        plt.savefig(save_path, dpi=150)
        plt.show()


def main():
    M = 16
    try:
        var_sim, ber_sim = load_simulation_data()
    except (FileNotFoundError, ValueError) as e:
        print(f"Ошибка загрузки данных: {e}")
        sys.exit(1)

    plotter = BERPlotter(M)
    plotter.plot(var_sim, ber_sim)


if __name__ == "__main__":
    main()