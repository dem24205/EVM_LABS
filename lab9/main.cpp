#include <algorithm>
#include <chrono>
#include <fstream>
#include <random>

//1 KB
constexpr int min_N = 256;
//32 MB
constexpr int max_N = 8388608;

void fill_forward(int* array, const size_t size) {
    for (size_t i = 0; i < size - 1; ++i) {
        array[i] = i + 1;
    }
    array[size - 1] = 0;
}

void fill_backward(int* array, const size_t size) {
    for (size_t i = size - 1; i > 0; --i) {
        array[i] = i - 1;
    }
    array[0] = size - 1;
}

void fill_random(int* array, size_t size) {
    int* temp = new int[size];
    for (size_t i = 0; i < size - 1; ++i) {
        temp[i] = i + 1;
    }
    temp[size - 1] = 0;
    std::mt19937 g(time(nullptr));
    std::shuffle(temp, temp + size, g);
    for (size_t i = 0; i < size - 1; ++i) {
        array[temp[i]] = temp[i + 1];
    }
    array[temp[size - 1]] = temp[0];
    delete[](temp);
}

void warm_up(const int* array, size_t size) {
    volatile size_t k = 0;
    for (size_t i = 0; i < size; ++i) {
        k = array[k];
    }
}

uint64_t getCpuTicks() {
    unsigned int lo, hi;
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
    return (static_cast<unsigned long long>(hi) << 32) | lo;
}

uint64_t measure(const int* array, const size_t size) {
    volatile size_t k = 0;
    size_t i;
    const uint64_t start = getCpuTicks();
    for (k = 0, i = 0; i < size; ++i) {
        k = array[k];
    }
    const uint64_t end = getCpuTicks();
    return (end - start) / size;
}

uint64_t measure_min_ticks(const int* array, const size_t size) {
    uint64_t min_ticks = UINT64_MAX;
    for (size_t i = 0; i < 7; ++i) {
        uint64_t ticks = measure(array, size);
        min_ticks = min_ticks > ticks ? ticks : min_ticks;
    }
    return min_ticks;
}

int main() {
    std::ofstream out;
    out.open("out.csv");
    out << "N,Forward,Backward,Random\n";
    const auto array = new int[max_N];
    size_t inc = 1;
    size_t border = 1;
    for (size_t n = min_N; n < max_N; n += inc) {
        out << (n * 4) / 1024;
        
        //Forward
        fill_forward(array, n);
        warm_up(array, n);
        out << "," << measure_min_ticks(array, n);
        
        //Backward
        fill_backward(array, n);
        warm_up(array, n);
        out << "," << measure_min_ticks(array, n);
        
        //Random
        fill_random(array, n);
        warm_up(array, n);
        out << "," << measure_min_ticks(array, n);
        
        out << "\n";
        if ((n * 4) / 1024 >= border) {
            inc <<= 1;
            border <<= 1;
        }
    }
    delete[](array);
    out.close();
    return 0;
}
