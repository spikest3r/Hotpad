#include "window.h"
#include "globals.h"
#include "datafile.h"
#include <string>
#include <QPushButton>
#include <QString>
#include <QMenu>
#include <QApplication>
#include <QMessageBox>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>

// vars
int selectedKey = 0;
bool ignoreComboEvent = false;

QLineEdit* actionArg1; // path/text
QLineEdit* actionArg2; // letter for combo
QCheckBox* ctrlBox;
QCheckBox* altBox;
QCheckBox* shiftBox;

// helpers
void UpdateUI(ActionType action) {
    actionArg1->setVisible(false);
    actionArg2->setVisible(false);
    ctrlBox->setVisible(false);
    altBox->setVisible(false);
    shiftBox->setVisible(false);
    switch(action) {
    case ActionType::ActionShellExecute:
        actionArg1->setVisible(true);
        actionArg1->setPlaceholderText("Path to file/URL");
        break;
    case ActionType::ActionEnterText:
        actionArg1->setVisible(true);
        actionArg1->setPlaceholderText("Text");
        break;
    case ActionType::ActionKeyCombo:
        actionArg2->setVisible(true);
        ctrlBox->setVisible(true);
        altBox->setVisible(true);
        shiftBox->setVisible(true);
        break;
    default:
        break;
    }
}

// constructor
Window::Window(QWidget *parent, QApplication *a)
    : QWidget(parent)  {

    setWindowTitle("Hotpad v1.0");
    setFixedSize(640,240);

    // init icons
    redIcon = QIcon(":/red.png");
    greenIcon = QIcon(":/green.png");

    // init QSystemTrayIcon
    trayIcon = new QSystemTrayIcon;
    QMenu *trayMenu = new QMenu;
    QAction *settings = new QAction("Settings");
    QAction *quit = new QAction("Exit Hotpad");
    trayMenu->addAction(settings);
    trayMenu->addSeparator();
    trayMenu->addAction(quit);
    trayIcon->setContextMenu(trayMenu);
    setTrayState(false); // by default hotpad is not active

    Window* baseRef = this;
    QObject::connect(settings, &QAction::triggered, [baseRef]() {
        baseRef->showWindow();
    });

    QObject::connect(quit, &QAction::triggered, a, &QApplication::quit);

    trayIcon->show();

    // init ui
    QPushButton* quitButton = new QPushButton("Quit", this);
    quitButton->setGeometry(640-100-10,240-30-10,100,30);
    QObject::connect(quitButton, &QPushButton::pressed, a, &QApplication::quit);

    QLabel* selectedKeyLabel = new QLabel("Select key", this);
    selectedKeyLabel->setGeometry(210,20,300,40);
    QFont font = selectedKeyLabel->font();
    font.setPointSize(18);
    selectedKeyLabel->setFont(font);

    QComboBox* combo = new QComboBox(this);
    combo->setGeometry(210,20+50,170,30);
    combo->addItem("No Action");
    combo->addItem("Open file/URL");
    combo->addItem("Input text");
    combo->addItem("Press key combo");
    combo->setVisible(false); // invisible until select key

    // open file/path & enter text
    actionArg1 = new QLineEdit(this);
    actionArg1->setGeometry(210,60+50,300,30);
    actionArg1->setVisible(false);

    // letter for key combo
    actionArg2 = new QLineEdit(this);
    actionArg2->setGeometry(210,60+50+50,100,30);
    actionArg2->setVisible(false);
    actionArg2->setPlaceholderText("Key");
    actionArg2->setMaxLength(6);

    // press key combo
    ctrlBox = new QCheckBox("CTRL",this);
    ctrlBox->setGeometry(210,60+50,300,30);
    altBox = new QCheckBox("ALT",this);
    altBox->setGeometry(270,60+50,300,30);
    shiftBox = new QCheckBox("SHIFT",this);
    shiftBox->setGeometry(270+60,60+50,300,30);

    QCheckBox* casArr[3] = {ctrlBox,altBox,shiftBox};
    for(int i = 0; i < 3; i++) {
        connect(casArr[i],&QCheckBox::checkStateChanged, [i](Qt::CheckState state) {
            if(ignoreComboEvent) return;
            switch(state) {
            case Qt::Checked:
                hotkeyActions[selectedKey].comboKeys[i] = true;
                break;
            default:
                hotkeyActions[selectedKey].comboKeys[i] = false;
                break;
            }
            SaveSettings();
        });
    }

    for(int y = 0; y < 3; y++) {
        int i = 7 - 3 * y;
        for(int x = 0; x < 3; x++) {
            QPushButton* button = new QPushButton(QString::number(i),this);
            button->setGeometry(10+(x*50+10),10+(y*50+10),50,50);
            QObject::connect(button, &QPushButton::pressed, [combo,i,selectedKeyLabel,casArr]() {
                combo->setVisible(true); // in case it isnt
                selectedKeyLabel->setText("Key: " + QString::number(i));
                ignoreComboEvent = true;
                ActionType action = hotkeyActions[i].type;
                combo->setCurrentIndex((int)action);
                actionArg1->setText(QString::fromStdWString(hotkeyActions[i].path));
                actionArg2->setText(QString(hotkeyActions[i].letter));
                for(int j = 0; j < 3; j++) {
                    casArr[j]->setCheckState(hotkeyActions[i].comboKeys[j] ? Qt::Checked : Qt::Unchecked);
                }
                UpdateUI(action);
                ignoreComboEvent = false;
                selectedKey = i;
            });
            i++;
        }
    }

    QPushButton* button = new QPushButton(QString::number(0),this);
    button->setGeometry(20,10+(3*50+10),50*2,50);
    QObject::connect(button, &QPushButton::pressed, [combo,selectedKeyLabel,casArr]() {
        combo->setVisible(true); // in case it isnt
        selectedKeyLabel->setText("Key: " + QString::number(0));
        ignoreComboEvent = true;
        ActionType action = hotkeyActions[0].type;
        combo->setCurrentIndex((int)action);
        actionArg1->setText(QString::fromStdWString(hotkeyActions[0].path));
        actionArg2->setText(QString(hotkeyActions[0].letter));
        for(int j = 0; j < 3; j++) {
            casArr[j]->setCheckState(hotkeyActions[0].comboKeys[j] ? Qt::Checked : Qt::Unchecked);
        }
        UpdateUI(action);
        ignoreComboEvent = false;
        selectedKey = 0;
    });

    QObject::connect(combo, &QComboBox::currentIndexChanged, [](int index) {
        if(ignoreComboEvent) return;
        ActionType action = (ActionType)index;
        hotkeyActions[selectedKey].type = action;
        UpdateUI(action);
        SaveSettings();
    });

    QObject::connect(actionArg1, &QLineEdit::textChanged, [](const QString& text) {
        if(ignoreComboEvent) return;
        hotkeyActions[selectedKey].path = text.toStdWString();
        SaveSettings();
    });

    QObject::connect(actionArg2, &QLineEdit::textChanged, [](const QString& text) {
        if(ignoreComboEvent) return;
        hotkeyActions[selectedKey].letter = text;
        SaveSettings();
    });

    UpdateUI(ActionType::NoAction);
    // show start message
    trayIcon->showMessage("Hotpad","Hotpad is running!");
    //QMessageBox::information(this,"Hotpad","Hotpad is running! Check tray icon.");
    //show();
}

void Window::showWindow() {
    showNormal();
    raise();
    activateWindow();
}

// override close and hide window instead of quitting
void Window::closeEvent(QCloseEvent *event) {
    event->ignore();
    hide();
}

// set tray and window icons
void Window::setTrayState(bool active) {
    if(active) {
        setWindowIcon(greenIcon);
        trayIcon->setIcon(greenIcon);
        trayIcon->setToolTip("Hotpad: Active");
        trayIcon->showMessage("Hotpad","Hotkeys activated",QSystemTrayIcon::Warning,2000);
    } else {
        setWindowIcon(redIcon);
        trayIcon->setIcon(redIcon);
        trayIcon->setToolTip("Hotpad: Not Active");
        trayIcon->showMessage("Hotpad","Hotkeys deactivated",QSystemTrayIcon::Warning,2000);
    }
}

void Window::sendTrayMessage(QString message) {
    trayIcon->showMessage("Hotpad",message);
}
