#include <windows.h>
#include <thread>
#include <atomic>
#include <chrono>
#include <cstring>
#include <fstream>

constexpr size_t ARRAY_SIZE = 256;
constexpr size_t ITERATIONS = 100000000;
constexpr size_t WRITER_POS = 128;

struct alignas(64) ThreadData {
    char* array;
    size_t read_pos;
    std::atomic<bool> start{false};
    std::atomic<bool> stop{false};
    double time_ms;
};

void writer_func(const ThreadData* data) {
    // Привязка к ядру 1 (Windows API)
    DWORD_PTR mask = 1ULL << 1;
    SetThreadAffinityMask(GetCurrentThread(), mask);

    while (!data->start.load()) {}

    volatile char* ptr = &data->array[WRITER_POS];
    while (!data->stop.load()) {
        *ptr = 1;
    }
}

void reader_func(ThreadData* data) {
    // Привязка к ядру 0 (Windows API)
    DWORD_PTR mask = 1ULL << 0;
    SetThreadAffinityMask(GetCurrentThread(), mask);

    data->start.store(true);

    auto start = std::chrono::steady_clock::now();
    volatile char dummy = 0;
    for (size_t i = 0; i < ITERATIONS; i++) {
        dummy += data->array[data->read_pos];
    }
    const auto end = std::chrono::steady_clock::now();
    data->time_ms = std::chrono::duration<double, std::milli>(end - start).count();

    data->stop.store(true);
}

int main() {
    std::ofstream csv_file("results.csv");
    csv_file << "position,time_ms" << std::endl;

    const auto array = new char[ARRAY_SIZE];
    std::memset(array, 0, ARRAY_SIZE);

    for (size_t pos = 0; pos < ARRAY_SIZE; pos++) {
        auto data = new ThreadData();
        data->array = array;
        data->read_pos = pos;

        std::thread writer(writer_func, data);
        std::thread reader(reader_func, data);
        reader.join();
        writer.join();
        csv_file << pos << "," << data->time_ms << std::endl;
        delete data;
    }

    delete[] array;
    csv_file.close();
    return 0;
}
