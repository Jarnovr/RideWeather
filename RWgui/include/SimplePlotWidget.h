#pragma once

#include <QWidget>
namespace Ui { class SimplePlotWidget; };

class SimplePlotWidget : public QWidget
{
	Q_OBJECT

public:
	SimplePlotWidget(QWidget *parent = Q_NULLPTR);
	~SimplePlotWidget();

private:
	Ui::SimplePlotWidget *ui;
};
