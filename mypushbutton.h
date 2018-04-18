#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H
//MyPushButton1.h

#include <QPushButton>
#include <sliderctrarea.h>
class MyPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit MyPushButton(QWidget *parent=0);
public:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // MYPUSHBUTTON_H
