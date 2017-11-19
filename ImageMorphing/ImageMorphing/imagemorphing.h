#ifndef IMAGEMORPHING_H
#define IMAGEMORPHING_H

#include <QMainWindow>
#include <QtGui>
#include "paint.h"

namespace Ui {
class imagemorphing;
}

class ImageMorphing : public QMainWindow
{
    Q_OBJECT

public:
    explicit ImageMorphing(QWidget *parent = 0);
    ~ImageMorphing();
    //点个数、显示图像个数、人脸切换序号
    int n,k,nface;
    int a,b,p;  //参数
    QPointF point[401]; //点坐标
    QPointF pos[3];   //图片位置
    QPointF d[2];   //动态线段
    int db; //动态线段flag
    QImage pix[3];  //存储图像
    double w,h; //图像宽高
    void Morphing(int n);   //morphing函数
    //设置像素点的颜色
    void GetColor(double x1, double y1, double x2, double y2, double q);
    QColor color;   //像素颜色
    void Init();    //初始化函数

private slots:  //设置槽函数
    void on_Run_clicked(bool checked);   //点击“Run”按键的关联函数
    void on_Reset_clicked(); //点击“Reset”按键的关联函数
    void on_Cancel_clicked();   //撤销按键
    void on_Change_clicked();   //更换图像

private:
    Ui::imagemorphing *ui;
    void mouseMoveEvent(QMouseEvent *m);    //鼠标追踪函数
    void mousePressEvent(QMouseEvent *m);   //捕捉鼠标点击画布的信息
    void paintEvent(QPaintEvent*);  //将得到的插值点等绘制成曲线
};
#endif // IMAGEMORPHING_H
