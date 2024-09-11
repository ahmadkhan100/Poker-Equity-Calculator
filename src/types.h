#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <algorithm> // For std::max

enum class Suited {
    On,
    Off,
    Pair,
};

enum class Value {
    Two = 2, Three, Four, Five, Six, Seven, Eight, Nine, Ten,
    Jack, Queen, King, Ace
};

struct Card {
    Value value;
    char suit; // 'c', 'd', 'h', 's' for clubs, diamonds, hearts, spades
};

class PokerHand {
public:
    Value a;
    Value b;
    Suited s;

    PokerHand() : a(Value::Ace), b(Value::Ace), s(Suited::On) {} // Default constructor

    static PokerHand Default() {
        return PokerHand();
    }

    std::string toString() const {
        char a_char = valueToChar(a);
        char b_char = valueToChar(b);
        switch (s) {
            case Suited::On:
                return std::string(1, a_char) + std::string(1, b_char) + 's';
            case Suited::Off:
                return std::string(1, a_char) + std::string(1, b_char) + 'o';
            case Suited::Pair:
                return std::string(1, a_char) + std::string(1, b_char);
        }
        return "";
    }

    static PokerHand newHand(const Card& card1, const Card& card2) {
        Value a = card1.value;
        Value b = card2.value;
        Suited s = (card1.suit == card2.suit) ? Suited::On : (a == b ? Suited::Pair : Suited::Off);

        if (a > b) {
            return {a, b, s};
        } else {
            return {b, a, s};
        }
    }

    std::pair<uint8_t, uint8_t> gridPos() const {
        if (s == Suited::On) {
            return {12 - static_cast<uint8_t>(a), 12 - static_cast<uint8_t>(b)};
        } else {
            return {12 - static_cast<uint8_t>(b), 12 - static_cast<uint8_t>(a)};
        }
    }

private:
    static char valueToChar(Value v) {
        switch (v) {
            case Value::Ten: return 'T';
            case Value::Jack: return 'J';
            case Value::Queen: return 'Q';
            case Value::King: return 'K';
            case Value::Ace: return 'A';
            default: return '0' + static_cast<int>(v);
        }
    }
};

struct Beats {
    std::pair<Card, Card> hand;
    bool we_beat;
};

#endif // TYPES_H
