#ifndef TOKENS_H
#define TOKENS_H

#include <vector>

#include "Token.h"

enum Flags : uint8_t
{
    skipws = 1U << 0,
    skipnewline = 1U << 1,
};

class Tokens
{
public:
    using iterator = std::vector<Token*>::iterator;
    using const_iterator = std::vector<Token*>::const_iterator;
    using reverse_iterator = std::vector<Token*>::reverse_iterator;
    using const_reverse_iterator = std::vector<Token*>::const_reverse_iterator;

    Tokens();
    Tokens(Tokens&& other) noexcept = default;
    ~Tokens();

    // Access
    Token* at(size_t pos) const;
    size_t pos() const;
    Token* back() const;
    Token* head() const;

    // Iterators
    const_iterator begin() const;
    const_iterator cbegin() const noexcept;
    const_iterator end() const;
    const_iterator cend() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    const_reverse_iterator crend() const noexcept;

    // Capacity
    void reserve(size_t capacity);
    bool empty() const;
    size_t size() const;

    // Modifiers
    void push_back(Token* token);
    void clear();

    // Movement
    Token* peek(int count, int flags = 0) const;
    Token* advance(int count, int flags = 0);

private:
    int move(int count, int flags) const;

    size_t m_nPos;
    std::vector<Token*> m_lData;
};

#endif  //  TOKENS_H
