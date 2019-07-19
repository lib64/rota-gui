#ifndef ROTAREPLY_H
#define ROTAREPLY_H

#include <QString>

class QByteArray;

class RotaReply {
  QString status;
  QString request;
  QString board;
  QString hash;
  int playerWins;
  int computerWins;
  int moves;
  int gamesWon;

public:
  RotaReply(const QByteArray &reply);
  QString getRequest() const;
  QString getStatus() const;
  QString getBoard() const;
  QString getHash() const;
  int getPlayerWins() const;
  int getComputerWins() const;
  int getMoves() const;
  int getGamesWon() const;
};

#endif // ROTAREPLY_H
