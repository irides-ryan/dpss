#include "DDEProxyModeManager.h"

DDEProxyModeManager::DDEProxyModeManager(QObject *parent)
    : SystemProxyModeManager(parent),
      networkInter("com.deepin.daemon.Network",
                   "/com/deepin/daemon/Network",
                   QDBusConnection::sessionBus(),
                   this)
{
  QString proxyType;
  auto reply = networkInter.GetProxyMethod();
  proxyType = reply.value() + "\n";

  if (proxyType.startsWith("auto")) {
    // just pac url
    reply = networkInter.GetAutoProxy();
    proxyType += reply.value();
  } else if (proxyType.startsWith("manual")) {
    // types / addr / port
    reply = networkInter.GetProxy("http");
    proxyType += "http:" + reply.argumentAt(0).toString()
                   + ":" + reply.argumentAt(1).toString() + "\n";
    reply = networkInter.GetProxy("https");
    proxyType += "https:" + reply.argumentAt(0).toString()
                    + ":" + reply.argumentAt(1).toString() + "\n";
    reply = networkInter.GetProxy("ftp");
    proxyType += "ftp:" + reply.argumentAt(0).toString()
                  + ":" + reply.argumentAt(1).toString() + "\n";
    reply = networkInter.GetProxy("socks");
    proxyType += "socks:" + reply.argumentAt(0).toString()
                    + ":" + reply.argumentAt(1).toString() + "\n";
    reply = networkInter.GetProxyIgnoreHosts();
    proxyType += reply.value();
  }

  m_storedProxy = proxyType;
  qDebug() << "Store system proxy: " << m_storedProxy;
}

DDEProxyModeManager::~DDEProxyModeManager() {
  auto list = m_storedProxy.split("\n");
  auto proxy = list.takeFirst();

  if (m_storedProxy.startsWith("auto")) {
    auto pacUri = list.takeLast();
    networkInter.SetAutoProxyQueued(pacUri);
  } else if (m_storedProxy.startsWith("manual")) {
    auto ignoreHosts = list.takeLast();
    for (auto& s : list) {
      auto sl = s.split(":");
      networkInter.SetProxyQueued(sl[1], sl[2], sl[0]);
    }
    networkInter.SetProxyIgnoreHostsQueued(ignoreHosts);
  }
  networkInter.SetProxyMethodQueued(proxy);
  qDebug() << "Restore system proxy settings done.";
}

void DDEProxyModeManager::setProxyMethod(QString proxyMethod) {
  auto watcher = new QDBusPendingCallWatcher(
                     networkInter.SetProxyMethod(proxyMethod),
                     this);
  QObject::connect(watcher, &QDBusPendingCallWatcher::finished, [=] {
    qDebug() << "success to set proxy method " << proxyMethod;
    delete watcher;
  });
}

void DDEProxyModeManager::switchToNone() {
    setProxyMethod("none");
}

void DDEProxyModeManager::switchToAuto(QString pacURI) {
  auto watcher = new QDBusPendingCallWatcher(
                     networkInter.SetAutoProxy(pacURI),
                     this);
  QObject::connect(watcher, &QDBusPendingCallWatcher::finished, [=]() {
    qDebug() << "set pac URI " << pacURI;
    setProxyMethod("auto");
    delete watcher;
  });
}

void DDEProxyModeManager::switchToManual(QString localAddress,
                                         int localPort,
                                         QString type) {
  if (type.isEmpty()) {
    type = "socks";
  }
  QString addr = localAddress;
  QString port = QString::number(localPort);
  auto watcher = new QDBusPendingCallWatcher(
                     networkInter.SetProxy(type, addr, port),
                     this);
  QObject::connect(watcher, &QDBusPendingCallWatcher::finished, [=] {
    qDebug() << "set proxy" << type << addr << port;
    setProxyMethod("manual");
    delete watcher;
  });
}
