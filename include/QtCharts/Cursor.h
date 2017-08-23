#ifndef JSCURSOR_H
#define JSCURSOR_H
#include<qgraphicsitem.h>
#include<QtCharts/qchart.h>
using namespace QtCharts;
class JSCursor
{
public:
	JSCursor(QChart *chart);
	~JSCursor();
	void setText(const QString &x_value, const QString &y_value);
	void setPos(double cursor_x, double cursor_y, QChart *chart);
	void hide();
	void show();
private:
	QGraphicsSimpleTextItem *x_text;
	QGraphicsSimpleTextItem *y_text;

	QGraphicsLineItem *x_line;
	QGraphicsLineItem *y_line;
};
#endif