#include "ModelUrlLoader.h"

#include "qtpaintview.h"
#include "FPLog.h"

#include <QFileInfo>
#include <QUrlQuery>

#ifdef Q_OS_WASM
#include <emscripten.h>
#else
#include <QDir>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <string>
#endif

namespace {

// Query parameter carrying the model to open: ?model=<relative-or-absolute-url>
const char *const MODEL_QUERY_KEY = "model";

bool isSupportedScheme(const QUrl &url)
{
    const QString scheme = url.scheme().toLower();
    return scheme == "http" || scheme == "https";
}

/// Base name to show in the UI and to use for the staged local copy.
QString modelFileName(const QUrl &url)
{
    const QString name = QFileInfo(url.path()).fileName();
    return name.isEmpty() ? QStringLiteral("model.fp2") : name;
}

void reportFailure(QtPaintView *view, const QUrl &url, const QString &reason)
{
    fp_error("ModelUrlLoader", "could not load {}: {}",
             url.toString().toStdString(), reason.toStdString());
    if (view != nullptr)
        view->showInfoOverlay(QStringLiteral("Could not load %1 — %2")
                                  .arg(modelFileName(url), reason));
}

void openStagedModel(QtPaintView *view, const QUrl &url, const std::string &localPath)
{
    view->openModel(localPath);                          // path-based loader
    view->setModelName(modelFileName(url).toStdString()); // not the staged copy
    fp_info("ModelUrlLoader", "loaded model from {}", url.toString().toStdString());
}

#ifdef Q_OS_WASM

// Kept alive between the request and its callback.
struct Download
{
    QtPaintView *view;
    QUrl url;
    std::string localPath;
};

void onDownloadLoaded(unsigned /*handle*/, void *arg, const char * /*file*/)
{
    Download *download = static_cast<Download *>(arg);
    openStagedModel(download->view, download->url, download->localPath);
    delete download;
}

void onDownloadFailed(unsigned /*handle*/, void *arg, int httpStatus)
{
    Download *download = static_cast<Download *>(arg);
    // A cross-origin request the browser refuses never reaches the server, so
    // the XHR reports status 0 rather than an HTTP code.
    const QString reason = httpStatus == 0
        ? QStringLiteral("download blocked (missing CORS headers?) or host unreachable")
        : QStringLiteral("HTTP %1").arg(httpStatus);
    reportFailure(download->view, download->url, reason);
    delete download;
}

#endif

} // namespace

QUrl fp::startupModelUrl(const QStringList &arguments)
{
#ifdef Q_OS_WASM
    Q_UNUSED(arguments);

    // The page URL is the only "command line" a browser hands us. Read it with
    // emscripten_run_script_string rather than EM_ASM + stringToNewUTF8: the
    // latter needs a runtime helper that the EM_ASM path does not pull in
    // automatically, and would fail at run time with "not defined".
    const QUrl pageUrl(QString::fromUtf8(emscripten_run_script_string("window.location.href")));
    const QString requested =
        QUrlQuery(pageUrl).queryItemValue(MODEL_QUERY_KEY, QUrl::FullyDecoded);
    if (requested.isEmpty())
        return {};

    // Resolves relative values against the page and passes absolute ones through.
    return pageUrl.resolved(QUrl(requested));
#else
    for (int i = 1; i < arguments.size(); ++i)
    {
        const QString &argument = arguments.at(i);
        if (argument.startsWith('-'))
            continue;

        const QUrl url(argument);
        return isSupportedScheme(url) ? url : QUrl();  // first positional wins
    }

    return {};
#endif
}

void fp::openModelFromUrl(QtPaintView *view, const QUrl &url)
{
    if (view == nullptr || url.isEmpty())
        return;

    if (!isSupportedScheme(url))
    {
        reportFailure(view, url, QStringLiteral("unsupported URL scheme '%1'").arg(url.scheme()));
        return;
    }

    view->showInfoOverlay(QStringLiteral("Loading %1 …").arg(modelFileName(url)));

#ifdef Q_OS_WASM
    // Stage the bytes in MEMFS so the existing path/fstream-based loader works
    // unchanged - the same trick doPickFile() uses for browser file dialogs.
    // The transfer is an XHR, so cross-origin models need CORS headers on the
    // host serving them; same-origin (relative) URLs always work.
    Download *download = new Download{view, url, "/tmp/" + modelFileName(url).toStdString()};
    emscripten_async_wget2(url.toString().toUtf8().constData(),
                           download->localPath.c_str(),
                           "GET", "", download,
                           &onDownloadLoaded, &onDownloadFailed, nullptr);
#else
    QNetworkAccessManager *manager = new QNetworkAccessManager(view);
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);

    QNetworkReply *reply = manager->get(request);
    QObject::connect(reply, &QNetworkReply::finished, view, [view, url, reply, manager]() {
        reply->deleteLater();
        manager->deleteLater();

        if (reply->error() != QNetworkReply::NoError)
        {
            reportFailure(view, url, reply->errorString());
            return;
        }

        const QString localPath = QDir(QDir::tempPath()).filePath(modelFileName(url));
        QFile staged(localPath);
        if (!staged.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            reportFailure(view, url, staged.errorString());
            return;
        }
        staged.write(reply->readAll());
        staged.close();

        openStagedModel(view, url, localPath.toStdString());
    });
#endif
}
