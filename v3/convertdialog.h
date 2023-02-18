/*
#
# Cyan Graphics - https://cyan.graphics - https://github.com/rodlie/cyan
#
# Copyright (c) Ole-André Rodlie <ole.andre.rodlie@gmail.com>. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
#
*/

#ifndef CONVERT_DIALOG_H
#define CONVERT_DIALOG_H

#include <QObject>
#include <QDialog>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QCloseEvent>
#include <QProgressBar>

#include "engine.h"

namespace Cyan {
    class ConvertDialog : public QDialog
    {
        Q_OBJECT

    public:
        ConvertDialog( QWidget *parent = nullptr,
                       Engine::ColorSettings cs = Engine::ColorSettings(),
                       const QString &inFilename = QString(),
                       const QString &outFilename = QString(),
                       const QString &title = tr("Convert") );

    private:
        bool _working;
        Engine::ColorSettings _cs;
        Engine::ImageOptions _opt;
        QString _inFilename;
        QString _outFilename;
        QComboBox *_boxCategory;
        QComboBox *_boxDestination;
        QComboBox *_boxIntent;
        QCheckBox *_checkBlackPoint;
        QPushButton *_buttonSave;
        QPushButton *_buttonCancel;
        QProgressBar *_progress;

    private slots:
        void init();
        void populateColorIntentMenu();
        void populateProfiles();
        void handleCategoryChanged(int index);
        void setImageCompression(QVariant compression);
        void setImageCompression(int compression);
        void setImageQuality(int quality);
        QWidget* getJPEGOptWidget();
        QWidget* getTIFFOptWidget();
        QWidget* getOptionsWidget();

    protected:
        void closeEvent(QCloseEvent *e) override;
    };
}
#endif // CONVERT_DIALOG_H
