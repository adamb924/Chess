#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

class ChessBoard;
class QSettings;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();


private:
    ChessBoard *scene;
    QSettings *settings;

    void getSettings();
    void setSettings();
    void setupMenus();

    QColor colorFromString(QString s) const;
    QString stringFromColor(QColor c) const;

    QAction *traditional, *secularized;

private slots:
    void save();
    void open();
    void createSvg();

    void setLightSquareColor();
    void setDarkSquareColor();
    void setLightPieceColor();
    void setDarkPieceColor();
};

#endif // MAINWINDOW_H
