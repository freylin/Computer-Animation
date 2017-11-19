#include "imagemorphing.h"
#include "ui_imagemorphing.h"
#include "paint.h"
#include "stdio.h"
#include "stdlib.h"
#include "iostream"
#include <QPainter>
#include <QPointF>
#include <QLineF>
#include <QPolygon>

using namespace std;


ImageMorphing::ImageMorphing(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::imagemorphing)
{
    //初始化
    nface = 0;
    Init();
    ui->setupUi(this);
}

void ImageMorphing::Init()
{
    n = 0;
    k = 2;
    db = 0;
    //读取nface.jpg和(nface+1).jpg编号的两张图像
    char c[1];
    itoa(nface,c,10);
    string s = ":/";
    s = s+c+".jpg";
    pix[0] = QImage(s.c_str());
    itoa(nface+1,c,10);
    s = ":/";
    s = s+c+".jpg";
    pix[1] = QImage(s.c_str());

    //设置不同排版位置，0.jpg和1.jpg图像为横板
    if (nface == 0)
    {
        w = 280;
        h = (int)(pix[0].height()/(pix[0].width()/w));
        pix[0] = pix[0].scaled(w,h);
        pix[1] = pix[1].scaled(w,h);
        pix[2] = QImage(pix[0].size(),QImage::Format_RGB32);
        //图像位置
        pos[0].setX(200);
        pos[1].setX(pos[0].x());
        pos[2].setX(pos[0].x() + w + 100);
        pos[0].setY(60);
        pos[1].setY(pos[0].y() + h + 30);
        pos[2].setY(pos[0].y() + h/2 + 15);
    }
    else    //其余为竖版
    {
        w = 220;
         h = (int)(pix[0].height()/(pix[0].width()/w));
        pix[0] = pix[0].scaled(w,h);
        pix[1] = pix[1].scaled(w,h);
        pix[2] = QImage(pix[0].size(),QImage::Format_RGB32);
        //图像位置
        pos[0].setX(200);
        pos[1].setX(pos[0].x() + w + 35);
        pos[2].setX(pos[0].x() + (w + 35)*2);
        pos[0].setY(100);
        pos[1].setY(pos[0].y());
        pos[2].setY(pos[0].y());
    }
}

ImageMorphing::~ImageMorphing()
{
    delete ui;
}

void norm(double* a, double b)
{
    if (*a < 0) *a = 0;
    if (*a > b) *a = b;
}

void ImageMorphing::GetColor(double x1, double y1, double x2, double y2, double q)
{
    norm(&x1, w-1);
    norm(&y1, h-1);
    norm(&x2, w-1);
    norm(&y2, h-1);
    //获取两张图像对应坐标的颜色
    QColor c1 = QColor(pix[0].pixel(x1,y1));
    QColor c2 = QColor(pix[1].pixel(x2,y2));
    //求取第三张图像的颜色
    color.setRed(c1.red()*(1.0-q) + c2.red()*q);
    color.setGreen(c1.green()*(1.0-q) + c2.green()*q);
    color.setBlue(c1.blue()*(1.0-q) + c2.blue()*q);
}

void ImageMorphing::Morphing(int n)
{
    int nn = 15, ii;    //变形图像数量
    double o[101], PQpq[101], absPQ[101], abspq[101], abs0[101];
    double abs0_2[101];
    double divabs1[101], divabs2[101];
    double m1[101], m2[101], n1[101], n2[101], a1[101], a2[101], b1[101], b2[101];
    double dm[101],dn[101],da[101],db[101];
    for (int i = 0; i < n; i+=4)
    {
        int ii = i/4;
        //存储线段端点的像素坐标
        m1[ii] = point[i+2].x()-pos[1].x();
        m2[ii] = point[i+3].x()-pos[1].x();
        n1[ii] = point[i+2].y()-pos[1].y();
        n2[ii] = point[i+3].y()-pos[1].y();
        a1[ii] = point[i].x()-pos[0].x();
        a2[ii] = point[i+1].x()-pos[0].x(),
        b1[ii] = point[i].y()-pos[0].y();
        b2[ii] = point[i+1].y()-pos[0].y();

        //存储坐标差值
        dm[ii] = m2[ii] - m1[ii];
        da[ii] = a2[ii] - a1[ii];
        dn[ii] = n2[ii] - n1[ii];
        db[ii] = b2[ii] - b1[ii];

        //计算对应特征线段夹角
        PQpq[ii] = ( m2[ii] - m1[ii] ) * ( a2[ii] - a1[ii] ) + ( n2[ii] - n1[ii] ) * ( b2[ii] - b1[ii] );
        absPQ[ii] = pow( pow(da[ii],2) + pow(db[ii],2), 0.5 );
        abspq[ii] = pow( pow(dm[ii],2) + pow(dn[ii],2), 0.5 );
        o[ii] = acos( PQpq[ii] / (absPQ[ii]*abspq[ii]) );
    }

    double x[3][101],y[3][101], dx[101], dy[101], dx2[101], dy2[101];
    double mina = 0.00001; //除数极小量
    for (int j = 1; j <= nn; j++)
    {
        for (int i = 0; i < n; i+=4)
        {
            ii = i/4;
            double ar = (j/(nn+1.0)) * o[ii]; //角度插值大小
            //线段中点、长度插值，获取变形图像特征线段的端点坐标
            double s = ( sin(ar) * absPQ[ii] ) / ( sin(ar)*absPQ[ii] + sin(o[ii]-ar)*abspq[ii] + mina);
            x[1][ii] = (m1[ii] - a1[ii]) * s + a1[ii];
            y[1][ii] = (n1[ii] - b1[ii]) * s + b1[ii];
            x[2][ii] = (m2[ii] - a2[ii]) * s + a2[ii];
            y[2][ii] = (n2[ii] - b2[ii]) * s + b2[ii];

            dx[ii] = x[2][ii]-x[1][ii];
            dx2[ii] = pow(dx[ii],2);
            if (dx[ii] == 0) dx[ii] = mina;
            dy[ii] = y[2][ii]-y[1][ii];
            dy2[ii] = pow(dy[ii],2);
            if (dy[ii] == 0) dy[ii] = mina;
            abs0[ii] = pow( dx2[ii] + dy2[ii], 0.5 );//变形图像特征线段长度
            divabs1[ii] = abs0[ii]/absPQ[ii]; //变形图像与源图像对应特征线段长度的比值
            divabs2[ii] = abs0[ii]/abspq[ii]; //变形图像与目标图像对应特征线段长度的比值
            abs0_2[ii] = pow(abs0[ii], 2);
        }

        double e1, f1;
        double sumou;
        double fzx1, fzy1;
        double fzx2, fzy2;
        double u,e2,f2;
        for (int r = 0; r < w; r++)
        {
            e1 = r;
            for (int t = 0; t < h; t++)
            {
                f1 =t;
                sumou = 0.0;
                fzx1 = 0.0; fzy1 = 0.0;
                fzx2 = 0.0; fzy2 = 0.0;
                for (int i = 0; i < n; i+=4)
                {
                    ii = i/4;
                    //以特征线段长度为单位的像素点(r,t)在特征线段上的投影到线段首端点的距离
                    u = ( (e1-x[1][ii])*dx[ii] + (f1-y[1][ii])*dy[ii] )/ abs0_2[ii];
                    //以特征线段长度为单位的像素点(r,t)在特征线段上的投影的坐标
                    e2 = dx[ii] * u + x[1][ii];
                    f2 = dy[ii] * u + y[1][ii];
                    //根据相似性，源图像中以特征线段长度为单位的对应点在特征线段上的投影的坐标
                    double c2 = (da[ii]) * u + a1[ii],
                            d2 = (db[ii]) * u + b1[ii];
                    //根据相似性与对应点到特征线段的距离相等，获取源图像中对应点的实际像素坐标
                    double c1 = divabs1[ii] * (e1-e2)/dy[ii] * (db[ii]) + c2,
                            d1 = divabs1[ii] * (f1-f2)/dx[ii] * (da[ii]) + d2;
                    //变形图像中像素点到特征线段的距离
                    double dist = pow( pow(e1 - e2, 2) + pow(f1 - f2, 2) , 0.5 );
                    double ou = pow( dx2[ii]+dy2[ii] ,p/2.0)
                            / ( a + dist);
                    ou = pow(ou, b);    //权值
                    sumou += ou;
                    fzx1 += ou * (c1-r);
                    fzy1 += ou * (d1-t);

                    //获取目标图像中对应点的像素坐标，原理同上
                    c2 = (dm[ii]) * u + m1[ii];
                    d2 = (dn[ii]) * u + n1[ii];
                    c1 = divabs2[ii] * (e1-e2)/dy[ii] * (dn[ii]) + c2;
                    d1 = divabs2[ii] * (f1-f2)/dx[ii] * (dm[ii]) + d2;
                    fzx2 += ou * (c1-r);
                    fzy2 += ou * (d1-t);
                }
                if (sumou == 0) sumou = mina;
                //获取颜色
                GetColor(fzx1/sumou+r, fzy1/sumou+t, fzx2/sumou+r, fzy2/sumou+t, (double)(j)/(nn+1.0));
                //修改颜色
                pix[2].setPixel(r, t, qRgb(color.red(), color.green(), color.blue()));
            }
        }
        //重新绘制
        repaint();
    }
}


void ImageMorphing::on_Run_clicked(bool checked)    //MORPHING触发函数
{
    if (!checked)
    {
        k = 3;
        a = (double)(ui->a->value()) / 100.0;
        b = (double)(ui->b->value()) / 10.0;
        p = (double)(ui->p->value()) / 10.0;
        Morphing(n/4*4);    //（n/4*4）为了剔除多余点
        update();
    }
}

void ImageMorphing::on_Reset_clicked()  //“重置”按钮触发槽函数
{
    ui->a->setValue(1);   //各种参数都清零，并刷新画布
    ui->b->setValue(5);
    ui->p->setValue(0);
    Init(); //初始化
    update();
}


void ImageMorphing::on_Cancel_clicked() //“撤销”按钮
{
    if (n > 0)
    {
        n-=2;   //删除最近两个点
        update();
    }
}

void ImageMorphing::on_Change_clicked() //更换图像按钮
{
    nface = (nface + 2) % 6;    //在三组图像中轮换
    Init(); //初始化所有参数
    update();
}
