#ifndef TUNNELMAP_H
#define TUNNELMAP_H

#include <map>
#include <list>
#include <string>


// V is Object Type
template <class V>
class Tunnelmap {
  private:
    std::map<std::reference_wrapper<const V>,std::list<std::reference_wrapper<const V>>,decltype([](const std::reference_wrapper<const V>& a, const std::reference_wrapper<const V>& b) { return ((std::string)a.get()) < ((std::string)b.get()); })> connections;

  public:
    Tunnelmap() {}
    ~Tunnelmap() {}

    bool addConnection(const V &from, const V &to);
    std::list<std::reference_wrapper<const V>> getConnection(const V &v) const;

};
#endif // TUNNELMAP_H
