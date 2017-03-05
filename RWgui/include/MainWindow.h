#pragma once

#include <QMainWindow>

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

protected:
	Ui::MainWindow *ui;
};