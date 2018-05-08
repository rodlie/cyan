/*
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QComboBox>

class Dialog : public QDialog
{
    Q_OBJECT
public:
   explicit Dialog(QWidget *parent = NULL);
private:
    QComboBox *lidActionBattery;
    QComboBox *lidActionAC;
    QComboBox *criticalActionBattery;
private slots:
    void populate();
    void loadSettings();
    void updatePM();
    void setDefaultAction(QComboBox *box, int action);
    void handleLidActionBattery(int index);
    void handleLidActionAC(int index);
    void handleCriticalAction(int index);
};

#endif // DIALOG_H
