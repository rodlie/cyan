#include "colorwheel.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

ColorWheel::ColorWheel(QWidget *parent) : QWidget(parent)
  /*, spinR(nullptr)
  , spinG(nullptr)
  , spinB(nullptr)
  , slideR(nullptr)
  , slideG(nullptr)
  , slideB(nullptr)
  , spinC(nullptr)
  , spinM(nullptr)
  , spinY(nullptr)
  , spinK(nullptr)
  , slideC(nullptr)
  , slideM(nullptr)
  , slideY(nullptr)
  , slideK(nullptr)
  , spinH(nullptr)
  , spinS(nullptr)
  , spinV(nullptr)
  , slideH(nullptr)
  , slideS(nullptr)
  , slideV(nullptr)*/
  , triangle(nullptr)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5,5,5,5);
    mainLayout->setSpacing(0);

    triangle = new QtColorTriangle(this);
    //triangle->setMinimumHeight(192);
    //triangle->setMaximumHeight(192);
    triangle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    /*QWidget *rWidget = new QWidget(this);
    QHBoxLayout *rLayout = new QHBoxLayout(rWidget);
    QWidget *gWidget = new QWidget(this);
    QHBoxLayout *gLayout = new QHBoxLayout(gWidget);
    QWidget *bWidget = new QWidget(this);
    QHBoxLayout *bLayout = new QHBoxLayout(bWidget);

    QWidget *cWidget = new QWidget(this);
    QHBoxLayout *cLayout = new QHBoxLayout(cWidget);
    QWidget *mWidget = new QWidget(this);
    QHBoxLayout *mLayout = new QHBoxLayout(mWidget);
    QWidget *yWidget = new QWidget(this);
    QHBoxLayout *yLayout = new QHBoxLayout(yWidget);
    QWidget *kWidget = new QWidget(this);
    QHBoxLayout *kLayout = new QHBoxLayout(kWidget);

    QWidget *hWidget = new QWidget(this);
    QHBoxLayout *hLayout = new QHBoxLayout(hWidget);
    QWidget *sWidget = new QWidget(this);
    QHBoxLayout *sLayout = new QHBoxLayout(sWidget);
    QWidget *vWidget = new QWidget(this);
    QHBoxLayout *vLayout = new QHBoxLayout(vWidget);

    rLayout->setContentsMargins(0,0,0,0);
    gLayout->setContentsMargins(0,0,0,0);
    bLayout->setContentsMargins(0,0,0,0);

    cLayout->setContentsMargins(0,0,0,0);
    mLayout->setContentsMargins(0,0,0,0);
    yLayout->setContentsMargins(0,0,0,0);
    kLayout->setContentsMargins(0,0,0,0);

    hLayout->setContentsMargins(0,0,0,0);
    sLayout->setContentsMargins(0,0,0,0);
    vLayout->setContentsMargins(0,0,0,0);

    slideR = new QSlider(this);
    slideG = new QSlider(this);
    slideB = new QSlider(this);

    slideC = new QSlider(this);
    slideM = new QSlider(this);
    slideY = new QSlider(this);
    slideK = new QSlider(this);

    slideH = new QSlider(this);
    slideS = new QSlider(this);
    slideV = new QSlider(this);

    slideR->setRange(0,255);
    slideG->setRange(0,255);
    slideB->setRange(0,255);

    slideC->setRange(0,255);
    slideM->setRange(0,255);
    slideY->setRange(0,255);
    slideK->setRange(0,255);

    slideH->setRange(0,1000);
    slideS->setRange(0,1000);
    slideV->setRange(0,1000);

    slideR->setOrientation(Qt::Horizontal);
    slideG->setOrientation(Qt::Horizontal);
    slideB->setOrientation(Qt::Horizontal);

    slideC->setOrientation(Qt::Horizontal);
    slideM->setOrientation(Qt::Horizontal);
    slideY->setOrientation(Qt::Horizontal);
    slideK->setOrientation(Qt::Horizontal);

    slideH->setOrientation(Qt::Horizontal);
    slideS->setOrientation(Qt::Horizontal);
    slideV->setOrientation(Qt::Horizontal);

    slideR->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    slideG->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    slideB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    slideC->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    slideM->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    slideY->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    slideK->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    slideH->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    slideS->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    slideV->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    spinR = new QSpinBox(this);
    spinG = new QSpinBox(this);
    spinB = new QSpinBox(this);

    spinC = new QSpinBox(this);
    spinM = new QSpinBox(this);
    spinY = new QSpinBox(this);
    spinK = new QSpinBox(this);

    spinH = new QDoubleSpinBox(this);
    spinS = new QDoubleSpinBox(this);
    spinV = new QDoubleSpinBox(this);

    spinR->setRange(0,255);
    spinG->setRange(0,255);
    spinB->setRange(0,255);

    spinC->setRange(0,255);
    spinM->setRange(0,255);
    spinY->setRange(0,255);
    spinK->setRange(0,255);

    spinH->setDecimals(3);
    spinS->setDecimals(3);
    spinV->setDecimals(3);

    spinH->setSingleStep(0.01);
    spinS->setSingleStep(0.01);
    spinV->setSingleStep(0.01);

    spinH->setRange(0,1);
    spinS->setRange(0,1);
    spinV->setRange(0,1);

    spinR->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    spinG->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    spinB->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    spinC->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    spinM->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    spinY->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    spinK->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    spinH->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    spinS->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    spinV->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    spinR->setMinimumWidth(70);
    spinR->setMaximumWidth(70);
    spinG->setMinimumWidth(70);
    spinG->setMaximumWidth(70);
    spinB->setMinimumWidth(70);
    spinB->setMaximumWidth(70);

    spinC->setMinimumWidth(70);
    spinC->setMaximumWidth(70);
    spinM->setMinimumWidth(70);
    spinM->setMaximumWidth(70);
    spinY->setMinimumWidth(70);
    spinY->setMaximumWidth(70);
    spinK->setMinimumWidth(70);
    spinK->setMaximumWidth(70);

    spinH->setMinimumWidth(70);
    spinH->setMaximumWidth(70);
    spinS->setMinimumWidth(70);
    spinS->setMaximumWidth(70);
    spinV->setMinimumWidth(70);
    spinV->setMaximumWidth(70);

    QLabel *labelR = new QLabel(this);
    QLabel *labelG = new QLabel(this);
    QLabel *labelB = new QLabel(this);
    QLabel *labelC = new QLabel(this);
    QLabel *labelM = new QLabel(this);
    QLabel *labelY = new QLabel(this);
    QLabel *labelK = new QLabel(this);
    QLabel *labelH = new QLabel(this);
    QLabel *labelS = new QLabel(this);
    QLabel *labelV = new QLabel(this);

    labelR->setText("R");
    labelG->setText("G");
    labelB->setText("B");

    labelC->setText("C");
    labelM->setText("M");
    labelY->setText("Y");
    labelK->setText("K");

    labelH->setText("H");
    labelS->setText("S");
    labelV->setText("V");

    labelR->setMinimumWidth(10);
    labelR->setMaximumWidth(10);
    labelG->setMinimumWidth(10);
    labelG->setMaximumWidth(10);
    labelB->setMinimumWidth(10);
    labelB->setMaximumWidth(10);

    labelC->setMinimumWidth(10);
    labelC->setMaximumWidth(10);
    labelM->setMinimumWidth(10);
    labelM->setMaximumWidth(10);
    labelY->setMinimumWidth(10);
    labelY->setMaximumWidth(10);
    labelK->setMinimumWidth(10);
    labelK->setMaximumWidth(10);

    labelH->setMinimumWidth(10);
    labelH->setMaximumWidth(10);
    labelS->setMinimumWidth(10);
    labelS->setMaximumWidth(10);
    labelV->setMinimumWidth(10);
    labelV->setMaximumWidth(10);

    rLayout->addWidget(labelR);
    rLayout->addWidget(slideR);
    rLayout->addWidget(spinR);
    gLayout->addWidget(labelG);
    gLayout->addWidget(slideG);
    gLayout->addWidget(spinG);
    bLayout->addWidget(labelB);
    bLayout->addWidget(slideB);
    bLayout->addWidget(spinB);

    cLayout->addWidget(labelC);
    cLayout->addWidget(slideC);
    cLayout->addWidget(spinC);
    mLayout->addWidget(labelM);
    mLayout->addWidget(slideM);
    mLayout->addWidget(spinM);
    yLayout->addWidget(labelY);
    yLayout->addWidget(slideY);
    yLayout->addWidget(spinY);
    kLayout->addWidget(labelK);
    kLayout->addWidget(slideK);
    kLayout->addWidget(spinK);

    hLayout->addWidget(labelH);
    hLayout->addWidget(slideH);
    hLayout->addWidget(spinH);
    sLayout->addWidget(labelS);
    sLayout->addWidget(slideS);
    sLayout->addWidget(spinS);
    vLayout->addWidget(labelV);
    vLayout->addWidget(slideV);
    vLayout->addWidget(spinV);*/

    mainLayout->addWidget(triangle);
    /*mainLayout->addSpacing(5);

    mainLayout->addWidget(hWidget);
    mainLayout->addWidget(sWidget);
    mainLayout->addWidget(vWidget);
    mainLayout->addSpacing(5);

    mainLayout->addWidget(rWidget);
    mainLayout->addWidget(gWidget);
    mainLayout->addWidget(bWidget);
    mainLayout->addSpacing(5);

    mainLayout->addWidget(cWidget);
    mainLayout->addWidget(mWidget);
    mainLayout->addWidget(yWidget);
    mainLayout->addWidget(kWidget);
    //mainLayout->addSpacing(5);

    mainLayout->addStretch();*/

    triangle->setColor(QColor(Qt::black));
    handleColorChanged(triangle->color());

    connect(triangle, SIGNAL(colorChanged(QColor)),
            this, SLOT(handleColorChanged(QColor)));

    /*connect(spinR, SIGNAL(valueChanged(int)),
            this, SLOT(handleColorRChanged(int)));
    connect(spinG, SIGNAL(valueChanged(int)),
            this, SLOT(handleColorGChanged(int)));
    connect(spinB, SIGNAL(valueChanged(int)),
            this, SLOT(handleColorBChanged(int)));

    connect(slideR, SIGNAL(sliderMoved(int)), spinR, SLOT(setValue(int)));
    connect(slideG, SIGNAL(sliderMoved(int)), spinG, SLOT(setValue(int)));
    connect(slideB, SIGNAL(sliderMoved(int)), spinB, SLOT(setValue(int)));

    connect(spinC, SIGNAL(valueChanged(int)),
            this, SLOT(handleColorCChanged(int)));
    connect(spinM, SIGNAL(valueChanged(int)),
            this, SLOT(handleColorMChanged(int)));
    connect(spinY, SIGNAL(valueChanged(int)),
            this, SLOT(handleColorYChanged(int)));
    connect(spinK, SIGNAL(valueChanged(int)),
            this, SLOT(handleColorKChanged(int)));

    connect(slideC, SIGNAL(sliderMoved(int)), spinC, SLOT(setValue(int)));
    connect(slideM, SIGNAL(sliderMoved(int)), spinM, SLOT(setValue(int)));
    connect(slideY, SIGNAL(sliderMoved(int)), spinY, SLOT(setValue(int)));
    connect(slideK, SIGNAL(sliderMoved(int)), spinK, SLOT(setValue(int)));

    connect(spinH, SIGNAL(valueChanged(double)),
            this, SLOT(handleColorHChanged(double)));
    connect(spinS, SIGNAL(valueChanged(double)),
            this, SLOT(handleColorSChanged(double)));
    connect(spinV, SIGNAL(valueChanged(double)),
            this, SLOT(handleColorVChanged(double)));

    connect(slideH, SIGNAL(sliderMoved(int)), this, SLOT(handleSliderHMoved(int)));
    connect(slideS, SIGNAL(sliderMoved(int)), this, SLOT(handleSliderSMoved(int)));
    connect(slideV, SIGNAL(sliderMoved(int)), this, SLOT(handleSliderVMoved(int)));*/
}

const QColor ColorWheel::getColor()
{
    return triangle->color();
}

void ColorWheel::setColor(const QColor &color)
{
    triangle->blockSignals(true);
    triangle->setColor(color);
    triangle->blockSignals(false);
    handleColorChanged(color, true);
}

/*void ColorWheel::setR(int r)
{
    spinR->blockSignals(true);
    slideR->blockSignals(true);
    if (spinR->value() != r) { spinR->setValue(r); }
    if (slideR->value() != r) { slideR->setValue(r); }
    spinR->blockSignals(false);
    slideR->blockSignals(false);
}

void ColorWheel::setG(int g)
{
    spinG->blockSignals(true);
    slideG->blockSignals(true);
    if (spinG->value() != g) { spinG->setValue(g); }
    if (slideG->value() != g) { slideG->setValue(g); }
    spinG->blockSignals(false);
    slideG->blockSignals(false);
}

void ColorWheel::setB(int b)
{
    spinB->blockSignals(true);
    slideB->blockSignals(true);
    if (spinB->value() != b) { spinB->setValue(b); }
    if (slideB->value() != b) { slideB->setValue(b); }
    spinB->blockSignals(false);
    slideB->blockSignals(false);
}

void ColorWheel::setC(int c)
{
    spinC->blockSignals(true);
    slideC->blockSignals(true);
    if (spinC->value() != c) { spinC->setValue(c); }
    if (slideC->value() != c) { slideC->setValue(c); }
    spinC->blockSignals(false);
    slideC->blockSignals(false);
}

void ColorWheel::setM(int m)
{
    spinM->blockSignals(true);
    slideM->blockSignals(true);
    if (spinM->value() != m) { spinM->setValue(m); }
    if (slideM->value() != m) { slideM->setValue(m); }
    spinM->blockSignals(false);
    slideM->blockSignals(false);
}

void ColorWheel::setY(int y)
{
    spinY->blockSignals(true);
    slideY->blockSignals(true);
    if (spinY->value() != y) { spinY->setValue(y); }
    if (slideY->value() != y) { slideY->setValue(y); }
    spinY->blockSignals(false);
    slideY->blockSignals(false);
}

void ColorWheel::setK(int k)
{
    spinK->blockSignals(true);
    slideK->blockSignals(true);
    if (spinK->value() != k) { spinK->setValue(k); }
    if (slideK->value() != k) { slideK->setValue(k); }
    spinK->blockSignals(false);
    slideK->blockSignals(false);
}

void ColorWheel::setH(qreal h)
{
    spinH->blockSignals(true);
    slideH->blockSignals(true);
    spinH->setValue(h);
    int slideValue = h*1000;
    slideH->setValue(slideValue);
    qDebug() << "!!!" << h << slideValue << slideH->value();
    spinH->blockSignals(false);
    slideH->blockSignals(false);
}

void ColorWheel::setS(qreal s)
{
    spinS->blockSignals(true);
    slideS->blockSignals(true);
    spinS->setValue(s);
    int slideValue = s*1000;
    slideS->setValue(slideValue);
    spinS->blockSignals(false);
    slideS->blockSignals(false);
}

void ColorWheel::setV(qreal v)
{
    spinV->blockSignals(true);
    slideV->blockSignals(true);
    spinV->setValue(v);
    int slideValue = v*1000;
    slideV->setValue(slideValue);
    spinV->blockSignals(false);
    slideV->blockSignals(false);
}*/

void ColorWheel::handleColorChanged(const QColor &color, bool doEmit)
{
   // setR(color.red());
    //emit colorRChanged(color);

   // setG(color.green());
    //emit colorGChanged(color);

  //  setB(color.blue());
    //emit colorBChanged(color);

   /* setC(color.toCmyk().cyan());
    setM(color.toCmyk().magenta());
    setY(color.toCmyk().yellow());
    setK(color.toCmyk().black());*/
  //  setH(color.toHsv().hueF());
   // setS(color.toHsv().saturationF());
    //setV(color.toHsv().valueF());
    if (doEmit) { emit colorChanged(color); }
}

/*void ColorWheel::handleColorRChanged(int value)
{
    QColor color;
    color.setRgb(value, spinG->value(), spinB->value());
    triangle->setColor(color);
}

void ColorWheel::handleColorGChanged(int value)
{
    QColor color;
    color.setRgb(spinR->value(), value, spinB->value());
    triangle->setColor(color);
}

void ColorWheel::handleColorBChanged(int value)
{
    QColor color;
    color.setRgb(spinR->value(), spinG->value(), value);
    triangle->setColor(color);
}

void ColorWheel::handleColorCChanged(int value)
{
    QColor color;
    color.setCmyk(value, spinM->value(), spinY->value(), spinK->value());
    triangle->setColor(color);
}

void ColorWheel::handleColorMChanged(int value)
{
    QColor color;
    color.setCmyk(spinC->value(), value, spinY->value(), spinK->value());
    triangle->setColor(color);
}

void ColorWheel::handleColorYChanged(int value)
{
    QColor color;
    color.setCmyk(spinC->value(), spinM->value(), value, spinK->value());
    triangle->setColor(color);
}

void ColorWheel::handleColorKChanged(int value)
{
    QColor color = triangle->color();
    color.setCmyk(spinC->value(), spinM->value(), spinY->value(), value);
    triangle->setColor(color);
}

void ColorWheel::handleColorHChanged(double value)
{
    QColor color = triangle->color();
    color.setHsvF(value, spinS->value(), spinV->value());
    triangle->setColor(color);
}

void ColorWheel::handleColorSChanged(double value)
{
    QColor color = triangle->color();
    color.setHsvF(spinH->value(), value, spinV->value());
    triangle->setColor(color);
}

void ColorWheel::handleColorVChanged(double value)
{
    QColor color = triangle->color();
    color.setHsvF(spinH->value(), spinS->value(), value);
    triangle->setColor(color);
}

void ColorWheel::handleSliderHMoved(int value)
{
    double newValue = (qreal)value/1000;
    spinH->setValue(newValue);
}

void ColorWheel::handleSliderSMoved(int value)
{
    double newValue = (qreal)value/1000;
    spinS->setValue(newValue);
}

void ColorWheel::handleSliderVMoved(int value)
{
    double newValue = (qreal)value/1000;
    spinV->setValue(newValue);
}*/
