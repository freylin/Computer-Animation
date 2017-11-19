#include "spline.h"
#include "ui_spline.h"
#include "stdio.h"
#include "stdlib.h"
#include "iostream"
#include <QPainter>
#include <QPointF>
#include <QLineF>

using namespace std;

int spline::n = 0;
int spline::Ngrain[30] = {0};
QPointF spline::point[1000] = {};
QPointF spline::point2[30][1000] = {};
double paint::t[30] = {0};

spline::spline(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::spline)
{
    grain = 0;
    ui->setupUi(this);
    pa = new paint(this);
    ui->verticalLayout->addWidget(pa);

    //connect(ui->groupBox, SIGNAL(clicked()),
                //this, SLOT(mousePressEvent(QMouseEvent *m)));
}

spline::~spline()
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

///////////////////////////////////////////////////////////////////////////// //
void spline::cspline()
{
    int i,np; n0=n; np=n0; CPt jd[100]; CPt *knots;

    //knots0=knots;
    for(i=1;i<=np;i++) {
            jd[i].x = point[i-1].x(); jd[i].y = point[i-1].y();
        }
    jd[0].x = point[0].x(); jd[0].y = point[0].y();
    jd[np+1].x = point[np-1].x(); jd[np+1].y = point[np-1].y();
    jd[np+2].x = point[np-1].x(); jd[np+2].y = point[np-1].y();

    np=np+3; knots=jd;
    CubicSpline(np, knots);
}

//spline message handlers
void spline::CubicSpline(int np, CPt *knots)
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
            point2[Ngrain[0]][++Ngrain[Ngrain[0]]-1].setX(Matrix(kml->x,k0->x,k1->x,k2->x,alpha[j]));   
            point2[Ngrain[0]][Ngrain[Ngrain[0]]-1].setY(Matrix(kml->y,k0->y,k1->y,k2->y,alpha[j]));
        }
        k0++; kml++; k1++; k2++;
    }
    pa->update();
}

void spline::GetCardinalMatrix(double a1)
{
    m[0]=-a1; m[1]=2.-a1; m[2]=a1-2.; m[3]=a1;
    m[4]=2.*a1; m[5]=a1-3.; m[6]=3.-2.*a1; m[7]=-a1;
    m[8]=-a1; m[9]=0.; m[10]=a1; m[11]=0.;
    m[12]=0.; m[13]=1.; m[14]=0.; m[15]=0.;
}

double spline::Matrix(double a, double b, double c, double d, double alpha)
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
        spline::point[spline::n].setX(m->x());
        spline::point[spline::n++].setY(m->y());
        update();
        //cout<<n<<" "<<xx<<" "<<yy<<endl;
    }
}

void paint::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPen pen;

    painter.setPen(QPen(Qt::black,4.0));
    if (spline::n>0) painter.drawPoints(spline::point,spline::n);

    if (spline::Ngrain[spline::Ngrain[0]]>0)
    {
        for (int i = 1; i <= spline::Ngrain[0]; i++)
        {
            //pen.setColor(qRgb((i*100+149) % 255, (i*200+55) % 255,(i*50+205) % 255));
            pen.setColor(qRgb((int)(t[i]*254) % 255, 0,90));
            pen.setWidthF(0.85);
            painter.setPen(pen);
            //painter.drawPoints(spline::point2[i],spline::Ngrain[i]);
            for (int j = 0; j < spline::Ngrain[i]-1; j++)
                painter.drawLine(spline::point2[i][j],spline::point2[i][j+1]);
        }
     }
}
/*void spline::on_groupBox_clicked()
{
    emit click();
}*/

void spline::on_Run_clicked(bool checked)
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

void spline::on_Reset_clicked()
{
    ui->grain->setValue(0);
    ui->tension->setValue(0);
    n = 0;
    Ngrain[0] = 0;
    pa->update();
}
