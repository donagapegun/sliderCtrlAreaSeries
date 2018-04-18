#ifndef SLIDERCTRAREA_H
#define SLIDERCTRAREA_H

#include <QMainWindow>
#include <QtWidgets/QGraphicsView>
#include <QtCharts/QChartGlobal>
#include <QtCharts/QChartView>
#include <QtWidgets/QRubberBand>
#include <QPushButton>
#include <QToolButton>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QMouseEvent;
class QResizeEvent;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE

class Callout;

QT_CHARTS_USE_NAMESPACE

class sliderCtrArea : public QGraphicsView
{
    Q_OBJECT

public:
    sliderCtrArea(QWidget *parent = 0);
    ~sliderCtrArea();

protected:
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void keepCallout();
    void tooltip(QPointF point, bool state);

private:
    QGraphicsSimpleTextItem *m_coordX;
    QGraphicsSimpleTextItem *m_coordY;
    QChart *m_chart;
    Callout *m_tooltip;
    QList<Callout *> m_callouts;

    QPushButton *m_wwBtn_L;
    QPushButton *m_wwBtn_R;
    QRubberBand *m_winLevelBtn;
    QRubberBand *rubberBand1;
    QRubberBand *rubberBandArea;
    QRubberBand *rubberBand2;
    QPointF chart_right_bottom;
    QPointF chart_left_top;
    QPoint origin;
    QPoint rubberBand_offset;
    int rubberBand_offset_x;
    int originBandWidth;
    bool move_winLevelBtn;
    bool move_rubberBand1;
    bool move_rubberBandArea;
    bool move_rubberBand2;
    qreal threshold_L;
    qreal threshold_R;
    qreal chartHeight;
    qreal chartWidth;
    qreal winLevel;
    qreal winWidth;
    qreal maxX;
    qreal minX;
    qreal maxY;
    qreal minY;

    QPainter *paintEllipse;


private:
    QChart *createSplineChart() const;
    QPushButton *createWinBtn() const;
    QChartView *m_chartView;

//protected:
//    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // SLIDERCTRAREA_H
