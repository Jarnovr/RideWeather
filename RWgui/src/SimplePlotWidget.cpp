#include "SimplePlotWidget.h"
#include "ui_SimplePlotWidget.h"

SimplePlotWidget::SimplePlotWidget(QWidget* parent)
	: QWidget(parent)
{
	ui = new Ui::SimplePlotWidget();
	ui->setupUi(this);

	QSizePolicy po;
	po.setHorizontalPolicy(QSizePolicy::Preferred);
	po.setVerticalPolicy(QSizePolicy::Preferred);
	po.setVerticalStretch(1);
	po.setHorizontalStretch(1);

	this->setSizePolicy(po);
}

SimplePlotWidget::~SimplePlotWidget()
{
	delete ui;
}
