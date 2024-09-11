#include "types.h"
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <execution>
#include <iostream>
#include <map>

// Function to create a standard deck of cards
std::vector<Card> createDeck() {
    std::vector<Card> deck;
    std::array<char, 4> suits = {'h', 'd', 'c', 's'};  // Hearts, Diamonds, Clubs, Spades
    for (char suit : suits) {
        for (int value = static_cast<int>(Value::Two); value <= static_cast<int>(Value::Ace); ++value) {
            deck.push_back(Card{static_cast<Value>(value), suit});
        }
    }
    return deck;
}

// Function to shuffle the deck of cards
void shuffleDeck(std::vector<Card>& deck) {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(deck.begin(), deck.end(), g);
}

// Function to remove used cards from the deck
void removeUsedCards(std::vector<Card>& deck, const std::vector<Card>& usedCards) {
    deck.erase(std::remove_if(deck.begin(), deck.end(), 
               [&usedCards](const Card& c) {
                   return std::find_if(usedCards.begin(), usedCards.end(), 
                          [&c](const Card& used) {
                              return c.value == used.value && c.suit == used.suit;
                          }) != usedCards.end();
               }), deck.end());
}

// Function to simulate a poker game and calculate hand probabilities
std::map<PokerHand, double> simulateGame(const std::vector<Card>& deck, const PokerHand& playerHand) {
    std::map<PokerHand, std::vector<bool>> outcomes;
    for (size_t i = 0; i < deck.size() - 1; ++i) {
        for (size_t j = i + 1; j < deck.size(); ++j) {
            PokerHand opponentHand = PokerHand::newHand(deck[i], deck[j]);
            bool result = evaluateHandStrength(playerHand, opponentHand); // Assumes this function exists
            outcomes[opponentHand].push_back(result);
        }
    }

    // Calculate probabilities using Laplace smoothing
    std::map<PokerHand, double> probabilities;
    for (auto& [hand, results]: outcomes) {
        int count = std::count(std::execution::par, results.begin(), results.end(), true);
        probabilities[hand] = static_cast<double>(count + 1) / static_cast<double>(results.size() + 2); // Adding Laplace smoothing
    }
    return probabilities;
}

// Placeholder for a real hand strength evaluation
bool evaluateHandStrength(const PokerHand& playerHand, const PokerHand& opponentHand) {
    // Implement actual poker hand comparison logic
    return playerHand.a > opponentHand.a; // Simplified comparison logic for demonstration
}

// Main driver function to initiate a poker game simulation
int main() {
    std::vector<Card> deck = createDeck();
    shuffleDeck(deck);

    // Draw two cards for the player and create the player's hand
    PokerHand playerHand = PokerHand::newHand(deck[0], deck[1]);
    std::vector<Card> usedCards = {deck[0], deck[1]};
    
    // Remove player's cards from the deck
    removeUsedCards(deck, usedCards);
    
    // Simulate the game
    auto probabilities = simulateGame(deck, playerHand);

    // Output probabilities
    for (const auto& [hand, probability] : probabilities) {
        std::cout << hand.toString() << ": " << probability * 100 << "%" << std::endl;
    }

    return 0;
}
