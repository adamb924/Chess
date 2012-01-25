#include "chessboard.h"

#include <QtGui>
#include <QGraphicsSvgItem>

ChessBoard::ChessBoard(QObject *parent) :
    QGraphicsScene(parent)
{
    bSvgRender = false;

    nPieceWidth = 45;
    nBorderWidth = 0;
    eVersion = Western;

    setDefaultColors(); // also draws the board
}

void ChessBoard::setDefaultColors()
{
    cLightPieceColor = Qt::black;
    cDarkPieceColor = Qt::black;
    cLightSquareColor = Qt::white;
    cDarkSquareColor = Qt::gray;
    redrawEntireBoard();
}

void ChessBoard::drawBoard()
{
    for(int i=0; i<8; i++)
    {
        for(int j=0; j<8; j++)
        {
            QGraphicsRectItem *rect = new QGraphicsRectItem( j * nPieceWidth , i * nPieceWidth , nPieceWidth , nPieceWidth );
            if( i % 2 == j % 2 )
                rect->setBrush( QBrush(cLightSquareColor, Qt::SolidPattern) );
            else
                rect->setBrush( QBrush(cDarkSquareColor, Qt::SolidPattern) );
            rect->setCacheMode(QGraphicsItem::NoCache);
            addItem(rect);
        }
    }
}

void ChessBoard::setItem(int i, int j, Piece p)
{
    board[i][j] = p;
    refreshImage(i,j);
}

void ChessBoard::refreshBoard()
{
    for(int i=0; i<8; i++)
        for(int j=0; j<8; j++)
            refreshImage(i,j);
}

void ChessBoard::redrawEntireBoard()
{
    qDeleteAll( items() );
    drawBoard();
    refreshBoard();
}

void ChessBoard::clearBoard()
{
    for(int i=0; i<8; i++)
        for(int j=0; j<8; j++)
            setItem(i,j,Piece());
}

void ChessBoard::refreshImage(int i, int j)
{
    QGraphicsItem *currentItem = itemAt( xFromCol(j) , yFromRow(i) );
    if( currentItem != 0 && currentItem->data(0) == 777 )
        delete currentItem;

    QString filename = getPieceFilename( board[i][j] );
    if(filename.isEmpty())
        return;

    quint32 y = nPieceWidth * i;
    quint32 x = nPieceWidth * j;
    QGraphicsSvgItem *item = new QGraphicsSvgItem( filename );

    if(!bSvgRender)
    {
        QGraphicsColorizeEffect *colorize = new QGraphicsColorizeEffect;
        if( board[i][j].color() == Piece::White )
            colorize->setColor(cLightPieceColor);
        else
            colorize->setColor(cDarkPieceColor);
        item->setGraphicsEffect( colorize );
    }

    item->setCacheMode(QGraphicsItem::NoCache); // needed for proper rendering
    item->setData(0, 777 );

    addItem(item);
    item->setPos(x,y);
}

QString ChessBoard::getPieceFilename(Piece p)
{
    if( p.type() == Piece::None )
        return "";

    QString filename = ":/resources/";
    switch(p.color())
    {
    case Piece::White:
        filename += "white";
        break;
    case Piece::Black:
    default:
        filename += "black";
        break;
    }

    switch(p.type())
    {
    case Piece::King:
        filename += "-king";
        break;
    case Piece::Queen:
        filename += "-queen";
        break;
    case Piece::Bishop:
        filename += "-bishop";
        break;
    case Piece::Knight:
        filename += "-knight";
        break;
    case Piece::Rook:
        filename += "-rook";
        break;
    case Piece::Pawn:
        filename += "-pawn";
        break;
    default:
        break;
    }

    if(eVersion == Secular && p.hasSecularVariant() )
        filename += "-secular";

    return filename + ".svg";
}

void ChessBoard::contextMenuEvent ( QGraphicsSceneContextMenuEvent * contextMenuEvent )
{
    QPointF scenePos = contextMenuEvent->scenePos();
    if( scenePos.x() < 0 || scenePos.y() < 0 || scenePos.x() > 8*nPieceWidth || scenePos.y() > 8*nPieceWidth )
    {
        focusRow = -1;
        focusCol = -1;
        return;
    }

    focusRow = rowFromPoint( scenePos.y() );
    focusCol = colFromPoint( scenePos.x() );

    QMenu menu;

    changePiece = new QActionGroup(this);

    if( board[focusRow][focusCol].type() != Piece::None )
    {
        QAction *clear = menu.addAction(tr("Clear space"));
        clear->setData( QString("%1 %2").arg(Piece::None).arg(Piece::White) );
        changePiece->addAction(clear);
        menu.addAction(tr("Change color"),this,SLOT(toggleColor()));
        menu.addSeparator();
    }

    menu.addAction( pieceMenuAction( tr("King") , Piece::King, Piece::White ) );
    menu.addAction( pieceMenuAction( tr("Queen") , Piece::Queen, Piece::White ) );
    menu.addAction( pieceMenuAction( tr("Bishop") , Piece::Bishop, Piece::White ) );
    menu.addAction( pieceMenuAction( tr("Knight") , Piece::Knight, Piece::White ) );
    menu.addAction( pieceMenuAction( tr("Rook") , Piece::Rook, Piece::White ) );
    menu.addAction( pieceMenuAction( tr("Pawn") , Piece::Pawn, Piece::White ) );
    menu.addSeparator();
    menu.addAction( pieceMenuAction( tr("King") , Piece::King, Piece::Black ) );
    menu.addAction( pieceMenuAction( tr("Queen") , Piece::Queen, Piece::Black ) );
    menu.addAction( pieceMenuAction( tr("Bishop") , Piece::Bishop, Piece::Black ) );
    menu.addAction( pieceMenuAction( tr("Knight") , Piece::Knight, Piece::Black ) );
    menu.addAction( pieceMenuAction( tr("Rook") , Piece::Rook, Piece::Black ) );
    menu.addAction( pieceMenuAction( tr("Pawn") , Piece::Pawn, Piece::Black ) );

    connect(changePiece,SIGNAL(triggered(QAction*)),this,SLOT(changePieceType(QAction*)));

    menu.exec(contextMenuEvent->screenPos());
}

QAction* ChessBoard::pieceMenuAction( const QString& label , Piece::Type t, Piece::Color c)
{
    QAction *tmp = new QAction(QIcon(getPieceFilename(Piece(t,c))),label,this);
    tmp->setData(QString("%1 %2").arg(t).arg(c));
    changePiece->addAction(tmp);
    return tmp;
}

void ChessBoard::setVersion(quint32 v)
{
    eVersion = (Version)v;
    refreshBoard();
}

void ChessBoard::setVersion(QAction *action)
{
    setVersion(action->data().toUInt());
}

void ChessBoard::changePieceType(QAction *action)
{
    QStringList values = action->data().toString().split(" ");
    if(values.count() != 2)
        return;
    Piece::Type t = (Piece::Type)values.at(0).toUInt();
    Piece::Color c = (Piece::Color)values.at(1).toUInt();
    board[focusRow][focusCol] = Piece(t,c);
    refreshImage(focusRow,focusCol);
}

void ChessBoard::toggleColor()
{
    if(board[focusRow][focusCol].color() == Piece::White )
        board[focusRow][focusCol].setColor( Piece::Black );
    else if(board[focusRow][focusCol].color() == Piece::Black )
        board[focusRow][focusCol].setColor( Piece::White );
    refreshImage(focusRow,focusCol);
}

QString ChessBoard::toString() const
{
    QString result;
    for(int i=0; i<8; i++)
    {
        for(int j=0; j<8; j++)
        {
            if( board[i][j].color() == Piece::White )
                result += "W";
            else
                result += "B";
            switch(board[i][j].type())
            {
            case Piece::King:
                result += "K ";
                break;
            case Piece::Queen:
                result += "Q ";
                break;
            case Piece::Bishop:
                result += "B ";
                break;
            case Piece::Knight:
                result += "K ";
                break;
            case Piece::Rook:
                result += "R ";
                break;
            case Piece::Pawn:
                result += "P ";
                break;
            case Piece::None:
                result += "N ";
                break;
            }
        }
    }
    return result.trimmed();
}

void ChessBoard::fromString(QString s)
{
    clearBoard();
    QStringList list = s.trimmed().split(" ");
    if(list.count() < 64)
        return;

    int pos = 0;
    for(int i=0; i<8; i++)
    {
        for(int j=0; j<8; j++)
        {
            Piece::Color c;
            Piece::Type t;

            if( list.at(pos).at(0) == 'B' )
                c = Piece::Black;
            else
                c = Piece::White;

            if( list.at(pos).at(1) == 'K' )
                t = Piece::King;
            else if( list.at(pos).at(1) == 'Q' )
                t = Piece::Queen;
            else if( list.at(pos).at(1) == 'B' )
                t = Piece::Bishop;
            else if( list.at(pos).at(1) == 'K' )
                t = Piece::Knight;
            else if( list.at(pos).at(1) == 'R' )
                t = Piece::Rook;
            else if( list.at(pos).at(1) == 'P' )
                t = Piece::Pawn;
            else if( list.at(pos).at(1) == 'N' )
                t = Piece::None;

            board[i][j] = Piece(t,c);

            pos++;
        }
    }
    refreshBoard();
}

void ChessBoard::setInitialPositions()
{
    clearBoard();
    setItem(0, 0, Piece(Piece::Rook,Piece::Black));
    setItem(0, 1, Piece(Piece::Knight,Piece::Black));
    setItem(0, 2, Piece(Piece::Bishop,Piece::Black));
    setItem(0, 3, Piece(Piece::Queen,Piece::Black));
    setItem(0, 4, Piece(Piece::King,Piece::Black));
    setItem(0, 5, Piece(Piece::Bishop,Piece::Black));
    setItem(0, 6, Piece(Piece::Knight,Piece::Black));
    setItem(0, 7, Piece(Piece::Rook,Piece::Black));
    setItem(1, 0, Piece(Piece::Pawn,Piece::Black));
    setItem(1, 1, Piece(Piece::Pawn,Piece::Black));
    setItem(1, 2, Piece(Piece::Pawn,Piece::Black));
    setItem(1, 3, Piece(Piece::Pawn,Piece::Black));
    setItem(1, 4, Piece(Piece::Pawn,Piece::Black));
    setItem(1, 5, Piece(Piece::Pawn,Piece::Black));
    setItem(1, 6, Piece(Piece::Pawn,Piece::Black));
    setItem(1, 7, Piece(Piece::Pawn,Piece::Black));

    setItem(7, 0, Piece(Piece::Rook,Piece::White));
    setItem(7, 1, Piece(Piece::Knight,Piece::White));
    setItem(7, 2, Piece(Piece::Bishop,Piece::White));
    setItem(7, 3, Piece(Piece::Queen,Piece::White));
    setItem(7, 4, Piece(Piece::King,Piece::White));
    setItem(7, 5, Piece(Piece::Bishop,Piece::White));
    setItem(7, 6, Piece(Piece::Knight,Piece::White));
    setItem(7, 7, Piece(Piece::Rook,Piece::White));
    setItem(6, 0, Piece(Piece::Pawn,Piece::White));
    setItem(6, 1, Piece(Piece::Pawn,Piece::White));
    setItem(6, 2, Piece(Piece::Pawn,Piece::White));
    setItem(6, 3, Piece(Piece::Pawn,Piece::White));
    setItem(6, 4, Piece(Piece::Pawn,Piece::White));
    setItem(6, 5, Piece(Piece::Pawn,Piece::White));
    setItem(6, 6, Piece(Piece::Pawn,Piece::White));
    setItem(6, 7, Piece(Piece::Pawn,Piece::White));
}
