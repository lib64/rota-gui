#include "rotareply.h"

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

// Parse a json byte array and put the relevant data
// in a rotaReply object, and return it.
RotaReply::RotaReply(const QByteArray &reply) {
  QJsonDocument doc = QJsonDocument::fromJson(reply);
  QJsonObject obj = doc.object();

  if (obj.contains("status")) {
    status = obj["status"].toString();
  }

  QJsonObject data;
  if (obj.contains("data")) {
    data = obj["data"].toObject();
  }

  if (data.contains("board")) {
    board = data["board"].toString();
  }
  if (data.contains("request")) {
    request = data["request"].toString();
  }
  if (data.contains("player_wins")) {
    playerWins = data["player_wins"].toInt();
  }
  if (data.contains("computer_wins")) {
    computerWins = data["computer_wins"].toInt();
  }
  if (data.contains("moves")) {
    moves = data["moves"].toInt();
  }
  if (data.contains("games_won")) {
    gamesWon = data["games_won"].toInt();
  }
  if (data.contains("hash")) {
    hash = data["hash"].toString();
  }
}

QString RotaReply::getStatus() const { return status; }

QString RotaReply::getRequest() const { return request; }

QString RotaReply::getBoard() const { return board; }

QString RotaReply::getHash() const { return hash; }

int RotaReply::getPlayerWins() const { return playerWins; }

int RotaReply::getComputerWins() const { return computerWins; }

int RotaReply::getMoves() const { return moves; }

int RotaReply::getGamesWon() const { return gamesWon; }
