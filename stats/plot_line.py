#!/usr/bin/env python3
import argparse
import csv
from pathlib import Path

import matplotlib.pyplot as plt


def parse_args():
    parser = argparse.ArgumentParser(description="Plot a line chart from a CSV file.")
    parser.add_argument("--csv", required=True, help="input CSV path")
    parser.add_argument("--x", required=True, help="CSV column name for x axis")
    parser.add_argument("--y", required=True, help="CSV column name for y axis")
    parser.add_argument("--xlabel", required=True, help="x axis label")
    parser.add_argument("--ylabel", required=True, help="y axis label")
    parser.add_argument("--unit", default="", help="unit displayed in point annotations, e.g. ms or x")
    parser.add_argument("--title", required=True, help="chart title")
    parser.add_argument("--output", required=True, help="output PNG path")
    parser.add_argument("--ideal", action="store_true", help="add ideal line y = x")
    return parser.parse_args()


def load_xy(csv_path, x_col, y_col):
    x_values = []
    y_values = []

    with csv_path.open(newline="") as f:
        reader = csv.DictReader(f)
        if reader.fieldnames is None:
            raise ValueError(f"empty CSV: {csv_path}")
        if x_col not in reader.fieldnames:
            raise ValueError(f"missing x column '{x_col}' in {csv_path}")
        if y_col not in reader.fieldnames:
            raise ValueError(f"missing y column '{y_col}' in {csv_path}")

        for row in reader:
            x_values.append(float(row[x_col]))
            y_values.append(float(row[y_col]))

    return x_values, y_values


def format_number(value):
    if abs(value - round(value)) < 1e-9:
        return str(int(round(value)))
    return f"{value:.3f}"


def format_annotation(value, unit):
    if unit:
        return f"{value:.2f}{unit}" if unit == "x" else f"{value:.1f} {unit}"
    return f"{value:.3f}"


def main():
    args = parse_args()
    csv_path = Path(args.csv)
    output_path = Path(args.output)
    output_path.parent.mkdir(parents=True, exist_ok=True)

    x_values, y_values = load_xy(csv_path, args.x, args.y)

    plt.figure(figsize=(8, 5))
    plt.plot(x_values, y_values, marker="o", linewidth=2, label=args.ylabel)

    if args.ideal:
        plt.plot(x_values, x_values, linestyle="--", alpha=0.6, label="Ideal")

    for x, y in zip(x_values, y_values):
        plt.annotate(
            format_annotation(y, args.unit),
            (x, y),
            textcoords="offset points",
            xytext=(0, 8),
            ha="center",
        )

    plt.xlabel(args.xlabel)
    plt.ylabel(f"{args.ylabel} ({args.unit})" if args.unit else args.ylabel)
    plt.title(args.title)
    plt.xticks(x_values, [format_number(x) for x in x_values])
    plt.grid(True, linestyle="--", alpha=0.5)
    if args.ideal:
        plt.legend()
    plt.tight_layout()
    plt.savefig(output_path, dpi=300)
    print(f"saved: {output_path}")


if __name__ == "__main__":
    main()
