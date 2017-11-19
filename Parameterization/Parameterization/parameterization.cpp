#include "parameterization.h"
#include "ui_parameterization.h"
#include "stdio.h"
#include "stdlib.h"
#include "iostream"
#include <QPainter>
#include <QPointF>
#include <QLineF>
#include <math.h>

#define ITERATION_COUNT 45
#define SEARCH_TOL 0.01

using namespace std;

int Parameterization::n = 0;
int Parameterization::n3 = 0;
int Parameterization::ii = -1;
int Parameterization::Ngrain[30] = {0};
QPointF Parameterization::point[1000] = {};
QPointF Parameterization::point3[3000] = {};
QPointF Parameterization::point2[30][1000] = {};
double paint::t[30] = {0};

Parameterization::Parameterization(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Parameterization)
{
    grain = 0;
    ui->setupUi(this);
    pa = new paint(this);
    //car = new QLabel(this);
    ui->verticalLayout->addWidget(pa);

}

Parameterization::~Parameterization()
{
    delete ui;
}

paint::paint(QWidget *parent) : QWidget(parent)
{
    setPalette(QPalette(Qt::white));
    setAutoFillBackground(true);
}

paint::~paint()
{

}

void Parameterization::cspline()
{
    int i,np; n0=n; np=n0; CPt jd[100]; CPt *knots;

    //knots0=knots;
    //将控制点坐标赋值到数组jd中
    for(i=1;i<=np;i++) {
            jd[i].x = point[i-1].x(); jd[i].y = point[i-1].y();
            p[i] = jd[i];
        }
    jd[0].x = point[0].x(); jd[0].y = point[0].y();
    jd[np+1].x = point[np-1].x(); jd[np+1].y = point[np-1].y();
    jd[np+2].x = point[np-1].x(); jd[np+2].y = point[np-1].y();
    p[0] = jd[0];
    p[np+1] = jd[np+1]; p[np+2] = jd[np+2];

    np=np+3; knots=jd;
    CubicSpline(np, knots);
    SetLengths(n + 2);  //建立累加弧长表
}

//spline message handlers
void Parameterization::CubicSpline(int np, CPt *knots)
{
    CPt *k0, *kml, *k1, *k2;
    int i,j;
    double alpha[50];

    GetCardinalMatrix(tension);

    for(i=0; i<=grain; i++) alpha[i] =((double)i)/(grain+1);
    kml = knots;
    k0=kml+1; k1=k0+1; k2=k1+1;
    for(i=1; i<np-2; i++) {
        for(j=0; j<=grain; j++)
        if (i == np-3 && j == 1) break;
        else
        {
            //cout<<s->x<<" xy "<<s->y<<endl;
            point2[Ngrain[0]][++Ngrain[Ngrain[0]]-1].setX(Matrix(kml->x,k0->x,k1->x,k2->x,alpha[j]));    //点坐标存入数组
            point2[Ngrain[0]][Ngrain[Ngrain[0]]-1].setY(Matrix(kml->y,k0->y,k1->y,k2->y,alpha[j]));
        }
        k0++; kml++; k1++; k2++;
    }
    pa->update();
}

void Parameterization::GetCardinalMatrix(double a1)
{
    m[0]=-a1; m[1]=2.-a1; m[2]=a1-2.; m[3]=a1;
    m[4]=2.*a1; m[5]=a1-3.; m[6]=3.-2.*a1; m[7]=-a1;
    m[8]=-a1; m[9]=0.; m[10]=a1; m[11]=0.;
    m[12]=0.; m[13]=1.; m[14]=0.; m[15]=0.;
}

double Parameterization::Matrix(double a, double b, double c, double d, double alpha)
{
    double p0,p1,p2,p3;
    p0=m[0]*a+m[1]*b+m[2]*c+m[3]*d;
    p1=m[4]*a+m[5]*b+m[6]*c+m[7]*d;
    p2=m[8]*a+m[9]*b+m[10]*c+m[11]*d;
    p3=m[12]*a+m[13]*b+m[14]*c+m[15]*d;
    return(p3+alpha*(p2+alpha*(p1+alpha*p0)));
}


void paint::mousePressEvent(QMouseEvent *m)
{
    if (m->button() == Qt::LeftButton)
    {
        Parameterization::point[Parameterization::n].setX(m->x());
        Parameterization::point[Parameterization::n++].setY(m->y());
        update();
    }
}

void paint::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPen pen;

    painter.setPen(QPen(Qt::black,4.0));
    if (Parameterization::n>0) painter.drawPoints(Parameterization::point,Parameterization::n);

    if (Parameterization::Ngrain[Parameterization::Ngrain[0]]>0)
    {
        for (int i = 1; i <= Parameterization::Ngrain[0]; i++)
        {
            //pen.setColor(qRgb((i*100+149) % 255, (i*200+55) % 255,(i*50+205) % 255));
            pen.setColor(qRgb((int)(t[i]*254) % 255, 0,90));
            pen.setWidthF(0.85);
            painter.setPen(pen);
            //painter.drawPoints(Parameterization::point2[i],Parameterization::Ngrain[i]);
            for (int j = 0; j < Parameterization::Ngrain[i]-1; j++)
                painter.drawLine(Parameterization::point2[i][j],Parameterization::point2[i][j+1]);
        }
     }

    int i = Parameterization::ii;
    double xx,yy,pi;
    pi = 3.1415926;
    if (i >= 0)
    {
        QImage* imgRotate = new QImage;
        *imgRotate = QImage(":/new/prefix1/car.png").scaled(100,50);    //导入小车图片
        double a= atan((Parameterization::point3[i+1].y()-Parameterization::point3[i].y())  //计算坡度斜率角度
                /(Parameterization::point3[i+1].x()-Parameterization::point3[i].x()));
        QMatrix matrix;
        //if (Parameterization::point3[i+1].x()-Parameterization::point3[i].x() < 0) a += pi; //如果车子“反重力”贴壁运动，则车子旋转180度
        matrix.rotate(a*180./pi);    //矩阵


        if (a > 0)  //根据不同上下坡调整车子的位置，贴合弧线
        {
            xx = 20.0;
            yy = 40.0;
        }
        else
        {
            xx = 40.0;
            yy = 60.0;
        }
        *imgRotate = imgRotate->transformed(matrix,Qt::SmoothTransformation);   //图片旋转
        painter.drawImage(Parameterization::point3[i].x()-xx,  //绘制小车
                            Parameterization::point3[i].y()-yy,
                            *imgRotate
                            );
    }
}

void Parameterization::on_Run_clicked(bool checked)
{
    if (!checked)
        {
            grain = ui->grain->value();
            tension = ui->tension->value();
            Ngrain[++Ngrain[0]] = 0;
            paint::t[Ngrain[0]] = tension;
            cspline();
    }
}

void Parameterization::on_Reset_clicked()   //“重置”函数，各种状态清零
{
    ui->grain->setValue(0);
    ui->tension->setValue(0);
    n = 0;
    Ngrain[0] = 0;
    //car->clear();
    n3 = 0;
    //anime2->clear();
    ii = -1;
    pa->update();
}

double Parameterization::ArcIntegrand(double u) //计算积分
{
    double A,B,C,D,E,u2,u3,u4;
    u2 = u * u;
    u3 = u2 * u;
    u4 = u3 * u;
    A = 9. * (Cx[0] * Cx[0] + Cy[0] * Cy[0]);
    B = 12. * (Cx[0] * Cx[1] + Cy[0] * Cy[1]);
    C = 6. * (Cx[0] * Cx[2] + Cy[0] * Cy[2]) +
        4. * (Cx[1] * Cx[1] + Cy[1] * Cy[1]);
    D = 4. * (Cx[1] * Cx[2] + Cy[1] * Cy[2]);
    E = Cx[2] * Cx[2] + Cy[2] * Cy[2];
    return sqrt(A * u4 + B * u3 + C * u2 + D * u + E);
}

//返回当前曲线段在区间内的弧长
double Parameterization::ArcLength(double a, double b)
{
    double h, sum, u;
    int i;
    //运用扩展Simpson法则来计算弧长
    h = (b - a) / (float)ITERATION_COUNT;
    sum = 0.0;
    u = a + h;

    for (i = 2; i <=ITERATION_COUNT; i++)
    {
        if (!(i & 1))
            sum += 4.0 * ArcIntegrand(u);
            //i为偶数时，ArcIntegrand(u)的系数为4
        else
            sum += 2.0 * ArcIntegrand(u);
            //i为奇数时，ArcIntegrand(u)的系数为4
        u += h;
    }
    return (h * (ArcIntegrand(a) + sum + ArcIntegrand(b)) / 3.0);
}

void Parameterization::SegCoef(int i)   //建立当前曲线段相关的系数
{
    int jj;
    for (int j = 0; j <= 3; j++)
    {
        jj = j * 4;
        Cx[j] = m[jj] * p[i].x + m[jj+1] * p[i+1].x +
                m[jj+2] * p[i+2].x + m[jj+3] * p[i+3].x;
        Cy[j] = m[jj] * p[i].y + m[jj+1] * p[i+1].y +
                m[jj+2] * p[i+2].y + m[jj+3] * p[i+3].y;
    }
}

void Parameterization::SetLengths(int np)   //建立累加弧长表
{
    int i, no_segments;
    double arclength;
    no_segments = np - 3;   //曲线段的个数为控制点个数-1
    arclength = 0.;
    seg_length[0] = 0.;
    for (i = 0; i < no_segments; i++)
    {
        SegCoef(i);   //建立当前曲线段相关的系数
        arclength += ArcLength(0.0, 1.0);   //计算每段曲线长并累加
        seg_length[i+1] = arclength;    //弧长累加
    }
    total_length = arclength;   //曲线全长
    for (i = 0; i <= no_segments; i++)
        seg_length[i] /= total_length;  //累加弧长参数化
}

void Parameterization::GetPointOnSpline(double u)   //得到参数u处的点坐标
{
    double u2, u3;
    u2 = u * u;
    u3 = u2 * u;
    point3[n3].setX(Cx[0] * u3 + Cx[1] * u2 + Cx[2] * u + Cx[3]);
    point3[n3].setY(Cy[0] * u3 + Cy[1] * u2 + Cy[2] * u + Cy[3]);
}

void Parameterization::ArcLengthPoint(double s) //返回样条上弧长为s的点
{
    double l = 0.0, r = 1.0, u;
    double segment_dist, ssearch;
    int segment_no = 0;

    //找出弧长s的终点坐落的曲线段
    do segment_no ++;
    while (seg_length[segment_no] < s);
    segment_no --;
    //求出s在s终点坐落的曲线段上的长度
    segment_dist = total_length * (s - seg_length[segment_no]);
    //建立当前曲线段相关的系数
    SegCoef(segment_no);
    //二分查找局部参数
    do
    {
        u = (l + r) /2.0;
        ssearch = ArcLength(l, u);
        if (segment_dist < ssearch + SEARCH_TOL)
            r = u;
        else
        {
            l = u;
            segment_dist -= ssearch;
        }
    }while ((segment_dist > ssearch + SEARCH_TOL) ||
            (segment_dist < ssearch - SEARCH_TOL));
    //重复递归过程，直到当前查找点位置与目标位置的差小于精度值
    GetPointOnSpline(u);    //获得查找到的局部参数处的点的坐标
}

//void Parameterization::run()    //运行动画
//{
//    double xx,yy,a;
//    xx = 100;
//    yy = 100;
//    anime2 = new QSequentialAnimationGroup();//新建动画组
//    QPixmap c=QPixmap(":/new/prefix1/car.png").scaled(100,50);
//    for(int i = 0; i < n3-1; i++)
//    {
//        a = atan((point3[i+1].y()-point3[i].y())/(point3[i+1].x()-point3[i].x()))*180./3.1415926;
//        QMatrix matrix;
//        matrix.rotate(a);

//        //导入小车资源并调整大小
//        car->setPixmap(c.transformed(matrix,Qt::SmoothTransformation));
//        //car->setPixmap(c);
//        //新建动画并绑定小车
//        anime = new QPropertyAnimation(car,"geometry");
//        anime->setDuration(50); //每帧动画50毫秒
//        anime->setStartValue(QRect(point3[i].x() + xx/0.7,
//                                   point3[i].y(), xx, yy)); //起始点
//        anime->setEndValue(QRect(point3[i+1].x() + xx/0.7,
//                                 point3[i+1].y(), xx, yy));   //终止点
//        anime2->addAnimation(anime);    //加入动画组
//    }
//    anime2->start();    //运行动画组
//}
void Parameterization::run()    //运行动画
{
    int  k = 0;
    for (int i = 0 ; i < n3-1; i++)
    {
        ii = i;
        if ((point3[i].x() - point3[i+1].x() < 0.1) && (point3[i].x() - point3[i+1].x() >-0.1))
        {
            k++;
            if (k > 10) break;
        }
        else k = 0;

        pa->repaint();
    }
}
void Parameterization::on_Run1_clicked()    //“匀速运动”
{
    double s,t,vt;
    n3 = 0;
    s = 0.;
    t = 0.;
    vt = ui->ve->value()/40.;   //速度
    do{
        ArcLengthPoint(s);
        n3++;
        t += 1./100;
        s = vt * t; //匀速运动
    }while (s <= 1. && n3 <998);

    run();
}

void Parameterization::on_Run2_clicked()    //“加速运动”
{
    double s,t,a,v0;
    n3 = 0;
    s = 0.;
    t = 0.;
    v0 = ui->ve->value()/40.;
    a = ui->ac->value()/15.;
    do{
        ArcLengthPoint(s);
        n3++;
        t += 1./100;
        s = (v0 * t) + (a * t * t)/2.0; //加速运动
    }while (s <= 1. && n3 <998);

    run();
}

void Parameterization::on_Run3_clicked()    //“先匀速后加速运动”
{
    double s,t,a,v0,s2;

    n3 = 0;
    s = 0.;
    t = 0.;
    v0 = ui->ve->value()/40.;   //速度
    a = ui->ac->value()/15.;    //加速度
    do{
        ArcLengthPoint(s);
        n3++;
        t += 1./100;
        s = v0 * t;
    }while (s <= 0.5);  //匀速运动到总长度的一半后停止
    t = 0.;
    s2 = s;
    //开始加速运动
    do{
        ArcLengthPoint(s);
        n3++;
        t += 1./100;
        s = s2 + (v0 * t) + (a * t * t)/2.0;
    }while (s <= 1. && n3 <998);

    run();
}

void Parameterization::on_Run4_clicked()        //“模拟重力运动”
{
    double s,t,vt,s0,dx,dy,v0,a0,d,m,g;
    n3 = 0;
    s = 0.;
    s0 = 0.;
    t = 1./100;
    //t0 = t/2.;
    vt = ui->ve->value()/40.;
    //a = ui->ac->value()/15.;
    v0 = vt;
    m = ui->m->value()/100.;    //摩擦系数
    g = ui->g->value()/5.;  //重力加速度
    do{
        ArcLengthPoint(s);
        n3++;
        s0 = s + v0 * t;
        if (s0>=1. || s0<=0)
            break;
        ArcLengthPoint(s0);
        dx = point3[n3].x() - point3[n3-1].x();
        dy = point3[n3].y() - point3[n3-1].y();
        d = sqrt(dx*dx + dy*dy);
        if (dy > 0) //测试前方斜率，计算合适的加速度
        {
            a0 = g/10 * (dy/d); //重力加速度
            if (vt>=0) a0 += -m*(dx/d); //根据速度方向加上摩擦阻力
            else a0 += m*(dx/d);
            if (a0 < 0 && ((vt<0.01 && vt>=0) || (vt<0 && vt>=-0.01)))  //特判，不能让摩擦阻力成为动力
            {
                a0 = 0;
                vt = 0;
            }
        }
        else    //同上
        {
            a0 = -g/10 * (-dy/d);
            if (vt>=0) a0 -= m*(dx/d);
            else a0 += m*(dx/d);
            if (a0 >0 && ((vt<0.01 && vt>=0) || (vt<0 && vt>=-0.01)))
            {
                a0 = 0;
                vt = 0;
            }
        }
        s += vt * t + (a0 * t * t)/2.0; //路程
        vt += a0 * t;   //速度
    }while (s <= 1. && s>=0. && n3 <998);

    run();
    ii = n3-1;
    pa->update();
}
