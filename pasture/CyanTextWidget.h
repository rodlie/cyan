/*
#
# Cyan <https://cyan.fxarena.net>
# Copyright Ole-André Rodlie, FxArena DA.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
*/

#ifndef CYANTEXTWIDGET_H
#define CYANTEXTWIDGET_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QFontComboBox>
#include <QSlider>

#include "CyanGlobal.h"

class CYANSHARED_EXPORT CyanTextWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CyanTextWidget(QWidget *parent = nullptr);
    ~CyanTextWidget();

signals:
    void textChanged();

public slots:
    void setText(const QString &text);
    const QString getText();
    void setTextAlign(int align);
    int getTextAlign();
    void setTextRotate(int value);
    int getTextRotate();

private:
    QTextEdit *htmlEditor;
    QPlainTextEdit *textEditor;
    QPushButton *textBoldButton;
    QPushButton *textItalicButton;
    QPushButton *textUnderlineButton;
    QPushButton *textColorButton;
    QFontComboBox *fontBox;
    QComboBox *fontSizeBox;
    QComboBox *textAlignBox;
    QSlider *textRotateSlider;

    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);
    void alignmentChanged(Qt::Alignment a);

private slots:
    void setup();
    void currentCharFormatChanged(const QTextCharFormat &format);
    void cursorPositionChanged();
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void handleBoldButton(bool triggered);
    void handleItalicButton(bool triggered);
    void handleUnderLineButton(bool triggered);
    void handleTextFamily(const QString &f);
    void handleTextSize(const QString &p);
    void handleTextAlign(int index);
    void handleTextRotate(int value);
    void handleTextColor();
    void handleTextChanged();
};

#endif // CYANTEXTWIDGET_H
