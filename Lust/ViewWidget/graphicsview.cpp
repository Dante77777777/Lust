#include "graphicsview.h"

#define ZOOMMAX 50
#define ZOOMMIN 0.1


GraphicsView::GraphicsView(QWidget* parent)
    : QGraphicsView(parent)
    , GVscene(new QGraphicsScene(this))
    , GVimageItem(new ImageItem(this))
    , imageInfoWidget(new QWidget(this))
    , imageInfoLabel(new QLabel(this))
{
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->setRenderHint(QPainter::Antialiasing);
    this->setTransformationAnchor(QGraphicsView::AnchorViewCenter);

    this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setDragMode(QGraphicsView::ScrollHandDrag);
    this->setSceneRect(INT_MIN / 2, INT_MIN / 2, INT_MAX, INT_MAX);
    prepareBackGroundBoard();
    centerOn(0, 0);

    if (false == setupWidget()) {
        throw std::bad_alloc();
    }
}

GraphicsView::~GraphicsView()
{
    GVscene->deleteLater();
    delete GVimageItem;
}

bool GraphicsView::setupWidget()
{
    this->setScene(GVscene);
    GVscene->addItem(GVimageItem);
    imageInfoLabel->setStyleSheet("color:rgb(200,255,200); "
                                   "background-color:rgba(50,50,50,160); "
                                   "font: Microsoft YaHei;"
                                   "font-size: 15px;");
    imageInfoLabel->setText(" W:0,H:0 | X:0,Y:0 | R:0,G:0,B:0");

    imageInfoWidget->setFixedHeight(25);
    imageInfoWidget->setGeometry(0,this->height()-25,this->height(),25);
    imageInfoWidget->setStyleSheet("background-color:rgba(0,0,0,0)");
    QHBoxLayout* infoLayout = new QHBoxLayout();
    infoLayout->setContentsMargins(0,0,0,0);
    infoLayout->setSpacing(0);
    infoLayout->addWidget(imageInfoLabel);
    imageInfoWidget->setLayout(infoLayout);
    connect(GVimageItem,&ImageItem::imageInfo,this,[this](QString info){
        imageInfoLabel->setText(info);
    });
    return true;
}

void GraphicsView::setImage(const QImage& image)
{
    static QMutex mutex;
    QMutexLocker<QMutex> lock(&mutex);
    GVimage = image.copy();
    auto GVpixmap = QPixmap::fromImage(GVimage);
    GVimageItem->w = GVpixmap.width();
    GVimageItem->h = GVpixmap.height();
    GVimageItem->setPixmap(GVpixmap);
    fitFrame();
    onCenter();
    show();
}

void GraphicsView::clear()
{
    imageInfoLabel->setText(" W:0,H:0 | X:0,Y:0 | R:0,G:0,B:0");
    setImage(QImage());
}

void GraphicsView::wheelEvent(QWheelEvent* event)
{
    QPoint scrollAmount = event->angleDelta();
    if((scrollAmount.y() > 0) && (zoomValue >= ZOOMMAX))
    {
        return ;
    }
    if((scrollAmount.y() < 0) && (zoomValue <= ZOOMMIN))
    {
        return ;
    }
    scrollAmount.y() > 0 ? onZoom(1.1) : onZoom(0.9);
}

void GraphicsView::mouseDoubleClickEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        fitFrame();
        onCenter();
    }
    QGraphicsView::mouseDoubleClickEvent(event);
}

void GraphicsView::resizeEvent(QResizeEvent* event)
{
    fitFrame();
    onCenter();
    imageInfoWidget->setGeometry(0,this->height()-25,this->height(),25);
    QGraphicsView::resizeEvent(event);
}

void GraphicsView::paintEvent(QPaintEvent* event)
{
    QPainter painter(this->viewport());
    painter.drawTiledPixmap(QRect(QPoint(0,0),QPoint(this->width(),this->height())),backGroundPixmap);
    QGraphicsView::paintEvent(event);
}

void GraphicsView::onCenter()
{
    this->centerOn(GVimageItem->pixmap().width()/2,GVimageItem->pixmap().height()/2);
    GVimageItem->setPos(0,0);
}

void GraphicsView::onZoom(double scaleFactor)
{
    zoomValue *= scaleFactor;
    this->scale(scaleFactor,scaleFactor);
}

void GraphicsView::fitFrame()
{
    //QMutexLocker<QMutex> lock(&mutex);

    if(this->width() < 1 || GVimage.width() < 1)
    {
        return ;
    }
    double winWidth = this->width();
    double winHeight = this->height();
    double widthScale = (GVimage.width()+1)/winWidth;
    double heightScale = (GVimage.height()+1)/winHeight;

    double minScale = widthScale >= heightScale ? 1/widthScale : 1/heightScale;

    double finalScale = minScale/zoomValue;
    onZoom(finalScale);
    zoomValue = minScale;
}

void GraphicsView::prepareBackGroundBoard(bool invertColor)
{
    backGroundPixmap.fill(invertColor ? QColor(220,220,200) : QColor(35,35,35));
    QPainter backgroundPainter(&backGroundPixmap);
    constexpr QColor color(50, 50, 50, 255);
    constexpr QColor invertedColor(210, 210, 210, 255);
    backgroundPainter.fillRect(0, 0, 18, 18, invertColor ? invertedColor : color);
    backgroundPainter.fillRect(18, 18, 18, 18, invertColor ? invertedColor : color);
    backgroundPainter.end();
}









