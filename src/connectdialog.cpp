#include "connectdialog.h"
#include "ui_connectdialog.h"

ConnectDialog::ConnectDialog(QWidget *parent)
    : QWidget(parent), ui(new Ui::ConnectDialog) {
  ui->setupUi(this);
}

ConnectDialog::~ConnectDialog() { delete ui; }

void ConnectDialog::on_pushButton_clicked() {
  emit requestConnection(ui->lineEdit->text(), ui->lineEdit_2->text());
  close();
}
