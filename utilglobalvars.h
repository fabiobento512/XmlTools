/*
 *
Copyright (C) 2017  Fábio Bento (random-guy)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#ifndef UTILGLOBALVARS_H
#define UTILGLOBALVARS_H

namespace GlobalVars{

static const QString AppName = "XmlTools";
static const QString AppVersion = "2.0d";
#ifdef Q_OS_WIN
static const QString AppExecutable=AppName+".exe";
#else
static const QString AppExecutable="./"+AppName; // Mac OS needs unix like executing
#endif

}

#endif // UTILGLOBALVARS_H
