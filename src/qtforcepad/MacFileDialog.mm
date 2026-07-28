#include "MacFileDialog.h"

#include <QFileInfo>
#include <QDir>
#include <QRegularExpression>
#include <QStringList>
#include <QWidget>

#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

namespace {

NSString *toNSString(const QString &value)
{
    return [NSString stringWithUTF8String:value.toUtf8().constData()];
}

QString fromNSString(NSString *value)
{
    if (value == nil)
        return {};
    return QString::fromUtf8([value UTF8String]);
}

QString fromNSError(NSError *error)
{
    if (error == nil)
        return {};
    return fromNSString(error.localizedDescription);
}

QByteArray fromNSData(NSData *data)
{
    if (data == nil)
        return {};
    return QByteArray(static_cast<const char *>(data.bytes),
                      static_cast<qsizetype>(data.length));
}

NSData *toNSData(const QByteArray &data)
{
    return [NSData dataWithBytes:data.constData()
                          length:static_cast<NSUInteger>(data.size())];
}

NSArray<UTType *> *allowedContentTypes(const QString &nameFilter)
{
    QStringList extensions;
    QRegularExpression re("\\*\\.([A-Za-z0-9]+)");
    QRegularExpressionMatchIterator it = re.globalMatch(nameFilter);
    while (it.hasNext())
    {
        const QString ext = it.next().captured(1).toLower();
        if (ext != "*" && !extensions.contains(ext))
            extensions.append(ext);
    }

    if (extensions.isEmpty())
        return nil;

    NSMutableArray<UTType *> *types = [NSMutableArray arrayWithCapacity:extensions.size()];
    for (const QString &ext : extensions)
    {
        UTType *type = [UTType typeWithFilenameExtension:toNSString(ext)];
        if (type != nil)
            [types addObject:type];
    }
    return types.count > 0 ? types : nil;
}

QByteArray securityScopedBookmark(NSURL *url)
{
    if (url == nil)
        return {};

    NSError *error = nil;
    NSData *bookmark = [url bookmarkDataWithOptions:NSURLBookmarkCreationWithSecurityScope
                    includingResourceValuesForKeys:nil
                                     relativeToURL:nil
                                             error:&error];
    return fromNSData(bookmark);
}

NSURL *resolvedUrl(const QString &path, const QByteArray &bookmark)
{
    if (!bookmark.isEmpty())
    {
        NSData *bookmarkData = toNSData(bookmark);
        BOOL stale = NO;
        NSError *error = nil;
        NSURL *url = [NSURL URLByResolvingBookmarkData:bookmarkData
                                               options:NSURLBookmarkResolutionWithSecurityScope
                                         relativeToURL:nil
                                   bookmarkDataIsStale:&stale
                                                 error:&error];
        if (url != nil)
            return url;
    }

    return [NSURL fileURLWithPath:toNSString(path)];
}

void configureDirectoryAndName(NSSavePanel *panel, const QString &defaultName)
{
    QFileInfo info(defaultName);
    QString name = info.fileName();
    if (name.isEmpty() || name == "noname.fp2")
        name = "model.fp2";
    panel.nameFieldStringValue = toNSString(name);

    if (info.exists() || info.dir().exists())
    {
        const QString dir = info.exists() ? info.absolutePath() : info.path();
        if (!dir.isEmpty() && dir != ".")
            panel.directoryURL = [NSURL fileURLWithPath:toNSString(dir)];
    }
}

} // namespace

namespace fp::macos {

PickedFile pickFile(QWidget *parent, const QString &title, const QString &nameFilter)
{
    Q_UNUSED(parent);

    @autoreleasepool {
        NSOpenPanel *panel = [NSOpenPanel openPanel];
        panel.title = toNSString(title);
        panel.canChooseFiles = YES;
        panel.canChooseDirectories = NO;
        panel.allowsMultipleSelection = NO;
        panel.allowedContentTypes = allowedContentTypes(nameFilter);

        if ([panel runModal] != NSModalResponseOK)
        {
            PickedFile result;
            result.cancelled = true;
            return result;
        }

        NSURL *url = panel.URL;
        if (url == nil)
        {
            PickedFile result;
            result.cancelled = true;
            return result;
        }

        const BOOL scoped = [url startAccessingSecurityScopedResource];
        NSError *error = nil;
        NSData *data = [NSData dataWithContentsOfURL:url options:0 error:&error];
        QByteArray bookmark = securityScopedBookmark(url);
        if (scoped)
            [url stopAccessingSecurityScopedResource];

        if (data == nil)
        {
            PickedFile result;
            result.path = fromNSString(url.path);
            result.error = fromNSError(error);
            return result;
        }

        const QString path = fromNSString(url.path);
        PickedFile result;
        result.path = path;
        result.displayName = path;
        result.content = fromNSData(data);
        result.bookmark = bookmark;
        return result;
    }
}

SavedFile saveFile(QWidget *parent, const QString &title, const QString &defaultName,
                   const QString &nameFilter, const QByteArray &content)
{
    Q_UNUSED(parent);

    @autoreleasepool {
        NSSavePanel *panel = [NSSavePanel savePanel];
        panel.title = toNSString(title);
        panel.allowedContentTypes = allowedContentTypes(nameFilter);
        configureDirectoryAndName(panel, defaultName);

        if ([panel runModal] != NSModalResponseOK)
        {
            SavedFile result;
            result.cancelled = true;
            return result;
        }

        NSURL *url = panel.URL;
        if (url == nil)
        {
            SavedFile result;
            result.cancelled = true;
            return result;
        }

        const BOOL scoped = [url startAccessingSecurityScopedResource];
        NSError *error = nil;
        const BOOL ok = [toNSData(content) writeToURL:url
                                              options:0
                                                error:&error];
        QByteArray bookmark = ok ? securityScopedBookmark(url) : QByteArray();
        if (scoped)
            [url stopAccessingSecurityScopedResource];

        if (!ok)
        {
            SavedFile result;
            result.path = fromNSString(url.path);
            result.error = fromNSError(error);
            return result;
        }

        SavedFile result;
        result.path = fromNSString(url.path);
        result.bookmark = bookmark;
        return result;
    }
}

bool writeFile(const QString &path, const QByteArray &bookmark,
               const QByteArray &content, QString *errorMessage)
{
    @autoreleasepool {
        NSURL *url = resolvedUrl(path, bookmark);
        const BOOL scoped = [url startAccessingSecurityScopedResource];
        NSError *error = nil;
        const BOOL ok = [toNSData(content) writeToURL:url
                                              options:0
                                                error:&error];
        if (scoped)
            [url stopAccessingSecurityScopedResource];

        if (!ok && errorMessage != nullptr)
            *errorMessage = fromNSError(error);
        return ok;
    }
}

} // namespace fp::macos
