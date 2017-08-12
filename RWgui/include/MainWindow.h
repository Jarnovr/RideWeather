#pragma once

#pragma warning(disable:4127)
#include <QMainWindow>
#pragma warning (default: 4127)
#include "StravaType.h"
#include "StravaApi.h"
#include "ActivityModel.h"
#include "StravaApiController.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void on_actionExit_triggered();
	void on_actionAbout_triggered();
	void on_actionAbout_QT_triggered();
	void on_btn_Load_Token_clicked();
	void on_btn_GetList_clicked();
	void on_btn_DownloadDetail_clicked();
public slots:
	void on_AthleteReady(const std::shared_ptr<RideWeather::Athlete_t> & Athlete);
protected:
	Ui::MainWindow *ui;
	std::shared_ptr<RideWeather::Athlete_t> athlete;
	RideWeather::StravaApiController_t *_stravaApiController;
	RideWeather::StravaApi_t * StravaApi;
	RideWeather::ActivityModel *_activityModel;
};

#pragma warning(disable:4127)