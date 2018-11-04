#ifndef PROFILEDIALOG_H
#define PROFILEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

class ProfileDialog : public QDialog
{
    Q_OBJECT

public:
    ProfileDialog(QWidget *parent = Q_NULLPTR);
    ~ProfileDialog();
    QLineEdit *profileFileName;
    QLineEdit *profileDescription;
    QLineEdit *profileCopyright;
    QPushButton *profileSaveButton;
    QPushButton *profileCloseButton;

private slots:
    void closeDialog();
};
#endif // PROFILEDIALOG_H
