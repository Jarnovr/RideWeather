#pragma once

#pragma warning(disable:4127)
#include <QMainWindow>
#pragma warning (default: 4127)
#include "StravaType.h"
#include "StravaApi.h"
#include "AthleteModelView.h"

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
protected:
	Ui::MainWindow *ui;
	std::shared_ptr<RideWeather::Athlete_t> athlete;
	RideWeather::StravaApi_t *StravaApi;
	RideWeather::ActivityModel *_activityModel;
};

#pragma warning(disable:4127)