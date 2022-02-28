#include "skiaitem.h"

#include <QtQuick/qquickwindow.h>
#include <QOpenGLContext>
#include <QtCore/QRunnable>

#include <core/SkCanvas.h>
#include <gpu/gl/GrGLInterface.h>

SkiaItem::SkiaItem()
    : m_t(0)
    , m_renderer(nullptr)
{
    connect(this, &QQuickItem::windowChanged, this, &SkiaItem::handleWindowChanged);
}

void SkiaItem::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &SkiaItem::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &SkiaItem::cleanup, Qt::DirectConnection);

        // Ensure we start with cleared to black.
        win->setColor(Qt::black);
    }
}

void SkiaItem::cleanup()
{
    delete m_renderer;
    m_renderer = nullptr;
}

class CleanupJob : public QRunnable
{
public:
    CleanupJob(SkiaRenderer *renderer) : m_renderer(renderer) { }
    void run() override { delete m_renderer; }
private:
    SkiaRenderer *m_renderer;
};

void SkiaItem::releaseResources()
{
    window()->scheduleRenderJob(new CleanupJob(m_renderer), QQuickWindow::BeforeSynchronizingStage);
    m_renderer = nullptr;
}

SkiaRenderer::~SkiaRenderer()
{
}

void SkiaItem::sync()
{
    if (!m_renderer) {
        m_renderer = new SkiaRenderer();
        connect(window(), &QQuickWindow::beforeRendering, m_renderer, &SkiaRenderer::init, Qt::DirectConnection);
        connect(window(), &QQuickWindow::beforeRenderPassRecording, m_renderer, &SkiaRenderer::paint, Qt::DirectConnection);
    }
    m_renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
    m_renderer->setT(m_t);
    m_renderer->setWindow(window());
}

sk_sp<SkSurface> SkiaRenderer::MakeSkiaSurface()
{
    sk_sp<SkSurface> surface = nullptr;
    GrGLFramebufferInfo info;
    info.fFBOID = 0;
    info.fFormat = GL_RGBA8;

    auto surfaceFormat = QOpenGLContext::currentContext()->format();

    int renderWidth = m_window->width();
    int renderHeight = m_window->height();

    GrBackendRenderTarget desc
            (renderWidth,
             renderHeight,
             surfaceFormat.samples(),
             surfaceFormat.stencilBufferSize(),
             info);
    sk_sp<SkColorSpace> srgb = SkColorSpace::MakeSRGB();
    surface = SkSurface::MakeFromBackendRenderTarget(m_skiaContext.get(),
                                                     desc,
                                                     kBottomLeft_GrSurfaceOrigin,
                                                     kRGBA_8888_SkColorType, srgb,
                                                     nullptr, nullptr);
    return surface;
}

void SkiaRenderer::init()
{
    GrContextOptions contextOptions;
    contextOptions.fDisableCoverageCountingPaths = false;
    contextOptions.fAvoidStencilBuffers = false;
    contextOptions.fInternalMultisampleCount = 0;

    if(!m_skiaContext)
        m_skiaContext = GrDirectContext::MakeGL(contextOptions);
    if(!m_skiaContext)
        qCritical() << "ERROR: UNABLE TO CREATE SKIA OPENGL CONTEXT";

    m_skiaContext->setResourceCacheLimit(8000000);
    m_skiaSurface = MakeSkiaSurface();
}

void SkiaRenderer::paint()
{
    // Play nice with the RHI. Not strictly needed when the scenegraph uses
    // OpenGL directly.
    m_window->beginExternalCommands();

    if(!m_skiaSurface){
        return;
    }
    if(m_skiaContext){
        m_skiaContext->resetContext();
    }
    auto canvas = m_skiaSurface->getCanvas();

    canvas->drawColor(SK_ColorWHITE);

    SkPaint paint;
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(8);
    paint.setColor(0xff4285F4);
    paint.setAntiAlias(true);
    paint.setStrokeCap(SkPaint::kRound_Cap);

    SkPath path;
    path.moveTo(10, 10);
    path.quadTo(256, 64, 128, 128);
    path.quadTo(10, 192, 250, 250);
    canvas->drawPath(path, paint);
    canvas->flush();

    m_window->resetOpenGLState();
    m_window->endExternalCommands();
}
