#include "mainwindow.h"

#include <QtGui>
#include <QGraphicsSvgItem>
#include <QSvgGenerator>
#include "chessboard.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    scene = new ChessBoard;
    settings = 0;
    setupMenus();
    getSettings();
    QGraphicsView *view = new QGraphicsView(scene);
    setCentralWidget(view);
}

MainWindow::~MainWindow()
{
    setSettings();
    delete settings;
}


void MainWindow::getSettings()
{
    if( scene == 0)
        return;

    settings = new QSettings("AdamBaker", "Chess");

    scene->setVersion( settings->value("piece-mode","0").toUInt() );
    if(scene->version()==ChessBoard::Traditional)
        traditional->setChecked(true);
    else
        secularized->setChecked(true);
    scene->fromString( settings->value("piece-positions","").toString() );
    scene->setLightPieceColor( colorFromString( settings->value("light-piece-color", "0 0 0" ).toString() ) );
    scene->setDarkPieceColor( colorFromString( settings->value("dark-piece-color", "0 0 0" ).toString() ) );
    scene->setLightSquareColor( colorFromString( settings->value("light-square-color", "255 255 255" ).toString() ) );
    scene->setDarkSquareColor( colorFromString( settings->value("dark-square-color", "160 160 160" ).toString() ) );
}

void MainWindow::setSettings()
{
    if( settings == 0 || scene == 0)
        return;

    settings->setValue("piece-mode",scene->version());
    settings->setValue("piece-positions",scene->toString());
    settings->setValue("light-piece-color",stringFromColor(scene->lightPieceColor()));
    settings->setValue("dark-piece-color",stringFromColor(scene->darkPieceColor()));
    settings->setValue("light-square-color",stringFromColor(scene->lightSquareColor()));
    settings->setValue("dark-square-color",stringFromColor(scene->darkSquareColor()));
}

void MainWindow::setupMenus()
{
    QMenu *file = new QMenu(tr("File"));
    file->addAction(tr("Clear board"),scene,SLOT(clearBoard()),QKeySequence::New);
    file->addAction(tr("Starting positions"),scene,SLOT(setInitialPositions()));
    file->addSeparator();
    file->addAction(tr("Save"),this,SLOT(save()),QKeySequence::Save);
    file->addAction(tr("Open"),this,SLOT(open()),QKeySequence::Open);
    file->addAction(tr("Create SVG"),this,SLOT(createSvg()),QKeySequence::Print);
    file->addAction(tr("Quit"),this,SLOT(close()),QKeySequence::Quit);

    QMenu *colors = new QMenu(tr("Colors"));
    colors->addAction(tr("Set Light Square Color"),this,SLOT(setLightSquareColor()));
    colors->addAction(tr("Set Dark Square Color"),this,SLOT(setDarkSquareColor()));
    colors->addSeparator();
    colors->addAction(tr("Set Light Piece Color"),this,SLOT(setLightPieceColor()));
    colors->addAction(tr("Set Dark Piece Color"),this,SLOT(setDarkPieceColor()));
    colors->addSeparator();
    colors->addAction(tr("Restore defaults"),scene,SLOT(setDefaultColors()));

    QMenu *version = new QMenu(tr("Pieces"));
    traditional = version->addAction(tr("Traditional"));
    traditional->setData(ChessBoard::Traditional);
    traditional->setCheckable(true);
    traditional->setChecked(true);
    secularized = version->addAction(tr("Secularized"));
    secularized->setData(ChessBoard::Secular);
    secularized->setCheckable(true);
    QActionGroup *versionGroup = new QActionGroup(this);
    versionGroup->setExclusive(true);
    versionGroup->addAction(traditional);
    versionGroup->addAction(secularized);
    connect(versionGroup,SIGNAL(triggered(QAction*)),scene,SLOT(setVersion(QAction*)));

    menuBar()->addMenu(file);
    menuBar()->addMenu(colors);
    menuBar()->addMenu(version);
}

void MainWindow::save()
{
    QString filename = QFileDialog::getSaveFileName(this,tr("Chess"),QString(),tr("Chess Files (*.chs)"));
    if(filename.isEmpty())
        return;

    QFile file(filename);
    if(!file.open(QFile::WriteOnly|QFile::Text))
    {
        qDebug() << "Could not open:" << filename;
        return;
    }
    QTextStream stream(&file);
    stream << scene->toString();
    file.close();
}

void MainWindow::open()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Chess"),QString(),tr("Chess Files (*.chs)"));
    if(filename.isEmpty())
        return;

    QFile file(filename);
    if(!file.open(QFile::ReadOnly|QFile::Text))
    {
        qDebug() << "Could not open:" << filename;
        return;
    }
    QTextStream stream(&file);
    scene->fromString( stream.readAll() );
    file.close();
}

void MainWindow::createSvg()
{
    QString filename = QFileDialog::getSaveFileName(this,tr("Chess"),QString(),tr("SVG Files (*.svg)"));
    if(filename.isEmpty())
        return;

    scene->setSvgRender(true);

    QSvgGenerator generator;
    generator.setFileName(filename);
    generator.setSize(QSize(200, 200));
    generator.setViewBox(QRect(0, 0, 200, 200));
    QPainter painter(&generator);
    scene->render(&painter);

    scene->setSvgRender(false);
}

void MainWindow::setLightSquareColor()
{
    QColor col = QColorDialog::getColor(scene->lightSquareColor(), this, tr("Choose a color") );
    if(col.isValid())
        scene->setLightSquareColor(col);
}

void MainWindow::setDarkSquareColor()
{
    QColor col = QColorDialog::getColor(scene->darkSquareColor(), this, tr("Choose a color") );
    if(col.isValid())
        scene->setDarkSquareColor(col);
}

void MainWindow::setLightPieceColor()
{
    QColor col = QColorDialog::getColor(scene->lightPieceColor(), this, tr("Choose a color") );
    if(col.isValid())
        scene->setLightPieceColor(col);
}

void MainWindow::setDarkPieceColor()
{
    QColor col = QColorDialog::getColor(scene->darkPieceColor(), this, tr("Choose a color") );
    if(col.isValid())
        scene->setDarkPieceColor(col);
}

QColor MainWindow::colorFromString(QString s) const
{
    QStringList list = s.split(" ");
    int r = list.at(0).toInt();
    int g = list.at(1).toInt();
    int b = list.at(2).toInt();
    return QColor(r,g,b);
}

QString MainWindow::stringFromColor(QColor c) const
{
    return QString("%1 %2 %3").arg(c.red()).arg(c.green()).arg(c.blue());
}
