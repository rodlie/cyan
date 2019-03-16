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
  , fontBox(nullptr)
  , fontSizeBox(nullptr)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,10);

    QWidget *formatWidget = new QWidget(this);
    QHBoxLayout *formatLayout = new QHBoxLayout(formatWidget);


    htmlEditor = new QTextEdit(this);
    textEditor = new QPlainTextEdit(this);

    textBoldButton = new QPushButton(this);
    textItalicButton = new QPushButton(this);
    textUnderlineButton = new QPushButton(this);

    fontBox = new QFontComboBox(this);

    fontSizeBox = new QComboBox(this);
    fontSizeBox->setEditable(true);

    const QList<int> standardSizes = QFontDatabase::standardSizes();
    foreach (int size, standardSizes)
        fontSizeBox->addItem(QString::number(size));
    fontSizeBox->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

    textBoldButton->setIcon(QIcon::fromTheme("format-text-bold"));
    textItalicButton->setIcon(QIcon::fromTheme("format-text-italic"));
    textUnderlineButton->setIcon(QIcon::fromTheme("format-text-underline"));

    textBoldButton->setCheckable(true);
    textItalicButton->setCheckable(true);
    textUnderlineButton->setCheckable(true);

    textBoldButton->setText(tr("Bold"));
    textItalicButton->setText(tr("Italic"));
    textUnderlineButton->setText(tr("Underline"));

    //htmlEditor->setStyleSheet(QString("background-color: #666;"));
    textEditor->hide();

    formatLayout->addWidget(textBoldButton);
    formatLayout->addWidget(textItalicButton);
    formatLayout->addWidget(textUnderlineButton);
    formatLayout->addWidget(fontBox);
    formatLayout->addWidget(fontSizeBox);

    mainLayout->addWidget(formatWidget);
    mainLayout->addWidget(htmlEditor);
    mainLayout->addWidget(textEditor);

    /*QPushButton *button = new QPushButton(this);
    button->setText(tr("Apply"));
    mainLayout->addWidget(button);
    connect(button,
            SIGNAL(released()),
            this,
            SLOT(handleTextChanged()));*/

    //TextEdit


    htmlEditor->document();

    connect(htmlEditor->document(), SIGNAL(contentsChanged()), this, SLOT(handleTextChanged()));

    connect(htmlEditor, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            this, SLOT(currentCharFormatChanged(QTextCharFormat)));

    connect(htmlEditor, SIGNAL(cursorPositionChanged()),
            this, SLOT(cursorPositionChanged()));





    connect(textBoldButton, SIGNAL(toggled(bool)), this, SLOT(handleBoldButton(bool)));
    connect(textItalicButton, SIGNAL(toggled(bool)), this, SLOT(handleItalicButton(bool)));
    connect(textUnderlineButton, SIGNAL(toggled(bool)), this, SLOT(handleUnderLineButton(bool)));



    connect(fontBox,
            QOverload<const QString &>::of(&QComboBox::activated),
            this,
            &CyanTextWidget::handleTextFamily);
    connect(fontSizeBox,
            QOverload<const QString &>::of(&QComboBox::activated),
            this,
            &CyanTextWidget::handleTextSize);

}

CyanTextWidget::~CyanTextWidget()
{

}

void CyanTextWidget::setText(const QString &text)
{
    qDebug() << "set layer text" << text;
    //textEditor->setPlainText(text);
    htmlEditor->setHtml(text);
}

const QString CyanTextWidget::getText()
{
    return htmlEditor->toHtml();
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
    /*QPixmap pix(16, 16);
    pix.fill(c);
    actionTextColor->setIcon(pix);*/
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

void CyanTextWidget::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
    /*qDebug() << html2Pango(textEdit->toHtml());*/
    emit textChanged();
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
