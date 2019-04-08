/*
# Copyright Ole-André Rodlie, INRIA.
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

#include "profiledialog.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

ProfileDialog::ProfileDialog(QWidget *parent, const QString &inputProfile)
    : QDialog(parent)
    , profileFileName(Q_NULLPTR)
    , profileDescription(Q_NULLPTR)
    , profileCopyright(Q_NULLPTR)
    , profileSaveButton(Q_NULLPTR)
    , profileCloseButton(Q_NULLPTR)
{
    setWindowTitle(tr("Edit Color Profile"));
    setWindowIcon(QIcon(":/cyan.png"));
    setAttribute(Qt::WA_DeleteOnClose, true);

    QFrame *containerFrame = new QFrame(this);
    QFrame *buttonFrame = new QFrame(this);
    QFrame *descriptionFrame = new QFrame(this);
    QFrame *copyrightFrame = new QFrame(this);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    QVBoxLayout *containerLayout = new QVBoxLayout(containerFrame);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonFrame);
    QHBoxLayout *descriptionLayout = new QHBoxLayout(descriptionFrame);
    QHBoxLayout *copyrightLayout = new QHBoxLayout(copyrightFrame);

    descriptionLayout->setContentsMargins(0,0,0,0);
    copyrightLayout->setContentsMargins(0,0,0,0);

    QLabel *profileLabel = new QLabel(this);
    profileLabel->setPixmap(QPixmap::fromImage(QImage(":/profile.png")));
    profileLabel->setAlignment(Qt::AlignTop);

    QLabel *descriptionLabel = new QLabel(this);
    descriptionLabel->setText(tr("Description"));
    descriptionLabel->setMinimumWidth(100);

    QLabel *copyrightLabel = new QLabel(this);
    copyrightLabel->setText(tr("Copyright"));
    copyrightLabel->setMinimumWidth(100);

    profileFileName = new QLineEdit(this);
    profileFileName->hide();
    profileFileName->setReadOnly(true);

    profileDescription = new QLineEdit(this);
    profileDescription->setMinimumWidth(400);
    profileCopyright = new QLineEdit(this);
    profileCopyright->setMinimumWidth(400);

    profileSaveButton = new QPushButton(this);
    profileSaveButton->setText(tr("Save"));
    profileSaveButton->setIcon(QIcon(":/cyan-save.png"));

    profileCloseButton = new QPushButton(this);
    profileCloseButton->setText(tr("Cancel"));

    mainLayout->addWidget(profileLabel);
    mainLayout->addWidget(containerFrame);

    containerLayout->addWidget(profileFileName);
    containerLayout->addWidget(descriptionFrame);
    containerLayout->addWidget(copyrightFrame);
    containerLayout->addWidget(buttonFrame);
    containerLayout->addStretch();

    copyrightLayout->addWidget(copyrightLabel);
    copyrightLayout->addWidget(profileCopyright);

    descriptionLayout->addWidget(descriptionLabel);
    descriptionLayout->addWidget(profileDescription);

    buttonLayout->addStretch();
    buttonLayout->addWidget(profileSaveButton);
    buttonLayout->addWidget(profileCloseButton);

    profileFileName->setText(inputProfile);
    profileSaveButton->setFocus();

    connect(profileCloseButton, SIGNAL(released()),
            this, SLOT(close()));
    connect(profileSaveButton, SIGNAL(released()),
            this, SLOT(saveProfile()));

    loadProfile();
}

void ProfileDialog::loadProfile()
{
    if (profileFileName->text().isEmpty()) { close(); }
    profileDescription->setText(QString::fromStdString(fx.getProfileTag(profileFileName->text()
                                                                        .toStdString(),
                                                                        FXX::ICCDescription)));
    profileCopyright->setText(QString::fromStdString(fx.getProfileTag(profileFileName->text()
                                                                      .toStdString(),
                                                                      FXX::ICCCopyright)));
}

void ProfileDialog::saveProfile()
{
    if (profileFileName->text().isEmpty()) { close(); }

    if (!fx.editProfile(profileFileName->text().toStdString(),
                        profileDescription->text().toStdString(),
                        profileCopyright->text().toStdString()))
    {
        QMessageBox::warning(this, tr("Failed to save color profile"),
                             tr("Failed to save color profile, check file permissions or similar."));
    }
    close();
}
