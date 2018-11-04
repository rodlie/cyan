#include "profiledialog.h"
#include <QHBoxLayout>
#include <QLabel>

ProfileDialog::ProfileDialog(QWidget *parent)
    : QDialog(parent)
    , profileFileName(Q_NULLPTR)
    , profileDescription(Q_NULLPTR)
    , profileCopyright(Q_NULLPTR)
    , profileSaveButton(Q_NULLPTR)
    , profileCloseButton(Q_NULLPTR)
{
    setWindowTitle(tr("Edit Color Profile"));
    setWindowIcon(QIcon(":/cyan.png"));

    QFrame *containerFrame = new QFrame();
    QFrame *buttonFrame = new QFrame();
    QFrame *descriptionFrame = new QFrame();
    QFrame *copyrightFrame = new QFrame();

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    QVBoxLayout *containerLayout = new QVBoxLayout(containerFrame);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonFrame);
    QHBoxLayout *descriptionLayout = new QHBoxLayout(descriptionFrame);
    QHBoxLayout *copyrightLayout = new QHBoxLayout(copyrightFrame);

    descriptionLayout->setContentsMargins(0,0,0,0);
    copyrightLayout->setContentsMargins(0,0,0,0);

    QLabel *profileLabel = new QLabel();
    profileLabel->setPixmap(QPixmap::fromImage(QImage(":/profile.png")));
    profileLabel->setAlignment(Qt::AlignTop);

    QLabel *descriptionLabel = new QLabel();
    descriptionLabel->setText(tr("Description"));
    descriptionLabel->setMinimumWidth(100);

    QLabel *copyrightLabel = new QLabel();
    copyrightLabel->setText(tr("Copyright"));
    copyrightLabel->setMinimumWidth(100);

    profileFileName = new QLineEdit();
    profileFileName->hide();
    profileFileName->setReadOnly(true);

    profileDescription = new QLineEdit();
    profileDescription->setMinimumWidth(400);
    profileCopyright = new QLineEdit();
    profileCopyright->setMinimumWidth(400);

    profileSaveButton = new QPushButton();
    profileSaveButton->setText(tr("Save"));
    profileSaveButton->setIcon(QIcon(":/cyan-save.png"));

    profileCloseButton = new QPushButton();
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
    buttonLayout->addWidget(profileCloseButton);
    buttonLayout->addWidget(profileSaveButton);

    connect(profileCloseButton, SIGNAL(released()), this, SLOT(closeDialog()));
}

ProfileDialog::~ProfileDialog()
{
}

void ProfileDialog::closeDialog()
{
    profileCopyright->clear();
    profileDescription->clear();
    profileFileName->clear();
    hide();
}
