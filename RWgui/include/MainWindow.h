#pragma once

#pragma warning(disable:4127)
#include <QMainWindow>
#pragma warning (default: 4127)
#include "StravaType.h"
#include "StravaApi.h"

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
protected:
	Ui::MainWindow *ui;
	std::shared_ptr<RideWeather::Athlete_t> athlete;
	RideWeather::AccessToken_t *access_token;
	RideWeather::StravaApi_t *StravaApi;
};

#pragma warning(disable:4127)