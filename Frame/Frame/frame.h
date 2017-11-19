#ifndef FRAME_H
#define FRAME_H

#include <QMainWindow>
#include <QtGui>
#include "paint.h"

namespace Ui {
class frame;
}

struct CPT { double x; double y;};
#define CPt CPT

class frame : public QMainWindow
{
    Q_OBJECT

public:
    explicit frame(QWidget *parent = 0);
    ~frame();

    //x[100], y[100]用来存放特征控制点坐标，n为控制点数量
    //grain控制每两个相邻控制点之间样条插值后的添加点数，一般grain<20可以满足大部分应用要求
    // tension ∈[0.0，1.]控制样条插值曲线通过控制点处的曲率，实验中应该考察tension数值变化对拟合曲线形状带来什么影响；
    void cspline();
    static int n[3];
    static int k;
    static int n2[3];
    int grain;
    double tension;
    static QPointF point[3][1000];
    static QPoint point2[3][1000];
    int last;
    static QColor color[3];
    void CubicSpline(int n, CPt *knots);

    double Min(double x, double y);
    void Linear();
    void VLinear();

private slots:  //设置槽函数
    void on_Run_clicked(bool checked);   //点击“Run”按键的关联函数

    void on_Reset_clicked(); //点击“Reset”按键的关联函数

    void on_tension_sliderMoved(int position);

    //void on_Second_clicked();

    void on_Run_2_clicked();

private:
    Ui::frame *ui;
    paint *pa;
    CPt *knots0;
    int n0;
    double m[16];
    double Matrix(double a, double b, double c, double d, double alpha);
    void GetCardinalMatrix(double a1);
    void mouseReleaseEvent(QMouseEvent *m);
    void mouseMoveEvent(QMouseEvent *m);
};
#endif // FRAME_H
