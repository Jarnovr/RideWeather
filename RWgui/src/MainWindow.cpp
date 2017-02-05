#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

void MainWindow::on_actionExit_triggered()
{
	QCoreApplication::quit();
}

MainWindow::~MainWindow()
{
	delete ui;
}