# Benchmarking Processes and Threads in Python

[![PyPI - Python Version](https://img.shields.io/pypi/pyversions/3)](https://www.python.org)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
![Build](https://img.shields.io/badge/build-manual-lightgrey)

This benchmark measures the overhead of creating processes, child processes, and threads in Python. The goal is to demonstrate that creating threads is far more efficient than creating processes.

This project is part of the **Concurrent Programming** module at the [Federal University of Rio Grande do Norte (UFRN)](https://www.ufrn.br), Natal, Brazil.

## 📃 Description

This benchmark measures the time to create 1,000 processes, child processes, and threads. This number is currently fixed and hard-coded, but it can be adjusted and even provided as a user input to the program. All the working units execute the very same task, which does almost nothing to allow for focusing on the overhead. For the case of child processes, a parent process is created, and then it creates multiple child processes.

The benchmark currently runs for only one. Future work involves modifying the implementation to support multiple runs (at least 20) for empirical validity, and recording the mean and standard deviation.

---

## 📂 Repository Structure

```
.
├── src/                # Source code
│   ├── benchmark.py    # Benchmark implementation
└── README.md
```

---

## 🚀 Getting Started

### ✅ Prerequisites

- A compiler with support to C++11 or higher
- A terminal or IDE

### 🔧 Compilation

Inside the project root, compile all sources from the [Makefile](Makefile):

```bash
makefile
```

This will create the compiled object files inside the `build/` directory and the executable `benchmark` inside the `bin` directory.

### ▶️ Running

```bash
./bin/benchmark
```

Expected output:

```
Processes       | 0.0817888 seconds for 1000 processes
Child Processes | 0.0662751 seconds for 1000 child processes (via parent process)
Threads         | 0.0133788 seconds for 1000 threads
```

The expected result is that creating multiple threads is far more efficient than creating processes.

---

## 🤝 Contributing

Contributions are welcome! Fork this repository and submit a pull request 🚀

---

## 📜 License

This project is licensed under the [MIT License](LICENSE).
