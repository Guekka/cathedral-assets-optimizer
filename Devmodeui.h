#ifndef DEVMODEUI_H
#define DEVMODEUI_H

#include "Optimiser.h"

namespace Ui {
class devModeUI;
}

class devModeUI : public QWidget
{

public:
    explicit devModeUI(Optimiser* optimiser);
    ~devModeUI();

private:

    Ui::devModeUI *ui;
};

#endif // DEVMODEUI_H
