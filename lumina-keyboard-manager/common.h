/*
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef COMMON_H
#define COMMON_H

#include <QStringList>
#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QProcess>
#include <QSettings>

#define SETXKBMAP "setxkbmap"

enum xkbType
{
    xkbModel,
    xkbLayout,
    xkbVariant
};

class Common
{
public:
    static QStringList parseXKB(xkbType type)
    {
        QStringList result;
        QString findType;
        switch(type) {
        case xkbLayout:
            findType = "! layout";
            break;
        case xkbModel:
            findType = "! model";
            break;
        case xkbVariant:
            findType = "! variant";
            break;
        default:;
        }
        if (findType.isEmpty()) { return result; }

        QString xkbRules = QString("%1/../share/X11/xkb/rules/xfree86.lst").arg(QApplication::applicationDirPath());
        QFileInfo xkbRulesFile;
        xkbRulesFile.setFile(xkbRules);
        if (!xkbRulesFile.exists()) {
            xkbRulesFile.setFile("/usr/share/X11/xkb/rules/xfree86.lst");
            if (!xkbRulesFile.exists()) { return result; }
        }

        QFile xkbFile(xkbRulesFile.absoluteFilePath());
        if (!xkbFile.open(QIODevice::ReadOnly)) { return result; }
        QTextStream lines(&xkbFile);
        bool getContent = false;
        while (!lines.atEnd()) {
            QString line = lines.readLine();
            if (getContent) {
                if (line.startsWith("!")) {
                    getContent = false;
                    continue;
                }
                if (!line.simplified().isEmpty()) { result << line.simplified(); }
            }
            if (line.startsWith(findType)) { getContent = true; }
        }
        xkbFile.close();
        return result;
    }
    static void saveKeyboard(QString type, QString value)
    {
        QSettings settings("lumina-desktop", "lumina-keyboard");
        settings.setValue(type, value);
    }
    static QString defaultKeyboard(QString type)
    {
        QSettings settings("lumina-desktop", "lumina-keyboard");
        return settings.value(type).toString();
    }
    static void loadKeyboard()
    {
        QString cmd;
        cmd.append(SETXKBMAP);

        QString layout = defaultKeyboard("layout");
        QString variant = defaultKeyboard("variant");
        QString model = defaultKeyboard("model");

        if (layout.isEmpty()) { return; }
        cmd.append(" -layout ");
        cmd.append(layout);

        if (!variant.isEmpty()) {
            cmd.append(" -variant ");
            cmd.append(variant);
        }
        if (!model.isEmpty()) {
            cmd.append(" -model ");
            cmd.append(model);
        }
        QProcess::startDetached(cmd);
    }
};


#endif // COMMON_H
