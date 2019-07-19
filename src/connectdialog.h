#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QWidget>

namespace Ui {
class ConnectDialog;
}

class ConnectDialog : public QWidget {
  Q_OBJECT

public:
  explicit ConnectDialog(QWidget *parent = nullptr);
  ~ConnectDialog();

private slots:
  void on_pushButton_clicked();

signals:
  void requestConnection(QString url, QString email);

private:
  Ui::ConnectDialog *ui;
};

#endif // CONNECTDIALOG_H
