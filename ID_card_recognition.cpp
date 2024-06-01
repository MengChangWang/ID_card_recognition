#include "ID_card_recognition.h"

ID_card_recognition::ID_card_recognition(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::ID_card_recognitionClass())
{
    ui->setupUi(this);
    QString digital_templete_path = ":/ID_card_recognition/number.png";
    img_proc = make_shared<image_processing>();
    img_proc->digital_template_preprocessing(digital_templete_path);
}

ID_card_recognition::~ID_card_recognition()
{
    delete ui;
}

void ID_card_recognition::on_open_clicked()
{
    QString curPath = QCoreApplication::applicationDirPath();
    QString fileName = QFileDialog::getOpenFileName(this, "打开文件", curPath, "图片信息 (*.jpg *.png)");
    ui->IDcard->setPixmap(QPixmap(fileName));
    ui->IDcard->setScaledContents(Qt::KeepAspectRatio);
    img_proc->pretreatment_image(fileName);
}

void ID_card_recognition::on_yes_clicked()
{
    if (!img_proc->recognise_ID_number())
        QMessageBox::warning(this, "警告", "识别失败,请重新提供图片");
    else
    {
        QString ID_number = img_proc->char_to_QString();
        ui->ID->setText(ID_number);
        QString province = img_proc->get_provience();
        ui->local->setText(province);
        QString birth_date = img_proc->get_birthday();
        ui->birth->setText(birth_date);
        QString sex = img_proc->get_sex();
        ui->sex->setText(sex);
    }

}

