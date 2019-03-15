/****************************************************************************
** 
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
** 
** This file is part of a Qt Solutions component.
**
** Commercial Usage  
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
** 
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
** 
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
** 
** GNU General Public License Usage 
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
** 
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
** 
** If you are unsure which license is appropriate for your use, please
** contact Nokia at qt-info@nokia.com.
** 
****************************************************************************/

#include "qtwindowlistmenu.h"

#include <QList>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenuBar>

/*!
    \class QtWindowListMenu

    \brief The QtWindowListMenu class is a menu that provides
    navigation commands for the subwindows in a QMdiArea.

    It is typically used as the conventional "Windows" item in an MDI
    application's menubar. It provides some standard commands for
    arranging and closing the MDI subwindows, corresponding to the
    slots of QMdiArea, as well as shortcuts to navigate directly to
    each of them.

    Usage: After creation, use attachToMdiArea() to tell the
    QtWindowListMenu object which QMdiArea object it should provide
    navigation commands for.  It can then be added to a QMenuBar (or
    other objects that can take QMenus or QActions, like QToolButton
    or QMenu) in the ordinary way, since QtWindowListMenu is a QMenu
    subclass.

    The subwindow navigation items may be given a common icon with
    setDefaultIcon(). The item icon for a specific subwindow can be
    set with setWindowIcon().

    Customization: Additional menu items (actions) can be inserted
    into this menu in the ordinary way for a QMenu. The
    standardAction() method gives access to the standard navigation
    items, which can be used to change their icon, shortcut,
    visibility etc. Ultimately, the whole menu can be customized by
    overriding syncWithMdiArea() in a subclass of QtWindowListMenu.

    If a QtWindowListMenu is showed and opened before being attached
    to a QMdiArea, it will try to auto-attach itself to the closest
    sibling object that is or inherits QMdiArea, if any exists. This
    behaviour is intended for special usecases; normally you will want
    to explicitly specify the desired QMdiArea with attachToMdiArea().

    \sa QMdiArea, QMdiSubWindow, QMenuBar, QAction
*/

/*! \enum QtWindowListMenu::StandardAction

  This enum specifies the standard menu items of a QtWindowListMenu.

  \value CloseAction    Ref. QMdiArea::closeActiveSubWindow()
  \value CloseAllAction Ref. QMdiArea::closeAllSubWindows()
  \value TileAction     Ref. QMdiArea::tileSubWindows()
  \value CascadeAction  Ref. QMdiArea::cascadeSubWindows()
  \value NextAction     Ref. QMdiArea::activateNextSubWindow()
  \value PrevAction     Ref. QMdiArea::activatePreviousSubWindow()

  \sa standardAction()
*/





/*!
    Constructs a QtWindowListMenu object. The \a parent parameter is
    passed to the QMenu constructor. Although this parameter has the
    conventional default of 0, you will normally want to explicitly
    provide a parent object, since the later adding of this menu
    object to an action container (e.g. QMenuBar) does not cause a
    reparenting. The container is normally the natural choice for \a parent.
*/

QtWindowListMenu::QtWindowListMenu(QWidget *parent)
    : QMenu(parent)
{
    mdi = nullptr;
    setTitle(tr("&Windows"));
    connect(this, SIGNAL(aboutToShow()), SLOT(syncWithMdiArea()));
    stdGroup = new QActionGroup(this);
    stdGroup->setExclusive(false);
    winGroup = new QActionGroup(this);
    winGroup->setExclusive(true);
    connect(winGroup, SIGNAL(triggered(QAction *)), SLOT(activateWindow(QAction *)));

    // Create the standard menu items.
    // Note: Creation order must match the StandardAction enum values ;-)
    QAction *act = new QAction(tr("Cl&ose"), stdGroup);
    act->setShortcut(tr("Ctrl+F4"));
    act->setStatusTip(tr("Close the active window"));

    act = new QAction(tr("Close &All"), stdGroup);
    act->setStatusTip(tr("Close all the windows"));

    act = stdGroup->addAction(QString());
    act->setSeparator(true);

    act = new QAction(tr("&Tile"), stdGroup);
    act->setStatusTip(tr("Tile the windows"));

    act = new QAction(tr("&Cascade"), stdGroup);
    act->setStatusTip(tr("Cascade the windows"));

    act = stdGroup->addAction(QString());
    act->setSeparator(true);

    act = new QAction(tr("Ne&xt"), stdGroup);
    act->setStatusTip(tr("Move the focus to the next window"));

    act = new QAction(tr("Pre&vious"), stdGroup);
    act->setStatusTip(tr("Move the focus to the previous window"));

    act = stdGroup->addAction(QString());
    act->setSeparator(true);

    addActions(stdGroup->actions());
}


/*!
  Instructs this menu to display navigation actions for the QMdiArea
  \a mdiArea. This should be done before this menu is shown.

  Specifying a 0 \a mdiArea is meaningless and will generate a warning.

  For special usecases, see the note about auto-attachment in the
  class description.
*/

void QtWindowListMenu::attachToMdiArea(QMdiArea* mdiArea)
{
    const QList<QAction *> acts = stdGroup->actions();
    if (mdi) {                   // i.e. we have previously been attached
        if (mdiArea == mdi)
            return;
        disconnect(acts[CloseAction], SIGNAL(triggered()), mdi, SLOT(closeActiveSubWindow()));
        disconnect(acts[CloseAllAction], SIGNAL(triggered()), mdi, SLOT(closeAllSubWindows()));
        disconnect(acts[TileAction], SIGNAL(triggered()), mdi, SLOT(tileSubWindows()));
        disconnect(acts[CascadeAction], SIGNAL(triggered()), mdi, SLOT(cascadeSubWindows()));
        disconnect(acts[NextAction], SIGNAL(triggered()), mdi, SLOT(activateNextSubWindow()));
        disconnect(acts[PrevAction], SIGNAL(triggered()), mdi, SLOT(activatePreviousSubWindow()));
    }

    mdi = mdiArea;
    if (!mdi) {
        qWarning("QtWindowListMenu::attachToMdiArea(): mdiArea is 0; menu will be empty.");
        return;
    }

    connect(acts[CloseAction], SIGNAL(triggered()), mdi, SLOT(closeActiveSubWindow()));
    connect(acts[CloseAllAction], SIGNAL(triggered()), mdi, SLOT(closeAllSubWindows()));
    connect(acts[TileAction], SIGNAL(triggered()), mdi, SLOT(tileSubWindows()));
    connect(acts[CascadeAction], SIGNAL(triggered()), mdi, SLOT(cascadeSubWindows()));
    connect(acts[NextAction], SIGNAL(triggered()), mdi, SLOT(activateNextSubWindow()));
    connect(acts[PrevAction], SIGNAL(triggered()), mdi, SLOT(activatePreviousSubWindow()));
}


/*!

  Returns the QMdiArea this menu is currently attached to, or 0 if not
  yet attached.

  \sa attachToMdiArea()
*/

QMdiArea *QtWindowListMenu::attachedMdiArea() const
{
    return mdi;
}

/*
  Heuristic method to auto-attach to the most likely intended mdiArea
  object, i.e. the closest "sibling" mdiArea widget. In the typical
  case, there will be only one in the mainwindow that owns this menu,
  and this method will find it.
*/

bool QtWindowListMenu::attachToClosestMdiAreaObject()
{
    if (mdi)
        return true;

    QMdiArea *m = 0;
    QWidget *p = this;
    do {
        p = p->parentWidget();
        if (!p)
            return false;
        m = qobject_cast<QMdiArea *>(p);
        if (!m)
            m = p->findChild<QMdiArea *>();
    } while (!m);

    attachToMdiArea(m);
    return true;
}

/*!

  This slot is executed immediately prior to each opening of this
  menu. It removes the previous subwindow navigation actions and adds new
  ones according to the current state of the attached QMdiArea.
*/

void QtWindowListMenu::syncWithMdiArea()
{
    if (!mdi && !attachToClosestMdiAreaObject())
        return;

    stdGroup->setEnabled(mdi->subWindowList().size() > 0);

    winMap.clear();
    foreach(QAction* act, winGroup->actions()) {
        removeAction(act);
        winGroup->removeAction(act);
        delete act;
    }

    int idx = 1;
    foreach(QMdiSubWindow *win, mdi->subWindowList()) {
        QString modMarker = win->isWindowModified() ? "*" : "";
        QString title = win->windowTitle().replace("[*]", modMarker);
        QString text;
        if (idx < 10)
            text = tr("&%1 %2").arg(idx).arg(title);
        else
            text = tr("%1 %2").arg(idx).arg(title);
        QIcon icon = iconMap.value(win, defIcon);
        QAction* action = new QAction(icon, text, winGroup);
        action->setCheckable(true);
        action->setChecked(win == mdi->activeSubWindow());
        winMap.insert(action, win);
        idx++;
    }

    addActions(winGroup->actions());
}


/*!
  This slot is executed when the user selects one of the subwindow
  navigation actions, given in \a act. It causes the corresponding
  subwindow in the attached QMdiArea object to be activated.
*/


void QtWindowListMenu::activateWindow(QAction *act)
{
    if (!mdi && !attachToClosestMdiAreaObject())
        return;
    mdi->setActiveSubWindow(winMap.value(act));
}


/*!
  This slot is executed whenever a subwindow (\a obj) of the attached
  QMdiArea, for which an icon has been, is deleted. It clears that icon.
*/

void QtWindowListMenu::windowDestroyed(QObject* obj)
{
    iconMap.remove(static_cast<QMdiSubWindow *>(obj));
}


/*!
  Sets \a icon as the icon of the menu item corresponding to the mdi
  subwindow \a window. If \a icon is a null icon, the current item
  icon will be cleared.

  \sa windowIcon()
*/
void QtWindowListMenu::setWindowIcon(const QMdiSubWindow *window, const QIcon &icon)
{
    if (!window)
        return;
    if (icon.isNull()) {
        iconMap.remove(window);
    }
    else {
        iconMap.insert(window, icon);
        connect(window, SIGNAL(destroyed(QObject *)), SLOT(windowDestroyed(QObject *)));
    }
}


/*!
  Returns the icon of the menu item corresponding to the mdi
  subwindow \a window. This will be a null icon if none has been
  explicitly set.

  \sa setWindowIcon()
*/
QIcon QtWindowListMenu::windowIcon(const QMdiSubWindow *window) const
{
    return iconMap.value(window);
}


/*!
  Sets \a icon as the default icon for the subwindow navigation
  items in this QtWindowListMenu. If \a icon is a null icon, then
  the default icon will be cleared.

  \sa defaultIcon()
*/
void QtWindowListMenu::setDefaultIcon(const QIcon &icon)
{
    defIcon = icon;
}


/*!
  Returns the default icon for the subwindow navigation items in
  this QtWindowListMenu. This will be a null icon if none has been
  explicitly set.

  \sa setDefaultIcon()
*/

QIcon QtWindowListMenu::defaultIcon() const
{
    return defIcon;
}


/*!
  Returns a pointer to the standard navigation action of this menu
  specified by \a item. This can be used to customize the look,
  shortcut, tool tip, etc. of this item, or to provide alternative
  access to it through a tool button etc.

  The returned object is owned by this QtWindowListMenu, and must not
  be deleted. If you want QtWindowListMenu to not display this action,
  set its "visible" property to false.
*/

QAction *QtWindowListMenu::standardAction(StandardAction item) const
{
    return stdGroup->actions().value(item);
}


/*!
  \obsolete
*/
void QtWindowListMenu::setCloseIcon(const QIcon &icon)
{
    standardAction(CloseAction)->setIcon(icon);
}

/*!
  \obsolete
*/
void QtWindowListMenu::setCascadeIcon(const QIcon &icon)
{
    standardAction(CascadeAction)->setIcon(icon);
}

/*!
  \obsolete
*/
void QtWindowListMenu::setTileIcon(const QIcon &icon)
{
    standardAction(TileAction)->setIcon(icon);
}

/*!
  \obsolete
*/
void QtWindowListMenu::setCloseAllIcon(const QIcon &icon)
{
    standardAction(CloseAllAction)->setIcon(icon);
}


/*!
  \obsolete
*/

QAction *QtWindowListMenu::addTo(const QString &text, QMenuBar *menubar, int idx)
{
    if (!menubar)
    	return 0;

    QList<QAction *> actions = menubar->actions();
    QAction *action = 0;
    if (idx >= 0 && idx < actions.size())
        action = actions[idx];

    setTitle(text);
    return menubar->insertMenu(action, this);
}
