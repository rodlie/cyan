/*
# Cyan <http://prepress.sf.net> <https://cyan.fxarena.net>,
# Copyright (C) 2016, 2017, 2018 Ole-André Rodlie<ole.andre.rodlie@gmail.com>
#
# Cyan is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.
#
# Cyan is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Cyan.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>
*/

#include "helpdialog.h"
#include <QIcon>
#include <QHBoxLayout>

HelpDialog::HelpDialog(QWidget *parent, QString html)
    : QDialog(parent)
    , browser(Q_NULLPTR)
{
    setWindowTitle(tr("Cyan Documentation"));
    setWindowIcon(QIcon(":/cyan.png"));
    setMinimumSize(QSize(550, 550));
    setAttribute(Qt::WA_DeleteOnClose, true);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    browser = new QTextBrowser(this);
    browser->setOpenExternalLinks(true);
    browser->setHtml(html);

    mainLayout->addWidget(browser);
}
