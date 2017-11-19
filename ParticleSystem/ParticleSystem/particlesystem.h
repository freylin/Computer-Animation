#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <QMainWindow>
#include <QtOpenGL>
#include <gl/glut.h>

//粒子属性
struct Particle
{
    float energy; // 粒子生命，表现为透明度
    float fade; // 衰减速度
    QVector3D pos;  //粒子位置
    QVector3D v;  //粒子速度
    QVector3D a;  //粒子加速度
    float w;    //粒子宽度
    float h;    //粒子高度
    int k;  //用于特殊的星空场景判断
};


class ParticleSystem : public QGLWidget
{
    Q_OBJECT

public:
    explicit ParticleSystem(QWidget *parent = 0);
    ~ParticleSystem();

protected:
    void initializeGL();    //初始化窗口
    void paintGL(); //绘制
    void resizeGL(int w, int h);   //调整窗口
    void InitializeSystem(int i); //初始化粒子状态
    void paint(int i);  //绘制单个粒子
    void keyPressEvent(QKeyEvent *k);   //键盘监听函数
protected slots:
    void timeOutSlot(); //计时器

private:
    QTimer *timer;    //计时器
    int maxParticles;  //粒子数量
    Particle pa[15000];    //粒子
    GLuint pc; // 当前的颜色序号
    GLuint tex[4];  //纹理
    int scene;  //场景编号
};

#endif // PARTICLESYSTEM_H
