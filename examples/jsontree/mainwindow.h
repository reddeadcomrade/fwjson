#pragma once

#include <QtWidgets/QMainWindow>

#include "fwjson.h"

class QTreeWidget;
class QTreeWidgetItem;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    typedef QMainWindow BaseClass;

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void addNode(QTreeWidgetItem* parent, FwJSON::Node* node);

public slots:
    void open();
    void open(const QString& fileName);

private:
    Ui::MainWindow *ui;
    FwJSON::Object m_rootObject;
};
