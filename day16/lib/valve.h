#ifndef VALVE_H
#define VALVE_H

#include <string>

// S is naming, T is flowrate
template <class S, class T>
class Valve {
  private:
    const S signature;
    const T rate;

  public:
    Valve() = delete;
    Valve(const S &signature);
    Valve(const S &signature, const T rate);
    Valve(const Valve<S,T> &old);
    ~Valve();

  T getRate() const;
  operator T() const;
  const bool operator<(const Valve<S,T> &r) const;
  const bool operator>(const Valve<S,T> &r) const;

  S getSignature() const;
  std::string getDescription() const;
  operator std::string() const;
  const bool operator==(const Valve<S,T> &r) const;
};

#endif // VALVE_H
