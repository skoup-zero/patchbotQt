#pragma once

#include <QtWidgets/QMainWindow>
#include <ui_patchbot_gui.h>

class patchbot_gui : public QMainWindow
{
    Q_OBJECT

public:
    patchbot_gui(QWidget *parent = Q_NULLPTR);

private:
    Ui::patchbot_guiClass ui;
};
