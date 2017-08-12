#include "MainWindow.h"
#pragma warning(default:4127)
#include "ui_MainWindow.h"

#include <memory>

#pragma warning (disable: 4127)
#include <QString>
#include <QMessageBox>
#include <QFileDialog>
#pragma warning (default: 4127)
#include "Config.h"

RideWeather::Configuration* Config;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent), ui(new Ui::MainWindow), _stravaApiController (nullptr), _activityModel(nullptr)
{
	QCoreApplication::setOrganizationDomain("roosmalen.org");
	QCoreApplication::setOrganizationName("JarnoSoft");
	QCoreApplication::setApplicationName("RideWeather");
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
	ui->btn_Load_Token->setEnabled(false);
	QCoreApplication::processEvents();
	//gets token from config exist.
	boost::filesystem::path token = Config->getToken();
	//check existiance, otherwise ask user for file
	if (!boost::filesystem::exists(token))
	{
		std::cerr << "Token filename from config does not exist." << std::endl;
		token = QFileDialog::getOpenFileName(this, "Open Strava Token", "", "Strava Tokens (*.token);;All Files (*)").toUtf8().toStdString();
		if (!boost::filesystem::exists(token))
		{
			QMessageBox::warning(this, "RideWeather", "NO file selected. aborting.\n");
			return;
		}
		Config->setToken(token);
	}
	//try opening token
	StravaApi = new RideWeather::StravaApi_t(RideWeather::AccessToken_t(token), Config->cacheFolder);
	_stravaApiController = new RideWeather::StravaApiController_t(StravaApi);
	connect(_stravaApiController, &RideWeather::StravaApiController_t::AthleteReadySignal, this, &MainWindow::on_AthleteReady);
	_stravaApiController->GetAthlete(0);
}


void MainWindow::on_AthleteReady(const std::shared_ptr<RideWeather::Athlete_t> & Athlete)
{
	athlete = Athlete;	
	StravaApi->LoadAthleteActivitiesList(*athlete);
	ui->btn_GetList->setEnabled(true);
}

void MainWindow::on_btn_GetList_clicked()
{
	ui->btn_GetList->setEnabled(false);
	QCoreApplication::processEvents();
	StravaApi->RefreshAthleteActivities(*athlete);
	ui->btn_GetList->setEnabled(true);
	ui->btn_DownloadDetail->setEnabled(true);
	_activityModel = new RideWeather::ActivityModel(athlete);
	ui->tableView_Activities->setModel(_activityModel);
}

void MainWindow::on_btn_DownloadDetail_clicked()
{
	ui->btn_DownloadDetail->setEnabled(false);
	QCoreApplication::processEvents();
	StravaApi->GetAthleteActivityStreams(*athlete);
	ui->btn_DownloadDetail->setEnabled(true);

}

MainWindow::~MainWindow()
{
	delete ui;
	if (StravaApi!=nullptr)
		delete StravaApi;
}