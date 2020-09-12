/*
#
# Cyan <https://cyan.fxarena.net>
# Copyright Ole-André Rodlie, FxArena DA.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
*/

#include "editor.h"
#include <QSettings>
#include <QTimer>
#include <QMessageBox>
#include <QApplication>
#include <QPixmap>

#include "CyanColorConvertDialog.h"
#include "CyanColorConvert.h"

void Editor::populateColorProfileMenu(QMenu *menu,
                                      QActionGroup *group,
                                      Magick::ColorspaceType colorspace)
{
    if (!menu || !group) { return; }
    QMapIterator<QString, QString> i(CyanCommon::getColorProfiles(colorspace));
    while (i.hasNext()) {
        i.next();
        QAction *action = new QAction(menu);
        //action->setIcon(QIcon::fromTheme("color-wheel"));
        action->setText(i.key());
        action->setData(i.value());
        action->setCheckable(true);
        group->addAction(action);
        connect(action,
                SIGNAL(triggered()),
                this,
                SLOT(handleColorProfileTriggered()));
    }
    menu->clear();
    menu->addActions(group->actions());
}

void Editor::handleColorProfileTriggered()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action) { return; }
    action->setChecked(true);
    saveColorProfileSettings();
}

void Editor::setDefaultColorProfiles()
{
    QSettings settings;
    settings.beginGroup(QString("color"));
    if (settings.value(QString("rgb_profile")).isValid() &&
        QFile::exists(settings.value(QString("rgb_profile")).toString()))
    {
        setDefaultColorProfileFromFilename(profileRGBGroup,
                                           settings.value(QString("rgb_profile"))
                                           .toString());
    } else {
        setDefaultColorProfileFromTitle(profileRGBGroup,
                                        QString("sRGB (built-in)"));
    }
    if (!profileRGBGroup->checkedAction()) {
        qDebug() << "RGB PROFILE FALLBACK! (first available)";
        if (profileRGBGroup->actions().size()>0) {
            profileRGBGroup->actions().at(0)->setChecked(true);
        }
    }

    if (settings.value(QString("cmyk_profile")).isValid() &&
        QFile::exists(settings.value(QString("cmyk_profile")).toString()))
    {
        setDefaultColorProfileFromFilename(profileCMYKGroup,
                                           settings.value(QString("cmyk_profile"))
                                           .toString());
    } else {
        setDefaultColorProfileFromTitle(profileCMYKGroup,
                                        QString("ISO Coated v2 (built-in)"));
    }
    if (!profileCMYKGroup->checkedAction()) {
        qDebug() << "CMYK PROFILE FALLBACK! (first available)";
        if (profileCMYKGroup->actions().size()>0) {
            profileCMYKGroup->actions().at(0)->setChecked(true);
        }
    }

    if (settings.value(QString("gray_profile")).isValid() &&
        QFile::exists(settings.value(QString("gray_profile")).toString()))
    {
        setDefaultColorProfileFromFilename(profileGRAYGroup,
                                           settings.value(QString("gray_profile"))
                                           .toString());
    } else {
        setDefaultColorProfileFromTitle(profileGRAYGroup,
                                        QString("ISO Coated v2 - GREY 1c - (built-in)"));
    }
    if (!profileGRAYGroup->checkedAction()) {
        qDebug() << "GRAY PROFILE FALLBACK! (first available)";
        if (profileGRAYGroup->actions().size()>0) {
            profileGRAYGroup->actions().at(0)->setChecked(true);
        }
    }

    settings.endGroup();
}

void Editor::setDefaultColorProfileFromFilename(QActionGroup *group,
                                                const QString &filename)
{
    if (!group) { return; }
    for (int i=0;i<group->actions().size();++i) {
        QAction *action = group->actions().at(i);
        if (!action) { continue; }
        if (action->data().toString() == filename) {
            action->setChecked(true);
        }
    }
}

void Editor::setDefaultColorProfileFromTitle(QActionGroup *group,
                                             const QString &title)
{
    if (!group) { return; }
    for (int i=0;i<group->actions().size();++i) {
        QAction *action = group->actions().at(i);
        if (!action) { continue; }
        if (action->text() == title) {
            action->setChecked(true);
        }
    }
}

const QString Editor::selectedDefaultColorProfile(QActionGroup *group)
{
    if (!group) { return QString(); }
    if (!group->checkedAction()) { return QString(); }
    return group->checkedAction()->data().toString();
}

Magick::Blob Editor::selectedDefaultColorProfileData(QActionGroup *group)
{
    if (!group) { return Magick::Blob(); }
    if (!group->checkedAction()) { return Magick::Blob(); }

    QString filename = group->checkedAction()->data().toString();
    if (QFile::exists(filename)) {
        try {
            Magick::Image image;
            image.read(filename.toStdString());
            Magick::Blob profile;
            image.write(&profile);
            return profile;
        }
        catch(Magick::Error &error_ ) { emit errorMessage(error_.what()); }
        catch(Magick::Warning &warn_ ) { emit warningMessage(warn_.what()); }
    }
    return Magick::Blob();
}

void Editor::populateColorIntentMenu()
{
    //QIcon intentIcon = QIcon::fromTheme("video-display");
    QAction *action1 = new QAction(colorIntentMenu);
    action1->setText(tr("Undefined"));
    action1->setData(CyanCommon::UndefinedRenderingIntent);
    action1->setCheckable(true);
    //action1->setIcon(intentIcon);
    connect(action1,
            SIGNAL(triggered()),
            this,
            SLOT(setDefaultColorIntent()));

    QAction *action2 = new QAction(colorIntentMenu);
    action2->setText(tr("Saturation"));
    action2->setData(CyanCommon::SaturationRenderingIntent);
    action2->setCheckable(true);
    //action2->setIcon(intentIcon);
    connect(action2,
            SIGNAL(triggered()),
            this,
            SLOT(setDefaultColorIntent()));

    QAction *action3 = new QAction(colorIntentMenu);
    action3->setText(tr("Perceptual"));
    action3->setData(CyanCommon::PerceptualRenderingIntent);
    action3->setCheckable(true);
    //action3->setIcon(intentIcon);
    connect(action3,
            SIGNAL(triggered()),
            this,
            SLOT(setDefaultColorIntent()));

    QAction *action4 = new QAction(colorIntentMenu);
    action4->setText(tr("Absolute"));
    action4->setData(CyanCommon::AbsoluteRenderingIntent);
    action4->setCheckable(true);
    //action4->setIcon(intentIcon);
    connect(action4,
            SIGNAL(triggered()),
            this,
            SLOT(setDefaultColorIntent()));

    QAction *action5 = new QAction(colorIntentMenu);
    action5->setText(tr("Relative"));
    action5->setData(CyanCommon::RelativeRenderingIntent);
    action5->setCheckable(true);
    //action5->setIcon(intentIcon);
    connect(action5,
            SIGNAL(triggered()),
            this,
            SLOT(setDefaultColorIntent()));

    colorIntentMenu->addAction(action1);
    colorIntentMenu->addAction(action2);
    colorIntentMenu->addAction(action3);
    colorIntentMenu->addAction(action4);
    colorIntentMenu->addAction(action5);
}

void Editor::setDefaultColorIntent()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action) { return; }

    for (int i=0;i<colorIntentMenu->actions().size();++i) {
        QAction *otherAction = colorIntentMenu->actions().at(i);
        if (!otherAction) { continue; }
        otherAction->setChecked(false);
    }
    action->setChecked(true);

    QSettings settings;
    settings.beginGroup(QString("color"));
    settings.setValue(QString("intent"),
                      action->data().toInt());
    settings.endGroup();
    settings.sync();
}

void Editor::loadDefaultColorIntent()
{
    QSettings settings;
    settings.beginGroup(QString("color"));
    for (int i=0;i<colorIntentMenu->actions().size();++i) {
        QAction *action = colorIntentMenu->actions().at(i);
        if (!action) { continue; }
        if (action->data().toInt() == settings.value(QString("intent"),
                                                     CyanCommon::PerceptualRenderingIntent).toInt())
        {
            action->setChecked(true);
        } else { action->setChecked(false); }
    }
    if (settings.value(QString("intent")).isNull()) {
        settings.setValue(QString("intent"), CyanCommon::PerceptualRenderingIntent);
    }
    settings.endGroup();
}

void Editor::handleColorConvertRGB(bool ignoreColor, const QString &title)
{
    handleColorConvert(ignoreColor,
                       Magick::sRGBColorspace,
                       title);
}

void Editor::handleColorConvertCMYK(bool ignoreColor, const QString &title)
{
    handleColorConvert(ignoreColor,
                       Magick::CMYKColorspace,
                       title);
}

void Editor::handleColorConvertGRAY(bool ignoreColor, const QString &title)
{
    handleColorConvert(ignoreColor,
                       Magick::GRAYColorspace,
                       title);
}

void Editor::handleColorProfileAssign()
{
    if (!getCurrentCanvas()) { return; }
    switch(getCurrentCanvas()->getCanvas().colorSpace()) {
    case Magick::CMYKColorspace:
        handleColorConvertCMYK(true,
                               tr("Assign Color Profile"));
        break;
    case Magick::GRAYColorspace:
        handleColorConvertGRAY(true,
                               tr("Assign Color Profile"));
        break;
    default:
        handleColorConvertRGB(true,
                              tr("Assign Color Profile"));
    }
}

void Editor::handleColorConvert(bool ignoreColor,
                                Magick::ColorspaceType colorspace,
                                const QString &title)
{
    if (!getCurrentCanvas()) { return; }
    if (getCurrentCanvas()->getCanvas().colorSpace() == colorspace && !ignoreColor) {
        emit statusMessage(tr("Already the requested colorspace"));
        return;
    }
    QString profile;
    switch(colorspace) {
    case Magick::CMYKColorspace:
        profile = selectedDefaultColorProfile(profileCMYKGroup);
        break;
    case Magick::GRAYColorspace:
        profile = selectedDefaultColorProfile(profileGRAYGroup);
        break;
    default:
        profile = selectedDefaultColorProfile(profileRGBGroup);
    }
    if (profile.isEmpty()) {
        QMessageBox::warning(this,
                             tr("Missing default profile"),
                             tr("Missing default profile for the selected color space!"));
        return;
    }
    if (getCurrentCanvas()->getCanvasProject().layers.size()>1 &&
        colorspace == Magick::CMYKColorspace)
    {
        // warn when converting to CMYK with multiple layers
        QMessageBox question;
        question.setWindowTitle(tr("Convert to CMYK?"));
        question.setIconPixmap(QIcon::fromTheme("colors").pixmap(QSize(32, 32)));
        question.setText(tr("Working in CMYK color space with multiple layers are not recommended. Convert to CMYK anyway?"));
        question.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        question.setDefaultButton(QMessageBox::No);
        int reply = question.exec();
        if (reply != QMessageBox::Yes) { return; }
    }
    ConvertDialog *dialog = new ConvertDialog(this,
                                              title,
                                              profile,
                                              colorspace);
    int ret = dialog->exec();
    if (ret == QDialog::Accepted &&
        !dialog->getProfile().isEmpty())
    {
        qDebug() << "CONVERT USING" << dialog->getProfile();
        CyanImageFormat::CyanCanvas canvas = getCurrentCanvas()->getCanvasProject();
        canvas.image = ColorConvert::convertColorspace(canvas.image,
                                                 canvas.profile,
                                                 dialog->getProfile());
        for (int i=0;i<canvas.layers.size();++i) {
            canvas.layers[i].image= ColorConvert::convertColorspace(canvas.layers[i].image,
                                                              canvas.profile,
                                                              dialog->getProfile());
        }
        canvas.profile = canvas.image.iccColorProfile();
        getCurrentCanvas()->updateCanvas(canvas);
        updateTabTitle(getCurrentCanvas());
    }

    QTimer::singleShot(100,
                       dialog,
                       SLOT(deleteLater()));
}

void Editor::hasColorProfiles()
{
    int rgbs = CyanCommon::getColorProfiles(Magick::sRGBColorspace).size();
    int cmyks = CyanCommon::getColorProfiles(Magick::CMYKColorspace).size();
    int grays = CyanCommon::getColorProfiles(Magick::GRAYColorspace).size();

    if (rgbs<1 || cmyks<1 || grays<1) {
        QMessageBox::warning(this,
                             tr("Missing color profiles"),
                             tr("Missing color profiles."
                                " Cyan needs a minimum of one color profile"
                                " for each supported color space (RGB/CMYK/GRAY)."));
        QTimer::singleShot(100,
                           qApp,
                           SLOT(quit()));
    }
}

void Editor::handleColorChanged(const QColor &color)
{
    qDebug() << "brush color changed" << color;
    QList<QMdiSubWindow*> list = mdi->subWindowList();
    for (int i=0;i<list.size();++i) {
        QMdiSubWindow *window = qobject_cast<QMdiSubWindow*>(list.at(i));
        if (!window) { continue; }
        View *view = qobject_cast<View*>(window->widget());
        if (!view) { continue; }
        view->setBrushColor(color);
    }
}

void Editor::saveColorProfileSettings()
{
    QSettings settings;
    bool doSave = false;
    settings.beginGroup(QString("color"));
    if (settings.value(QString("rgb_profile")).toString() !=
        selectedDefaultColorProfile(profileRGBGroup))
    {
        settings.setValue(QString("rgb_profile"),
                          selectedDefaultColorProfile(profileRGBGroup));
        doSave = true;
    }
    if (settings.value(QString("cmyk_profile")).toString() !=
        selectedDefaultColorProfile(profileCMYKGroup))
    {
        settings.setValue(QString("cmyk_profile"),
                          selectedDefaultColorProfile(profileCMYKGroup));
        doSave = true;
    }
    if (settings.value(QString("gray_profile")).toString() !=
        selectedDefaultColorProfile(profileGRAYGroup))
    {
        settings.setValue(QString("gray_profile"),
                          selectedDefaultColorProfile(profileGRAYGroup));
        doSave = true;
    }
    settings.endGroup();
    if (doSave) { settings.sync(); }
}
