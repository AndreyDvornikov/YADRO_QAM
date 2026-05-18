#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <algorithm>
#include "modulator.h"
#include "demodulator.h"
#include "awgn.h"

struct SimParams {
    int M;                          // порядок модуляции
    size_t num_bits;                // общее число бит
    unsigned int bit_seed;          // seed для генератора бит
    unsigned int channel_seed_base; // базовый seed для канала
};

struct SimResult {
    double variance;
    double ber;
};

std::vector<SimResult> run_simulation(const SimParams& params,
                                      const std::vector<double>& variances)
{
    Modulator modulator(params.M);
    Demodulator demodulator(params.M,
                           modulator.constellation(),
                           modulator.bitMapping());
    const auto k = static_cast<size_t>(std::log2(params.M));

    std::mt19937 bit_rng(params.bit_seed);
    std::uniform_int_distribution<int> bit_dist(0, 1);
    std::vector<int> tx_bits(params.num_bits);
    for (auto& b : tx_bits) b = bit_dist(bit_rng);

    std::vector<SimResult> results;
    results.reserve(variances.size());

    #pragma omp parallel for
    for (size_t idx = 0; idx < variances.size(); ++idx) {
        double var = variances[idx];
        unsigned int seed = params.channel_seed_base + static_cast<unsigned int>(idx);
        AwgnChannel channel(var, seed);
        size_t errors = 0;

        for (size_t i = 0; i + k <= params.num_bits; i += k) {
            std::vector<int> chunk(tx_bits.begin() + i, tx_bits.begin() + i + k);
            auto symbol = modulator.modulate(chunk);
            auto noisy = channel.addNoise(symbol);
            auto rx_bits = demodulator.demodulate(noisy);
            for (size_t j = 0; j < k; ++j)
                if (rx_bits[j] != chunk[j]) ++errors;
        }

        double ber = static_cast<double>(errors) / params.num_bits;
        #pragma omp critical
        results.push_back({var, ber});
    }
    std::sort(results.begin(), results.end(),
          [](const SimResult& a, const SimResult& b) {
              return a.variance < b.variance;
          });
    return results;
}

int main() {
    constexpr SimParams params {
        .M = 16,
        .num_bits = 1'000'000,
        .bit_seed = 42,
        .channel_seed_base = 43
    };
    const std::vector<double> variances = {0.05, 0.1, 0.2, 0.5, 1.0, 2.0};

    auto results = run_simulation(params, variances);

    std::ofstream out("ber_results.txt");
    if (!out) {
        std::cerr << "Ошибка открытия файла\n";
        return 1;
    }

    for (const auto& r : results) {
        out << r.variance << " " << r.ber << "\n";
        std::cout << "Variance: " << r.variance << " BER: " << r.ber << std::endl;
    }

    return 0;
}