#include "sliderctrarea.h"
//#include "mypushbutton.h"
#include "mysort.h"
#include <QtGui/QResizeEvent>
#include <QtWidgets/QGraphicsScene>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QAreaSeries>
#include <QtWidgets/QGraphicsTextItem>
#include "callout.h"
#include <QtGui/QMouseEvent>
#include <QtWidgets/QGridLayout>
#include <QDebug>
#include <QSizeGrip>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#define MaxPoint 30
#define minWidth 2

sliderCtrArea::sliderCtrArea(QWidget *parent)
    : QGraphicsView(new QGraphicsScene, parent),
      m_coordX(0),
      m_coordY(0),
      m_chart(0),
      m_tooltip(0),
      threshold_L(0),
      threshold_R(0)
{
    setDragMode(QGraphicsView::NoDrag);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

//    QGraphicsScene scene;

    // create chart
    m_chart = new QChart;
//    m_chart = createSplineChart();
    m_chart->setMinimumSize(640, 480);
//    m_chart->setTitle("Hover the line to show callout. Drag the line to resize it's width");
    m_chart->legend()->hide();
    m_chart->setContentsMargins(0,0,0,0);

    QLineSeries *series = new QLineSeries;
    series->append(1, 3);
    series->append(4, 5);
    series->append(5, 4.5);
    series->append(7, 1);
    series->append(11, 2);
    m_chart->addSeries(series);

    //create axis
    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, 12);
    axisX->setTickCount(13);
    axisX->setLinePen(series->pen());

    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0,5);
    axisY->setLabelFormat("%.2f");
    axisY->setLinePenColor(series->pen().color());

//    m_chart->addAxis(axisX, Qt::AlignBottom);
//    m_chart->addAxis(axisY, Qt::AlignLeft);
//    series->attachAxis(axisX);
//    series->attachAxis(axisY);
//    lowerSeries->attachAxis(axisX);
//    lowerSeries->attachAxis(axisY);

//    m_chart->createDefaultAxes();
//    m_chart->axisX()->setRange(0, 12);
//    m_chart->axisY()->setRange(0, 5);
    m_chart->setAcceptHoverEvents(true);

    //create AreaSeries
    QLineSeries *lowerSeries = 0;
    //*****  gradient area *******************************************
    QAreaSeries *area = new QAreaSeries(series, lowerSeries);
    QPen pen(0x059605);
    pen.setWidth(1);
//    area->setPen(pen);

    QLinearGradient gradient(QPointF(0, 0), QPointF(0, 1));
//    gradient.setColorAt(0.0, 0x3cc63c);
//    gradient.setColorAt(1.0, 0x26f626);
    gradient.setColorAt(0.0, Qt::darkCyan);  //darkCyan
    gradient.setColorAt(1.0, Qt::cyan);  //cyan
//    gradient.setColorAt(0.0, Qt::darkGreen);  //darkGreen
//    gradient.setColorAt(1.0, Qt::darkYellow);  //darkYellow
//    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
//    area->setBrush(gradient);
    area->setOpacity(0.75);
    area->attachAxis(axisX);
    area->attachAxis(axisY);
    axisX->setLinePenColor(series->pen().color());

    m_chart->addSeries(area);
    //*****************************************************************
    m_chart->setAxisX(axisX, area);
    m_chart->setAxisY(axisY, area);
    setRenderHint(QPainter::Antialiasing);
    scene()->addItem(m_chart);

    /*
     * UI background setting
     *ChartThemeLight, ChartThemeDark, ChartThemeBlueCerulean, ChartThemeBrownSand...
    */
    m_chart->setTheme(QChart::ChartThemeBrownSand);
    this->setBackgroundRole(QPalette::Midlight);
//    QPalette pal = window()->palette();
//    //ChartThemeDark
//        pal.setColor(QPalette::Window, QRgb(0x121218));
//        pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
//    //ChartThemeBlueCerulean
//        pal.setColor(QPalette::Window, QRgb(0x40434a));
//        pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
//    window()->setPalette(pal);


    // x and y position on the 4 corner for the graph
    maxX = axisX->max();
    minX = axisX->min();
    maxY = axisY->max();
    minY = axisY->min();

        qDebug()<<"maxX:"<<maxX;
            qDebug()<<"minX:"<<minX;
                qDebug()<<"maxY:"<<maxY;
                    qDebug()<<"minY:"<<minY;

    chart_left_top.setX(minX);
    chart_left_top.setY(maxY);
    chart_right_bottom.setX(maxX);
    chart_right_bottom.setY(minY);


    //create rubberBand area
    QPalette rBandPal;
    rBandPal.setBrush(QPalette::Highlight, QBrush(Qt::blue));

    move_rubberBandArea = false;
    rubberBandArea = new QRubberBand(QRubberBand::Line, this);
    rubberBandArea->show();
    rubberBandArea->setMinimumWidth(2);

    move_rubberBand1 = false;
    rubberBand1 = new QRubberBand(QRubberBand::Line, this);
    rubberBand1->resize(10,10);
    rubberBand1->show();

    move_rubberBand2 = false;
    rubberBand2 = new QRubberBand(QRubberBand::Line, this);
    rubberBand2->resize(10,10);
    rubberBand2->show();

    //create window level button
    QString css("QPushButton , QLabel{"
                "color: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, stop:0 rgba(0, 0, 0, 200), stop:1 rgba(255, 255, 255, 200));"
                "background: qlineargradient( x1:0 y1:0, x2:0 y2:1, stop:0 gray, stop:1 lightGray);"
                "border: 1px solid;"
                "border-image: url(:/image/slider-handle.png);"
                "border-radius: 8px;"
                "max-width:15px;"
                "max-height:15px;"
                "min-width:15px;"
                "min-height:15px;"
                "}"

                "QPushButton:pressed , QLabel:hover{"
                "color: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, stop:0 rgba(0, 0, 0, 200), stop:1 rgba(255, 255, 255, 200));"
                "background: qlineargradient( x1:0 y1:0, x2:0 y2:1, stop:0 gray, stop:1 darkGray);"
                "border: 2px solid ;"
                "border-color: qlineargradient( x1:0 y1:0, x2:0 y2:1, stop:0 #6C6C6C, stop:1 darkGray);"
                "border-radius: 8px;"
                "max-width:15px;"
                "max-height:15px;"
                "min-width:15px;"
                "min-height:15px;"
                "}"
                );
    this->setStyleSheet(css);

//    m_wwBtn_L = sliderCtrArea::createWinBtn();
//    scene()->addWidget(m_wwBtn_L);

    move_winLevelBtn = false;
    m_winLevelBtn = new QRubberBand(QRubberBand::Rectangle, this);
    m_winLevelBtn->resize(10,10);
    m_winLevelBtn->show();


//    paintEllipse = new QPainter(this);
//    paintEllipse->setRenderHint(QPainter :: Antialiasing, true);

//    /***** draw Circle *****/
//    QPainterPath path;
//    path.addRoundRect(QRect(200, 60, 150, 150), 1000);
//    paintEllipse->setBrush(Qt::white);
//    paintEllipse->setPen(Qt::black);
//    paintEllipse->drawPath(path);

    //create obj to display chart_pos()
    m_coordX = new QGraphicsSimpleTextItem(m_chart);
    m_coordX->setPos(m_chart->size().width()/2 - 50, m_chart->size().height());
    m_coordX->setText("X: ");
    m_coordY = new QGraphicsSimpleTextItem(m_chart);
    m_coordY->setPos(m_chart->size().width()/2 + 50, m_chart->size().height());
    m_coordY->setText("Y: ");

    connect(series, &QLineSeries::clicked, this, &sliderCtrArea::keepCallout);
    connect(series, &QLineSeries::hovered, this, &sliderCtrArea::tooltip);

//    connect(series2, &QSplineSeries::clicked, this, &sliderCtrArea::keepCallout);
//    connect(series2, &QSplineSeries::hovered, this, &sliderCtrArea::tooltip);

    this->setMouseTracking(true);
}

//method for creating buttons
QPushButton *sliderCtrArea::createWinBtn() const
{
    QPushButton *windowBtn = new QPushButton(parentWidget());
    QString css("QPushButton {"
                "color: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, stop:0 rgba(0, 0, 0, 200), stop:1 rgba(255, 255, 255, 200));"
                "background: qlineargradient( x1:0 y1:0, x2:0 y2:1, stop:0 gray, stop:1 lightGray);"
                "border: 2px solid;"
                "border-color: qlineargradient( x1:0 y1:0, x2:0 y2:1, stop:0 #6C6C6C, stop:1 darkGray);"
                "border-radius: 8px;"
                "max-width:15px;"
                "max-height:15px;"
                "min-width:15px;"
                "min-height:15px;"
                "}"

                "QPushButton:pressed {"
                "color: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, stop:0 rgba(0, 0, 0, 200), stop:1 rgba(255, 255, 255, 200));"
                "background: qlineargradient( x1:0 y1:0, x2:0 y2:1, stop:0 gray, stop:1 darkGray);"
                "border: 2px solid ;"
                "border-color: qlineargradient( x1:0 y1:0, x2:0 y2:1, stop:0 #6C6C6C, stop:1 darkGray);"
                "border-radius: 8px;"
                "max-width:15px;"
                "max-height:15px;"
                "min-width:15px;"
                "min-height:15px;"
                "}"
                );
    windowBtn->setStyleSheet(css);
    return windowBtn;
}

void sliderCtrArea::resizeEvent(QResizeEvent *event)
{

    if (scene()) {
        scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
         m_chart->resize(event->size());
         m_coordX->setPos(m_chart->size().width()/2 - 50, m_chart->size().height() - 20);
         m_coordY->setPos(m_chart->size().width()/2 + 50, m_chart->size().height() - 20);

         chartHeight = m_chart->mapToPosition(chart_right_bottom).y()-m_chart->mapToPosition(chart_left_top).y()+2;
         chartWidth = m_chart->mapToPosition(chart_right_bottom).x()-m_chart->mapToPosition(chart_left_top).x()+1;
         rubberBandArea->setGeometry(m_chart->mapToPosition(chart_left_top).x(), m_chart->mapToPosition(chart_left_top).y(), 50, chartHeight);
         rubberBand2->setGeometry(rubberBandArea->pos().x()+rubberBandArea->width(), m_chart->mapToPosition(chart_left_top).y()-rubberBand2->height(), 10, 10);
         threshold_R = int(m_chart->mapToValue(rubberBand2->pos()).x());
         qDebug()<<"threshold2:"<<threshold_R;
         rubberBand1->setGeometry(rubberBandArea->pos().x()-rubberBand2->width(), m_chart->mapToPosition(chart_left_top).y()-rubberBand1->height(), 10, 10);
         threshold_L = int(m_chart->mapToValue(rubberBand1->pos()).x());
         qDebug()<<"threshold1:"<<threshold_L;
         m_winLevelBtn->setGeometry(rubberBandArea->pos().x()+rubberBandArea->width()*0.5-m_winLevelBtn->width()*0.5, m_chart->mapToPosition(chart_right_bottom).y(), 10, 10);
//         m_wwBtn_L->setGeometry(rubberBandArea->pos().x()+rubberBandArea->width(), m_chart->mapToPosition(chart_left_top).y()-rubberBand2->height(), 10, 10);

         const auto callouts = m_callouts;
         for (Callout *callout : callouts)
             callout->updateGeometry();
    }
    QGraphicsView::resizeEvent(event);
}

void sliderCtrArea::mousePressEvent(QMouseEvent *event)
{
    origin = event->pos();
    originBandWidth = rubberBandArea->width();
    m_coordX->setText(QString("X: %1").arg(m_chart->mapToValue(event->pos()).x()));
    m_coordY->setText(QString("Y: %1").arg(m_chart->mapToValue(event->pos()).y()));

    //if PressEvent pos() within range of button, let button movable
    if (rubberBand1->geometry().contains(event->pos()) & (rubberBandArea->size().width() >= minWidth))
    {
        rubberBand_offset_x = origin.x() - rubberBand1->pos().x();
        move_rubberBand1 = true;
        qDebug()<<"rbb1_press pos().x():"<<rubberBand1->pos().x();
    }
    //if PressEvent pos() within range of rubberBandArea, let rubberBandArea movable
    if (rubberBandArea->geometry().contains(event->pos()))
    {
        rubberBand_offset_x = origin.x() - rubberBandArea->pos().x();
        move_rubberBandArea = true;
        qDebug()<<"rbA_press pos().x():"<<rubberBandArea->pos().x();
        qDebug()<<"rubberBand_offset_x:"<<rubberBand_offset_x;
    }
    //if PressEvent pos() within range of button, let button movable
    if (rubberBand2->geometry().contains(event->pos()) & (rubberBandArea->size().width() >= minWidth) )
    {
        rubberBand_offset_x = origin.x() - rubberBand2->pos().x();
        move_rubberBand2 = true;
        qDebug()<<"rbb2_press pos().x():"<<rubberBand2->pos().x();
    }

    if (m_winLevelBtn->geometry().contains(event->pos()))
    {
        rubberBand_offset_x = origin.x() - m_winLevelBtn->pos().x();
        move_winLevelBtn = true;
        qDebug()<<"m_winLevelBtn pos().x():"<<m_winLevelBtn->pos().x();
    }

    QGraphicsView::mouseMoveEvent(event);
}

void sliderCtrArea::mouseMoveEvent(QMouseEvent *event)
{ 
    m_coordX->setText(QString("X: %1").arg(m_chart->mapToValue(event->pos()).x()));
    m_coordY->setText(QString("Y: %1").arg(m_chart->mapToValue(event->pos()).y()));

    if( move_rubberBand1 )
    {
        rubberBand1->move(event->pos().x() - rubberBand_offset_x, m_chart->mapToPosition(chart_left_top).y()-rubberBand1->height());
        rubberBandArea->setGeometry(QRect(rubberBand1->pos().x()+rubberBand1->width(), m_chart->mapToPosition(chart_left_top).y(), originBandWidth - event->pos().x() + origin.x(), chartHeight).normalized());
        m_winLevelBtn->setGeometry(rubberBandArea->pos().x()+rubberBandArea->width()*0.5-m_winLevelBtn->width()*0.5, m_chart->mapToPosition(chart_right_bottom).y(), 10, 10);

        if(rubberBand2->pos().x() < (rubberBand1->pos().x()+rubberBand1->width()))
        {
            move_rubberBand1 = false;
            rubberBand1->move(rubberBand2->pos().x()-minWidth-rubberBand1->width(), m_chart->mapToPosition(chart_left_top).y()-rubberBand1->height());
            rubberBandArea->setGeometry(QRect(rubberBand1->pos().x()+rubberBand1->width(), m_chart->mapToPosition(chart_left_top).y(), minWidth, chartHeight).normalized());
            m_winLevelBtn->setGeometry(rubberBandArea->pos().x()+rubberBandArea->width()*0.5-m_winLevelBtn->width()*0.5, m_chart->mapToPosition(chart_right_bottom).y(), 10, 10);
            threshold_L = int(m_chart->mapToValue(event->pos()).x());
            qDebug()<<"threshold1:"<<threshold_L;
        }
        if( (rubberBand2->pos().x()-rubberBandArea->pos().x()) > chartWidth)
        {
            move_rubberBand1 = false;
            rubberBand1->move(rubberBand2->pos().x()-chartWidth-rubberBand1->width(), m_chart->mapToPosition(chart_left_top).y()-rubberBand1->height());
            rubberBandArea->setGeometry(QRect(rubberBand1->pos().x()+rubberBand1->width(), m_chart->mapToPosition(chart_left_top).y(), chartWidth, chartHeight).normalized());
            m_winLevelBtn->setGeometry(rubberBandArea->pos().x()+rubberBandArea->width()*0.5-m_winLevelBtn->width()*0.5, m_chart->mapToPosition(chart_right_bottom).y(), 10, 10);
            threshold_L = int(m_chart->mapToValue(event->pos()).x());
            qDebug()<<"threshold1:"<<threshold_L;
        }
        //calculate windows_Level(WL) and windows_Width(WW)
        threshold_L = qreal(m_chart->mapToValue(rubberBandArea->pos()).x());
        qDebug()<<"threshold1:"<<threshold_L;
        threshold_R = qreal(m_chart->mapToValue(rubberBand2->pos()).x());
        qDebug()<<"threshold2:"<<threshold_R;
        winLevel = (threshold_L + threshold_R) * 0.5;
        winWidth = threshold_R - threshold_L;
        qDebug()<<"winLevel:"<<winLevel;
        qDebug()<<"winWidth:"<<winWidth;
    }

    if( move_rubberBand2 )
    {
        rubberBand2->move(event->pos().x() - rubberBand_offset_x, m_chart->mapToPosition(chart_left_top).y()-rubberBand1->height());
        rubberBandArea->setGeometry(QRect(rubberBandArea->pos().x(), m_chart->mapToPosition(chart_left_top).y(), originBandWidth + event->pos().x() - origin.x(), chartHeight).normalized());
        m_winLevelBtn->setGeometry(rubberBandArea->pos().x()+rubberBandArea->width()*0.5-m_winLevelBtn->width()*0.5, m_chart->mapToPosition(chart_right_bottom).y(), 10, 10);
        if(rubberBand2->pos().x() < (rubberBand1->pos().x()+rubberBand1->width()))
        {
            move_rubberBand2 = false;
            rubberBand2->move(rubberBand1->pos().x()+rubberBand1->width()+minWidth, m_chart->mapToPosition(chart_left_top).y()-rubberBand1->height());
            rubberBandArea->setGeometry(QRect(rubberBand1->pos().x()+rubberBand1->width(), m_chart->mapToPosition(chart_left_top).y(), minWidth, chartHeight).normalized());
            m_winLevelBtn->setGeometry(rubberBandArea->pos().x()+rubberBandArea->width()*0.5-m_winLevelBtn->width()*0.5, m_chart->mapToPosition(chart_right_bottom).y(), 10, 10);
            threshold_R = int(m_chart->mapToValue(event->pos()).x());
            qDebug()<<"threshold2:"<<threshold_R;
        }
        if( (rubberBand2->pos().x()-rubberBandArea->pos().x()) > chartWidth)
        {
            move_rubberBand2 = false;
            rubberBand2->move(rubberBandArea->pos().x()+chartWidth, m_chart->mapToPosition(chart_left_top).y()-rubberBand1->height());
            rubberBandArea->setGeometry(QRect(rubberBand1->pos().x()+rubberBand1->width(), m_chart->mapToPosition(chart_left_top).y(), chartWidth, chartHeight).normalized());
            m_winLevelBtn->setGeometry(rubberBandArea->pos().x()+rubberBandArea->width()*0.5-m_winLevelBtn->width()*0.5, m_chart->mapToPosition(chart_right_bottom).y(), 10, 10);
            threshold_R = int(m_chart->mapToValue(event->pos()).x());
            qDebug()<<"threshold2:"<<threshold_R;
        }
        //calculate windows_Level(WL) and windows_Width(WW)
        threshold_L = qreal(m_chart->mapToValue(rubberBandArea->pos()).x());
        qDebug()<<"threshold1:"<<threshold_L;
        threshold_R = qreal(m_chart->mapToValue(rubberBand2->pos()).x());
        qDebug()<<"threshold2:"<<threshold_R;
        winLevel = (threshold_L + threshold_R) * 0.5;
        winWidth = threshold_R - threshold_L;
        qDebug()<<"winLevel:"<<winLevel;
        qDebug()<<"winWidth:"<<winWidth;
    }

    if( move_rubberBandArea )
    {
        rubberBand2->move(rubberBandArea->width()+event->pos().x() - rubberBand_offset_x, m_chart->mapToPosition(chart_left_top).y()-rubberBand1->height());
        rubberBandArea->move(event->pos().x() - rubberBand_offset_x, m_chart->mapToPosition(chart_left_top).y());
        rubberBand1->move(rubberBandArea->pos().x()-rubberBand1->width(), m_chart->mapToPosition(chart_left_top).y()-rubberBand1->height());
        m_winLevelBtn->setGeometry(rubberBandArea->pos().x()+rubberBandArea->width()*0.5-m_winLevelBtn->width()*0.5, m_chart->mapToPosition(chart_right_bottom).y(), 10, 10);

        //calculate windows_Level(WL) and windows_Width(WW)
        threshold_L = qreal(m_chart->mapToValue(rubberBandArea->pos()).x());
        qDebug()<<"threshold1:"<<threshold_L;
        threshold_R = qreal(m_chart->mapToValue(rubberBand2->pos()).x());
        qDebug()<<"threshold2:"<<threshold_R;
        winLevel = (threshold_L + threshold_R) * 0.5;
        winWidth = threshold_R - threshold_L;
        qDebug()<<"winLevel:"<<winLevel;
        qDebug()<<"winWidth:"<<winWidth;
        if( winLevel <= minX )
        {
            move_rubberBandArea = false;
            rubberBandArea->move(m_chart->mapToPosition(chart_left_top).x()-rubberBandArea->width()*0.5, m_chart->mapToPosition(chart_left_top).y());
            rubberBand2->move(rubberBandArea->pos().x()+rubberBandArea->width(), m_chart->mapToPosition(chart_left_top).y()-rubberBand1->height());
            rubberBand1->move(rubberBandArea->pos().x()-rubberBand1->width(), m_chart->mapToPosition(chart_left_top).y()-rubberBand1->height());
            m_winLevelBtn->setGeometry(rubberBandArea->pos().x()+rubberBandArea->width()*0.5-m_winLevelBtn->width()*0.5, m_chart->mapToPosition(chart_right_bottom).y(), 10, 10);
        }
        if( winLevel >= maxX )
        {
            move_rubberBandArea = false;
            rubberBandArea->move(m_chart->mapToPosition(chart_right_bottom).x()-rubberBandArea->width()*0.5, m_chart->mapToPosition(chart_left_top).y());
            rubberBand2->move(rubberBandArea->pos().x()+rubberBandArea->width(), m_chart->mapToPosition(chart_left_top).y()-rubberBand1->height());
            rubberBand1->move(rubberBandArea->pos().x()-rubberBand1->width(), m_chart->mapToPosition(chart_left_top).y()-rubberBand1->height());
            m_winLevelBtn->setGeometry(rubberBandArea->pos().x()+rubberBandArea->width()*0.5-m_winLevelBtn->width()*0.5, m_chart->mapToPosition(chart_right_bottom).y(), 10, 10);
        }

        //calculate windows_Level(WL) and windows_Width(WW)
        threshold_L = qreal(m_chart->mapToValue(rubberBandArea->pos()).x());
        qDebug()<<"threshold1:"<<threshold_L;
        threshold_R = qreal(m_chart->mapToValue(rubberBand2->pos()).x());
        qDebug()<<"threshold2:"<<threshold_R;
        winLevel = (threshold_L + threshold_R) * 0.5;
        winWidth = threshold_R - threshold_L;
        qDebug()<<"winLevel:"<<winLevel;
        qDebug()<<"winWidth:"<<winWidth;

    }

    if( move_winLevelBtn )
    {
        m_winLevelBtn->setGeometry(event->pos().x() - rubberBand_offset_x, m_chart->mapToPosition(chart_right_bottom).y(), 10, 10);
        rubberBand2->move(m_winLevelBtn->pos().x()+m_winLevelBtn->width()*0.5+rubberBandArea->width()*0.5, m_chart->mapToPosition(chart_left_top).y()-rubberBand1->height());
        rubberBandArea->move(rubberBand2->pos().x() - rubberBandArea->width(), m_chart->mapToPosition(chart_left_top).y());
        rubberBand1->move(rubberBandArea->pos().x()-rubberBand1->width(), m_chart->mapToPosition(chart_left_top).y()-rubberBand1->height());
        //calculate windows_Level(WL) and windows_Width(WW)
        threshold_L = qreal(m_chart->mapToValue(rubberBandArea->pos()).x());
        qDebug()<<"threshold1:"<<threshold_L;
        threshold_R = qreal(m_chart->mapToValue(rubberBand2->pos()).x());
        qDebug()<<"threshold2:"<<threshold_R;
        winLevel = (threshold_L + threshold_R) * 0.5;
        winWidth = threshold_R - threshold_L;
        qDebug()<<"winLevel:"<<winLevel;
        qDebug()<<"winWidth:"<<winWidth;
        if( winLevel < minX )
        {
            move_winLevelBtn = false;
            rubberBandArea->move(m_chart->mapToPosition(chart_left_top).x()-rubberBandArea->width()*0.5, m_chart->mapToPosition(chart_left_top).y());
            rubberBand2->move(rubberBandArea->pos().x()+rubberBandArea->width(), m_chart->mapToPosition(chart_left_top).y()-rubberBand1->height());
            rubberBand1->move(rubberBandArea->pos().x()-rubberBand1->width(), m_chart->mapToPosition(chart_left_top).y()-rubberBand1->height());
            m_winLevelBtn->setGeometry(rubberBandArea->pos().x()+rubberBandArea->width()*0.5-m_winLevelBtn->width()*0.5, m_chart->mapToPosition(chart_right_bottom).y(), 10, 10);
        }
        if( winLevel >= maxX )
        {
            move_winLevelBtn = false;
            rubberBandArea->move(m_chart->mapToPosition(chart_right_bottom).x()-rubberBandArea->width()*0.5, m_chart->mapToPosition(chart_left_top).y());
            rubberBand2->move(rubberBandArea->pos().x()+rubberBandArea->width(), m_chart->mapToPosition(chart_left_top).y()-rubberBand1->height());
            rubberBand1->move(rubberBandArea->pos().x()-rubberBand1->width(), m_chart->mapToPosition(chart_left_top).y()-rubberBand1->height());
            m_winLevelBtn->setGeometry(rubberBandArea->pos().x()+rubberBandArea->width()*0.5-m_winLevelBtn->width()*0.5, m_chart->mapToPosition(chart_right_bottom).y(), 10, 10);
        }

        //calculate windows_Level(WL) and windows_Width(WW)
        threshold_L = qreal(m_chart->mapToValue(rubberBandArea->pos()).x());
        qDebug()<<"threshold1:"<<threshold_L;
        threshold_R = qreal(m_chart->mapToValue(rubberBand2->pos()).x());
        qDebug()<<"threshold2:"<<threshold_R;
        winLevel = (threshold_L + threshold_R) * 0.5;
        winWidth = threshold_R - threshold_L;
        qDebug()<<"winLevel:"<<winLevel;
        qDebug()<<"winWidth:"<<winWidth;
    }

    QGraphicsView::mouseMoveEvent(event);
}

void sliderCtrArea::mouseReleaseEvent(QMouseEvent *event)
{

    if( move_rubberBand1)
    {
        threshold_L = int(m_chart->mapToValue(event->pos()).x());
        move_rubberBand1 = false;
        qDebug()<<"threshold1:"<<threshold_L;
        qDebug()<<"rbb2_release pos().x():"<<rubberBand2->pos().x();
    }

    if( move_rubberBand2)
    {
        threshold_R = int(m_chart->mapToValue(event->pos()).x());
        move_rubberBand2 = false;
        qDebug()<<"threshold2:"<<threshold_R;
        qDebug()<<"rbb2_release pos().x():"<<rubberBand2->pos().x();
    }

    if( move_rubberBandArea)
    {
        move_rubberBandArea = false;
    }

    if( move_winLevelBtn)
    {
        move_winLevelBtn = false;
    }
}

void sliderCtrArea::keepCallout()
{
    m_callouts.append(m_tooltip);
    m_tooltip = new Callout(m_chart);
}

void sliderCtrArea::tooltip(QPointF point, bool state)
{
    if (m_tooltip == 0)
        m_tooltip = new Callout(m_chart);

    if (state) {
        m_tooltip->setText(QString("X: %1 \nY: %2 ").arg(point.x()).arg(point.y()));
        m_tooltip->setAnchor(point); //show callout
        m_tooltip->setZValue(11); //stack overlapping, give it an bigger one than 1~3 to avoid being coverd
        m_tooltip->updateGeometry();
        m_tooltip->show();
    } else {
        m_tooltip->hide();
    }
}

//bool sliderCtrArea::eventFilter(QObject *obj, QEvent *event)
//{
//    if(obj == m_wwRSlider)
//    {
//        if (event->type()==QEvent::MouseButtonPress)
//        {
//            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
//            if (mouseEvent->button()== Qt::LeftButton)
//            {
//                int dur = m_wwRSlider->maximum() - m_wwRSlider->minimum();
//                int pos = m_wwRSlider->minimum() + dur * ((double)mouseEvent->x() / m_wwRSlider->width());
//                if(pos != m_wwRSlider->sliderPosition()){
//                    m_wwRSlider->setValue(pos);
//                }
//            }
//        }
//    }

//    if(obj == m_wwLSlider)
//    {
//        if (event->type()==QEvent::MouseButtonPress)
//        {
//            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
//            if (mouseEvent->button()== Qt::LeftButton)
//            {
//                int dur = m_wwLSlider->maximum() - m_wwLSlider->minimum();
//                int pos = m_wwLSlider->minimum() + dur * ((double)mouseEvent->x() / m_wwLSlider->width());
//                if(pos != m_wwLSlider->sliderPosition()){
//                    m_wwLSlider->setValue(pos);
//                }
//            }
//        }
//    }
//    return QObject::eventFilter(obj,event);
//}

sliderCtrArea::~sliderCtrArea()
{

}
