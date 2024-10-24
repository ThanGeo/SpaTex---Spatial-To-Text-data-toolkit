#ifndef UTILS_H
#define UTILS_H

#include <dirent.h>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

#include "def.h"

/** @brief Logging methods for the system. Logs are printed in terminal. */
namespace logger
{
    /** @brief Base case of the variadic template recursion. */
    static inline void print_args() {
        std::cout << std::endl;
        std::cout.flush();
    }

    /** @brief Recursive template function. */
    template<typename T, typename... Args>
    static inline void print_args(T first, Args... rest) {
        std::cout << first << " ";
        std::cout.flush();
        print_args(rest...);
    }

    /** @brief Error logging function with template arguments. Separates input parameters with spaces. 
     * Prints the node's rank and process type (Agent or Controller), coloured appropriately. */
    template<typename T, typename... Args>
    inline void log_error(int errorCode, T first, Args... rest) {
        std::cout << RED "[ERROR: " + std::to_string(errorCode) + "]" NC ": ";
        std::cout.flush();
        print_args(first, rest...);
    }

    /** @brief Warning logging function with template arguments. Separates input parameters with spaces. 
     * Prints the node's rank and process type (Agent or Controller), coloured appropriately. */
    template<typename T, typename... Args>
    inline void log_warning(T first, Args... rest) {
        std::cout << ORANGE "[WARNING]" NC ": ";
        std::cout.flush();
        print_args(first, rest...);
    }

    /** @brief Success logging function with template arguments. Separates input parameters with spaces. 
     * Prints the node's rank and process type (Agent or Controller), coloured appropriately. */
    template<typename T, typename... Args>
    inline void log_success(T first, Args... rest) {
        std::cout << GREEN "[SUCCESS]" NC ": ";
        std::cout.flush();
        print_args(first, rest...);
    }

    /** @brief Task logging function with template arguments. Separates input parameters with spaces. 
     * Prints the node's rank and process type (Agent or Controller), coloured appropriately. */
    template<typename T, typename... Args>
    inline void log_task(T first, Args... rest) {
        std::cout << "[.......]: ";
        print_args(first, rest...);
    }
}

namespace mapping
{
    std::string dataTypeIntToStr(DataType val);

    DataType dataTypeTextToInt(std::string str);

    FileFormat fileFormatTextToInt(std::string str);

    std::string cardinalDirectionIntToString(CardinalDirection val);

    std::string relationIntToStr(TopologyRelation relation);
}

/**
 * @brief Returns the extension (.suffix) of a string that represents a filepath.
 * 
 */
std::string getFileExtension(const std::string& filePath);

bool verifyFilepath(std::string filePath);

bool verifyDirectory(std::string directoryPath);

double convertDegreesToSquareKilometers(double areaInDegrees, double lat);

namespace text_generator
{   
    /** @brief Generates text based on the given cardinal direction and two entities. 
     * Semantics: entityNameR is 'direction' of entityNameS */
    std::string generateDirectionalRelation(std::string &entityNameR, std::string &entityNameS, CardinalDirection direction);

    /** @brief Generates text based on the given topological relation and two entities. 
     * Semantics: entityNameR 'relation text' entityNameS */
    std::string generateTopologicalRelation(std::string &entityNameR, std::string &entityNameS, TopologyRelation relation);


    std::string generateAreaInSqkm(std::string &entityNameR, std::string &entityNameS, double area);
}

#endif