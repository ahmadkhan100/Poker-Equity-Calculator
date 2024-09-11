#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
#include <execution>
#include "PokerLib.h"  // Placeholder for the poker library you're using.

/**
 * Generate all possible 2-card hands from the remaining deck.
 * @param remaining The remaining cards in the deck.
 * @return A vector of 2-card vectors representing hands.
 */
std::vector<std::vector<Card>> enemy_hands(const FlatDeck& remaining) {
    return CardIterator::generate_hands(remaining, 2);
}

/**
 * Calculate the rank of the combined hand and board.
 * @param hand The player's hand.
 * @param board The cards on the board.
 * @return The rank of the combined hand and board.
 */
Rank hand_board_rank(const std::vector<Card>& hand, const std::vector<Card>& board) {
    std::vector<Card> combined = board;
    combined.insert(combined.end(), hand.begin(), hand.end());
    return PokerLib::rank_hand(combined);
}

/**
 * Calculate the probability with Laplace smoothing given a list of boolean outcomes.
 * @param list A vector of boolean values where true indicates a win.
 * @return The calculated probability.
 */
double prob(const std::vector<bool>& list) {
    size_t count = std::count(list.begin(), list.end(), true);
    return static_cast<double>(count) / static_cast<double>(list.size());
}

/**
 * Calculate the probabilities of beating given outcomes.
 * @param outcomes A vector of outcomes.
 * @return A map from hands to their probabilities.
 */
std::map<PokerHand, double> prob_calc(const std::vector<Beats>& outcomes) {
    std::map<PokerHand, std::vector<bool>> string_hands;
    for (const auto& item : outcomes) {
        string_hands[item.hand].push_back(item.we_beat);
    }

    std::map<PokerHand, double> probs;
    for (const auto& [name, v] : string_hands) {
        probs[name] = prob(v);
    }

    return probs;
}

/**
 * Determine if the user's hand beats the enemy hands based on their ranks.
 * @param usr_rank The rank of the user's hand.
 * @param board The current board cards.
 * @param enemy A list of enemy hands.
 * @return A vector of outcomes.
 */
std::vector<Beats> hand_beats(Rank usr_rank, const std::vector<Card>& board, const std::vector<std::vector<Card>>& enemy) {
    std::vector<Beats> results;
    std::transform(std::execution::par, enemy.begin(), enemy.end(), std::back_inserter(results), [&](const std::vector<Card>& hand) {
        return Beats{hand, hand_board_rank(hand, board) < usr_rank};
    });
    return results;
}

/**
 * Remove used cards from the deck.
 * @param hand The user's hand.
 * @param board The board cards.
 * @return A deck with the used cards removed.
 */
Deck deck_without(const std::vector<Card>& hand, const std::vector<Card>& board) {
    Deck deck = Deck::standard_deck();
    for (const auto& card : board) {
        deck.remove(card);
    }
    for (const auto& card : hand) {
        deck.remove(card);
    }
    return deck;
}

/**
 * Calculate the probabilities on the river stage.
 * @param user_hand The user's hand.
 * @param board The current board.
 * @param remaining The remaining deck.
 * @return A map of poker hands to their probabilities.
 */
std::map<PokerHand, double> river_calc(const std::vector<Card>& user_hand, const std::vector<Card>& board, Deck& remaining) {
    Rank usr_rank = hand_board_rank(user_hand, board);
    std::vector<std::vector<Card>> enemy = enemy_hands(remaining.flatten());
    return prob_calc(hand_beats(usr_rank, board, enemy));
}

/**
 * Calculate the probabilities on the turn stage.
 * @param user_hand The user's hand.
 * @param board The current board.
 * @param remaining The remaining deck.
 * @return A map of poker hands to their probabilities.
 */
std::map<PokerHand, double> turn_calc(const std::vector<Card>& user_hand, const std::vector<Card>& board, Deck& remaining) {
    std::vector<Card> remaining_cards = remaining.to_vector();
    std::vector<Beats> beats;
    std::for_each(std::execution::par, remaining_cards.begin(), remaining_cards.end(), [&](const Card& card) {
        std::vector<Card> river_board = board;
        river_board.push_back(card);
        Deck river_remaining = deck_without(user_hand, river_board);

        Rank usr_rank = hand_board_rank(user_hand, river_board);
        std::vector<std::vector<Card>> enemy = enemy_hands(river_remaining.flatten());
        auto partial_beats = hand_beats(usr_rank, river_board, enemy);
        beats.insert(beats.end(), partial_beats.begin(), partial_beats.end());
    });

    return prob_calc(beats);
}

/**
 * Calculate the probabilities on the flop stage.
 * @param user_hand The user's hand.
 * @param board The current board.
 * @param remaining The remaining deck.
 * @return A map of poker hands to their probabilities.
 */
std::map<PokerHand, double> flop_calc(const std::vector<Card>& user_hand, const std::vector<Card>& board, Deck& remaining) {
    std::vector<Card> remaining_cards = remaining.to_vector();
    std::vector<Beats> beats;
    std::for_each(std::execution::par, remaining_cards.begin(), remaining_cards.end(), [&](const Card& card) {
        std::vector<Card> turn_board = board;
        turn_board.push_back(card);
        Deck turn_remaining = deck_without(user_hand, turn_board);

        std::vector<Card> river_cards = turn_remaining.to_vector();
        std::for_each(std::execution::par, river_cards.begin(), river_cards.end(), [&](const Card& river_card) {
            std::vector<Card> river_board = turn_board;
            river_board.push_back(river_card);
            Deck river_remaining = deck_without(user_hand, river_board);

            Rank usr_rank = hand_board_rank(user_hand, river_board);
            std::vector<std::vector<Card>> enemy = enemy_hands(river_remaining.flatten());
            auto partial_beats = hand_beats(usr_rank, river_board, enemy);
            beats.insert(beats.end(), partial_beats.begin(), partial_beats.end());
        });
    });

    return prob_calc(beats);
}
