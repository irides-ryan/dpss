#include "stdafx.h"
#include "utils.h"
#include "MainWindow.h"
#include "ShareDialog.h"

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

static void onSignalRecv(int sig) {
  if (sig == SIGINT || sig == SIGTERM) {
    qApp->quit();
  } else {
    qWarning("Unhandled signal %d", sig);
  }
}

int main(int argc, char *argv[]) {
  DApplication::loadDXcbPlugin();
  DApplication app(argc, argv);
  app.setAttribute(Qt::AA_UseHighDpiPixmaps);
  const QString socket_path(QString("dpss_%1").arg(getuid()));
  if (app.setSingleInstance(socket_path)) {
    signal(SIGINT, onSignalRecv);
    signal(SIGTERM, onSignalRecv);
    app.setTheme("light");

    QTranslator translator;
    translator.load(QLocale::system(), "dpss", "_", ":/i18n");
    app.installTranslator(&translator);

    const QString descriptionText = QApplication::tr(
      "If you want to keep a secret, you must also hide it from yourself.");

    const QString acknowledgementLink = "https://github.com/irides-ryan/dpss";

    app.setOrganizationName("irides");
    app.setApplicationName("dpss");
    app.setApplicationDisplayName(QApplication::tr("ss for deepin"));
    app.setApplicationVersion("1.3.0");

    app.setProductIcon(QIcon(Utils::getIconQrcPath("ssw128.svg")));
    app.setProductName(QApplication::tr("dpss"));
    app.setApplicationDescription(descriptionText);
    app.setApplicationAcknowledgementPage(acknowledgementLink);

    app.setWindowIcon(QIcon(Utils::getIconQrcPath("ssw128.png")));
    app.setQuitOnLastWindowClosed(false);
    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();
    MainWindow window;

    QObject::connect(&app, &DApplication::newInstanceStarted,
                     &window, &MainWindow::activateWindow);

    Dtk::Widget::moveToCenter(&window);
    return app.exec();
  }
  qDebug() << "app has started";
  return 0;
}
