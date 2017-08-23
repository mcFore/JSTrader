#include"JSIndicatorChart.h"
JSINDICATOR::JSINDICATOR()
{
	this->setScene(&scene);
	this->setMouseTracking(true);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->chart = new QChart;
	this->jscursor = new JSCursor(this->chart);
	this->scene.addItem(chart);
	this->setRenderHint(QPainter::Antialiasing);

	//create axisX and axisY
	this->axisX = new QBarCategoryAxis;
	QStringList categories;
	this->axisX->setCategories(categories);
	this->axisX->setTickCount(5);
	this->axisX->setGridLineVisible(false);

	this->axisY = new QValueAxis;

	this->chart->setAxisX(axisX);
	this->chart->setAxisY(axisY);

	this->showMaximized();
}

JSINDICATOR::~JSINDICATOR()
{

	delete this->jscursor;

	delete this->axisX;
	delete this->axisY;

	for (std::map<QString, QLineSeries*>::iterator iter = this->name_mapping_lineseries.begin(); iter != this->name_mapping_lineseries.end(); ++iter)
	{
		delete iter->second;
	}

	delete this->chart;
}

void JSINDICATOR::resizeEvent(QResizeEvent *event)
{
	scene.setSceneRect(QRect(QPoint(0, 0), event->size()));
	this->chart->resize(event->size());
	QGraphicsView::resizeEvent(event);
}

void JSINDICATOR::checkLastCategory(const QString &X_value)
{
	QString lastCategory = this->axisX->max();
	if (lastCategory != X_value)
	{
		this->axisX->append(X_value);
		timestamp_mapping_Xpoint[X_value] = pointBegin;
		++pointBegin;

		if (pointBegin > 40)
		{
			qreal x = this->chart->plotArea().width() / (this->axisX->count() + 2);
			this->chart->scroll(x, 0);
		}
	}
}

void JSINDICATOR::createLineSeries(const QString &name, const QPen& pen)
{
	QLineSeries *lineseries = new QLineSeries;
	lineseries->setUseOpenGL(true);
	lineseries->setName(name);
	lineseries->setPen(pen);
	this->name_mapping_lineseries[name] = lineseries;
}

void JSINDICATOR::addSeriesToChart(const QString &name)
{
	this->chart->addSeries(this->name_mapping_lineseries[name]);
	this->name_mapping_lineseries[name]->attachAxis(this->axisX);
	this->name_mapping_lineseries[name]->attachAxis(this->axisY);
}

void JSINDICATOR::addSeriesPoint(double value, double timestamp, const QString &name)
{
	std::unique_lock<std::mutex>lck(mtx);
	const QString X_value = QDateTime::fromMSecsSinceEpoch(timestamp).toString("yyyy-MM-dd hh:mm:ss");
	checkLastCategory(X_value);
	this->name_mapping_lineseries[name]->append(this->timestamp_mapping_Xpoint[X_value], value);

	QValueAxis *axisY = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).at(0));
	axisY->setMax(qMax(value * 1.01, this->axisY->max()));
	axisY->setMin(qMin(value * 0.99, this->axisY->min()));
}

void JSINDICATOR::addSeriesPoint(double value, const QString &datetimestr, const QString &name)
{
	std::unique_lock<std::mutex>lck(mtx);
	const QString X_value = datetimestr;
	checkLastCategory(X_value);
	this->name_mapping_lineseries[name]->append(this->timestamp_mapping_Xpoint[X_value], value);

	QValueAxis *axisY = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).at(0));
	axisY->setMax(qMax(value * 1.01, this->axisY->max()));
	axisY->setMin(qMin(value * 0.99, this->axisY->min()));
}

void JSINDICATOR::keyPressEvent(QKeyEvent *event)
{
	if (hasFocus())
	{
		emit  sendKey(event);
	}
	switch (event->key()) {
	case Qt::Key_Left:
		this->chart->scroll(-10, 0);
		break;
	case Qt::Key_Right:
		this->chart->scroll(10, 0);
		break;
	case Qt::Key_Up:
		this->chart->zoomIn();
		break;
	case Qt::Key_Down:
		this->chart->zoomOut();
		break;
	default:
		QGraphicsView::keyPressEvent(event);
		break;
	}
}

void JSINDICATOR::mouseMoveEvent(QMouseEvent *event)
{
	if (is_press)
	{
		const  int Xpoint = (int)this->chart->mapToValue(event->pos()).x();
		if (Xpoint <= this->axisX->count() - 1 && Xpoint >= 0)
		{
			jscursor->setPos(this->mapToScene(event->pos()).x(), this->mapToScene(event->pos()).y(), this->chart);
			jscursor->setText(this->axisX->at(Xpoint), QString("Y: %1").arg(this->chart->mapToValue(event->pos()).y()));
		}
	}
	QGraphicsView::mouseMoveEvent(event);
}

void JSINDICATOR::mousePressEvent(QMouseEvent *event)
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
		const  int Xpoint = (int)this->chart->mapToValue(event->pos()).x();
		if (Xpoint <= this->axisX->count() - 1 && Xpoint >= 0)
		{
			jscursor->setPos(this->mapToScene(event->pos()).x(), this->mapToScene(event->pos()).y(), this->chart);
			jscursor->setText(this->axisX->at(Xpoint), QString("Y: %1").arg(this->chart->mapToValue(event->pos()).y()));
		}
	}
}
