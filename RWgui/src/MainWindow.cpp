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
#include "SimplePlotWidget.h"

RideWeather::Configuration* Config;

MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent), ui(new Ui::MainWindow), _stravaApiController(nullptr), _activityModel(nullptr), StravaApi(nullptr)
{
	QCoreApplication::setOrganizationDomain("roosmalen.org");
	QCoreApplication::setOrganizationName("JarnoSoft");
	QCoreApplication::setApplicationName("RideWeather");
	
	Config = new RideWeather::Configuration();
	ui->setupUi(this);
	ui->lbl_Output->setText(QString::fromStdString(Config->configFileName.string()));
	progressBar = new QProgressBar(ui->statusbar);
	progressBar->setAlignment(Qt::AlignRight);
	progressBar->setMaximumWidth(500);
	ui->statusbar->addPermanentWidget(progressBar);
	progressBar->setVisible(false);
}

void MainWindow::on_actionExit_triggered()
{
	QCoreApplication::quit();
}

void MainWindow::on_actionAbout_triggered()
{
	QMessageBox::about(this, "RideWeather", "RideWeather is an application for analysing Strava Activities with respect to weather conditions occuring during activities.");
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
	connect(_stravaApiController->Worker(), &RideWeather::StravaApiWorker::AthleteReady, this, &MainWindow::on_AthleteReady);
	connect(_stravaApiController->Worker(), &RideWeather::StravaApiWorker::AthleteReady, ui->plotWidget, &SimplePlotWidget::on_AthleteReady);
	connect(this, &MainWindow::GetAthleteSignal, _stravaApiController->Worker(), &RideWeather::StravaApiWorker::GetAthlete);
	connect(_stravaApiController->Worker(), &RideWeather::StravaApiWorker::SignalProgress, this, &MainWindow::on_SetProgress);
	ui->statusbar->showMessage("Loading athlete...", 300);
	emit GetAthleteSignal(0);
}


void MainWindow::on_AthleteReady(const std::shared_ptr<RideWeather::Athlete_t>& Athlete)
{
	athlete = Athlete;
	ui->btn_GetList->setEnabled(true);
	progressBar->setVisible(false);
	ui->statusbar->clearMessage();
}

void MainWindow::on_btn_GetList_clicked()
{
	ui->btn_GetList->setEnabled(false);

	connect(_stravaApiController->Worker(), &RideWeather::StravaApiWorker::ListReady, this, &MainWindow::on_ListReady);
	connect(_stravaApiController->Worker(), &RideWeather::StravaApiWorker::ListReady, ui->plotWidget, &SimplePlotWidget::on_ListReady);
	connect(this, &MainWindow::GetListSignal, _stravaApiController->Worker(), &RideWeather::StravaApiWorker::GetList);
	ui->statusbar->showMessage("Getting athlete activities...", 1200);
	emit GetListSignal(athlete);
}

void MainWindow::on_ListReady()
{
	ui->btn_GetList->setEnabled(true);
	ui->btn_DownloadDetail->setEnabled(true);
	_activityModel = new RideWeather::ActivityModel(athlete);
	ui->tableView_Activities->setModel(_activityModel);
	progressBar->setVisible(false);
	ui->statusbar->clearMessage();
}

void MainWindow::on_btn_DownloadDetail_clicked()
{
	ui->btn_DownloadDetail->setEnabled(false);
	connect(_stravaApiController->Worker(), &RideWeather::StravaApiWorker::DownloadDetailReady, this, &MainWindow::on_DownloadDetailComplete);
	connect(this, &MainWindow::DownloadDetailSignal, _stravaApiController->Worker(), &RideWeather::StravaApiWorker::DownloadDetail);
	ui->statusbar->showMessage("Downloading activity details...", 3600);
	emit DownloadDetailSignal(athlete);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
	if (index != 2)
	{
		return; //only for plot tab
	}

	// Fill Dynamic inputs:

}

void MainWindow::on_DownloadDetailComplete()
{
	ui->btn_DownloadDetail->setEnabled(true);
	progressBar->setVisible(false);
	ui->statusbar->clearMessage();
}

MainWindow::~MainWindow()
{
	delete ui;
	if (StravaApi != nullptr)
		delete StravaApi;
}