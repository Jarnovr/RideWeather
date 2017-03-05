#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QString>
#include <QMessageBox>
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

void MainWindow::on_actionAbout_triggered()
{
	QMessageBox::about(this,"RideWeather","RideWeather is an application for analysing Strava Activities with respect to weather conditions occuring during activities.");
}

void MainWindow::on_actionAbout_QT_triggered()
{
	QMessageBox::aboutQt(this, "RideWeather");
}

void MainWindow::on_btn_Load_Token_clicked()
{
	QMessageBox::information(this,"RideWeather","btn_load_Token clicked.\n");

}


MainWindow::~MainWindow()
{
	delete ui;
}