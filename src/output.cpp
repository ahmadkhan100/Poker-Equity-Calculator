#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>  // For std::setw and std::setprecision
#include "PokerHand.h"  // Assuming this class has the necessary methods implemented

// Function to determine the color based on probability
int cellColor(const std::optional<double>& prob) {
    if (!prob.has_value()) {
        return 11; // Yellow
    }

    double p = prob.value();
    if (p >= 0.50) {
        return 4; // Blue
    } else if (p >= 0.33) {
        return 7; // White
    } else {
        return 1; // Red
    }
}

// Function to create the grid based on the probabilities
std::vector<std::vector<std::pair<PokerHand, std::optional<double>>>> makeGrid(const std::map<PokerHand, double>& probs) {
    std::vector<std::vector<std::pair<PokerHand, std::optional<double>>>> arr(13, std::vector<std::pair<PokerHand, std::optional<double>>>(13));
    for (const auto& [hand, val] : probs) {
        auto [a, b] = hand.gridPos();
        arr[a][b] = std::make_pair(hand, std::make_optional(val));
    }

    return arr;
}

// Function to convert cell data to a string
std::string gridString(const std::pair<PokerHand, std::optional<double>>& cell) {
    if (!cell.second.has_value()) {
        return "Not\nPoss";
    }
    std::stringstream ss;
    ss << cell.first.toString() << ":\n" << std::fixed << std::setprecision(2) << cell.second.value();
    return ss.str();
}

// Function to print the table
void prettyPrint(const std::map<PokerHand, double>& probs) {
    auto grid = makeGrid(probs);

    // Print the table headers or other initial info if necessary
    for (const auto& row : grid) {
        for (const auto& cell : row) {
            int color = cellColor(cell.second);
            std::string cellText = gridString(cell);

            // Simulating text color output in console (pseudo-code, actual method depends on your console/system)
            std::cout << "\033[" << color << "m" << std::setw(15) << std::left << cellText << "\033[0m";
        }
        std::cout << std::endl;
    }
}
