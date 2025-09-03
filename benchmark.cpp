#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <vector>

#if defined(_WIN32)
#include <process.h> // For _spawnl
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

#define NUM_WORKERS 1000
#define COL_WIDTH 16

// Task to run
void do_task() {
    // Intentionally empty to measure overhead
}

// Benchmark threads
void benchmark_threads(const std::string& label, int num_threads) {
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> workers;
    for (int i = 0; i < num_threads; ++i) {
        workers.emplace_back(do_task);
    }

    for (auto& t : workers) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(end - start).count();
    std::cout << label << std::string(COL_WIDTH - label.size(), ' ') << "| "
              << duration << " seconds for " << num_threads << " threads\n";
}

// Benchmark processes
void benchmark_processes(const std::string& label, int num_processes,
                         const char* programName) {
    auto start = std::chrono::high_resolution_clock::now();

    #if defined(_WIN32)
        std::vector<int> num_processes;
        for (int i = 0; i < num_processes; ++i) {
            children.push_back(_spawnl(_P_WAIT, programName, programName, "task", nullptr));
        }
    #else
        std::vector<pid_t> pids;
        for (int i = 0; i < num_processes; ++i) {
            pid_t pid = fork();
            if (pid == 0) {
                do_task();
                _exit(0);
            } else if (pid > 0) {
                pids.push_back(pid);
            } else {
                std::cerr << "Fork failed\n";
                exit(1);
            }
        }
        for (pid_t pid : pids) {
            int status;
            waitpid(pid, &status, 0);
        }
    #endif

    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(end - start).count();
    std::cout << label << std::string(COL_WIDTH - label.size(), ' ') << "| "
              << duration << " seconds for " << num_processes << " processes\n";
}

// Benchmark child processes
void benchmark_child_processes(const std::string& label, int num_child_processes, 
                              const char* programName) {
    auto start = std::chrono::high_resolution_clock::now();

    #if defined(_WIN32)
        // On Windows: parent process spawns children using command-line arguments
        _spawnl(_P_WAIT, programName, programName, "parent",
                std::to_string(num_child_processes).c_str(), nullptr);
    #else
        pid_t parent_pid = fork();
        if (parent_pid == 0) {
            // Parent process spawns child processes
            std::vector<pid_t> children;
            for (int i = 0; i < num_child_processes; ++i) {
                pid_t pid = fork();
                if (pid == 0) {
                    do_task();
                    _exit(0);
                } else if (pid > 0) {
                    children.push_back(pid);
                } else {
                    std::cerr << "Fork failed\n";
                    _exit(1);
                }
            }
            // Wait for all children
            for (pid_t pid : children) {
                waitpid(pid, nullptr, 0);
            }
            _exit(0);
        } else if (parent_pid > 0) {
            waitpid(parent_pid, nullptr, 0);  // Wait for parent process
        } else {
            std::cerr << "Fork failed for parent process\n";
            exit(1);
        }
    #endif

    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(end - start).count();
    std::cout << label << std::string(COL_WIDTH - label.size(), ' ') << "| " << duration
              << " seconds for " << num_child_processes << " child processes (via parent process)\n";
}

// Main function
int main(int argc, char* argv[]) {
    // Only on Windows, a new instance of the same program is started to simulate a
    // process because it does not have fork() as Linux and macOS. 
    // _spawnl requires markers like "task" and "parent" (it could be anything else) 
    // passed as a command-line argument to the program to tell the program whether 
    // it is the main benchmark, a parent process, or a child process. 
    // Without this check, the new process would rerun the benchmark main code, 
    // which would recursively spawn more processes — an unintended behavior.
    #if defined(_WIN32)
        if (argc > 1) {
            std::string arg = argv[1];
            if (arg == "task") {
                do_task();
                return 0;
            }
            if (arg == "parent") {
                int child_count = std::stoi(argv[2]);
                for (int i = 0; i < child_count; ++i)
                    _spawnl(_P_WAIT, argv[0], argv[0], "task", nullptr);
                return 0;
            }
        }
    #endif

    std::cout << "Benchmarking creation and joining of threads vs processes:\n\n";

    benchmark_processes("Processes", NUM_WORKERS, argv[0]);
    benchmark_child_processes("Child Processes", NUM_WORKERS, argv[0]);
    benchmark_threads("Threads", NUM_WORKERS);

    return 0;
}