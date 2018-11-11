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
