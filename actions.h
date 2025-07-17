#include <windows.h>
#include <string>
#include <QApplication>

#ifndef ACTIONS_H
#define ACTIONS_H

enum class ActionType : int {
    NoAction = 0,
    ActionShellExecute = 1, // open file/url (or exe if full path is specified)
    ActionEnterText = 2,
    ActionKeyCombo = 3,
    ActionMultiAction = 4,
};

struct ActionData {
    bool isMultiAction = false;
    ActionType type = ActionType::NoAction;
    std::wstring path;

    QString letter; // for combo
    bool comboKeys[4] = {false,false,false,false}; // ctrl, alt, shift, win
};

#endif // ACTIONS_H
