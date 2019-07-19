#ifndef BOARD_H
#define BOARD_H

#include "move.h"

#include <QGraphicsItem>

#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::vector;
using std::pair;
using std::invalid_argument;

class Board : public QGraphicsItem {
  int _round;
  char _turn;
  string _board;
  QPixmap *_background;

public:
  static const vector<vector<size_t>> adjacencyMatrix;
  static const vector<vector<size_t>> winMatrix;
  static const vector<QPoint> positionOffset;

  Board(const string &board, int round, char turn);
  Board(const Board &board);

  ~Board();

  QRectF boundingRect() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

  vector<Move> findMoves() const;
  vector<size_t> findPlacements() const;

  void move(size_t from, size_t to);
  void move(const Move &move);
  void place(size_t index);

  int round() const;
  void setRound(int round);
  void incRound();
  void resetRound();

  int score() const;
  bool isPlace() const;
  bool isEmpty();

  char turn() const;
  void changeTurn();

  void setBoard(string board);
  string toString() const { return _board; }

  Move findBestMove();
  size_t findBestPlace();
};

bool isBoardEmpty(const string &board);
int minimax(const Board &board, int depth);

#endif // BOARD_H
