#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "board.h"
#include "rotareply.h"

#include <QMainWindow>
#include <memory>

using std::shared_ptr;

class QNetworkReply;
class QNetworkAccessManager;
class QNetworkCookieJar;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  void requestNewGame();
  void requestPlacement(size_t index);
  void requestMove(size_t from, size_t to);
  void requestNextGame();

private slots:
  void on_actionConnect_triggered();
  void on_actionQuit_triggered();

public slots:
  void on_connectDialogFinished(QString url, QString email);

  void requestFinished(QNetworkReply *reply);

private:
  Ui::MainWindow *ui;
  bool _running;
  QString _url;
  QString _email;
  const static QByteArray _userAgent;
  shared_ptr<QGraphicsScene> _scene;
  shared_ptr<Board> _board;
  shared_ptr<QNetworkAccessManager> _manager;
  shared_ptr<QNetworkCookieJar> _jar;
};

#endif // MAINWINDOW_H
