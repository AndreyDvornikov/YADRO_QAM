#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
BUILD_DIR="$SCRIPT_DIR/build"
QAM_SIM="$BUILD_DIR/qam_sim"
PLOTTER="$SCRIPT_DIR/plot_ber.py"

M=16
NUM_BITS=1000000
SEED=42
OUT_FILE="ber_results.txt"
PLOT_OUTPUT="ber_plot.png"

while [[ $# -gt 0 ]]; do
    case "$1" in
        --M) M="$2"; shift 2 ;;
        --num-bits) NUM_BITS="$2"; shift 2 ;;
        --seed) SEED="$2"; shift 2 ;;
        --out) OUT_FILE="$2"; shift 2 ;;
        --plot-output) PLOT_OUTPUT="$2"; shift 2 ;;
        *) echo "Неизвестный аргумент: $1" >&2; exit 1 ;;
    esac
done

if [ ! -f "$QAM_SIM" ]; then
    echo "Исполняемый файл $QAM_SIM не найден. Соберите проект: cd build && cmake .. && cmake --build ." >&2
    exit 1
fi

if [ ! -f "$PLOTTER" ]; then
    echo "Скрипт $PLOTTER не найден." >&2
    exit 2
fi

if ! command -v python3 &> /dev/null; then
    echo "Python3 не найден" >&2
    exit 3
fi

if ! python3 -c "import matplotlib" &> /dev/null; then
    echo "Matplotlib не установлен. Установите: pip install -r requirements.txt" >&2
    exit 4
fi

echo "Запуск симуляции QAM (M=$M, бит=$NUM_BITS, seed=$SEED)..."
"$QAM_SIM" --M "$M" --num-bits "$NUM_BITS" --seed "$SEED" --out "$OUT_FILE"
if [ $? -ne 0 ]; then
    echo "Ошибка при выполнении симуляции" >&2
    exit 5
fi

echo "Построение графика..."
python3 "$PLOTTER" --input "$OUT_FILE" --M "$M" --output "$PLOT_OUTPUT"
if [ $? -ne 0 ]; then
    echo "Ошибка при построении графика" >&2
    exit 6
fi

echo "Готово. Результаты: $OUT_FILE, график: $PLOT_OUTPUT"
