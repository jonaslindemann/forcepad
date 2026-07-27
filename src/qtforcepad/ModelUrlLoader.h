#pragma once

#include <QStringList>
#include <QUrl>

class QtPaintView;

namespace fp {

/// The model requested at startup, or an empty QUrl if none was.
///
/// WebAssembly: the "model" parameter of the page's query string, resolved
/// against the page URL, so both of these work:
///     .../app/?model=models/beam.fp2
///     .../app/?model=https://example.org/beam.fp2
///
/// Desktop: the first positional argument that is an http/https URL. A
/// positional argument that is a plain path is left to the caller's existing
/// local-file handling.
QUrl startupModelUrl(const QStringList &arguments);

/// Downloads url and opens it in view. Asynchronous on every platform: the
/// model appears once the transfer finishes, failures are reported through the
/// view's info overlay and the log.
void openModelFromUrl(QtPaintView *view, const QUrl &url);

} // namespace fp
