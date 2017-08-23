#ifndef JSMAINCHART_H
#define JSMAINCHART_H
#include<map>
#include<mutex>
#include<qdatetime.h>
#include<qlist.h>
#include<QtCharts\qlineseries.h>
#include<QtCharts\qcandlestickseries.h>
#include<QtCharts\qcandlestickset.h>
#include<QtCharts\qvalueaxis.h>
#include<QtCharts\qchart.h>
#include <qpen.h>
#include<qgraphicsview.h>
#include<qgraphicsscene.h>
#include"qbarcategoryaxis.h"
#include"Cursor.h"
using namespace QtCharts;
class JSMainChart :public QGraphicsView
{
	Q_OBJECT
signals :
	void sendKey(QKeyEvent *event);
public:
	explicit JSMainChart(const QString &symbol);
	~JSMainChart();
protected:
	void resizeEvent(QResizeEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
private:
	QGraphicsScene scene;
	QChart *chart;
	JSCursor *jscursor;
	bool is_press = false;
	int pointBegin = 0;
	std::map<QString, QLineSeries*>name_mapping_lineseries;
	std::map<QString, int>timestamp_mapping_Xpoint;

	QBarCategoryAxis *axisX = NULL;
	QValueAxis *axisY = NULL;
	QCandlestickSeries *candlestickseries = NULL;
	QStringList category;

	QGraphicsTextItem infoTextItem;
	public slots:
	void keyPressEvent(QKeyEvent *event);
	void addCandleStickSet(double open, double high, double low, double close, double timestamp);//tiemstamp needs time_t *1000
	void addSeriesPoint(double value, double timestamp, const QString &name);
	void createLineSeries(const QString &name, const QPen& pen);
	void finished();
private:
	void resetRange();
};
#endif