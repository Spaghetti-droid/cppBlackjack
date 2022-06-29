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

// enums

enum class EndState{
    win,
    loss,
    tie
};

// Classes

class Card{
public:

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

private:

    Rank m_rank{ Rank::two };
    Suit m_suit{ Suit::clubs };

    char toChar(Suit suit) const{
        switch (suit)
        {
            case Suit::clubs: return 'C';
            case Suit::diamonds: return 'D';
            case Suit::hearts: return 'H';
            case Suit::spades: return 'S';    
            default: return '?';
        }
    }

    char toChar(Rank rank) const{
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

public:

    Card() = default;
    Card(Rank rank, Suit suit) : m_rank{ rank }, m_suit{ suit }{
    }

    std::string toString() const{
        std::string string{};
        string += toChar(m_rank);
        string += toChar(m_suit);
        return string;
    }

    void print() const{
        std::cout << toString() << ' ';
    }

    int getValue() const{
        int enumValue{ static_cast<int>(m_rank) };
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
};

class Deck{
public:
    using deck_t = std::array<Card, g_totalCardsInDeck>;
    using arrayIndex_t = deck_t::size_type;
private:
    deck_t m_deck{};
    arrayIndex_t m_cardIndex{};
public:
    Deck(){
        arrayIndex_t cardNumber{ 0 };
        const int maxRanks{ static_cast<int>(Card::Rank::max_ranks) };
        for(int s{ 0 }; s<static_cast<int>(Card::Suit::max_suits); ++s){
            for(int r{ 0 }; r<maxRanks; ++r){
                m_deck.at(cardNumber) = Card{static_cast<Card::Rank>(r), static_cast<Card::Suit>(s)};
                ++cardNumber;
            }
        }

        shuffle();  // We have no use for an ordered deck
    }

    void print(){
        for(Card card : m_deck){
            card.print();
        }

        std::cout << '\n';
    }

    void shuffle(){
        std::shuffle(m_deck.begin(), m_deck.end(), Random::mt);
        m_cardIndex = 0;
    }

    const Card& dealCard(){
        return m_deck.at(m_cardIndex++);
    }

};

class Participant{
private:
    std::string m_name{};
    int m_score{};
    std::vector<Card> m_hand{};
public:
    Participant(const std::string& name) : m_name{ name }{};

    int drawCard(Deck& deck){
        Card card{ deck.dealCard() };
        std::cout << m_name << " draws a card: " << card.toString() << '\n';
        m_hand.push_back(card);
        int value{ card.getValue() };
        m_score += value;
        return value;
    }

    std::string_view getName() const { return m_name; }
    int getScore() const { return m_score; }
    bool isBust() const { return m_score > g_targetScore; }

    void printHandAndValue() const {
        std::cout << m_name << " hand: ";
        for(Card card: m_hand){
            card.print();
        }
        std::cout << '\n';
        std::cout << "Score: " << m_score << '\n';
    }
};

// Functions

// return true if busted
bool performPlayerTurn(Participant& player, Deck& deck){
    
    std::cout << player.getName() << "'s turn\n";

    while(true){
        char res{ getFromUser<char>("Hit or Stand (h/s)? ") };
        std::cout << '\n';
        if(res == 's'){
            return false;
        } else if(res == 'h') {
            player.drawCard(deck);
            player.printHandAndValue();
            if(player.isBust()){
                return true;
            }
        }
    }
}

// return true if dealer busted
bool performDealerTurn(Participant& dealer, Deck& deck, int playerScore){

    std::cout << dealer.getName() << "'s turn\n";

    while(dealer.getScore() < playerScore){
        //std::cout << "Dealer Hits\n";
        dealer.drawCard(deck);
    }

    if(!dealer.isBust()){
        std::cout << dealer.getName() << " Stands\n";
    }

    std::cout << '\n';
    dealer.printHandAndValue();
    return dealer.isBust();
}

EndState playBlackjack(){

    Deck deck{};

    Participant dealer{"Dealer"};
    Participant player{"Player"};

    std::cout << "Welcome to Blackjack!\n";

    dealer.drawCard(deck);
    player.drawCard(deck);
    player.drawCard(deck);
    if(player.isBust()){        // Happens if 2 aces (as ace is only worth 11 in this version)
        std::cout << "Bust!" << '\n';
        return EndState::loss;
    }
    std::cout << '\n';
    dealer.printHandAndValue();
    std::cout << '\n';
    player.printHandAndValue();
    std::cout << '\n';
    if(performPlayerTurn(player, deck)){
        std::cout << "Bust!" << '\n';
        return EndState::loss;
    }
    if(performDealerTurn(dealer, deck, player.getScore())){
        std::cout << "Bust!" << '\n';
        return EndState::win;
    }

    if(player.getScore()<dealer.getScore()){        
        return EndState::loss;
    } 
    if(player.getScore() == dealer.getScore()){
        return EndState::tie;
    }

    // Normally dealer has either busted, won, or tied by this point, so shouldn't be possible

    return EndState::win;
}

int main()
{
    EndState endState{ playBlackjack() };

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