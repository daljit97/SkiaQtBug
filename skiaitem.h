#ifndef SQUIRCLE_H
#define SQUIRCLE_H

#include <QtQuick/QQuickItem>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions>

#include <gpu/GrDirectContext.h>
#include <core/SkSurface.h>

class SkiaRenderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    SkiaRenderer() : m_t(0) { }
    ~SkiaRenderer();

    void setT(qreal t) { m_t = t; }
    void setViewportSize(const QSize &size) { m_viewportSize = size; }
    void setWindow(QQuickWindow *window) { m_window = window; }

public slots:
    void init();
    void paint();

private:
    sk_sp<SkSurface> MakeSkiaSurface();
    QSize m_viewportSize;
    qreal m_t;
    QQuickWindow *m_window;

    sk_sp<GrDirectContext> m_skiaContext;
    sk_sp<SkSurface> m_skiaSurface;
};

class SkiaItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
public:
    SkiaItem();

    qreal t() const { return m_t; }
    void setT(qreal t);

signals:
    void tChanged();

public slots:
    void sync();
    void cleanup();

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    void releaseResources() override;

    qreal m_t;
    SkiaRenderer *m_renderer;
};

#endif // SQUIRCLE_H
