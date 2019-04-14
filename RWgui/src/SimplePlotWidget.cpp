#include "SimplePlotWidget.h"
#include "ui_SimplePlotWidget.h"

#include "Common.h"

using namespace RideWeather;

SimplePlotWidget::SimplePlotWidget(QWidget* parent)
	: QWidget(parent), _athlete(nullptr), _chart(nullptr), ui(nullptr)
{
	ui = new Ui::SimplePlotWidget();
	ui->setupUi(this);

	QSizePolicy po;
	po.setHorizontalPolicy(QSizePolicy::Preferred);
	po.setVerticalPolicy(QSizePolicy::Preferred);
	po.setVerticalStretch(1);
	po.setHorizontalStretch(1);

	this->setSizePolicy(po);

	// fill property dropdowns
	ui->cb_xAxes->clear();
	ui->cb_yAxes->clear();
	ui->cb_filter1Prop->clear();
	ui->cb_filter2Prop->clear();
	for (auto prop : Property_t::_values())
	{
		ui->cb_xAxes->addItem(QString::fromUtf8(prop._to_string()));
		ui->cb_yAxes->addItem(QString::fromUtf8(prop._to_string()));
		ui->cb_filter1Prop->addItem(QString::fromUtf8(prop._to_string()));
		ui->cb_filter2Prop->addItem(QString::fromUtf8(prop._to_string()));
	}

	//fill activity dropDown
	for (auto type : ActivityType_t::_values())
	{
		ui->cb_activityType->addItem(QString::fromUtf8(type._to_string()));
	}

	_chart = ui->graphicsView->chart();
	_chart->setTitle("Acitivity plot");
}

SimplePlotWidget::~SimplePlotWidget()
{
	delete ui;
}

void SimplePlotWidget::on_dte_FromDate_dateTimeChanged(const QDateTime& datetime)
{
	ui->dte_ToDate->setMinimumDateTime(datetime);
	RefreshChart();
}

void SimplePlotWidget::on_dte_ToDate_dateTimeChanged(const QDateTime& datetime)
{
	ui->dte_FromDate->setMaximumDateTime(datetime);
	RefreshChart();
}

void SimplePlotWidget::on_cb_PlotType_currentIndexChanged(int index)
{
	RefreshChart();
}

void SimplePlotWidget::on_cb_activityType_currentIndexChanged(int index)
{
	RefreshChart();
}

void SimplePlotWidget::on_cb_xAxes_currentIndexChanged(int index)
{
	RefreshChart();
}

void SimplePlotWidget::on_cb_yAxes_currentIndexChanged(int index)
{
	RefreshChart();
}

void SimplePlotWidget::on_cb_filter1Op_currentIndexChanged(int index)
{
	RefreshChart();
}

void SimplePlotWidget::on_cb_filter1Prop_currentIndexChanged(int index)
{
	RefreshChart();
}

void SimplePlotWidget::on_cb_filter2Op_currentIndexChanged(int index)
{
	RefreshChart();
}

void SimplePlotWidget::on_cb_filter2Prop_currentIndexChanged(int index)
{
	RefreshChart();
}

void SimplePlotWidget::on_AthleteReady(const std::shared_ptr<Athlete_t>& Athlete)
{
	_athlete = Athlete;
}

void SimplePlotWidget::on_ListReady()
{
	Times_t min = boost::date_time::max_date_time;
	Times_t max = boost::date_time::min_date_time;

	//find first and last event.
	for (auto pair : _athlete->activities)
	{
		auto activity = pair.second;
		auto actDate = activity.start_date;
		if (actDate < min)
			min = actDate;
		if (actDate > max)
			max = actDate;
	}

	ui->dte_FromDate->setMinimumDateTime(ToQDateTime(min));
	ui->dte_ToDate->setMinimumDateTime(ToQDateTime(min));
	ui->dte_FromDate->setMaximumDateTime(ToQDateTime(max));
	ui->dte_ToDate->setMaximumDateTime(ToQDateTime(max));
	ui->dte_ToDate->setDateTime(ToQDateTime(max));
	RefreshChart();
}

inline double GetPropertyFromActivity(const Property_t& prop, const Activity_t& activity)
{
	double result;
	switch (prop)
	{
	case Property_t::Distance:
		result = activity.distance;
		break;
	case Property_t::MovingTime:
		result = activity.moving_time;
		break;
	case Property_t::TotalElevation:
		result = activity.total_elevation_gain;
		break;
	case Property_t::StartDate:
		result = activity.start_date.date().day_of_year();
		break;
	case Property_t::AchievementCount:
		result = activity.achievement_count;
		break;
	case Property_t::AverageSpeed:
		result = activity.average_speed;
		break;
	case Property_t::AverageHeartRate:
		result = activity.average_heartrate;
		break;
	case Property_t::SufferScore:
		result = activity.suffer_score;
		break;
	}

	return result;
}

void SimplePlotWidget::RefreshChart()
{
	try
	{
		if (_athlete == nullptr)
			return;

		//clean chart
		_chart->removeAllSeries();

		//parse settings
		auto xProp = Property_t::_from_string_nocase(qPrintable(ui->cb_xAxes->currentText()));
		auto yProp = Property_t::_from_string_nocase(qPrintable(ui->cb_yAxes->currentText()));
		auto activityType = ActivityType_t::_from_string_nocase(qPrintable(ui->cb_activityType->currentText()));
		auto fromDate = ToTimes_t(ui->dte_FromDate->dateTime());
		auto toDate = ToTimes_t(ui->dte_ToDate->dateTime());

		std::cout << "plot type: " << ui->cb_PlotType->currentText().toStdString() << std::endl;
		//if (ui->cb_PlotType->currentText().compare(QString::fromStdString("Scatter Plot")))
		if (true)
		{
			QScatterSeries* series = nullptr;
			int year = 0;
			for (auto pair : _athlete->activities)
			{
				auto activity = pair.second;
				auto actDate = activity.start_date;
				if (actDate < fromDate || actDate > toDate)
					continue;
				// Start new series every year
				if (actDate.date().year() > year)
				{
					if (series != nullptr) _chart->addSeries(series);
					series = new QScatterSeries();
					series->setMarkerSize(10);
					year = actDate.date().year();
					series->setName(QString::number(year));
				}

				auto x = GetPropertyFromActivity(xProp, activity);
				auto y = GetPropertyFromActivity(yProp, activity);
				series->append(x, y);
			}
			_chart->addSeries(series);
			_chart->createDefaultAxes();
			_chart->legend()->setAlignment(Qt::AlignBottom);
			_chart->legend()->setVisible(true);

		}
		else
		{
			return;
		}
	}
	catch (const std::exception & e)
	{
		std::cout << "Error plotting chart:" << std::endl;
		std::cout << e.what() << std::endl;
	}
}
