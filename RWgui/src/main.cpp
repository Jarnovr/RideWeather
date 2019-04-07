#include "MainWindow.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
	
	std::ofstream out("RWGui_logfile.txt");
	std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
	std::cerr.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
	
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();

}