#include "WidgetMarqueeLabel.h"
#include <QPainter>
#include <QWidget>

WidgetMarqueeLabel::WidgetMarqueeLabel(QWidget *parent)//*parent)
{
        px = 0;
        py = 10;
        speed = 3;
        direction = RightToLeft;
        connect(&timer, SIGNAL(timeout()), this, SLOT(refreshLabel()));
        timer.start(10);
}

void WidgetMarqueeLabel::refreshLabel()
{
    repaint();
}

WidgetMarqueeLabel::~WidgetMarqueeLabel()
{}

void WidgetMarqueeLabel::close()
{
        QLabel::close();
}

void WidgetMarqueeLabel::show()
{
    QLabel::show();
}

void WidgetMarqueeLabel::setAlignment(Qt::Alignment al)
{
    m_align = al;
    updateCoordinates();
    QLabel::setAlignment(al);
}

void WidgetMarqueeLabel::paintEvent(QPaintEvent *evt)
{
    QPainter p(this);
    if(direction==RightToLeft)
    {
        px -= speed;
        if(px <= (-textLength))
            px = width();
    }
    else
    {
        px += speed;
        if(px >= width())
            px = - textLength;
    }
        p.drawText(px, py+fontPointSize, text());
    p.translate(px,0);
}

void WidgetMarqueeLabel::resizeEvent(QResizeEvent *evt)
{
    updateCoordinates();
    QLabel::resizeEvent(evt);
}

void WidgetMarqueeLabel::updateCoordinates()
{
    switch(m_align)
    {
        case Qt::AlignTop:
            py = 10;
            break;
        case Qt::AlignBottom:
                        py = height()-10;
            break;
        case Qt::AlignVCenter:
            py = height()/2;
            break;
    }
    fontPointSize = font().pointSize()/2;
    textLength = fontMetrics().width(text());
}

void WidgetMarqueeLabel::setSpeed(int s)
{
    speed = s;
}

int WidgetMarqueeLabel::getSpeed()
{
    return speed;
}

void WidgetMarqueeLabel::setDirection(int d)
{
    direction = d;
    if (direction==RightToLeft)
        px = width() - textLength;
    else
        px = 0;
    refreshLabel();
}
