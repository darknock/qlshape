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

#ifndef DARKNOCK_H
#define DARKNOCK_H

#include <QMainWindow>
#include <QPrinter>

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
class History;
class FilterSelector;
class GaussSelector;
class DogSelector;
class LapSelector;
class EdgeSelector;
class ZeroSelector;
class CannySelector;
class AiSelector;
class AboutQLShape;

class QLShape : public QMainWindow
{
    Q_OBJECT

public:
    QLShape();
    ~QLShape();
    void preOpen(QString fileName);

protected:
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent * event);
    void showEvent(QShowEvent * event);

private slots:
    void open();
    bool save();
    bool saveAs();
    void print();

    void undo();
    void redo();

    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();

    void about();

    void toGrayscale();
    void toMonochromatic();
    void negative();
    void normalize();

    void filterSelect();
    void filterGauss();
    void filterEdges();
    void filterLaplace();
    void filterDog();
    void filterZero();
    void filterCanny();
    void fillLines();
    void thinLines();
    void aiEdges();

private:
    QPrinter printer;
    QImage *curImage;
    QImage::Format curImageFormat;
    QString curDirPath;

    //Effects & Edges
    void enableEffects();
    FilterSelector *filterSelector;
    DogSelector *dogSelector;
    LapSelector *lapSelector;
    EdgeSelector *edgeSelector;
    ZeroSelector *zeroSelector;
    CannySelector *cannySelector;
    GaussSelector *gaussSelector;
    AiSelector *aiSelector;
    AboutQLShape *aboutQLShape;

    //History
    friend class History;
//     friend QImage* filterColor(const QImage* image, double f[]);
    class History *history;
    void updateImage(QImage *i, QString dscr);
    void setCurrentImage(QImage* i);

    //Files
    QString curFile;
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    bool maybeSave();
    bool saveFile(const QString &fileName);
    void documentWasModified();

    //Resizeing
    QLabel *imageLabel;
    QLabel *zoomLabel;
    QScrollArea *scrollArea;
    double scaleFactor;
    void updateActions();
    void scaleImage(double factor);
    void fitSize();
    void previousSize();
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    //Interface
    //Menus
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *effectsMenu;
    QMenu *filtersMenu;
    QMenu *edgesMenu;
    QMenu *aiMenu;
    QMenu *binaryMenu;
    QMenu *helpMenu;
    //Toolbars
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *viewToolBar;
    //File
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *printAct;
    QAction *exitAct;
    //Edit
    QAction *undoAct;
    QAction *redoAct;
    //Viev
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitToWindowAct;
    //Effects
    QAction *toGrayscaleAct;
    QAction *toMonochromaticAct;
    QAction *negativeAct;
    QAction *normalizeAct;
    //Filters
    QAction *filterSelectAct;
    QAction *filterGaussAct;
    //Edges
    QAction *filterEdgesAct;
    QAction *filterLaplaceAct;
    QAction *filterDogAct;
    QAction *filterZeroAct;
    QAction *filterCannyAct;
    //Artificial Intelligence
    QAction *aiEdgesAct;
    //Binary
    QAction *fillLinesAct;
    QAction *thinLinesAct;
    //Help
    QAction *aboutAct;
    QAction *aboutQtAct;
    //GUI Setup
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    //Settings
    void readSettings();
    void writeSettings();
};

#endif //DARKNOCK_H
