#ifndef MOVE_H
#define MOVE_H

#include <cstddef>

class Move {
  size_t _from;
  size_t _to;

public:
  Move(size_t from, size_t to);
  size_t from() const;
  void setFrom(size_t from);
  size_t to() const;
  void setTo(size_t to);
};

#endif // MOVE_H
