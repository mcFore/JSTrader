#ifndef JSTRADINGCHART_H
#define JSTRADINGCHART_H
#include <qmainwindow.h>
#include <qdockwidget.h>
#include"JSMainChart.h"
#include"JSIndicatorChart.h"
class JSTradingChart :public QMainWindow
{
	Q_OBJECT
public:
	JSTradingChart();
	~JSTradingChart();
private:
	
	JSMainChart *mainchart;
	JSINDICATOR *indicatorchart;
	private slots:
	void generateset();
};
#endif