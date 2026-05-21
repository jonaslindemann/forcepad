#pragma once
#include <spdlog/sinks/base_sink.h>
#include <mutex>
#include <QPlainTextEdit>
#include <QMetaObject>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QColor>
#include <QString>

// Thread-safe spdlog sink that appends formatted messages to a QPlainTextEdit.
// Dispatches to the Qt main thread via QueuedConnection so it is safe to call
// from background threads (e.g. the FEM solver).
class QtLogSink : public spdlog::sinks::base_sink<std::mutex>
{
public:
    explicit QtLogSink(QPlainTextEdit *view) : m_view(view) {}

protected:
    void sink_it_(const spdlog::details::log_msg &msg) override
    {
        spdlog::memory_buf_t buf;
        formatter_->format(msg, buf);
        QString text = QString::fromUtf8(buf.data(), static_cast<int>(buf.size())).trimmed();

        auto level = msg.level;
        QMetaObject::invokeMethod(m_view, [view = m_view, text, level]() {
            QTextCharFormat fmt;
            switch (level) {
                case spdlog::level::debug:
                    fmt.setForeground(QColor("#737373"));
                    break;
                case spdlog::level::warn:
                    fmt.setForeground(QColor("#d97706"));
                    break;
                case spdlog::level::err:
                case spdlog::level::critical:
                    fmt.setForeground(QColor("#ef4444"));
                    break;
                default:
                    fmt.setForeground(QColor("#d4d4d4"));
                    break;
            }
            QTextCursor cursor = view->textCursor();
            cursor.movePosition(QTextCursor::End);
            cursor.insertText(text + "\n", fmt);
            view->setTextCursor(cursor);
            view->ensureCursorVisible();
        }, Qt::QueuedConnection);
    }

    void flush_() override {}

private:
    QPlainTextEdit *m_view{nullptr};
};
