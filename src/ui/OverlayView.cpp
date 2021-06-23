#include "OverlayView.h"
#include "src/Context.h"

#include <QtGlobal>
#include <QScreen>

OverlayView::OverlayView() : movingItem_(false) {
    setWindowFlags(
        Qt::Popup |
//        Qt::BypassWindowManagerHint |
        Qt::CustomizeWindowHint |
        Qt::BypassGraphicsProxyWidget |
        Qt::WindowStaysOnTopHint |
//        Qt::X11BypassWindowManagerHint |
        Qt::WindowOverridesSystemGestures |
        Qt::MaximizeUsingFullscreenGeometryHint
    );

    setAttribute(Qt::WA_MouseTracking);
    setAttribute(Qt::WA_NoMousePropagation);
    setAttribute(Qt::WA_NoSystemBackground);

    setFocusPolicy(Qt::WheelFocus);
    setMouseTracking(true);

    setFrameShape(QFrame::NoFrame);
    setRenderHints(
        QPainter::Antialiasing |
        QPainter::TextAntialiasing
    );

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setScene(&scene);

    Context& ctx = Context::getInstance();

    connect(ctx.app, &App::screenshotFinished, this, [&]() {
        hide();
    });
}

void OverlayView::show() {
#if defined(Q_OS_MAC)
    Context::getInstance().trayIcon->hide();
#endif

    setCursor(Qt::BlankCursor);
    QGraphicsView::show();

    activateWindow();
    raise();
    setFocus(Qt::MouseFocusReason);

    emit(shown());
}

void OverlayView::hide() {
#if defined(Q_OS_MAC)
    Context::getInstance().trayIcon->show();
#endif

    QGraphicsView::hide();
    emit(hided());
}

void OverlayView::mousePressEvent(QMouseEvent* e) {
    Context& ctx = Context::getInstance();

    const int x = e->x();
    const int y = e->y();

    // If dragging
    const QGraphicsItem* item = scene.itemAt(x, y, QGraphicsView::transform());
    const QGraphicsItemGroup* group = item->group();

    if (item->flags().testFlag(QGraphicsItem::ItemIsMovable)
        || (group && group->flags().testFlag(QGraphicsItem::ItemIsMovable))
    ) {
        movingItem_ = true;
        QGraphicsView::mousePressEvent(e);
        ctx.overlayView->setCursor(Qt::BlankCursor);
        return;
    }

    if (ctx.itemManager->visibleAreaItem.isInnerArea(x, y)) {
        ctx.itemManager->init(e);
    } else {
        ctx.itemManager->visibleAreaItem.init(e);
    }
}

void OverlayView::mouseMoveEvent(QMouseEvent* e) {
    if (movingItem_) {
        QGraphicsView::mouseMoveEvent(e);
        return;
    }

    int x = e->x();
    int y = e->y();

    const QRect& geo = geometry();

    if (x < 0) {
        x = 0;
    }

    if (y < 0) {
        y = 0;
    }

    if (x >= geo.width()) {
        x = geo.width() - 1;
    }

    if (y >= geo.height()) {
        y = geo.height() - 1;
    }

    const Context& ctx = Context::getInstance();

    const QGraphicsItem* item = scene.itemAt(x, y, QGraphicsView::transform());
    const QGraphicsItemGroup* group = item->group();

    if (item->flags().testFlag(QGraphicsItem::ItemIsMovable) || (group && group->flags().testFlag(QGraphicsItem::ItemIsMovable))) {
        ctx.overlayView->setCursor(Qt::OpenHandCursor);
    } else {
        ctx.itemManager->move(e);
    }
}

void OverlayView::mouseReleaseEvent(QMouseEvent* e) {
    if (movingItem_) {
        movingItem_ = false;
        QGraphicsView::mouseReleaseEvent(e);
        return;
    }

    Context::getInstance().itemManager->stop(e);
}

void OverlayView::keyReleaseEvent(QKeyEvent* e) {
    int key = e->key();

    switch (key) {
        case Qt::Key_Escape:
            emit escapeClicked();
            break;

        case Qt::Key_Return:
            emit enterClicked();
            break;
    }

    QWidget::keyReleaseEvent(e);
}

void OverlayView::wheelEvent(QWheelEvent* e) {
    Context& ctx = Context::getInstance();

    if (e->angleDelta().y() < 0) {
        ctx.toolbar->selectNext();
    } else {
        ctx.toolbar->selectPrevious();
    }
}
