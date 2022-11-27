#pragma once
#include <QWidget>
#include <QtCharts/QChart>
#include "enum.h"
#include "StravaType.h"

namespace Ui { class SimplePlotWidget; };

class SimplePlotWidget : public QWidget
{
	Q_OBJECT

public:
	SimplePlotWidget(QWidget *parent = Q_NULLPTR);
	~SimplePlotWidget();
private slots:
	void on_dte_FromDate_dateTimeChanged(const QDateTime & datetime);
	void on_dte_ToDate_dateTimeChanged(const QDateTime& datetime);

	void on_cb_PlotType_currentIndexChanged(int index);
	void on_cb_activityType_currentIndexChanged(int index);
	void on_cb_xAxes_currentIndexChanged(int index);
	void on_cb_yAxes_currentIndexChanged(int index);
	void on_cb_filter1Op_currentIndexChanged(int index);
	void on_cb_filter1Prop_currentIndexChanged(int index);
	void on_cb_filter2Op_currentIndexChanged(int index);
	void on_cb_filter2Prop_currentIndexChanged(int index);
public slots:
	void on_AthleteReady(const std::shared_ptr<RideWeather::Athlete_t>& Athlete);
	void on_ListReady();
private:
	Ui::SimplePlotWidget *ui;
	std::shared_ptr<RideWeather::Athlete_t> _athlete;
	QChart * _chart;
	void RefreshChart();
};

BETTER_ENUM(Property_t, char, Distance, MovingTime, TotalElevation, StartDate, AchievementCount, AverageSpeed, AverageHeartRate, SufferScore)
