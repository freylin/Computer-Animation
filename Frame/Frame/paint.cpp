#include "frame.h"
#include "ui_frame.h"
#include "paint.h"
#include "stdio.h"
#include "stdlib.h"
#include "iostream"
#include <QPainter>
#include <QPointF>
#include <QLineF>
#include <QPolygon>
#include <math.h>

using namespace std;

paint::paint(QWidget *parent) : QWidget(parent)
{
    setPalette(QPalette(Qt::white));
    setAutoFillBackground(true);
    setMouseTracking(true);
}

paint::~paint()
{

}

void frame::Linear()    //线性插值函数
{
    int m = 600;    //帧数设置为600帧
    float t, jj, mm;
    mm = (float)m;
    for (int j = 0; j <= m; j++)
    {
        t = (float)j/(float)m;
        k = 2;  //曲线编号，开始操作插值图形
        n2[2] = 0;
        for (int i = 0; i < n[2]; i++)
        {
            point[k][i].setX( (1-t) * point[0][i].x()
                    + t * point[1][i].x() );
            point[k][i].setY( (1-t) * point[0][i].y()
                    + t * point[1][i].y() );
        }
        jj = (float)j;
        //图形颜色根据始末图形的颜色渐变
        color[2] = qRgb( (int)(jj*(255./mm)),
                         (int)(180.-jj*(120./mm)),
                         (int)(255.-jj*(255./mm)) );
        cspline();  //样条曲线插值
        //pa->repaint();
    }
}

void frame::VLinear()   //矢量线性插值函数
{
    int m = 600;
    double t, jj, mm, x, y, pi = 3.141592353;
    mm = (float)m;
    float r[3][1000],a[3][1000];
    for (int i = 0; i <= 1; i++)
        for (int j = 0; j < n[i]; j++)
        {
            if (j == 0)
            {
                x = point[i][j].x();    //第一个控制点坐标作为矢量的出发点
                y = point[i][j].y();
            }
            else
            {
                //矢量坐标为相邻两点的坐标差
                x = point[i][j].x()-point[i][j-1].x();
                y = point[i][j].y()-point[i][j-1].y();
            }
            r[i][j] = sqrt(x*x + y*y); //矢量的长度
            a[i][j] = asin(y/r[i][j]); //矢量的角度
            //角度取值范围为0~2π
            if (x < 0) a[i][j] = pi - a[i][j];
            if ((x > 0)&&(y < 0)) a[i][j] = 2.0*pi + a[i][j];
        }
    //初始图形与终止图形对应矢量间的劣弧为中间图形矢量的旋转路径
    for (int i = 0; i < n[2]; i++)
    {
        if(a[0][i]-a[1][i]>pi) a[0][i] = a[0][i] - pi*2;
        if(a[0][i]-a[1][i]<-pi) a[0][i] = a[0][i] + pi*2;
    }
    for (int j = 0; j <= m; j++)
    {
        t = (double)j/(double)m;
        k = 2;
        n2[2] = 0;

        for (int i = 0; i < n[2]; i++)
        {
            r[k][i] = (1-t)*r[0][i] + t*r[1][i];
            a[k][i] = (1-t)*a[0][i] + t*a[1][i];
        }

        point[k][0].setX( r[k][0]*cos(a[k][0]) );
        point[k][0].setY( r[k][0]*sin(a[k][0]) );
        for (int i = 1; i < n[0]; i++) //矢量首尾相连
        {
            point[k][i].setX
                    ( point[k][i-1].x() + r[k][i]*cos(a[k][i]) );
            point[k][i].setY
                    ( point[k][i-1].y() + r[k][i]*sin(a[k][i]) );
        }
        point[k][n[0]-1] = point[k][0];

        jj = (double)j;
        color[2] = qRgb( (int)(jj*(255./mm)),
                         (int)(180.-jj*(120./mm)),
                         (int)(255.-jj*(255./mm)) );
        cspline();
    }
}

void paint::mousePressEvent(QMouseEvent *m)
{
    if (m->button() == Qt::LeftButton)
    {
        if ( (abs(m->x() - frame::point[frame::k][0].x())<=10) && (abs(m->y() - frame::point[frame::k][0].y())<=10) )
        {
            //setCursor(Qt::CrossCursor);
            frame::point[frame::k][frame::n[frame::k]].setX(frame::point[frame::k][0].x());
            frame::point[frame::k][frame::n[frame::k]++].setY(frame::point[frame::k][0].y());
        }
        else
        {
            frame::point[frame::k][frame::n[frame::k]].setX(m->x());
            frame::point[frame::k][frame::n[frame::k]++].setY(m->y());
        }
        update();
    }
}

void paint::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPen pen;
    //QPainterPath path;
    QPolygon poly[3];

    painter.setPen(QPen(Qt::black,4.0));
    for (int i = 0; i <= frame::k; i++)
        if (frame::n[i]>0) painter.drawPoints(frame::point[i],frame::n[i]);

    pen.setColor(qRgb(255, 200, 0));
    pen.setWidthF(0.85);
    painter.setPen(pen);    //设置画笔
      for (int j = 0; j <= frame::k; j++)
      if (frame::n2[j]>0)
      {
        for (int i = 0; i < frame::n2[j]-1; i++)
        {
            //存下多边形元素点
            poly[j].operator <<(QPoint(frame::point2[j][i]));
            painter.drawLine
                    (frame::point2[j][i],frame::point2[j][i+1]);//连线
        }
        poly[j] << QPoint(frame::point2[j][frame::n2[j]-1]);
        if (frame::n[j]>1 &&
                frame::point[j][frame::n[j]-1] == frame::point[j][0])
        {
            painter.setBrush(QColor(frame::color[j]));  //设置笔刷
            painter.drawPolygon(poly[j]);   //画出多边形，带填充效果
            if (frame::k == 0)
            {
                frame::n2[1] = 0;
                frame::n[1] = 0;
                frame::k = 1;
            }
        }
        //path.addPolygon(poly[j]);
      }

}
