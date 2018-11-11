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

#ifndef PROFILEDIALOG_H
#define PROFILEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include "FXX.h"

class ProfileDialog : public QDialog
{
    Q_OBJECT

public:
    ProfileDialog(QWidget *parent = Q_NULLPTR,
                  const QString &inputProfile = QString());
    QLineEdit *profileFileName;
    QLineEdit *profileDescription;
    QLineEdit *profileCopyright;
    QPushButton *profileSaveButton;
    QPushButton *profileCloseButton;

private:
    FXX fx;

private slots:
    void loadProfile();
    void saveProfile();
};
#endif // PROFILEDIALOG_H
