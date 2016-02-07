/***************************************************************************
 *   Copyright (C) 2007 - 2016 by Konrad Ciekot                                   *
 *   darknock@o2.pl                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "qlshape.h"

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QSettings>

int main(int argc, char *argv[])
{
      Q_INIT_RESOURCE(application);
      QApplication app(argc, argv);

      #ifdef Q_OS_X11
        QString locale = QLocale::system().name();
        QString lang_file = QString("/usr/share/darknock/locale/qlshape_") + locale;
      #endif

      #ifdef Q_OS_MAC
        // use Library folder instead
        QString locale = QLocale::system().name();
        QString lang_file = QString("/usr/share/darknock/locale/qlshape_") + locale;
      #endif

      #ifdef Q_OS_WIN
        QSettings settings("darknock", "qlshape");
        int code = settings.value("Installer Language", 1033).toInt();
        QString locale;
        switch(code) {
            case 1033: locale = "en"; break;
            case 1036: locale = "fr"; break;
            case 1040: locale = "it"; break;
            case 1045: locale = "pl"; break;
            case 1048: locale = "ro"; break;
            case 1053: locale = "sv"; break;
            case 1055: locale = "tr"; break;
        }
        QString lang_file = QString("locale/qlshape_") + locale;
      #endif

      QTranslator translator;
      translator.load(lang_file);
      app.installTranslator(&translator);

      QLShape * mw = new QLShape();
      mw->show();
      if(argc == 2) mw->preOpen(argv[1]);
      return app.exec();
}
