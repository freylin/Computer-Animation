#ifndef PARAMETERIZATION_H
#define PARAMETERIZATION_H

#include <QMainWindow>
#include <QtGui>
#include <QPropertyAnimation>
#include <QLabel>

namespace Ui {
class Parameterization;
}

struct CPT { double x; double y;};
#define CPt CPT

class paint : public QWidget    //画布类
{
    Q_OBJECT
public:
    explicit paint(QWidget *parent = 0);
    ~paint();

    static double t[30];
    void mousePressEvent(QMouseEvent *m);   //捕捉鼠标点击画布的信息
    void paintEvent(QPaintEvent*);  //将得到的插值点等绘制成曲线

};

class Parameterization : public QMainWindow
{
    Q_OBJECT

public:
    explicit Parameterization(QWidget *parent = 0);
    ~Parameterization();
public:
    //x[100], y[100]用来存放特征控制点坐标，n为控制点数量
    //grain控制每两个相邻控制点之间样条插值后的添加点数，一般grain<20可以满足大部分应用要求
    // tension ∈[0.0，1.]控制样条插值曲线通过控制点处的曲率，实验中应该考察tension数值变化对拟合曲线形状带来什么影响；
    void cspline();
    static int n;
    static int n3;  //图像帧数
    static int Ngrain[30];
    int grain;
    double tension;
    CPt p[100];
    double Cx[4], Cy[4];
    double seg_length[1000];    //累加弧长表
    double total_length;    //曲线总长度
    static QPointF point[1000];
    static QPointF point2[30][1000];
    static QPointF point3[3000];   //每一帧图像所在位置
    QPropertyAnimation *anime;  //动画
    QSequentialAnimationGroup *anime2;  //动画组
    QLabel *car;    //小车
    static int ii;
    int last;
    void CubicSpline(int n, CPt *knots);
    void SetLengths(int np);
    void SegCoef(int i);
    double ArcLength(double a, double b);
    double ArcIntegrand(double u);
    void ArcLengthPoint(double s);
    void GetPointOnSpline(double u);
    void run(); //运行动画

private slots:  //设置槽函数
    void on_Run_clicked(bool checked);   //点击“生成曲线”按键的关联函数

    void on_Reset_clicked(); //点击“重置”按键的关联函数

    void on_Run1_clicked();   //点击“匀速运动”按键的关联函数

    void on_Run2_clicked();   //点击“加速运动”按键的关联函数

    void on_Run3_clicked();   //点击“先匀速后加速运动”按键的关联函数

    void on_Run4_clicked();   //点击“模拟重力运动”按键的关联函数

private:
    Ui::Parameterization *ui;
    paint *pa;
    CPt *knots0;
    int n0;
    double m[16];
    double Matrix(double a, double b, double c, double d, double alpha);
    void GetCardinalMatrix(double a1);
};

#endif // PARAMETERIZATION_H
