#include"JSMainChart.h"
#include<qdebug.h>
JSMainChart::JSMainChart(const QString &symbol)
{
	this->setScene(&scene);
	this->setMouseTracking(true);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->chart = new QChart;
	this->jscursor = new JSCursor(this->chart);
	this->scene.addItem(chart);
	infoTextItem.setParentItem(this->chart);
	this->setRenderHint(QPainter::Antialiasing);

	//create a empty candlestickseries
	this->candlestickseries = new QCandlestickSeries;
	this->candlestickseries->setName(symbol);
	this->candlestickseries->setUseOpenGL(true);
	this->candlestickseries->setIncreasingColor(Qt::red);
	this->candlestickseries->setDecreasingColor(Qt::darkGreen);
	this->candlestickseries->setBodyOutlineVisible(false);
}

JSMainChart::~JSMainChart()
{
	delete this->jscursor;

	if (this->axisX != NULL && this->axisY != NULL)
	{
		delete this->axisX;
		delete this->axisY;
	}

	QList<QCandlestickSet *>list = this->candlestickseries->sets();
	delete this->candlestickseries;

	for (int i = 0; i < list.size(); ++i)
	{
		delete list[i];
	}

	for (std::map<QString, QLineSeries*>::iterator iter = this->name_mapping_lineseries.begin(); iter != this->name_mapping_lineseries.end(); ++iter)
	{
		if (iter->second != nullptr)
		{
			delete iter->second;
		}
	}
	this->chart->deleteLater();

}

void JSMainChart::resizeEvent(QResizeEvent *event)
{
	scene.setSceneRect(QRect(QPoint(0, 0), event->size()));
	this->chart->resize(event->size());
	infoTextItem.setPos(this->chart->rect().left(), this->chart->rect().top()*0.95);
	QGraphicsView::resizeEvent(event);
}

void JSMainChart::addCandleStickSet(double open, double high, double low, double close, double timestamp)
{
	const QString X_value = QDateTime::fromMSecsSinceEpoch(timestamp).toString("yyyy-MM-dd hh:mm:ss");
	QCandlestickSet *set = new QCandlestickSet(open, high, low, close, timestamp);
	this->candlestickseries->append(set);
	this->category << X_value;
	timestamp_mapping_Xpoint[X_value] = timestamp_mapping_Xpoint.size();
}

void JSMainChart::createLineSeries(const QString &name, const QPen& pen)
{
	QLineSeries *lineseries = new QLineSeries;
	lineseries->setName(name);
	lineseries->setUseOpenGL(true);
	lineseries->setPen(pen);
	this->name_mapping_lineseries[name] = lineseries;
	this->chart->addSeries(lineseries);
	this->chart->setAxisX(this->axisX, lineseries);
	this->chart->setAxisY(this->axisY, lineseries);
}

void JSMainChart::addSeriesPoint(double value, double timestamp, const QString &name)
{

}

void JSMainChart::keyPressEvent(QKeyEvent *event)
{
	if (hasFocus())
	{
		emit  sendKey(event);
	}
	int movestepleft1 = timestamp_mapping_Xpoint[this->axisX->min()] - 10;
	int movestepright1 = timestamp_mapping_Xpoint[this->axisX->max()] - 10;
	int movestepleft2 = timestamp_mapping_Xpoint[this->axisX->min()] + 10;
	int movestepright2 = timestamp_mapping_Xpoint[this->axisX->max()] + 10;
	switch (event->key()) {
	case Qt::Key_Left:
		if (movestepleft1 >= 0 && movestepright1 < timestamp_mapping_Xpoint.size() && movestepright1 >= 0)
		{
			this->axisX->setRange(this->axisX->at(movestepleft1), this->axisX->at(movestepright1));
		}
		else
		{
			this->axisX->setRange(0, this->axisX->at(movestepright1 + 10));
		}
		this->chart->update();
		resetRange();
		break;
	case Qt::Key_Right:
		if (movestepleft2 >= 0 && movestepright2 < timestamp_mapping_Xpoint.size() && movestepleft2<timestamp_mapping_Xpoint.size())
		{
			this->axisX->setRange(this->axisX->at(movestepleft2), this->axisX->at(movestepright2));
		}
		else
		{
			this->axisX->setRange(this->axisX->at(movestepleft2 - 10), this->axisX->at(this->axisX->categories().size() - 1));
		}
		this->chart->update();
		resetRange();
		break;
	case Qt::Key_Up:
		this->chart->zoomIn();
		this->chart->update();
		resetRange();
		break;
	case Qt::Key_Down:
		this->chart->zoomOut();
		this->chart->update();
		resetRange();
		break;
	default:
		QGraphicsView::keyPressEvent(event);
		break;
	}
}

void JSMainChart::mouseMoveEvent(QMouseEvent *event)
{

	if (is_press)
	{
		/*const  int Xpoint = (int)this->chart->mapToValue(event->pos()).x();
		if (Xpoint <= this->axisX->count() - 1 && Xpoint >= 0)
		{
		jscursor->setPos(this->mapToScene(event->pos()).x(), this->mapToScene(event->pos()).y(), this->chart);
		}*/
	}
	QGraphicsView::mouseMoveEvent(event);
}

void JSMainChart::finished()
{
	this->setFocus();
	this->chart->addSeries(candlestickseries);
	this->chart->createDefaultAxes();

	//set axisX Y


	this->axisX = new QBarCategoryAxis;
	this->axisX->setCategories(this->category);
	this->axisX->setTickCount(5);
	this->axisX->setGridLineVisible(false);
	this->axisX->setRange(this->category.front(), this->category.back());
	this->chart->setAxisX(axisX, candlestickseries);

	axisY = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).at(0));

	QList<QCandlestickSet *>list = this->candlestickseries->sets();

	double max;
	double min;
	for (int i = 0; i < list.size(); ++i)
	{
		if (i != 0)
		{
			max = qMax(max, list[i]->high());
			min = qMin(min, list[i]->low());
		}
		else
		{
			max = list[i]->high();
			min = list[i]->low();
		}

		if (i == list.size() - 101)
		{
			//获取第倒数100个的坐标
			//this->chart->mapToPosition(QPointF(this->axisX->at(i), 0));
		}
	}

	axisY->setRange(min, max);
}

void JSMainChart::mousePressEvent(QMouseEvent *event)
{
	if (is_press)
	{
		is_press = false;
		jscursor->hide();
	}
	else
	{
		is_press = true;
		jscursor->show();
	}
}

void JSMainChart::resetRange()
{
	int LeftNumber = timestamp_mapping_Xpoint[this->axisX->min()];
	int RightNumber = timestamp_mapping_Xpoint[this->axisX->max()];
	double max;
	double min;
	QList<QCandlestickSet *>list = this->candlestickseries->sets();
	for (int i = LeftNumber; i < RightNumber; ++i)
	{
		if (i != LeftNumber)
		{
			max = qMax(max, list[i]->high());
			min = qMin(min, list[i]->low());
		}
		else
		{
			max = list[i]->high();
			min = list[i]->low();
		}
	}
	this->axisY->setRange(min, max);
}