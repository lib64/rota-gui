#include "move.h"

Move::Move(size_t from, size_t to) {
  _from = from;
  _to = to;
}

size_t Move::to() const { return _to; }

void Move::setTo(size_t to) { _to = to; }

size_t Move::from() const { return _from; }

void Move::setFrom(size_t from) { _from = from; }
