#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ID_card_recognition.h"
#include <QFileDialog>
#include <QFile>
#include "image_processing.h"
#include <memory>



QT_BEGIN_NAMESPACE
namespace Ui { class ID_card_recognitionClass; };
QT_END_NAMESPACE

class ID_card_recognition : public QMainWindow
{
    Q_OBJECT

public:
    ID_card_recognition(QWidget* parent = nullptr);
    ~ID_card_recognition();

private slots:
    void on_open_clicked();
    void on_yes_clicked();

private:
    Ui::ID_card_recognitionClass* ui;
    shared_ptr<image_processing> img_proc;
};
