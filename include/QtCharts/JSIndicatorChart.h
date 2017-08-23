#ifndef JSINDICATOR_H
#define JSINDICATOR_H
#include<map>
#include<mutex>
#include<qdatetime.h>
#include<qlist.h>
#include<QtCharts\qlineseries.h>
#include<QtCharts\qvalueaxis.h>
#include<QtCharts\qchart.h>
#include <qpen.h>
#include<qgraphicsview.h>
#include<qgraphicsscene.h>
#include"qbarcategoryaxis.h"
#include"Cursor.h"
using namespace QtCharts;
class JSINDICATOR :public QGraphicsView
{
	Q_OBJECT
signals :
	void sendKey(QKeyEvent *event);
public:
	 JSINDICATOR();
	 ~JSINDICATOR();
protected:
	void resizeEvent(QResizeEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);

private:
	QGraphicsScene scene;
	QChart *chart;
	JSCursor *jscursor;
	bool is_press = false;
	std::mutex mtx;
	int pointBegin = 0;
	std::map<QString, QLineSeries*>name_mapping_lineseries;
	std::map<QString, int>timestamp_mapping_Xpoint;

	QBarCategoryAxis *axisX;
	QValueAxis *axisY;

	void checkLastCategory(const QString &X_value);
	public slots:
	void keyPressEvent(QKeyEvent *event);
	void addSeriesPoint(double value, double timestamp, const QString &name);
	void addSeriesPoint(double value, const QString &datetimestr, const QString &name);
	void createLineSeries(const QString &name, const QPen& pen);
	void addSeriesToChart(const QString &name);
};
#endif