#include "mainwindow.h"
#include "connectdialog.h"
#include "ui_mainwindow.h"

#include <QDesktopWidget>
#include <QGraphicsScene>
#include <QJsonDocument>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStatusBar>
#include <QUrl>

#define MAX_ROUND 65
#define PLACE_COUNT 6

const QByteArray MainWindow::_userAgent = "NCSA_Mosaic/2.0 (Windows 3.1)";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  _running = false;
  _scene = shared_ptr<QGraphicsScene>(new QGraphicsScene(this));
  ui->graphicsView->setScene(_scene.get());
  _board = nullptr;
  _manager = shared_ptr<QNetworkAccessManager>(new QNetworkAccessManager(this));
  _jar = shared_ptr<QNetworkCookieJar>(new QNetworkCookieJar(_manager.get()));
  _manager->setCookieJar(_jar.get());

  connect(_manager.get(), &QNetworkAccessManager::finished, this,
          &MainWindow::requestFinished);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::requestNewGame() {
  QNetworkRequest request(QUrl(_url + "?request=new&email=" + _email));
  request.setRawHeader("User-Agent", _userAgent);
  _manager->get(request);
  _running = true;
}

void MainWindow::requestPlacement(size_t index) {
  QNetworkRequest request(
      QUrl(_url + "?request=place&location=" + QString::number(index + 1)));
  request.setRawHeader("User-Agent", _userAgent);
  _manager->get(request);
}

void MainWindow::requestMove(size_t from, size_t to) {
  QNetworkRequest request(QUrl(_url + "?request=move&from=" +
                               QString::number(from + 1) + "&to=" +
                               QString::number(to + 1)));
  request.setRawHeader("User-Agent", _userAgent);
  _manager->get(request);
}

void MainWindow::requestNextGame() {
  QNetworkRequest request(QUrl(_url + "?request=next"));
  request.setRawHeader("User-Agent", _userAgent);
  _manager->get(request);
}

// Open a connect dialog.
void MainWindow::on_actionConnect_triggered() {
  if (!_running) {
    ConnectDialog *dialog = new ConnectDialog();
    connect(dialog, &ConnectDialog::requestConnection, this,
            &MainWindow::on_connectDialogFinished);
    const QPoint global = this->mapToGlobal(rect().center());
    dialog->move(global.x() - dialog->width() / 2,
                 global.y() - dialog->height() / 2);
    dialog->show();
  }
}

void MainWindow::on_actionQuit_triggered() { QApplication::quit(); }

// User has requested a new game.
void MainWindow::on_connectDialogFinished(QString url, QString email) {
  _url = url;
  _email = email;
  ui->listWidget->addItem("Starting new game.");
  requestNewGame();
}

// We have recieved a response.
// if board is null we have started a new game.
// otherwise we place, move, or request next game.
void MainWindow::requestFinished(QNetworkReply *reply) {
  if (!_running) {
    return;
  }

  // Parse the json.
  RotaReply rotaReply(reply->readAll());

  // We can haz hashes plz?
  if (rotaReply.getHash() != "") {
    ui->listWidget->addItem("Hash Found: " + rotaReply.getHash());
    ui->listWidget->scrollToBottom();
    _running = false;
    _board = nullptr;
    return;
  }

  // Server returned an error.
  if (rotaReply.getBoard() == "") {
    throw invalid_argument("Empty reply in requestFinished");
  }

  // New game
  if (_board == nullptr) {
    bool boardEmpty = isBoardEmpty(rotaReply.getBoard().toStdString());
    int round = (boardEmpty) ? 0 : 1;
    _board = shared_ptr<Board>(
        new Board(rotaReply.getBoard().toStdString(), round, 'p'));
    _scene->addItem(_board.get());
  }
  // Game is already started
  else {
    _board->setBoard(rotaReply.getBoard().toStdString());
    _board->incRound();
  }

  // Advance to next game
  if (_board->round() > MAX_ROUND) {
    _board->resetRound();
    requestNextGame();
    return;
  } else {
    if (_board->round() < PLACE_COUNT) {
      // Place a piece
      size_t place = _board->findBestPlace();
      QString msg = QString::number(_board->round()) + " Placing at " +
                    QString::number(place) + ".";
      ui->listWidget->addItem(msg);
      ui->listWidget->scrollToBottom();
      requestPlacement(place);
    } else {
      // Move a piece
      Move move = _board->findBestMove();
      QString msg = " Moving from " + QString::number(move.from()) + " to " +
                    QString::number(move.to()) + ".";
      ui->listWidget->addItem(msg);
      ui->listWidget->scrollToBottom();
      requestMove(move.from(), move.to());
    }

    // Player has moved
    _board->incRound();
  }
  QString stat = "    Round:" + QString::number(_board->round()) +
                 "   Status:" + rotaReply.getStatus() + "   Comp wins:" +
                 QString::number(rotaReply.getComputerWins()) +
                 "   Player wins:" +
                 QString::number(rotaReply.getPlayerWins()) + "   Games won:" +
                 QString::number(rotaReply.getGamesWon()) + "   Moves:" +
                 QString::number(rotaReply.getMoves());

  // Print stats on statusBar.
  ui->statusBar->showMessage(stat);
  _board->update();
}
