/*
#
# Cyan - https://github.com/rodlie/cyan
#
# Copyright (c) 2020-2022 Ole-André Rodlie. All rights reserved.
# Copyright (c) 2018-2019 Ole-André Rodlie, FxArena. All rights reserved.
# Copyright (c) 2016-2017 Ole-André Rodlie, INRIA. All rights reserved.
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

#ifndef CYAN_H
#define CYAN_H

#include <QMainWindow>

namespace Cyan
{
    class Window : public QMainWindow
    {
        Q_OBJECT

    public:
        Window(QWidget *parent = nullptr);
        ~Window();
    };
}

#endif // CYAN_H
