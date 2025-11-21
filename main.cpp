#include <algorithm>
#include <cstdint>
#include <fstream>

uint64_t getCpuTicks() {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return (static_cast<uint64_t>(hi) << 32) | lo;
}

int* getArray(const size_t n, const size_t size, const size_t offset) {
    const size_t arraySize = offset * n;
    int* arr = new int[arraySize];
    for (size_t i = 0; i < size / n; ++i) {
        for (size_t j = 0; j < n - 1; ++j) {
            arr[i + j * offset] = static_cast<int>(i + j * offset + offset);
        }
        arr[i + (n - 1) * offset] = static_cast<int>(i + 1);
    }
    arr[size / n - 1 + (n - 1) * offset] = 0;
    return arr;
}

uint64_t measure(const int* array, const size_t size) {
    volatile size_t k;
    const uint64_t start = getCpuTicks();
    for (size_t i = 0, k = 0; i < size; i++) {
        k = array[k];
    }
    const uint64_t end = getCpuTicks();
    return (end - start) / size;
}

uint64_t findMinTicks(const int* arr, const size_t size, const size_t iterations) {
    uint64_t minTicks = UINT64_MAX;
    for (size_t i = 0; i < iterations; ++i) {
        uint64_t ticks = measure(arr, size);
        minTicks = std::min(minTicks, ticks);
    }
    return minTicks;
}

int main() {
    // 32KB L1d.
    constexpr size_t size = (32 * 1024) / sizeof(size_t);
    // 16MB Offset.
    constexpr size_t offset = (16 * 1024 * 1024) / sizeof(size_t);
    constexpr size_t N = 32;
    std::ofstream out("lab9.txt");
    for (size_t n = 1; n < N + 1; ++n) {
        constexpr size_t iterations = 1024;
        const int* arr = getArray(n, size, offset);
        const uint64_t minTicks = findMinTicks(arr, size, iterations);
        out << n << ' ' << minTicks << '\n';
        delete[] arr;
    }
    out.close();
    return 0;
}
