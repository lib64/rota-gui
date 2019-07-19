#include "board.h"

#include <QJsonObject>
#include <QPainter>

#include <algorithm>

const int ELLIPSE_SIZE = 10;
const int RECURSION_DEPTH = 5;

// Each entry is a list of positions that are next to the
// position denoted by the index of the entry.
const vector<vector<size_t>> Board::adjacencyMatrix = {
    {1, 3, 4}, {0, 4, 2}, {1, 4, 5}, {0, 4, 6}, {0, 1, 2, 3, 5, 6, 7, 8},
    {2, 4, 8}, {3, 4, 7}, {6, 4, 8}, {7, 4, 5}};

// If a player has a piece occupying all three points
// of an entry in the list then they win.
const vector<vector<size_t>> Board::winMatrix = {
    {0, 1, 2}, {1, 2, 5}, {2, 5, 8}, {5, 8, 7}, {8, 7, 6}, {7, 6, 3},
    {6, 3, 0}, {3, 0, 1}, {0, 4, 8}, {1, 4, 7}, {2, 4, 6}, {5, 4, 3}};

// Offsets to where each ellipse needs to be drawn on the screen
// to line up with the image.
const vector<QPoint> Board::positionOffset = {
    QPoint(76, 76),  QPoint(2, 107),  QPoint(-74, 76),
    QPoint(109, 0),  QPoint(3, 0),    QPoint(-102, -2),
    QPoint(75, -75), QPoint(3, -101), QPoint(-72, -73)};

Board::Board(const string &board, int round, char turn) : QGraphicsItem() {
  _board = board;
  _round = round;
  _turn = turn;
  _background = new QPixmap(":/images/rota.jpg");
}

Board::Board(const Board &board) : QGraphicsItem() {
  _board = board.toString();
  _round = board.round();
  _turn = board.turn();
  _background = new QPixmap(":/images/rota.jpg");
}

Board::~Board() { delete _background; }

int Board::round() const { return _round; }

void Board::setRound(int round) { _round = round; }

void Board::incRound() { ++_round; }

void Board::resetRound() { _round = 0; }

// return 100 if the player wins, -100 if the computer
// wins, and 0 if neither win.
int Board::score() const {
  for (size_t i = 0; i < winMatrix.size(); i++) {
    vector<size_t> win = winMatrix.at(i);
    if (_board.at(win.at(0)) == 'p' && _board.at(win.at(1)) == 'p' &&
        _board.at(win.at(2)) == 'p') {
      return 100;
    } else if (_board.at(win.at(0)) == 'c' && _board.at(win.at(1)) == 'c' &&
               _board.at(win.at(2)) == 'c') {
      return -100;
    }
  }
  return 0;
}

// Return true if we are placing, false if moving.
bool Board::isPlace() const {
  int match = 0;
  for (size_t i = 0; i < _board.size(); i++) {
    if (_board.at(i) == _turn) {
      match++;
    }
  }
  return (match < 3);
}

bool Board::isEmpty() { return isBoardEmpty(_board); }

char Board::turn() const { return _turn; }

void Board::changeTurn() { _turn = (_turn == 'p') ? 'c' : 'p'; }

void Board::setBoard(string board) { _board = board; }

// Fill a vector with the minimax scores for each move
// that the root state can produce.
// Find the move with the best score and make the move.
Move Board::findBestMove() {
  vector<Move> moves = findMoves();
  vector<int> scores;

  for (size_t i = 0; i < moves.size(); i++) {
    Board tmp = *this;
    tmp.move(moves.at(i).from(), moves.at(i).to());
    tmp.incRound();
    tmp.changeTurn();
    scores.push_back(minimax(tmp, RECURSION_DEPTH));
  }

  size_t bestScoreIndex;
  bestScoreIndex =
      (turn() == 'c')
          ? static_cast<size_t>(distance(scores.begin(), min_element(scores.begin(), scores.end())))
          : static_cast<size_t>(distance(scores.begin(), max_element(scores.begin(), scores.end())));

  return moves.at(bestScoreIndex);
}

// Fill a vector with the minimax scores for each placement
// that the root state can produce.
// Find the placement with the best score and make the move.
size_t Board::findBestPlace() {
  vector<size_t> placements = findPlacements();
  vector<int> scores;

  for (size_t i = 0; i < placements.size(); i++) {
    Board tmp = *this;
    tmp.place(placements.at(i));
    tmp.incRound();
    tmp.changeTurn();
    scores.push_back(minimax(tmp, RECURSION_DEPTH));
  }

  size_t bestScoreIndex;
  bestScoreIndex =
      (turn() == 'c')
          ? static_cast<size_t>(distance(scores.begin(), min_element(scores.begin(), scores.end())))
          : static_cast<size_t>(distance(scores.begin(), max_element(scores.begin(), scores.end())));

  return placements[bestScoreIndex];
}

QRectF Board::boundingRect() const { return QRectF(_background->rect()); }

// Paint the Rota background, and draw the pieces that are present.
void Board::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                  QWidget *) {
  painter->drawPixmap(0, 0, _background->width(), _background->height(),
                      *_background);

  QBrush b1(Qt::blue);
  QBrush b2(Qt::red);

  for (size_t i = 0; i < _board.size(); i++) {
    if (_board.at(i) == 'p') {
      painter->setBrush(b1);
      painter->drawEllipse(boundingRect().center() - positionOffset.at(i),
                           ELLIPSE_SIZE, ELLIPSE_SIZE);
    }
    if (_board.at(i) == 'c') {
      painter->setBrush(b2);
      painter->drawEllipse(boundingRect().center() - positionOffset.at(i),
                           ELLIPSE_SIZE, ELLIPSE_SIZE);
    }
  }
}

// Find all the moves a player can make. Shuffle them to prevent the
// same set of move's from being made over and over.
vector<Move> Board::findMoves() const {
  vector<Move> moves;
  vector<size_t> pieces;
  for (size_t i = 0; i < _board.size(); i++) {
    if (_board.at(i) == _turn) {
      pieces.push_back(i);
    }
  }

  for (size_t i = 0; i < pieces.size(); i++) {
    vector<size_t> adj = adjacencyMatrix.at(pieces.at(i));
    for (size_t j = 0; j < adj.size(); j++) {
      if (_board.at(adj.at(j)) == '-') {
        moves.push_back(Move(pieces.at(i), adj.at(j)));
      }
    }
  }
  random_shuffle(moves.begin(), moves.end());
  return moves;
}

// Find all the placements a player can make. Shuffle them.
vector<size_t> Board::findPlacements() const {
  vector<size_t> indexes;
  for (size_t i = 0; i < _board.size(); i++) {
    if (_board.at(i) == '-') {
      indexes.push_back(i);
    }
  }
  random_shuffle(indexes.begin(), indexes.end());
  return indexes;
}

// Helper function for move
void Board::move(size_t from, size_t to) { move(Move(from, to)); }

// Move a piece. An exception will be thrown for illegal moves.
void Board::move(const Move &_move) {
  if (_board.at(_move.from()) != _turn) {
    throw invalid_argument("In function move: Can't move from a spot not owned "
                           "by current player.");
  }
  if (_board.at(_move.to()) != '-') {
    throw invalid_argument("In function move: Can't move to an occupied spot.");
  }
  _board.at(_move.to()) = _board.at(_move.from());
  _board.at(_move.from()) = '-';
}

// Place a piece. An exception will be thrown for illegal placements.
void Board::place(size_t index) {
  if (_board.at(index) != '-') {
    throw invalid_argument(
        "In function place: Can't place piece in an occupied spot.");
  }
  _board.at(index) = _turn;
}

// Recursive minimax function to find scores for
// the board passed to it.
int minimax(const Board &board, int depth) {
  if (board.score() != 0 || depth == 0) {
    return board.score();
  }

  // If we are moving
  if (!board.isPlace()) {
    vector<Move> moves = board.findMoves();
    vector<int> scores;
    for (size_t i = 0; i < moves.size(); i++) {
      Board boardCopy(board);
      boardCopy.move(moves.at(i).from(), moves.at(i).to());
      boardCopy.incRound();
      boardCopy.changeTurn();
      scores.push_back(minimax(boardCopy, depth - 1));
    }

    return (board.turn() == 'c') ? *min_element(scores.begin(), scores.end())
                                 : *max_element(scores.begin(), scores.end());
  }
  // If we are placing
  else {
    vector<size_t> placements = board.findPlacements();
    vector<int> scores;
    for (size_t i = 0; i < placements.size(); i++) {
      Board boardCopy(board);
      boardCopy.place(placements.at(i));
      boardCopy.incRound();
      boardCopy.changeTurn();
      scores.push_back(minimax(boardCopy, depth - 1));
    }

    return (board.turn() == 'c') ? *min_element(scores.begin(), scores.end())
                                 : *max_element(scores.begin(), scores.end());
  }
}

bool isBoardEmpty(const string &board) {
  bool found = false;
  for (auto &b : board) {
    if (b != '-' && b != 'p' && b != 'c') {
      throw invalid_argument("Invalid charachter in isBoardEmpty");
    }
    if (b != '-') {
      found = true;
    }
  }
  return found;
}
