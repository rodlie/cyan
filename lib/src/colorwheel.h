#ifndef COLORWHEEL_H
#define COLORWHEEL_H

#include <QWidget>
#include <QColor>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>

#include "qtcolortriangle.h"

class ColorWheel : public QWidget
{
    Q_OBJECT
public:

    explicit ColorWheel(QWidget *parent = nullptr);

signals:

    void colorChanged(const QColor &color);
    /*void colorRChanged(const QColor &color);
    void colorGChanged(const QColor &color);
    void colorBChanged(const QColor &color);*/

public slots:

    const QColor getColor();
    void setColor(const QColor &color);

    /*void setR(int r);
    void setG(int g);
    void setB(int b);*/

    /*void setC(int c);
    void setM(int m);
    void setY(int y);
    void setK(int k);

    void setH(qreal h);
    void setS(qreal s);
    void setV(qreal v);*/

private:

    /*QSpinBox *spinR;
    QSpinBox *spinG;
    QSpinBox *spinB;

    QSlider *slideR;
    QSlider *slideG;
    QSlider *slideB;

    QSpinBox *spinC;
    QSpinBox *spinM;
    QSpinBox *spinY;
    QSpinBox *spinK;

    QSlider *slideC;
    QSlider *slideM;
    QSlider *slideY;
    QSlider *slideK;

    QDoubleSpinBox *spinH;
    QDoubleSpinBox *spinS;
    QDoubleSpinBox *spinV;

    QSlider *slideH;
    QSlider *slideS;
    QSlider *slideV;*/

    QtColorTriangle *triangle;

private slots:

    void handleColorChanged(const QColor &color,
                            bool doEmit = true);

    /*void handleColorRChanged(int value);
    void handleColorGChanged(int value);
    void handleColorBChanged(int value);

    void handleColorCChanged(int value);
    void handleColorMChanged(int value);
    void handleColorYChanged(int value);
    void handleColorKChanged(int value);

    void handleColorHChanged(double value);
    void handleColorSChanged(double value);
    void handleColorVChanged(double value);

    void handleSliderHMoved(int value);
    void handleSliderSMoved(int value);
    void handleSliderVMoved(int value);*/
};

#endif // COLORWHEEL_H
