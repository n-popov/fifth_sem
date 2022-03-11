#include <iostream>
#include <future>
#include <thread>
#include <sstream>
#include <utility>

class WordHolder {
public:
    explicit WordHolder(std::string word) : word(std::move(word)) {}
    void print_word() const {
        std::cout << word << std::endl;
    }

private:
    std::string word;
};

void print_word(const std::string& word) {
    std::cout << "The word is " << word << "!" << std::endl;
}

std::string return_word(const std::string& word) {
    std::cout << "Returning word " + word << std::endl;
    std::stringstream ss;
    ss << "The word is " << word << "!" << std::endl;
    return ss.str();
}

void write_word(const std::string& word, std::string& output) {
    std::cout << "Writing!" << std::endl;
    output = word;
}

int main() {

    // Потоки

    std::thread first_thread(print_word, "FIRST"); // Запускаем поток
    std::thread second_thread(print_word, "SECOND");
    print_word("MAIN");

    first_thread.join(); // Ждём
    second_thread.join(); // Ждём


// Фьючерсы



    // std::launch::async -- немедленное выполнение
    std::future<std::string> first_future =
            std::async(std::launch::async, return_word, "FIRST");
    auto second_future = std::async(
            std::launch::async, return_word, "SECOND");
    std::cout << return_word("MAIN") << std::endl;
    std::cout << first_future.get() << std::endl;
        // .get() -- требуем рез-т немедленно
    std::cout << second_future.get() << std::endl;


// Аспект: ссылки

    std::string to_write = "WORD";
    std::string result;
    // std::ref, std::cref -- передача ссылок
    std::thread write_thread(write_word, std::cref(to_write), std::ref(result));
    write_thread.join();
    std::cout << result << std::endl;

// Аспект: передача метода

    WordHolder holder("HELD");
    std::thread class_thread(&WordHolder::print_word, &holder);
    class_thread.join();
}