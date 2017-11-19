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

using namespace std;

int frame::n[3] = {};
int frame::n2[3] = {};
int frame::k = 0;
QPointF frame::point[3][1000] = {};
QPoint frame::point2[3][1000] = {};
QColor frame::color[3] = {qRgb(0,180,255),qRgb(255,60,0),qRgb(0,0,0)};

frame::frame(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::frame)
{
    grain = 20; //grain直接设定为20，基本满足插值要求
    ui->setupUi(this);
    pa = new paint(this);
    ui->verticalLayout->addWidget(pa);
    ui->verticalLayoutWidget->setMouseTracking(true);   //设置鼠标追踪
    ui->centralWidget->setMouseTracking(true);
}

frame::~frame()
{
    delete ui;
}


void frame::cspline()
{
    int i,np; n0=n[k]; np=n0; CPt jd[100]; CPt *knots;

    //knots0=knots;
    //将控制点坐标赋值到数组jd中
    for(i=1;i<=np;i++) {
            jd[i].x = point[k][i-1].x(); jd[i].y = point[k][i-1].y();
        }
    jd[0].x = point[k][0].x(); jd[0].y = point[k][0].y();
    jd[np+1].x = point[k][np-1].x(); jd[np+1].y = point[k][np-1].y();
    jd[np+2].x = point[k][np-1].x(); jd[np+2].y = point[k][np-1].y();
    np=np+3; knots=jd;
    CubicSpline(np, knots);
}

//spline message handlers
void frame::CubicSpline(int np, CPt *knots)
{
    CPt *k0, *kml, *k1, *k2;
    int i,j;
    double alpha[50];

    GetCardinalMatrix(float(ui->tension->value())/100.);

    for(i=0; i<=grain; i++) alpha[i] =((double)i)/(grain+1);
    kml = knots;
    k0=kml+1; k1=k0+1; k2=k1+1;
    for(i=1; i<np-2; i++) {
        for(j=0; j<=grain; j++)
        if (i == np-3 && j == 1) break;
        else
        {
            point2[k][++n2[k]-1].setX(Matrix(kml->x,k0->x,k1->x,k2->x,alpha[j]));    //点坐标存入数组
            point2[k][n2[k]-1].setY(Matrix(kml->y,k0->y,k1->y,k2->y,alpha[j]));
        }
        k0++; kml++; k1++; k2++;
    }
    if (k > 1)pa->repaint();
    else pa->update();
}

void frame::GetCardinalMatrix(double a1)
{
    m[0]=-a1; m[1]=2.-a1; m[2]=a1-2.; m[3]=a1;
    m[4]=2.*a1; m[5]=a1-3.; m[6]=3.-2.*a1; m[7]=-a1;
    m[8]=-a1; m[9]=0.; m[10]=a1; m[11]=0.;
    m[12]=0.; m[13]=1.; m[14]=0.; m[15]=0.;
}

double frame::Matrix(double a, double b, double c, double d, double alpha)
{
    double p0,p1,p2,p3;
    p0=m[0]*a+m[1]*b+m[2]*c+m[3]*d;
    p1=m[4]*a+m[5]*b+m[6]*c+m[7]*d;
    p2=m[8]*a+m[9]*b+m[10]*c+m[11]*d;
    p3=m[12]*a+m[13]*b+m[14]*c+m[15]*d;
    return(p3+alpha*(p2+alpha*(p1+alpha*p0)));
}

void frame::mouseMoveEvent(QMouseEvent *m)  //实时追踪鼠标运动
{
    //若鼠标运动到距离起始点半径10个坐标单位内，则鼠标光标变为十字形
    if ( (n[k]>1) && (abs(m->x()-170 - point[k][0].x())<=10) &&
         (abs(m->y()-50 - point[k][0].y())<=10) )
        setCursor(Qt::CrossCursor);
    else setCursor(Qt::ArrowCursor);
}

void frame::mouseReleaseEvent(QMouseEvent *m)   //鼠标左键释放追踪函数
{
    if (m->button() == Qt::LeftButton && n[k] > 0)
    {
        n2[k] = 0;
        cspline();
    }
}


void frame::on_Run_clicked(bool checked)    //线性插值函数触发函数
{
    if (!checked)
        {
            n[2] = Min(n[0], n[1]); //曲线三的点数量为两曲线中点数较小值
            Linear();   //线性插值函数
    }
}

void frame::on_Reset_clicked()  //“重置”按钮触发槽函数
{
    ui->tension->setValue(0);   //tension等各种参数都清零，并刷新画布
    n[0] = 0;
    n[1] = 0;
    n2[0] = 0;
    n2[1] = 0;
    k = 0;
    pa->update();
}


void frame::on_tension_sliderMoved(int position)    //tension值拖动槽
{
    tension = (double)position/100.;
    int kk = k;
    n2[0] = 0;
    k = 0;
    cspline();
    if (kk > 0)
    {
        n2[1] = 0;
        k = 1;
        cspline();
    }
}

void frame::on_Run_2_clicked()  //矢量线性插值函数触发函数
{
    n[2] = Min(n[0], n[1]); //曲线三的点数量为两曲线中点数较小值
    VLinear();  //矢量线性插值函数
}

double frame::Min(double x, double y)
{
    if (x < y) return x;
    else return y;
}
