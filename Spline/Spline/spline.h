#ifndef SPLINE_H
#define SPLINE_H

#include <QMainWindow>
#include <QtGui>

namespace Ui {
class spline;
}

struct CPT { double x; double y;};
#define CPt CPT

class paint : public QWidget 
{
    Q_OBJECT
public:
    explicit paint(QWidget *parent = 0);
    ~paint();

    static double t[30];
    void mousePressEvent(QMouseEvent *m);
    void paintEvent(QPaintEvent*);

};

class spline : public QMainWindow
{
    Q_OBJECT
/*signals:
    void click();*/
    // Construction
public:
    explicit spline(QWidget *parent = 0);
    ~spline();
public:
    void cspline();
    static int n;
    static int Ngrain[30];
    int grain;
    double tension;
    static QPointF point[1000];
    static QPointF point2[30][1000];
    int last;
    void CubicSpline(int n, CPt *knots);

private slots:
    void on_Run_clicked(bool checked);

    void on_Reset_clicked();

private:
    Ui::spline *ui;
    paint *pa;
    CPt *knots0;
    int n0;
    double m[16];
    double Matrix(double a, double b, double c, double d, double alpha);
    void GetCardinalMatrix(double a1);
};

#endif // SPLINE_H
