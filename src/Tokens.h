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

    // Tip 3-1: Six special member functions
    //
    //  1. Default constructor      Created only if there are no other ctors defined.
    //  2. Copy constructor         Created only if no move ctors/assignments are defined.
    //  3. Copy assignment          Same as 2.
    //  4. Move constructor         Created only if none of 2-6 are defined.
    //  5. Move assignment          Same as 4.
    //  6. Destructor               Created always (but can be overridden).
    //
    //  Rule of 3: custom-defined versions of 2, 3, and 6 are usually needed if the class
    //  manages raw resources (pointers, files, sockets, etc).
    //
    //  Rule of 5: C++11 added move semantics. Add custom versions of 4 and 5 to be fast.

    Tokens() = default;                               // 1
    Tokens(const Tokens& other) = delete;             // 2 (redundant; implicitly deleted)
    Tokens& operator=(const Tokens& other) = delete;  // 3 (redundant; implicitly deleted)
    Tokens(Tokens&& other) noexcept = default;        // 4
    Tokens& operator=(Tokens&& other) = default;      // 5
    ~Tokens();                                        // 6

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

    size_t m_nPos = 0;
    std::vector<Token*> m_lData;
};

#endif  //  TOKENS_H
