#include <thread>
#include <numeric>
#include <iostream>
#include <vector>

template <typename Iterator, typename T>
void accumulate_block(Iterator first, Iterator last, T init, T& result) {
    result = std::accumulate(first, last, init);
}

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
    // 1. Проверили длину
    auto length = std::distance(first, last);
    if (length < 32) {
        return std::accumulate(first, last, init);
    }
    // 2. Длина достаточна, распараллеливаем
    auto num_workers = std::thread::hardware_concurrency();
    // Вычислили длину для одного потока
    auto length_per_thread = (length + num_workers - 1) / num_workers;
    // Векторы с потоками и результатами
    std::vector<std::thread> threads;
    std::vector<T> results(num_workers - 1);
    // 3. Распределяем данные (концепция полуинтервалов!)
    for(auto i = 0u; i < num_workers - 1; i++) {
        auto beginning = std::next(first, i * length_per_thread);
        auto ending = std::next(first, (i + 1) * length_per_thread);
        // 4. Запускаем исполнителей
        threads.push_back(std::thread(
            accumulate_block<Iterator, T>,
            beginning, ending, 0, std::ref(results[i])));
    }
    // Остаток данных -- в родительском потоке
    auto main_result = std::accumulate(std::next(first, (num_workers - 1) * length_per_thread),
                                       last, init);
    // std::mem_fun_ref -- для оборачивания join().
    std::for_each(std::begin(threads), std::end(threads), std::mem_fun_ref(&std::thread::join));
    // 5. Собираем результаты
    return std::accumulate(std::begin(results), std::end(results), main_result);
}

int main() {
    std::vector<int> test_sequence(100u);
    std::iota(test_sequence.begin(), test_sequence.end(), 0);
    auto result =
            parallel_accumulate(std::begin(test_sequence),
                                std::end(test_sequence), 0);
    std::cout << "Result is " << result << std::endl;
}