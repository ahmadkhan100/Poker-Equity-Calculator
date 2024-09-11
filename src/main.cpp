#include <iostream>
#include <string>
#include <vector>
#include "CLI11.hpp"  // CLI11 for command line parsing
#include "PokerLib.h"  // Placeholder for the actual poker library you use

// Assuming 'driver.h' and 'output.h' provide necessary functionalities
#include "driver.h"  
#include "output.h"

int main(int argc, char** argv) {
    CLI::App app{"Calculates your equity against opponent starting hands for No-Limit Texas Hold 'Em",
                 "Ahmad Abdur Rahman Khan, <ar5khan@uwaterloo.ca>"};
    app.version("1.1.7");

    std::string board_cards;
    std::string hand_cards;
    bool interactive = false;

    app.add_option("-b,--board", board_cards, "Takes a string of the board so far, with cards indicated in RANKsuit form (e.g., Th is the 10 of hearts). Cards are unseparated (e.g., AhAsAcAd9s)")
        ->required(!interactive);
    app.add_option("-h,--hand", hand_cards, "Takes a string of your hole cards, with cards indicated in RANKsuit form (e.g., 9s is the 9 of spades). Cards are unseparated (e.g., AhAs)")
        ->required(!interactive);
    app.add_flag("-I,--interactive", interactive, "Starts the tool in interactive mode. Exit with 'exit'");

    CLI11_PARSE(app, argc, argv);

    if (interactive) {
        std::string input_hand;
        std::string input_board;
        std::string input_turn;
        std::string input_river;
        
        while (true) {
            std::cout << "Enter hand, or type 'exit' to exit: ";
            std::getline(std::cin, input_hand);
            if (input_hand == "exit" || input_hand.empty()) break;

            std::cout << "Enter board, or type 'exit' to exit: ";
            std::getline(std::cin, input_board);
            if (input_board == "exit" || input_board.empty()) break;

            Hand user_hand = Hand::from_string(input_hand);
            Board board = Board::from_string(input_board);

            // Example of processing a poker hand
            Deck deck = Deck::standard();  // Get a standard deck
            deck.remove_used_cards(user_hand, board);  // Remove used cards from the deck

            auto flop_results = driver::flop_calc(user_hand, board, deck);
            output::pretty_print(flop_results);

            std::cout << "Enter turn card, or type 'exit' to exit: ";
            std::getline(std::cin, input_turn);
            if (input_turn == "exit" || input_turn.empty()) continue;

            board.push_back(Card::from_string(input_turn));  // Parse and add the turn card
            deck.remove(input_turn);  // Remove the turn card from the deck

            auto turn_results = driver::turn_calc(user_hand, board, deck);
            output::pretty_print(turn_results);

            std::cout << "Enter river card, or type 'exit' to exit: ";
            std::getline(std::cin, input_river);
            if (input_river == "exit" || input_river.empty()) continue;

            board.push_back(Card::from_string(input_river));  // Parse and add the river card
            deck.remove(input_river);  // Remove the river card from the deck

            auto river_results = driver::river_calc(user_hand, board, deck);
            output::pretty_print(river_results);
        }
    } else {
        if (!board_cards.empty() && !hand_cards.empty()) {
            Hand user_hand = Hand::from_string(hand_cards);
            Board board = Board::from_string(board_cards);

            Deck deck = Deck::standard();
            deck.remove_used_cards(user_hand, board);

            // Depending on the board size, calculate the results at the appropriate stage
            std::map<PokerHand, double> results;
            if (board.size() == 3) {
                results = driver::flop_calc(user_hand, board, deck);
            } else if (board.size() == 4) {
                results = driver::turn_calc(user_hand, board, deck);
            } else if (board.size() == 5) {
                results = driver::river_calc(user_hand, board, deck);
            }

            output::pretty_print(results);
        }
    }

    return 0;
}

