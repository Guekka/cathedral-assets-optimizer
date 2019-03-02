#ifndef DEVMODEUI_H
#define DEVMODEUI_H

#include <QWidget>
#include "Optimiser.hpp"

namespace Ui {
class devModeUI;
}

class devModeUI : public QWidget
{

public:
    explicit devModeUI(QWidget *parent = nullptr);
    ~devModeUI();



private:

    Ui::devModeUI *ui;
    Optimiser *optimiser;


};

#endif // DEVMODEUI_H
