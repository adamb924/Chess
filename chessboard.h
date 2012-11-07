#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QGraphicsScene>

class QAction;
class QActionGroup;

class Piece
{
public:
    enum Type { King, Queen, Bishop, Knight, Rook, Pawn, None };
    enum Color { White, Black };

    Piece() { eType = None; eColor = White; }
    Piece(Type t, Color c) { eType = t; eColor = c; }

    bool hasSecularVariant() const { if( eType == King || eType == Bishop ) return true; else return false; }

    Type type() const { return eType; }
    Color color() const { return eColor; }
    void setType(Type t) { eType = t; }
    void setColor(Color c) { eColor = c; }

private:
    Type eType;
    Color eColor;
};

class ChessBoard : public QGraphicsScene
{
    Q_OBJECT
public:
    enum Version { Traditional, Secular };

    explicit ChessBoard(QObject *parent = 0);

    QString toString() const;
    void fromString(QString s);

    inline Version version() const { return eVersion; }

    inline QColor lightSquareColor() const { return cLightSquareColor; }
    inline QColor darkSquareColor() const { return cDarkSquareColor; }
    inline QColor lightPieceColor() const { return cLightPieceColor; }
    inline QColor darkPieceColor() const { return cDarkPieceColor; }

    inline void setLightSquareColor(QColor c) { cLightSquareColor = c; redrawEntireBoard(); }
    inline void setDarkSquareColor(QColor c) { cDarkSquareColor = c; redrawEntireBoard(); }
    inline void setLightPieceColor(QColor c) { cLightPieceColor = c; refreshBoard(); }
    inline void setDarkPieceColor(QColor c) { cDarkPieceColor = c; refreshBoard(); }

    inline void setSvgRender(bool v) { bSvgRender = v; refreshBoard(); }

signals:

public slots:
    void setItem(int i, int j, Piece p);
    void setVersion(quint32 v);
    void setVersion(QAction *action);
    void clearBoard();

    void setInitialPositions();
    void setDefaultColors();

private:

    bool bSvgRender;

    QActionGroup *changePiece;
    QAction* pieceMenuAction( const QString& label , Piece::Type t, Piece::Color c);

    void refreshImage(int i, int j);
    void refreshBoard();
    void redrawEntireBoard();

    void drawBoard();
    QString getPieceFilename(Piece p);

    Piece board[8][8];

    void contextMenuEvent ( QGraphicsSceneContextMenuEvent * contextMenuEvent );

    quint8 rowFromPoint(int y) const { return y / nPieceWidth; }
    quint8 colFromPoint(int x) const { return x / nPieceWidth; }

    quint32 xFromCol(int c) const { return c*nPieceWidth + 0.5*nPieceWidth; }
    quint32 yFromRow(int r) const { return r*nPieceWidth + 0.5*nPieceWidth; }

    QColor cLightSquareColor;
    QColor cDarkSquareColor;
    QColor cLightPieceColor;
    QColor cDarkPieceColor;

    quint32 nPieceWidth;
    quint32 nBorderWidth;

    qint8 focusRow, focusCol;

    Version eVersion;

private slots:
    void changePieceType(QAction *action);
    void toggleColor();
};

#endif // CHESSBOARD_H
