#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QString>
#include "Config.h"

RideWeather::Configuration* Config;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent), ui(new Ui::MainWindow)
{
	Config = new RideWeather::Configuration();
	ui->setupUi(this);
	ui->lbl_Output->setText(QString::fromStdString(Config->configFileName.string()));
}

void MainWindow::on_actionExit_triggered()
{
	QCoreApplication::quit();
}

MainWindow::~MainWindow()
{
	delete ui;
}