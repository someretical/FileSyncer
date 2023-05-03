#include <QApplication>
#include <QStyleFactory>

#include "MainWindow.h"

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    if (!MainWindow::s_console) {
        QByteArray localMsg = msg.toLocal8Bit();
        switch (type) {
            case QtDebugMsg:
                fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line,
                        context.function);
                break;
            case QtInfoMsg:
                fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line,
                        context.function);
                break;
            case QtWarningMsg:
                fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line,
                        context.function);
                break;
            case QtCriticalMsg:
                fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line,
                        context.function);
                break;
            case QtFatalMsg:
                fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line,
                        context.function);
                abort();
        }
    } else {
        switch (type) {
            case QtDebugMsg:
            case QtInfoMsg:
            case QtWarningMsg:
            case QtCriticalMsg:
                MainWindow::s_console->appendPlainText(msg);
                break;
            case QtFatalMsg:
                abort();
        }
    }
}

int main(int argc, char *argv[]) {
    qInstallMessageHandler(messageHandler);
    QApplication a(argc, argv);
    QApplication::setApplicationName("FileSyncer");
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    MainWindow w;
    w.show();
    return QApplication::exec();
}
