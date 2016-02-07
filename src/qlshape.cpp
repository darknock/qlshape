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
#include "history.h"
#include "effects.h"
#include "filterselector.h"
#include "gaussselector.h"
#include "dogselector.h"
#include "lapselector.h"
#include "edgeselector.h"
#include "zeroselector.h"
#include "cannyselector.h"
#include "aiselector.h"
#include "about.h"
#include "aieffects.h"

#include <QScrollArea>
#include <QTime>
#include <QStatusBar>
#include <QFileInfo>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPainter>
#include <QScrollBar>
#include <QMenuBar>
#include <QToolBar>
#include <QSettings>

QLShape::QLShape()
{
    setWindowIcon(QIcon(":/images/qlshape.png"));

    curImage = new QImage();

    history = new History(this);
    filterSelector = new FilterSelector(this);
    gaussSelector = new GaussSelector(this);
    dogSelector = new DogSelector(this);
    lapSelector = new LapSelector(this);
    edgeSelector = new EdgeSelector(this);
    zeroSelector = new ZeroSelector(this);
    cannySelector = new CannySelector(this);
    aiSelector = new AiSelector(this);
    aboutQLShape = new AboutQLShape(this);

    imageLabel = new QLabel;
    imageLabel->setBackgroundRole(QPalette::Dark);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    imageLabel->setAlignment(Qt::AlignCenter);

    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setWidget(imageLabel);
    setCentralWidget(scrollArea);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    readSettings();

    setCurrentFile("");
}

/**************************** Effects *****************************/
void QLShape::enableEffects() {
    bool null = curImage->isNull();
    bool binary = isBinary(curImage);
    toGrayscaleAct->setEnabled(!curImage->isGrayscale());
    toMonochromaticAct->setEnabled(!null);
    negativeAct->setEnabled(!null);
    normalizeAct->setEnabled(!null);
    filterSelectAct->setEnabled(!null);
    filterGaussAct->setEnabled(!null);
    filterEdgesAct->setEnabled(!null);
    filterLaplaceAct->setEnabled(!null);
    filterDogAct->setEnabled(!null);
    filterZeroAct->setEnabled(!null);
    filterCannyAct->setEnabled(!null);
    fillLinesAct->setEnabled(binary);
    thinLinesAct->setEnabled(binary);
    aiEdgesAct->setEnabled(!null);
}

void QLShape::toGrayscale() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QTime t;
    t.start();
    QImage *newImage;
    newImage = toGray(curImage);
    updateImage(newImage, tr("converting to grayscale"));
    statusBar()->showMessage(tr("Done in %1 seconds").arg(t.elapsed()/1000.0), 3000);
    QApplication::restoreOverrideCursor();
}

void QLShape::toMonochromatic() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QTime t;
    t.start();
    QImage *newImage;
    newImage = toMono(curImage);
    updateImage(newImage, tr("converting to monochromatic"));
    statusBar()->showMessage(tr("Done in %1 seconds").arg(t.elapsed()/1000.0), 3000);
    QApplication::restoreOverrideCursor();
}

void QLShape::negative() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QTime t;
    t.start();
    QImage *newImage;
    newImage = negate(curImage);
    updateImage(newImage, tr("changing image to negative"));
    statusBar()->showMessage(tr("Done in %1 seconds").arg(t.elapsed()/1000.0), 3000);
    QApplication::restoreOverrideCursor();
}

void QLShape::normalize() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QTime t;
    t.start();
    QImage *newImage;
    newImage = normalized(curImage);
    updateImage(newImage, tr("incresing contrast of the image"));
    statusBar()->showMessage(tr("Done in %1 seconds").arg(t.elapsed()/1000.0), 3000);
    QApplication::restoreOverrideCursor();
}

void QLShape::filterSelect() {
    if (filterSelector->exec()) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QTime t;
        t.start();
        QImage *newImage = filter(curImage, filterSelector->mask(),  filterSelector->rows(), filterSelector->cols());
        updateImage(newImage, tr("applying %1 filter").arg(filterSelector->filter()));
        statusBar()->showMessage(tr("Done in %1 seconds").arg(t.elapsed()/1000.0), 3000);
        QApplication::restoreOverrideCursor();
    }
}

void QLShape::filterGauss() {
    gaussSelector->setPreview(curImage);
    if (gaussSelector->exec()) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QTime t;
        t.start();
        QImage *newImage;
        newImage = gaussianBlur(curImage, gaussSelector->radius());
        updateImage(newImage, tr("applying gaussian filter"));
        statusBar()->showMessage(tr("Done in %1 seconds").arg(t.elapsed()/1000.0), 3000);
        QApplication::restoreOverrideCursor();
    }
}

void QLShape::filterEdges() {
    edgeSelector->setPreview(curImage);
    if (edgeSelector->exec()) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QTime t;
        t.start();
        QImage *newImage = edges(curImage, edgeSelector->filter(), edgeSelector->thold(), edgeSelector->blur(), edgeSelector->negative(), edgeSelector->binary());
        updateImage(newImage, tr("applying %1 filter").arg(edgeSelector->filter()));
        statusBar()->showMessage(tr("Done in %1 seconds").arg(t.elapsed()/1000.0), 3000);
        QApplication::restoreOverrideCursor();
    }
}

void QLShape::filterLaplace() {
    lapSelector->setPreview(curImage);
    if (lapSelector->exec()) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QTime t;
        t.start();
        QImage *newImage;
        newImage = laplace(curImage, lapSelector->mask(), lapSelector->blur(), lapSelector->negative(), lapSelector->normalize());
        updateImage(newImage, tr("detecting edges using Laplace filter"));
        statusBar()->showMessage(tr("Done in %1 seconds").arg(t.elapsed()/1000.0), 3000);
        QApplication::restoreOverrideCursor();
    }
}

void QLShape::filterDog() {
    dogSelector->setPreview(curImage);
    if (dogSelector->exec()) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QTime t;
        t.start();
        QImage *newImage;
        newImage = diffOfGaussian(curImage, dogSelector->radius1(), dogSelector->radius2(), dogSelector->negative(), dogSelector->normalize());
        updateImage(newImage, tr("detecting edges using Difference of Gauss operator"));
        statusBar()->showMessage(tr("Done in %1 seconds").arg(t.elapsed()/1000.0), 3000);
        QApplication::restoreOverrideCursor();
    }
}

void QLShape::filterZero() {
    zeroSelector->setPreview(curImage);
    if (zeroSelector->exec()) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QTime t;
        t.start();
        QImage *newImage;
        newImage = zeroCrossing(curImage, zeroSelector->blur(), zeroSelector->thold());
        updateImage(newImage, tr("detecting edges using zero-crossing operator"));
        statusBar()->showMessage(tr("Done in %1 seconds").arg(t.elapsed()/1000.0), 3000);
        QApplication::restoreOverrideCursor();
    }
}

void QLShape::filterCanny() {
    cannySelector->setPreview(curImage);
    if (cannySelector->exec()) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QTime t;
        t.start();
        QImage *newImage;
        newImage = canny(curImage, cannySelector->tholdHi(), cannySelector->tholdLow(), cannySelector->blur());
        updateImage(newImage, tr("detecting edges using Canny's operator"));
        statusBar()->showMessage(tr("Done in %1 seconds").arg(t.elapsed()/1000.0), 3000);
        QApplication::restoreOverrideCursor();
    }
}

void QLShape::aiEdges() {
    aiSelector->setPreview(curImage);
    if (aiSelector->exec()) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QTime t;
        t.start();
        QImage *newImage;
        newImage = hammingAiEdges(curImage, aiSelector->input(), aiSelector->xMin(), aiSelector->xMax(), aiSelector->hammingNet(), aiSelector->thold(), aiSelector->blur());
        updateImage(newImage, tr("detecting edges using Hamming's artificial neural network"));
        statusBar()->showMessage(tr("Done in %1 seconds").arg(t.elapsed()/1000.0), 3000);
        QApplication::restoreOverrideCursor();
    }
}


void QLShape::fillLines() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QTime t;
    t.start();
    QImage *newImage;
    newImage = completeLines(curImage);
    updateImage(newImage, tr("lines filling"));
    statusBar()->showMessage(tr("Done in %1 seconds").arg(t.elapsed()/1000.0), 3000);
    QApplication::restoreOverrideCursor();
}

void QLShape::thinLines() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QTime t;
    t.start();
    QImage *newImage;
    newImage = thinnerLines(curImage);
    updateImage(newImage, tr("lines thinning"));
    statusBar()->showMessage(tr("Done in %1 seconds").arg(t.elapsed()/1000.0), 3000);
    QApplication::restoreOverrideCursor();
}

/**************************** History *****************************/
void QLShape::setCurrentImage(QImage *i) {
    if(curImage != i) delete curImage;
    curImage = i;
    imageLabel->setPixmap(QPixmap::fromImage(*curImage));
    imageLabel->adjustSize();
    //scaleFactor = 1.0;
    updateActions();
    fitToWindow();
}

void QLShape::updateImage(QImage *i, QString dscr) {
    setCurrentImage(i);
    undoAct->setEnabled(true);
    redoAct->setEnabled(false);
    fitToWindow();
    history->add(*curImage, dscr);
    undoAct->setStatusTip(tr("Cancel %1").arg(dscr));
    documentWasModified();
}

void QLShape::undo() {
    history->undo();
    undoAct->setEnabled(history->hasPrevious());
    redoAct->setEnabled(true);

    if (history->hasPrevious()) undoAct->setStatusTip(tr("Cancel %1").arg(history->peekPreviousDscr()));
    else {
        undoAct->setStatusTip("");
        statusBar()->clearMessage();
    }
    if (history->hasNext()) redoAct->setStatusTip(tr("Repeat %1").arg(history->peekNextDscr()));
    else {
        redoAct->setStatusTip("");
        statusBar()->clearMessage();
    }
    documentWasModified();
}

void QLShape::redo() {
    history->redo();
    redoAct->setEnabled(history->hasNext());
    undoAct->setEnabled(true);

    if (history->hasPrevious()) undoAct->setStatusTip(tr("Cancel %1").arg(history->peekPreviousDscr()));
    else {
        undoAct->setStatusTip("");
        statusBar()->clearMessage();
    }
    if (history->hasNext()) redoAct->setStatusTip(tr("Repeat %1").arg(history->peekNextDscr()));
    else {
        redoAct->setStatusTip("");
        statusBar()->clearMessage();
    }
    documentWasModified();
}

/***************************** Files ******************************/
void QLShape::setCurrentFile(const QString &fileName) {
    curFile = fileName;
    setWindowModified(false);

    QString shownName;
    if (curFile.isEmpty())
        shownName = "untitled";
    else
        shownName = strippedName(curFile);

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("darknocK")));
}

QString QLShape::strippedName(const QString &fullFileName) {
    return QFileInfo(fullFileName).fileName();
}

void QLShape::closeEvent(QCloseEvent *event) {
    if (maybeSave()) {
        writeSettings();
        filterSelector->saveData();
        event->accept();
    } else {
        event->ignore();
    }
}

void QLShape::open() {
    if (maybeSave()) {
        QImage *tempImage = curImage;
        QString fileName = QFileDialog::getOpenFileName(this,
                           tr("Open File"), curDirPath,   tr("Images (*.png *.xpm *.jpg *.bmp);;All files (*.*)"));
        if (!fileName.isEmpty()) {
            curImage = new QImage(fileName);
            if (curImage->isNull()) {
                QMessageBox::information(this, tr("darknocK"), tr("Cannot load %1.").arg(fileName));
                curImage = tempImage;
                return;
            }
            curDirPath = QFileInfo(fileName).absolutePath();
            curImageFormat = curImage->format();
            curImage = new QImage(curImage->convertToFormat(QImage::Format_ARGB32));
            history->reset(*curImage);
            undoAct->setEnabled(false);
            redoAct->setEnabled(false);

            imageLabel->setPixmap(QPixmap::fromImage(*curImage));
            setCurrentFile(fileName);
            if (fitToWindowAct->isChecked()) fitToWindow();
            scaleFactor = 1.0;

            saveAsAct->setEnabled(true);
            printAct->setEnabled(true);
            updateActions();

            enableEffects();

            if (!fitToWindowAct->isChecked())
                imageLabel->adjustSize();
        }
    }
}

void QLShape::preOpen(QString fileName) {
    if (!fileName.isEmpty()) {
        curImage = new QImage(fileName);
        if (curImage->isNull()) {
            QMessageBox::information(this, tr("darknocK"), tr("Cannot load %1.").arg(fileName));
            return;
        }
        curImageFormat = curImage->format();
        curImage = new QImage(curImage->convertToFormat(QImage::Format_ARGB32));
        history->reset(*curImage);
        undoAct->setEnabled(false);
        redoAct->setEnabled(false);

        imageLabel->setPixmap(QPixmap::fromImage(*curImage));
        setCurrentFile(fileName);
        if (fitToWindowAct->isChecked()) fitToWindow();
        scaleFactor = 1.0;

        saveAsAct->setEnabled(true);
        printAct->setEnabled(true);
        updateActions();

        enableEffects();

        if (!fitToWindowAct->isChecked())
            imageLabel->adjustSize();
    }
}

bool QLShape::save() {
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool QLShape::saveAs() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                       curFile, tr("Portable Networks Graphics (*.png);;XPixMap (*.xpm);;JPEG (*.jpg);;Windows Bitmap (*.bmp)"));
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool QLShape::maybeSave() {
    if (isWindowModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("darknocK"),
                                   tr("Image has been modified.\n"
                                      "Do you want to save your changes?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

bool QLShape::saveFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("darknocK"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QImage saveImage(*curImage);
    // TODO: check why do we need this
    if (saveImage.isGrayscale()) {
        saveImage.convertToFormat(curImageFormat);
    }
    saveImage.save(fileName);
    saveAct->setEnabled(false);
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void QLShape::print() {
    Q_ASSERT(imageLabel->pixmap());
    QPrintDialog dialog(&printer, this);
    if (dialog.exec()) {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QSize size = imageLabel->pixmap()->size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(imageLabel->pixmap()->rect());
        painter.drawPixmap(0, 0, *imageLabel->pixmap());
    }
}

void QLShape::documentWasModified() {
    setWindowModified(true);
    saveAct->setEnabled(true);
    enableEffects();
}

/*************************** Resizeing ****************************/
void QLShape::updateActions() {
    if (!curImage->isNull()) {
        zoomInAct->setEnabled(!fitToWindowAct->isChecked() && scaleFactor < 3.0);
        zoomOutAct->setEnabled(!fitToWindowAct->isChecked() && scaleFactor > 0.333);
        normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
    }
}

void QLShape::scaleImage(double factor) {
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->setVisible(false);
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());
    imageLabel->setVisible(true);
    zoomLabel->setText(tr("Zoom: %1%").arg(int(scaleFactor * 100)));

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void QLShape::adjustScrollBar(QScrollBar *scrollBar, double factor) {
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

void QLShape::fitSize() {
    int w = scrollArea->width()<curImage->width() ? scrollArea->width() : curImage->width();
    int h = scrollArea->height()<curImage->height() ? scrollArea->height() : curImage->height();

    imageLabel->setPixmap(QPixmap::fromImage(*curImage).scaled(w, h, Qt::KeepAspectRatio, Qt::FastTransformation));

    int zoom = 100 * w / curImage->width() < 100 * h / curImage->height() ?  100 * w / curImage->width() : 100 * h / curImage->height();
    zoomLabel->setText(tr("Zoom: %1%").arg(zoom));
}

void QLShape::zoomIn() {
    if (!curImage->isNull() && !fitToWindowAct->isChecked()) scaleImage(1.25);
}

void QLShape::zoomOut() {
    if (!curImage->isNull() && !fitToWindowAct->isChecked()) scaleImage(0.8);
}

void QLShape::normalSize() {
    imageLabel->setPixmap(QPixmap::fromImage(*curImage));
    imageLabel->adjustSize();

    scaleFactor = 1.0;
    zoomLabel->setText(tr("Zoom: %1%").arg(scaleFactor * 100));

    zoomInAct->setEnabled(true);
    zoomOutAct->setEnabled(true);
}

void QLShape::previousSize() {
    imageLabel->setPixmap(QPixmap::fromImage(*curImage));
    scaleImage(1.0);
}


void QLShape::fitToWindow() {
    int isFitToWindow = fitToWindowAct->isChecked();
    scrollArea->setWidgetResizable(isFitToWindow);

    if (!curImage->isNull()) {
        if (!isFitToWindow) {
            imageLabel->setScaledContents(true);
            imageLabel->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
            previousSize();
        } else {
            imageLabel->setVisible(false);
            imageLabel->setScaledContents(false);
            imageLabel->setMaximumSize(curImage->size());
            fitSize();
            imageLabel->setVisible(true);
        }
    }

    updateActions();
}

void QLShape::resizeEvent(QResizeEvent * event) {
    if (fitToWindowAct->isChecked() && !curImage->isNull()) fitSize();
    event->accept();
}

/************************ Interface setup *************************/
void QLShape::createActions() {
    openAct = new QAction(QIcon(":/images/fileopen.png"), tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O", "Open"));
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/images/filesave.png"), tr("&Save"), this);
    saveAct->setShortcut(tr("Ctrl+S", "Save"));
    saveAct->setStatusTip(tr("Save image to disk"));
    saveAct->setEnabled(false);
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(QIcon(":/images/filesaveas.png"), tr("Save &As..."), this);
    saveAsAct->setStatusTip(tr("Save image under a new name"));
    saveAsAct->setEnabled(false);
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    printAct = new QAction(QIcon(":/images/fileprint.png"), tr("&Print..."), this);
    printAct->setShortcut(tr("Ctrl+P", "Print"));
    printAct->setEnabled(false);
    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

    exitAct = new QAction(QIcon(":/images/exit.png"), tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q", "Quit"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    //Edit
    undoAct = new QAction(QIcon(":/images/undo.png"), tr("&Undo"), this);
    undoAct->setShortcut(tr("Ctrl+Z", "Undo"));
    undoAct->setEnabled(false);
    connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()));

    redoAct = new QAction(QIcon(":/images/redo.png"), tr("&Redo"), this);
    redoAct->setShortcut(tr("Ctrl+Y", "Redo"));
    redoAct->setEnabled(false);
    connect(redoAct, SIGNAL(triggered()), this, SLOT(redo()));

    //View
    zoomInAct = new QAction(QIcon(":/images/viewmag+.png"), tr("Zoom &In (25%)"), this);
    zoomInAct->setShortcut(tr("Ctrl++", "Zoom in"));
    zoomInAct->setEnabled(false);
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(QIcon(":/images/viewmag-.png"), tr("Zoom &Out (25%)"), this);
    zoomOutAct->setShortcut(tr("Ctrl+-", "Zoom out"));
    zoomOutAct->setEnabled(false);
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    normalSizeAct = new QAction(QIcon(":/images/viewmag1.png"), tr("&Normal Size"), this);
    normalSizeAct->setShortcut(tr("Ctrl+N", "Normal size"));
    normalSizeAct->setEnabled(false);
    connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

    fitToWindowAct = new QAction(QIcon(":/images/viewmagfit.png"), tr("&Fit to Window"), this);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setShortcut(tr("Ctrl+F", "Fit to window"));
    connect(fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));

    //Effects
    toGrayscaleAct = new QAction(tr("&Grayscale"), this);
    toGrayscaleAct->setStatusTip(tr("Change image format to grayscale"));
    toGrayscaleAct->setEnabled(false);
    connect(toGrayscaleAct, SIGNAL(triggered()), this, SLOT(toGrayscale()));

    toMonochromaticAct = new QAction(tr("&Monochromatic"), this);
    toMonochromaticAct->setStatusTip(tr("Change image format to monochromatic"));
    toMonochromaticAct->setEnabled(false);
    connect(toMonochromaticAct, SIGNAL(triggered()), this, SLOT(toMonochromatic()));

    negativeAct = new QAction(tr("Negati&ve"), this);
    negativeAct->setStatusTip(tr("Change image to negative"));
    negativeAct->setEnabled(false);
    connect(negativeAct, SIGNAL(triggered()), this, SLOT(negative()));

    normalizeAct = new QAction(tr("Normali&ze"), this);
    normalizeAct->setStatusTip(tr("Increse contrast of the image"));
    normalizeAct->setEnabled(false);
    connect(normalizeAct, SIGNAL(triggered()), this, SLOT(normalize()));

    //Filters
    filterSelectAct = new QAction(QIcon(":/images/filter.png"), tr("&Filter selector"), this);
    filterSelectAct->setStatusTip(tr("Open a filter selection dialog"));
    filterSelectAct->setEnabled(false);
    connect(filterSelectAct, SIGNAL(triggered()), this, SLOT(filterSelect()));

    filterGaussAct = new QAction(tr("Gaussian &blur"), this);
    filterGaussAct->setStatusTip(tr("Apply gaussian blur filter to image"));
    filterGaussAct->setEnabled(false);
    connect(filterGaussAct, SIGNAL(triggered()), this, SLOT(filterGauss()));

    //Edges
    filterEdgesAct = new QAction(tr("Filter &Edges"), this);
    filterEdgesAct->setStatusTip(tr("Open edge detection dialog"));
    filterEdgesAct->setEnabled(false);
    connect(filterEdgesAct, SIGNAL(triggered()), this, SLOT(filterEdges()));

    filterLaplaceAct = new QAction(tr("&Laplace"), this);
    filterLaplaceAct->setStatusTip(tr("Apply Laplace filter to image"));
    filterLaplaceAct->setEnabled(false);
    connect(filterLaplaceAct, SIGNAL(triggered()), this, SLOT(filterLaplace()));

    filterDogAct = new QAction(tr("&DOG"), this);
    filterDogAct->setStatusTip(tr("Apply Difference of Gauss filter to image"));
    filterDogAct->setEnabled(false);
    connect(filterDogAct, SIGNAL(triggered()), this, SLOT(filterDog()));

    filterZeroAct = new QAction(tr("&Zero-Crossing"), this);
    filterZeroAct->setStatusTip(tr("Apply zero-crossing operator to image"));
    filterZeroAct->setEnabled(false);
    connect(filterZeroAct, SIGNAL(triggered()), this, SLOT(filterZero()));

    filterCannyAct = new QAction(tr("&Canny"), this);
    filterCannyAct->setStatusTip(tr("Apply Canny's operator to image"));
    filterCannyAct->setEnabled(false);
    connect(filterCannyAct, SIGNAL(triggered()), this, SLOT(filterCanny()));

    //AI
    aiEdgesAct = new QAction(tr("&Hamming ANN"), this);
    aiEdgesAct->setStatusTip(tr("Detect edges with Hamming's Artificial Neural Network"));
    aiEdgesAct->setEnabled(false);
    connect(aiEdgesAct, SIGNAL(triggered()), this, SLOT(aiEdges()));

    //Binary
    fillLinesAct = new QAction(tr("Lines &filling"), this);
    fillLinesAct->setStatusTip(tr("filling of 1 pixel gaps between lines in binary images"));
    fillLinesAct->setEnabled(false);
    connect(fillLinesAct, SIGNAL(triggered()), this, SLOT(fillLines()));

    thinLinesAct = new QAction(tr("Lines &thinning"), this);
    thinLinesAct->setStatusTip(tr("thinning of thick lines in binary images"));
    thinLinesAct->setEnabled(false);
    connect(thinLinesAct, SIGNAL(triggered()), this, SLOT(thinLines()));

    //Help
    aboutAct = new QAction(QIcon(":/images/help.png"), tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(QIcon(":/images/qt4.png"), tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void QLShape::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(normalSizeAct);
    viewMenu->addSeparator();
    viewMenu->addAction(fitToWindowAct);

    effectsMenu = menuBar()->addMenu(tr("Effe&cts"));
    effectsMenu->addAction(toGrayscaleAct);
    effectsMenu->addAction(toMonochromaticAct);
    effectsMenu->addAction(negativeAct);
    effectsMenu->addAction(normalizeAct);

    filtersMenu = menuBar()->addMenu(tr("Fil&ters"));
    filtersMenu->addAction(filterSelectAct);
    filtersMenu->addAction(filterGaussAct);

    edgesMenu = menuBar()->addMenu(tr("Ed&ges"));
    edgesMenu->addAction(filterEdgesAct);
    edgesMenu->addAction(filterLaplaceAct);
    edgesMenu->addAction(filterDogAct);
    edgesMenu->addAction(filterZeroAct);
    edgesMenu->addAction(filterCannyAct);

    aiMenu = menuBar()->addMenu(tr("A&I"));
    aiMenu->addAction(aiEdgesAct);

    binaryMenu = menuBar()->addMenu(tr("&Binary images"));
    binaryMenu->addAction(fillLinesAct);
    binaryMenu->addAction(thinLinesAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void QLShape::createToolBars() {
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->addAction(saveAsAct);
    fileToolBar->addAction(printAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(undoAct);
    editToolBar->addAction(redoAct);

    viewToolBar = addToolBar(tr("View"));
    viewToolBar->addAction(zoomInAct);
    viewToolBar->addAction(zoomOutAct);
    viewToolBar->addAction(normalSizeAct);
    viewToolBar->addSeparator();
    viewToolBar->addAction(fitToWindowAct);
}

void QLShape::createStatusBar() {
    statusBar()->showMessage(tr("Ready"));
    zoomLabel = new QLabel();
    zoomLabel->setText(tr("Zoom: 100%"));
    statusBar()->addPermanentWidget(zoomLabel);
}

//Settings
void QLShape::readSettings() {
    QSettings settings("darknock", "darknocK");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    curDirPath = settings.value("dir", QDir::currentPath()).toString();
    resize(size);
    move(pos);
}

void QLShape::writeSettings() {
    QSettings settings("darknock", "darknocK");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("fitted", fitToWindowAct->isChecked());
    settings.setValue("dir", curDirPath);
}

void QLShape::showEvent(QShowEvent * event) {
    QSettings settings("darknock", "darknocK");
    bool fitted = settings.value("fitted", false).toBool();
    fitToWindowAct->setChecked(fitted);
    event->accept();
}

void QLShape::about() {
    aboutQLShape->exec();
}

QLShape::~QLShape() {
    delete curImage;
    delete history;
}

