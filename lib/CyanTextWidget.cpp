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

#include "CyanTextWidget.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QPushButton>
#include <QColorDialog>
#include <QFontDatabase>
#include <QApplication>

CyanTextWidget::CyanTextWidget(QWidget *parent) :
    QWidget(parent)
  , htmlEditor(nullptr)
  , textEditor(nullptr)
  , textBoldButton(nullptr)
  , textItalicButton(nullptr)
  , textUnderlineButton(nullptr)
  , textColorButton(nullptr)
  , fontBox(nullptr)
  , fontSizeBox(nullptr)
  , textAlignBox(nullptr)
  , textRotateSlider(nullptr)
{
    setContentsMargins(0,0,0,0);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    QWidget *formatWidget = new QWidget(this);
    formatWidget->setContentsMargins(0,0,0,0);
    QHBoxLayout *formatLayout = new QHBoxLayout(formatWidget);
    formatLayout->setContentsMargins(0,0,0,0);

    htmlEditor = new QTextEdit(this);
    textEditor = new QPlainTextEdit(this);
    textBoldButton = new QPushButton(this);
    textItalicButton = new QPushButton(this);
    textUnderlineButton = new QPushButton(this);
    textColorButton = new QPushButton(this);
    fontBox = new QFontComboBox(this);
    fontSizeBox = new QComboBox(this);
    textAlignBox = new QComboBox(this);
    textRotateSlider = new QSlider(this);

    fontSizeBox->setEditable(true);
    fontSizeBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    const QList<int> standardSizes = QFontDatabase::standardSizes();
    foreach (int size, standardSizes) {
        fontSizeBox->addItem(QString::number(size));
    }
    fontSizeBox->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

    textAlignBox->addItem(QIcon::fromTheme("folder"), tr("Left"), 0);
    textAlignBox->addItem(QIcon::fromTheme("folder"), tr("Right"), 1);
    textAlignBox->addItem(QIcon::fromTheme("folder"), tr("Center"), 2);

    textRotateSlider->setMinimum(-180);
    textRotateSlider->setMaximum(180);
    textRotateSlider->setValue(0);
    textRotateSlider->setOrientation(Qt::Horizontal);

    textBoldButton->setIcon(QIcon::fromTheme("format-text-bold"));
    textItalicButton->setIcon(QIcon::fromTheme("format-text-italic"));
    textUnderlineButton->setIcon(QIcon::fromTheme("format-text-underline"));

    textBoldButton->setCheckable(true);
    textItalicButton->setCheckable(true);
    textUnderlineButton->setCheckable(true);

    textBoldButton->setToolTip(tr("Bold"));
    textItalicButton->setToolTip(tr("Italic"));
    textUnderlineButton->setToolTip(tr("Underline"));
    textColorButton->setToolTip(tr("Color"));

    textBoldButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    textItalicButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    textUnderlineButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    textColorButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    htmlEditor->setStyleSheet(QString("QTextEdit { background-color: #666; color: black; }"));
    htmlEditor->setAcceptRichText(false);
    textEditor->hide();

    QWidget *fontWidget = new QWidget(this);
    QHBoxLayout *fontLayout = new QHBoxLayout(fontWidget);
    fontLayout->addWidget(fontBox);
    fontLayout->addWidget(fontSizeBox);
    //fontLayout->addWidget(textColorButton);

    formatLayout->addWidget(textBoldButton);
    formatLayout->addWidget(textItalicButton);
    formatLayout->addWidget(textUnderlineButton);
    formatLayout->addWidget(textColorButton);
    //formatLayout->addWidget(fontBox);
    //formatLayout->addWidget(fontSizeBox);
    formatLayout->addWidget(textAlignBox);
    //formatLayout->addWidget(textRotateSlider);

    mainLayout->addWidget(formatWidget);
    mainLayout->addWidget(fontWidget);
    mainLayout->addWidget(htmlEditor);
    mainLayout->addWidget(textEditor);
    mainLayout->addWidget(textRotateSlider);

    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    textColorButton->setIcon(pix);

    connect(htmlEditor->document(), SIGNAL(contentsChanged()),
            this, SLOT(handleTextChanged()));
    connect(htmlEditor, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            this, SLOT(currentCharFormatChanged(QTextCharFormat)));
    connect(htmlEditor, SIGNAL(cursorPositionChanged()),
            this, SLOT(cursorPositionChanged()));

    connect(textBoldButton, SIGNAL(toggled(bool)),
            this, SLOT(handleBoldButton(bool)));
    connect(textItalicButton, SIGNAL(toggled(bool)),
            this, SLOT(handleItalicButton(bool)));
    connect(textUnderlineButton, SIGNAL(toggled(bool)),
            this, SLOT(handleUnderLineButton(bool)));
    connect(textColorButton, SIGNAL(released()),
            this, SLOT(handleTextColor()));


    connect(textAlignBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleTextAlign(int)));
    connect(textRotateSlider, SIGNAL(valueChanged(int)), this ,SLOT(handleTextRotate(int)));



    connect(fontBox, SIGNAL(activated(QString)), this, SLOT(handleTextFamily(QString)));
    connect(fontSizeBox, SIGNAL(activated(QString)), this, SLOT(handleTextSize(QString)));

    setup();
}

CyanTextWidget::~CyanTextWidget()
{

}

void CyanTextWidget::setText(const QString &text)
{
    blockSignals(true);
    htmlEditor->setHtml(text);
    setup();
    blockSignals(false);
}

const QString CyanTextWidget::getText()
{
    return htmlEditor->toHtml();
}

void CyanTextWidget::setTextAlign(int align)
{
    for (int i=0;i<textAlignBox->count();++i) {
        if (textAlignBox->itemData(i).toInt() == align) {
            blockSignals(true);
            textAlignBox->setCurrentIndex(i);
            blockSignals(false);
            return;
        }
    }
}

int CyanTextWidget::getTextAlign()
{
    return textAlignBox->currentData().toInt();
}

void CyanTextWidget::setTextRotate(int value)
{
    blockSignals(true);
    textRotateSlider->setValue(value);
    blockSignals(false);
}

int CyanTextWidget::getTextRotate()
{
    return textRotateSlider->value();
}

void CyanTextWidget::fontChanged(const QFont &f)
{
    fontBox->setCurrentIndex(fontBox->findText(QFontInfo(f).family()));
    fontSizeBox->setCurrentIndex(fontSizeBox->findText(QString::number(f.pointSize())));
    textBoldButton->setChecked(f.bold());
    textItalicButton->setChecked(f.italic());
    textUnderlineButton->setChecked(f.underline());
}

void CyanTextWidget::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    textColorButton->setIcon(pix);
}

void CyanTextWidget::alignmentChanged(Qt::Alignment a)
{
    Q_UNUSED(a)
    /*if (a & Qt::AlignLeft)
        actionAlignLeft->setChecked(true);
    else if (a & Qt::AlignHCenter)
        actionAlignCenter->setChecked(true);
    else if (a & Qt::AlignRight)
        actionAlignRight->setChecked(true);
    else if (a & Qt::AlignJustify)
        actionAlignJustify->setChecked(true);*/
}

void CyanTextWidget::setup()
{
    QFont textFont(fontBox->currentText());
    //textFont.setStyleHint(QFont::SansSerif);
    htmlEditor->setFont(textFont);
    fontChanged(htmlEditor->font());
    colorChanged(htmlEditor->textColor());
    alignmentChanged(htmlEditor->alignment());
    handleTextChanged();
}

void CyanTextWidget::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
    emit textChanged();
    qDebug() << htmlEditor->toHtml();
}

void CyanTextWidget::cursorPositionChanged()
{
    //alignmentChanged(htmlEditor->alignment());
}

void CyanTextWidget::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = htmlEditor->textCursor();
    if (!cursor.hasSelection()) { cursor.select(QTextCursor::WordUnderCursor); }
    cursor.mergeCharFormat(format);
    htmlEditor->mergeCurrentCharFormat(format);
}

void CyanTextWidget::handleBoldButton(bool triggered)
{
    Q_UNUSED(triggered)
    QTextCharFormat fmt;
    fmt.setFontWeight(textBoldButton->isChecked()?QFont::Bold:QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void CyanTextWidget::handleItalicButton(bool triggered)
{
    Q_UNUSED(triggered)
    QTextCharFormat fmt;
    fmt.setFontItalic(textItalicButton->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void CyanTextWidget::handleUnderLineButton(bool triggered)
{
    Q_UNUSED(triggered)
    QTextCharFormat fmt;
    fmt.setFontUnderline(textUnderlineButton->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void CyanTextWidget::handleTextFamily(const QString &f)
{
    qDebug() << "TEXT FAMILY" << f;
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
}

void CyanTextWidget::handleTextSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}

void CyanTextWidget::handleTextAlign(int index)
{
    Q_UNUSED(index);
    emit textChanged();
}

void CyanTextWidget::handleTextRotate(int value)
{
    Q_UNUSED(value)
    emit textChanged();
}

void CyanTextWidget::handleTextColor()
{
    QColor col = QColorDialog::getColor(htmlEditor->textColor(), this);
    if (!col.isValid()) { return; }
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}

void CyanTextWidget::handleTextChanged()
{
    emit textChanged();
}
