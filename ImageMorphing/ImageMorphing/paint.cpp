#include "imagemorphing.h"
#include "ui_imagemorphing.h"
#include "stdio.h"
#include "stdlib.h"
#include "iostream"
#include <QPainter>
#include <QPointF>
#include <QLineF>
#include <QPolygon>
#include <math.h>

using namespace std;

//左键单击
void ImageMorphing::mousePressEvent(QMouseEvent *m)
{
    if (m->button() == Qt::LeftButton)
    {
        double x = m->x(), y = m->y();
        int i = (n % 4)/2;  //判断轮到左边图像还是右边图像
        if (x>=pos[i].x() && x<pos[i].x()+w)
            if (y>=pos[i].y() && y<pos[i].y()+h)
            {
                point[n].setX(x);
                point[n].setY(y);
                n++;
                update();
            }
    }
}

//鼠标移动
void ImageMorphing::mouseMoveEvent(QMouseEvent *m)  //实时追踪鼠标运动
{
    double x = m->x(), y = m->y();
    //若点击次数为奇数，则绘制动态跟踪线段
    if ( (n % 2) == 1)
    {
        db = 1;
        //首端点为最新点击的点
        d[0].setX(point[n-1].x());
        d[0].setY(point[n-1].y());
        //末端点为当前鼠标移动到的点
        d[1].setX(x);
        d[1].setY(y);
        //更新绘制
        update();
    }
    else db = 0;
}

void ImageMorphing::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPen pen;
    //根据k值判断需要绘制的图像个数
    for (int i = 0; i < k; i++)
        painter.drawImage(pos[i].x(),pos[i].y(),pix[i]);

    painter.setPen(QPen(Qt::black,4.0));
    pen.setColor(qRgb(255, 255, 255));
    pen.setWidthF(2);
    painter.setPen(pen);    //设置画笔
    double r = 3;
    //动态线段
    if (db) painter.drawLine(d[0],d[1]);
    //连线
    for (int i = 0; i < n; i+=4)
    {
        if (i+1<n)
        {
            painter.drawLine(point[i],point[i+1]);
            painter.drawEllipse(point[i+1].x()-r, point[i+1].y()-r, 2*r, 2*r);
        }
        if (i+3<n)
        {
            painter.drawLine(point[i+2],point[i+3]);
            painter.drawEllipse(point[i+3].x()-r, point[i+3].y()-r, 2*r, 2*r);
        }
    }
}
