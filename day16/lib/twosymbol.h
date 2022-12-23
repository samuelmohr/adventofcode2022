#ifndef TWOSYMBOL_H
#define TWOSYMBOL_H

#include <string>

class TwoSymbol {
  private:
    char symb[2];
  public:
     TwoSymbol() = default;
     TwoSymbol(char c1,char v2);
     ~TwoSymbol() = default;
    char& operator [](int idx);
    char operator [](int idx) const;
    friend bool operator==(const TwoSymbol& lhs, const TwoSymbol& rhs);
    operator std::string() const;
};

#endif // TWOSYMBOL_H
