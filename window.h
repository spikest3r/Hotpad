#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QCloseEvent>
#include <QSystemTrayIcon>

class Window : public QWidget
{
    Q_OBJECT
public:
    explicit Window(QWidget *parent = nullptr, QApplication *a = nullptr);
    void setTrayState(bool active); // set color of window icon and tray icon
    void showWindow();
    void sendTrayMessage(QString message);
private:
    QSystemTrayIcon *trayIcon;
    QIcon redIcon;
    QIcon greenIcon;
protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // WINDOW_H
