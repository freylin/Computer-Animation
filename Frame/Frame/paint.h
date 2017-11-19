#ifndef PAINT
#define PAINT

#include <QMainWindow>
#include <QtGui>

class paint : public QWidget    //画布类
{
    Q_OBJECT
public:
    explicit paint(QWidget *parent = 0);
    ~paint();
    void mousePressEvent(QMouseEvent *m);   //捕捉鼠标点击画布的信息
    void paintEvent(QPaintEvent*);  //将得到的插值点等绘制成曲线
};

#endif // PAINT

