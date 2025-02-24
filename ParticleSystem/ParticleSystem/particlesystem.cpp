#include "ParticleSystem.h"
#include <ctime>

//颜色
float color[5][3] = {{0.9, 0.9, 0.9}, {0.5, 0.5, 1.0}, {0.5, 1.0, 0.5},
                     {1.0, 0.5, 0.5}, {1.0, 0.75, 0.5}};

ParticleSystem::ParticleSystem(QWidget *parent) :
    QGLWidget(parent)
{
    maxParticles = 5000;   //粒子数
    pc = 0; //颜色序号
    scene = 0; //场景编号
    setWindowTitle("ParticleSystem");
    showMaximized();    //全屏效果较好
    timer = new QTimer(this);   //计时器
    connect(timer, SIGNAL(timeout()), this, SLOT(timeOutSlot()));
    timer->start(30);
}

void ParticleSystem::InitializeSystem(int i)
{
    if (scene == 0) //雪景
    {
        glBindTexture(GL_TEXTURE_2D, tex[0]);  //选择纹理
        //粒子初始位置
        pa[i].pos.setX(rand() % 20 - 10.0);
        pa[i].pos.setY(12);
        pa[i].pos.setZ( (rand() % 249 - 250.0)/10.0 );
        //粒子尺寸
        pa[i].w = 0.18;
        pa[i].h = 0.18;
        //粒子速度
        pa[i].v.setX( (rand() % 80 - 40.0) / 1000.0);
        pa[i].v.setY( (rand() % 50 - 70.0) / 1000.0);
        pa[i].fade = (rand() % 100 + 0.05 ) / 1000.0;  //衰减量
        pa[i].energy = 1.0;   //粒子能量
    }
    else if (scene == 1)    //雨景
    {
        glBindTexture(GL_TEXTURE_2D, tex[2]);  //选择纹理
        //粒子初始位置
        pa[i].pos.setY(0.5);
        pa[i].pos.setZ((rand() % 100 - 100.0)/100.0);
        pa[i].pos.setX( (rand() % 15000 - 7500.0)/10000.0 );
        //粒子尺寸
        pa[i].w = 0.001;
        pa[i].h = ( rand() % 100 ) / 1000.0 + 0.001;
        //粒子速度
        pa[i].v.setX(0.0);
        pa[i].v.setY( (rand() % 50 - 200.0) / 10000.0); //-0.02~-0.015
        pa[i].fade = (rand() % 100 + 0.05 ) / 1000.0;  //衰减量
        pa[i].energy = 1.0;   //粒子能量
    }
    else    //星空
    {
        pa[i].k = -1;   //星空粒子能量初始为递减
        glBindTexture(GL_TEXTURE_2D, tex[3]); //选择纹理
        //粒子初始位置
        pa[i].pos.setY( (rand() % 100 - 50.0)/100.0 );  //-0.5~0.5
        pa[i].pos.setZ((rand() % 100 - 100.0)/100.0);   //-1~0
        pa[i].pos.setX( (rand() % 15000 - 7500.0)/10000.0 );    //-0.75~0.75
        //粒子尺寸,0.005~0.015
        pa[i].w = ( rand() % 100 ) / 10000.0 + 0.005;
        pa[i].h = pa[i].w;
        //粒子速度
        pa[i].v.setX(0.0);
        pa[i].v.setY(0.0);
        pa[i].fade = (rand() % 100 + 0.05 ) / 5000.0;  //衰减量
        pa[i].energy = 1.0;   //粒子能量
    }
    pa[i].v.setZ(0.0);
    //加速度
    pa[i].a.setX(0.0);
    pa[i].a.setY(0.0);

}

void ParticleSystem::initializeGL()
{
    //载入纹理图片
    QImage pix1 = QGLWidget::convertToGLFormat(QImage(":/1.bmp"));
    QImage pix2 = QGLWidget::convertToGLFormat(QImage(":/2.jpg"));
    QImage pix3 = QGLWidget::convertToGLFormat(QImage(":/3.jpg"));
    QImage pix4 = QGLWidget::convertToGLFormat(QImage(":/4.jpg"));
    //绑定纹理
    glGenTextures(4, tex);
    //设置像素存储模式控制所读取的图像数据的行对齐方式
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glBindTexture(GL_TEXTURE_2D, tex[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, pix1.width(), pix1.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, pix1.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glBindTexture(GL_TEXTURE_2D, tex[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, pix2.width(), pix2.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pix2.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glBindTexture(GL_TEXTURE_2D, tex[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, pix3.width(), pix3.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pix3.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glBindTexture(GL_TEXTURE_2D, tex[3]);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, pix4.width(), pix4.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pix4.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClearDepth(1.0);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    //随机数
    srand(time(NULL));
    for (int i = 0; i < maxParticles; i++) {
        InitializeSystem(i);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    }
}

void ParticleSystem::paint(int i)
{
    //粒子颜色
    if (scene == 2) //星体特殊判断
    {
        if (pa[i].w < 0.006)
            glColor4f(color[i%2+3][0],
                    color[i%2+3][1],
                    color[i%2+3][2],
                    pa[i].energy);
        else glColor4f(color[i%2][0],
                color[i%2][1],
                color[i%2][2],
                pa[i].energy);
    } else glColor4f(color[pc][0],
            color[pc][1],
            color[pc][2],
            pa[i].energy);

    //绘制
    glBegin(GL_POLYGON);
    {
        //对应纹理坐标和粒子坐标
        glTexCoord2d(1, 1);
        glVertex3f(pa[i].pos.x() + pa[i].w/2.0,
                   pa[i].pos.y() + pa[i].h/2.0,
                   pa[i].pos.z());
        glTexCoord2d(0, 1);
        glVertex3f(pa[i].pos.x() - pa[i].w/2.0,
                   pa[i].pos.y() + pa[i].h/2.0,
                   pa[i].pos.z());
        glTexCoord2d(0, 0);
        glVertex3f(pa[i].pos.x() - pa[i].w/2.0,
                   pa[i].pos.y() - pa[i].h/2.0,
                   pa[i].pos.z());
        glTexCoord2d(1, 0);
        glVertex3f(pa[i].pos.x() + pa[i].w/2.0,
                   pa[i].pos.y() - pa[i].h/2.0,
                   pa[i].pos.z());
    }
    glEnd();
}


void ParticleSystem::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    for (int i = 0; i < maxParticles; i++) {
        paint(i);    //绘制该粒子
        //速度变化
        pa[i].v.setX(pa[i].v.x() + pa[i].a.x());
        pa[i].v.setY(pa[i].v.y() + pa[i].a.y());
        //位置变化
        pa[i].pos.setX(pa[i].pos.x() + pa[i].v.x());
        pa[i].pos.setY(pa[i].pos.y() + pa[i].v.y());
        //星空粒子不消亡(流星除外),生命值循环
        if (scene == 2 )
        {
            pa[i].energy += pa[i].k * pa[i].fade;
            if (pa[i].energy< 0.1)  //若能量小于0.1
            {
                //粒子能量开始递增，不再衰减
                pa[i].k = 1;
                pa[i].energy = 0.1;
            };
            if (pa[i].energy>1.0)  //若能量大于1.0
            {
                //满足以下条件的粒子将成为流星
                //随机量为0，尺寸大于0.01且深度较浅、高度高于0.42
                if (rand()%30 == 0 && pa[i].w>0.01 && pa[i].pos.z() < 0.05 && pa[i].pos.y()> 0.45)
                {
                    //具备速度
                    pa[i].v.setX(-0.03);
                    pa[i].v.setY(-0.04);
                }
                //粒子能量开始衰减
                pa[i].k = -1;
                pa[i].energy = 1.0;
            };
            //若流星位置超过一定范围则回收
            if (pa[i].pos.x() > 10) InitializeSystem(i);
        }
        else
        {
            pa[i].energy -= pa[i].fade; //线性衰减
            if (pa[i].energy < 0.0)
                InitializeSystem(i); //若消亡则回收粒子
        }
    }
}

//调整窗口
void ParticleSystem::resizeGL(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (w > h)
        gluPerspective(45.0, (GLdouble)w / (GLdouble)h, 0.1, 100.0);
    else
        gluPerspective(45.0, (GLdouble)h / (GLdouble)w, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

//键盘监听函数
void ParticleSystem::keyPressEvent(QKeyEvent *k)
{
    switch (k->key()) {
    //Space键切换场景
    case Qt::Key_Space:
        scene = (scene+1) % 3;
        for (int i = 0; i < maxParticles; i++)
            InitializeSystem(i);
        break;
    //R键重新运行
    case Qt::Key_R:
        for (int i = 0; i < maxParticles; i++)
            InitializeSystem(i);
        paintGL();
        break;
    //C键切换颜色
    case Qt::Key_C:
        pc = (pc + 1) % 5;
        break;
    //"+"键增加粒子数
    case Qt::Key_Plus:
        if (maxParticles < 15000)
        {
            for (int i = maxParticles; i < maxParticles + 1000; i++)
                InitializeSystem(i);
            maxParticles += 1000;
            paintGL();
        }
        break;
    //"-"键减少粒子数
    case Qt::Key_Minus:
        if (maxParticles > 1000)
        {
            maxParticles -= 1000;
        }
        break;
    //Esc键退出
    case Qt::Key_Escape:
        close();
        break;
    default:
        break;
    }
}

//计时函数
void ParticleSystem::timeOutSlot()
{
    updateGL();
}

//析构
ParticleSystem::~ParticleSystem()
{

}
