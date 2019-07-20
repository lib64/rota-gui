#include "minimax.h"

// Recursive minimax function to find scores for
// the board passed to it.
int minimax(const Board &board, int depth) {

  // base case
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
