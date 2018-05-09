/*
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef SCREENSAVER_H
#define SCREENSAVER_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <QTime>
#include <QMap>
#include <QMapIterator>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QCoreApplication>

#define XSCREENSAVER "xscreensaver-command -deactivate"
#define TIMEOUT 30000
#define MAX_INHIBIT 18000

#define PM_SERVICE "org.freedesktop.PowerManagement"
#define PM_PATH "/PowerManagement"

class ScreenSaver : public QObject
{
    Q_OBJECT

public:
    explicit ScreenSaver()
    {
        timer.setInterval(TIMEOUT);
        connect(&timer, SIGNAL(timeout()), this, SLOT(timeOut()));
        timer.start();
    }

private:
    QTimer timer;
    QMap<quint32, QTime> clients;

private slots:
    int randInt(int low, int high)
    {
        QTime time = QTime::currentTime();
        qsrand((uint)time.msec());
        return qrand() % ((high + 1) - low) + low;
    }
    quint32 genCookie()
    {
        int low = 0;
        int high = 1000;
        quint32 cookie = randInt(low, high);
        while(!clients.contains(cookie)) {
            if (!clients.contains(cookie)) { clients[cookie] = QTime::currentTime(); }
            else { cookie = randInt(low, high); }
        }
        return cookie;
    }
    void checkForExpiredClients()
    {
        QMapIterator<quint32, QTime> client(clients);
        while (client.hasNext()) {
            client.next();
            if (client.value().secsTo(QTime::currentTime())>=MAX_INHIBIT) { clients.remove(client.key()); }
        }
    }
    bool canInhibit()
    {
        checkForExpiredClients();
        if (clients.size()>0) { return true; }
        return false;
    }
    void checkForDBusSession()
    {
        if (!QDBusConnection::sessionBus().isConnected()) {
            // DBus session has probably ended(?), so quit ...
            qApp->quit();
        }
    }
    void timeOut()
    {
        checkForDBusSession();
        if (canInhibit()) { SimulateUserActivity(); }
    }
    void pingPM()
    {
        QDBusInterface iface(PM_SERVICE, PM_PATH, PM_SERVICE, QDBusConnection::sessionBus());
        if (!iface.isValid()) {
            return;
        }
        iface.call("SimulateUserActivity");
    }
public slots:
    void SimulateUserActivity()
    {
        QProcess::startDetached(XSCREENSAVER);
        pingPM();
    }
    quint32 Inhibit(QString /*application*/, QString /*reason*/)
    {
        quint32 cookie = genCookie();
        timeOut();
        return cookie;
    }
    void UnInhibit(quint32 cookie)
    {
        if (clients.contains(cookie)) { clients.remove(cookie); }
        timeOut();
    }
};

#endif // SCREENSAVER_H
