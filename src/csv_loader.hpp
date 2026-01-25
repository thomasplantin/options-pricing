#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "option.hpp"

/**
 * CSV loader for options data
 * Expected format: symbol,S,K,r,sigma,T,isCall
 */
class CSVLoader {
public:
    /**
     * Load options from CSV file
     * @param filename Path to CSV file
     * @return Vector of Option structs
     * @throws std::runtime_error if file cannot be opened or data is invalid
     */
    static std::vector<Option> load(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        
        std::vector<Option> options;
        std::string line;
        
        // Skip header line
        std::getline(file, line);
        
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            Option opt = parse_option(line);
            validate(opt);
            options.push_back(opt);
        }
        
        return options;
    }
    
private:
    static Option parse_option(const std::string& line) {
        std::stringstream ss(line);
        std::string token;
        Option opt;
        
        std::getline(ss, opt.symbol, ',');
        std::getline(ss, token, ','); opt.S = std::stod(token);
        std::getline(ss, token, ','); opt.K = std::stod(token);
        std::getline(ss, token, ','); opt.r = std::stod(token);
        std::getline(ss, token, ','); opt.sigma = std::stod(token);
        std::getline(ss, token, ','); opt.T = std::stod(token);
        std::getline(ss, token, ','); opt.isCall = (std::stoi(token) == 1);
        
        return opt;
    }
    
    static void validate(const Option& opt) {
        if (opt.S <= 0.0) {
            throw std::runtime_error("Invalid spot price: " + opt.symbol);
        }
        if (opt.K <= 0.0) {
            throw std::runtime_error("Invalid strike price: " + opt.symbol);
        }
        if (opt.T <= 0.0) {
            throw std::runtime_error("Invalid time to maturity: " + opt.symbol);
        }
        if (opt.sigma <= 0.0) {
            throw std::runtime_error("Invalid volatility: " + opt.symbol);
        }
    }
};
