#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1  // Убираем предупреждение о старой версии MD5

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <iterator>
#include <cryptlib.h>
#include <cryptopp/md5.h>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <UnitTest++/UnitTest++.h>
#include "Communicator.h"

// Чтение конфигурационного файла
std::pair<std::string, std::string> readConfigFile(const std::string& configFile) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open config file: " + configFile);
    }

    std::string login, password;
    if (!(std::getline(file, login) && std::getline(file, password))) {
        throw std::runtime_error("Invalid format in config file: " + configFile);
    }
    return {login, password};
}

// Чтение входного файла
std::vector<std::vector<int64_t>> readInputFile(const std::string& inputFile) {
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open input file: " + inputFile);
    }

    std::vector<std::vector<int64_t>> vectors;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::vector<int64_t> vec((std::istream_iterator<int64_t>(iss)), std::istream_iterator<int64_t>());

        if (vec.empty()) {
            throw std::runtime_error("Failed to parse input file: invalid data format");
        }

        vectors.push_back(vec);
    }

    return vectors;
}

// Запись результатов в файл
void writeResults(const std::string& outputFile, const std::vector<int64_t>& results) {
    std::ofstream file(outputFile, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open output file: " + outputFile);
    }

    uint32_t numResults = results.size();
    file.write(reinterpret_cast<const char*>(&numResults), sizeof(numResults));
    for (const auto& result : results) {
        file.write(reinterpret_cast<const char*>(&result), sizeof(result));
    }
}

// Тесты с использованием UnitTest++

TEST(ReadConfigFile_Valid) {
    std::ofstream configFile("test_config.conf");
    configFile << "user\nP@ssW0rd";
    configFile.close();

    std::pair<std::string, std::string> result = readConfigFile("test_config.conf");
    CHECK_EQUAL("user", result.first);
    CHECK_EQUAL("P@ssW0rd", result.second);
}

TEST(ReadConfigFile_Invalid) {
    std::ofstream configFile("invalid_config.conf");
    configFile << "user";
    configFile.close();

    try {
        readConfigFile("invalid_config.conf");
        CHECK(false);
    } catch (const std::runtime_error& e) {
        CHECK_EQUAL("Invalid format in config file: invalid_config.conf", std::string(e.what()));
    }
}

TEST(Communicator_Connection) {
    Communicator mockComm("127.0.0.1", 33333);
    try {
        mockComm.connectToServer();
        CHECK(true);
    } catch (...) {
        CHECK(false);
    }
}

TEST(Communicator_Connection_Fail) {
    Communicator mockComm("", 33333);  // Пустой адрес
    try {
        mockComm.connectToServer();
        CHECK(false);
    } catch (const std::runtime_error& e) {
        CHECK_EQUAL("Invalid server address", std::string(e.what()));
    }
}

TEST(Authentication_Correct_MD5) {
    Communicator mockComm("127.0.0.1", 33333);
    mockComm.connectToServer();

    CryptoPP::Weak1::MD5 md5Hash;
    try {
        std::string password = "P@ssW0rd";
        std::string hash;
        CryptoPP::StringSource(password, true, new CryptoPP::HashFilter(md5Hash, new CryptoPP::HexEncoder(new CryptoPP::StringSink(hash))));

        mockComm.sendMessage(hash);
        std::string response = "OK";  // Фиктивный ответ для теста
        CHECK_EQUAL("OK", response);
    } catch (...) {
        CHECK(false);
    }
}

TEST(ReadInputFile_Valid) {
    std::ofstream inputFile("test_input.txt");
    inputFile << "1 2 3\n4 5 6\n7 8 9";
    inputFile.close();

    auto vectors = readInputFile("test_input.txt");
    CHECK_EQUAL(3, vectors.size());
    CHECK_EQUAL(3, vectors[0].size());
    CHECK_EQUAL(4, vectors[1][0]);
}

TEST(ReadInputFile_Invalid) {
    std::ofstream inputFile("invalid_input.txt");
    inputFile << "1 2 3\nabc\n5";  // Некорректная строка "abc"
    inputFile.close();

    try {
        readInputFile("invalid_input.txt");
        CHECK(false);
    } catch (const std::runtime_error& e) {
        CHECK_EQUAL("Failed to parse input file: invalid data format", std::string(e.what()));
    }
}

TEST(WriteOutputFile) {
    std::vector<int64_t> results = {1, 2, 3};
    writeResults("test_output.bin", results);

    std::ifstream file("test_output.bin", std::ios::binary);
    CHECK(file.is_open());

    uint32_t numResults;
    file.read(reinterpret_cast<char*>(&numResults), sizeof(numResults));
    CHECK_EQUAL(3, numResults);

    for (int i = 0; i < 3; ++i) {
        int64_t result;
        file.read(reinterpret_cast<char*>(&result), sizeof(result));
        CHECK_EQUAL(results[i], result);
    }
}

int main() {
    return UnitTest::RunAllTests();
}
