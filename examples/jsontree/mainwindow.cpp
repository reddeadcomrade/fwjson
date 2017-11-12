#include <QtCore/qdebug.h>

#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QFileDialog>

#include "fwjsonexception.h"

#include "mainwindow.h"

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    BaseClass(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralwidget->setColumnCount(3);
    ui->centralwidget->setHeaderLabels(QStringList() << "Node" << "Type" << "Value");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addNode(QTreeWidgetItem* parent, FwJSON::Node* node)
{
    switch(node->type())
    {
    case FwJSON::T_Object:
        {
            parent->setText(1, "object");

                FwJSON::Object* object = FwJSON::cast<FwJSON::Object>(node);
                QHash<QByteArray, FwJSON::Node*> attributes = object->attributes();
                for(QHash<QByteArray, FwJSON::Node*>::const_iterator iter = attributes.begin(); iter != attributes.end(); ++iter)
                {
                    QTreeWidgetItem* childItem = new QTreeWidgetItem(parent);
                    childItem->setText(0, QString::fromUtf8(iter.key()));
                    addNode(childItem, iter.value());
                }

                QString str = "<empty>";
                if(object->hasValue<FwJSON::String>("stroka", &str))
                {
                    qDebug() << str;
                }
                qDebug() << object->value<FwJSON::String>("stroka");
        
        }
        break;

    case FwJSON::T_String:
        {
            bool bOk = false;
            parent->setText(1, "string");
            FwJSON::String* stringNode = FwJSON::cast<FwJSON::String>(node);
            parent->setText(2, stringNode->isEmpty() ? "<empty>" : stringNode->toString(&bOk));
        }
        break;

    case FwJSON::T_Number:
        {
            parent->setText(1, "number");
            parent->setText(2, QString::fromUtf8(node->toUtf8()));
        }
        break;

    case FwJSON::T_Array:
        {
            FwJSON::Array* array = FwJSON::cast<FwJSON::Array>(node);

            parent->setText(1, QString("array[%1]").arg(array->size()));

            int i = 0;
            foreach(FwJSON::Node* child, array->toQVector())
            {
                QTreeWidgetItem* childItem = new QTreeWidgetItem(parent);
                childItem->setText(0, QString("[%1]").arg(i++));
                addNode(childItem, child);
            }
        }
        break;
    case FwJSON::T_Bool:
        {
            parent->setText(1, "boolean");
            parent->setText(2, QString::fromUtf8(node->toUtf8()));
        }
        break;
    default:
        break;

    }

}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open fwml file"));
    if(!fileName.isEmpty())
    {
        open(fileName);
    }
}

void MainWindow::open(const QString& fileName)
{
    m_rootObject.clear();
    ui->centralwidget->clear();

    QTreeWidgetItem* rootItem = new QTreeWidgetItem(ui->centralwidget);
    rootItem->setText(0, "root");

    try
    {
         m_rootObject.parseFile(fileName);
         addNode(rootItem, &m_rootObject);
    }
    catch(const FwJSON::Exception& e)
    {
        QTreeWidgetItem* errorItem = new QTreeWidgetItem(rootItem);
        errorItem->setText(0, "Error");
        errorItem->setText(1, QString::fromUtf8(e.error()));
    }

}
