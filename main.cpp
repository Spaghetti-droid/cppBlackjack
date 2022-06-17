#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <random>

constexpr int g_totalCardsInDeck{ 52 };
constexpr int g_targetScore{ 21 };

namespace Random{
    std::random_device rd;
    std::seed_seq seedSeq{ rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd() };
    std::mt19937_64 mt{ seedSeq };
}

// enums

enum class Suit{
    clubs,
    diamonds,
    hearts,
    spades,

    max_suits
};

enum class Rank{
    two,
    three,
    four,
    five,
    six,
    seven,
    eight,
    nine,
    ten,
    jack,
    queen,
    king,
    ace,

    max_ranks
};

enum class EndState{
    win,
    loss,
    tie
};

struct Card{
    Suit suit{};
    Rank rank {};
};

struct Participant{
    std::string_view name{};
    std::vector<Card> hand{};
    int score{};
};

// aliases

using deck_t = std::array<Card, g_totalCardsInDeck>;
using arrayIndex_t = deck_t::size_type;

// Templates

template <typename T>
inline T getFromUser(const std::string_view prompt){
    T userResponse;
    bool inputFailed = true;

    do{
        std::cout << prompt;
        std::cin >> userResponse;
        inputFailed = std::cin.fail();
        if(inputFailed){
            std::cin.clear();
            std::cerr << "Please enter a valid value.\n";
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');   // Clear any remaining characters on the line
    }while(inputFailed);
    return userResponse;
}

// Functions

char toChar(Suit suit){
    switch (suit)
    {
        case Suit::clubs: return 'C';
        case Suit::diamonds: return 'D';
        case Suit::hearts: return 'H';
        case Suit::spades: return 'S';    
        default: return '?';
    }
}

char toChar(Rank rank){
    switch(rank){
        case Rank::two: return '2';
        case Rank::three: return '3';
        case Rank::four: return '4';
        case Rank::five: return '5';
        case Rank::six: return '6';
        case Rank::seven: return '7';
        case Rank::eight: return '8';
        case Rank::nine: return '9';
        case Rank::ten: return 'T';
        case Rank::jack: return 'J';
        case Rank::queen: return 'Q';
        case Rank::king: return 'K';
        case Rank::ace: return 'A';
        default: return '?';
    }
}

std::string toString(const Card& card){
    std::string string{};
    string += toChar(card.rank);
    string += toChar(card.suit);
    return string;
}

void printCard(const Card& card){
    std::cout << toString(card) << ' ';
}

deck_t createDeck(){

    deck_t deck{};

    arrayIndex_t cardNumber{ 0 };
    const int maxRanks{ static_cast<int>(Rank::max_ranks) };
    for(int s{ 0 }; s<static_cast<int>(Suit::max_suits); ++s){
        for(int r{ 0 }; r<maxRanks; ++r){
            deck.at(cardNumber).rank = static_cast<Rank>(r);
            deck.at(cardNumber).suit = static_cast<Suit>(s);
            ++cardNumber;
        }
    }

    return deck;
}

void shuffleDeck(deck_t& deck){
    std::shuffle(deck.begin(), deck.end(), Random::mt);
}

int getCardValue(const Card& card){
    int enumValue{ static_cast<int>(card.rank) };
    if(enumValue<9){        
        // 2-9 are worth their face value
        return enumValue+2;
    }
    if(enumValue == static_cast<int>(Rank::ace)){
        // ace is worth 11
        return 11;
    }

    // 10, jack, queen, and king are worth 10 
    return 10;
}

void printHandAndValue(Participant participant){
    std::cout << participant.name << " hand: ";
    for(Card card: participant.hand){
        printCard(card);
    }
    std::cout << '\n';
    std::cout << "Score: " << participant.score << '\n';
}

int getHandValue(const std::vector<Card> hand){
    int total{ 0 };
    for(Card card: hand){
        total += getCardValue(card);
    }
    return total;
}

Card drawCard(const deck_t& shuffledDeck){
    static int nextCard{ 0 };
    return shuffledDeck.at(nextCard++);
}

Card drawCard(const deck_t& shuffledDeck, bool reset){
    static int nextCard{ 0 };
    if(reset){
        nextCard = 0;
    }
    return shuffledDeck.at(nextCard++);
}

void drawCard(Participant& participant, const Card*& nextCardPtr){
    std::cout << participant.name << " draws a card: ";
    std::cout << toString(*nextCardPtr) << "\n";
    auto& hand = participant.hand;
    auto handSize{ hand.size() };
    hand.resize(handSize+1);
    hand.at(handSize) = *nextCardPtr;
    ++nextCardPtr;
}

void performPlayerTurn(Participant& player, const Card*& nextCardPtr){
    
    std::cout << player.name << "'s turn\n";

    while(true){
        char res{ getFromUser<char>("Hit or Stand (h/s)? ") };
        std::cout << '\n';
        if(res == 's'){
            break;
        } else if(res == 'h') {
            player.score += getCardValue(*nextCardPtr); // Before draw, as draw moves pointer
            drawCard(player, nextCardPtr);
            printHandAndValue(player);
            if(player.score > g_targetScore){
                break;
            }
        }
    }
}

void performDealerTurn(Participant& dealer, const Card*& nextCardPtr, int playerScore){

    std::cout << dealer.name << "'s turn\n";

    while(dealer.score < playerScore){
        //std::cout << "Dealer Hits\n";
        dealer.score += getCardValue(*nextCardPtr); // Before draw, as draw moves pointer
        drawCard(dealer, nextCardPtr);
    }

    if(dealer.score<=g_targetScore){
        std::cout << dealer.name << " Stands\n";
    }

    std::cout << '\n';
    printHandAndValue(dealer);
}

EndState playBlackjack(const deck_t& shuffledDeck){

    auto nextCardPtr{ shuffledDeck.begin() };

    Participant dealer{"Dealer"};
    Participant player{"Player"};

    std::cout << "Welcome to Blackjack!\n";
    drawCard(dealer, nextCardPtr);
    drawCard(player, nextCardPtr);
    drawCard(player, nextCardPtr);
    dealer.score = getHandValue(dealer.hand);
    player.score = getHandValue(player.hand);
    if(player.score>g_targetScore){
        std::cout << "Bust!" << '\n';
        return EndState::loss;
    }
    std::cout << '\n';
    printHandAndValue(dealer);
    std::cout << '\n';
    printHandAndValue(player);
    std::cout << '\n';
    performPlayerTurn(player, nextCardPtr);
    if(player.score>g_targetScore){
        std::cout << "Bust!" << '\n';
        return EndState::loss;
    }
    performDealerTurn(dealer, nextCardPtr, player.score);
    if(dealer.score>g_targetScore){
        std::cout << "Bust!" << '\n';
        return EndState::win;
    }

    if(player.score>dealer.score){
        return EndState::win;
    } 
    if(player.score == dealer.score){
        return EndState::tie;
    }
    return EndState::loss;
}

int main()
{
    deck_t deck{ createDeck() };
    shuffleDeck(deck);
    EndState endState{ playBlackjack(deck) };

    switch (endState)
    {
    case EndState::win:
        std::cout << "You won!!!\n";
        break;
    case EndState::tie:
        std::cout << "A tie!\n";
        break;
    case EndState::loss:
        std::cout << "You lost :(\n";
        break;
    default:
        break;
    }
    return 0;
}