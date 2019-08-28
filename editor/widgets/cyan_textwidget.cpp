/*
# Copyright Ole-André Rodlie.
#
# ole.andre.rodlie@gmail.com
#
# This software is governed by the CeCILL license under French law and
# abiding by the rules of distribution of free software. You can use,
# modify and / or redistribute the software under the terms of the CeCILL
# license as circulated by CEA, CNRS and INRIA at the following URL
# "https://www.cecill.info".
#
# As a counterpart to the access to the source code and rights to
# modify and redistribute granted by the license, users are provided only
# with a limited warranty and the software's author, the holder of the
# economic rights and the subsequent licensors have only limited
# liability.
#
# In this respect, the user's attention is drawn to the associated risks
# with loading, using, modifying and / or developing or reproducing the
# software by the user in light of its specific status of free software,
# that can mean that it is complicated to manipulate, and that also
# so that it is for developers and experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to test and test the software's suitability
# Requirements in the conditions of their systems
# data to be ensured and, more generally, to use and operate
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL license and that you accept its terms.
*/

#include "cyan_textwidget.h"
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

    textAlignBox->addItem(QIcon::fromTheme("folder"), tr("Left"), QString("left"));
    textAlignBox->addItem(QIcon::fromTheme("folder"), tr("Right"), QString("right"));
    textAlignBox->addItem(QIcon::fromTheme("folder"), tr("Center"), QString("center"));

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

    connect(fontBox,
            QOverload<const QString &>::of(&QComboBox::activated),
            this,
            &CyanTextWidget::handleTextFamily);
    connect(fontSizeBox,
            QOverload<const QString &>::of(&QComboBox::activated),
            this,
            &CyanTextWidget::handleTextSize);

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

void CyanTextWidget::setTextAlign(const QString &align)
{
    for (int i=0;i<textAlignBox->count();++i) {
        if (textAlignBox->itemData(i).toString() == align) {
            blockSignals(true);
            textAlignBox->setCurrentIndex(i);
            blockSignals(false);
            return;
        }
    }
}

const QString CyanTextWidget::getTextAlign()
{
    return textAlignBox->currentData().toString();
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
