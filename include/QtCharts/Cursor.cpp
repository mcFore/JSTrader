#include"Cursor.h"
#include <qdebug.h>
JSCursor::JSCursor(QChart *chart)
{
	x_text = new QGraphicsSimpleTextItem(chart);
	y_text = new QGraphicsSimpleTextItem(chart);

	x_line = new QGraphicsLineItem(chart);
	y_line = new QGraphicsLineItem(chart);
}

JSCursor::~JSCursor()
{
	delete x_text;
	delete y_text;
	delete x_line;
	delete y_line;
}

void JSCursor::setText(const QString &x_value, const QString &y_value)
{
	x_text->setText(x_value);
	y_text->setText(y_value);
}

void JSCursor::hide()
{
	x_line->hide();
	y_line->hide();
	x_text->hide();
	y_text->hide();
}

void JSCursor::show()
{
	x_line->show();
	y_line->show();
	x_text->show();
	y_text->show();
}

void JSCursor::setPos(double cursor_x, double cursor_y, QChart *chart)
{
	if (cursor_x<chart->plotArea().right() && cursor_x>chart->plotArea().left() && cursor_y>chart->plotArea().top() && cursor_y<chart->plotArea().bottom())
	{
		qreal LeftPart = cursor_x - chart->plotArea().left();
		qreal RightPart = chart->plotArea().right() - cursor_x;

		qreal TopPart = cursor_y - chart->plotArea().top();
		qreal BottomPart = chart->plotArea().bottom() - cursor_y;

		this->x_line->setLine(-LeftPart, 0, RightPart, 0);
		this->y_line->setLine(0, -TopPart, 0, BottomPart);

		x_line->setPos(cursor_x, cursor_y);
		y_line->setPos(cursor_x, cursor_y);

		x_text->setPos(cursor_x, (cursor_y + BottomPart)*0.95);
		y_text->setPos((cursor_x + RightPart)*0.95, cursor_y);

	}

	qDebug() << "cursor_x :" << cursor_x << "cursor_y" << cursor_y << "top" << chart->plotArea().top() << "bottom" << chart->plotArea().bottom();
}