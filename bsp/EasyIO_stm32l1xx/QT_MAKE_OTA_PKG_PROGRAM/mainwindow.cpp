#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCryptographicHash>
#include <QDebug>

#include "md5sum.h"

#define CONF_FILENAME "./config.ini"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QSettings *ConfigIni = new QSettings(CONF_FILENAME,QSettings::IniFormat);
    ui->setupUi(this);
    memset(&m_Header,0x0,sizeof(m_Header));

    ui->inputEdit->setText(ConfigIni->value("inputpath").toString());
    ui->outputEdit->setText(ConfigIni->value("outputpath").toString());

    ConfigIni->deleteLater();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//const char jsonstr[] = "{\"version\":10 , \"version_name\":\"V1.03\" , \"mode\":\"auto\" , \"size\": 128083 , \"filename\":\"ct02_ota.bin\" , \"checksum\":\"DF645B05C173A8D1F0C90A5D434B76CC\" , \"data\":\"2015\/12\/21\"}";

void MainWindow::on_pushButton_clicked()
{

    int di;
    static md5_state_t state;
    static md5_byte_t digest[16];
    static char hex_output[16*2 + 1];
    memset(hex_output,0x0,sizeof(hex_output));
    QString fwvsersionname;
    int vsersionnum;

    QString filename;
    QSettings *ConfigIni = new QSettings(CONF_FILENAME,QSettings::IniFormat);
    QByteArray filearray;
    QFile *file_out;
    //QFile *file=new QFile("D:\\SoftwareDevelop\\嵌入式\\EasyIO\\EasyIO_SRC\\RT-Thread_1.2.0\\bsp\\EasyIO_stm32l1xx\\binfile\\fw.bin");
    QFile *file=new QFile(ui->inputEdit->text());
    file->open(QIODevice::ReadOnly);
    filearray = file->readAll();
    ui->textBrowser->append(QString("Bin size :").append(QString::number(filearray.length())));


    //file_out = new QFile("D:\\SoftwareDevelop\\嵌入式\\EasyIO\\EasyIO_SRC\\RT-Thread_1.2.0\\bsp\\EasyIO_stm32l1xx\\binfile\\fw_ota.bin");
    file_out = new QFile(ui->outputEdit->text());
    file_out->open(QIODevice::ReadWrite);

    m_Header.size = filearray.length();

    filearray.indexOf(QString("@@@@@@Version:"));

    //qDebug() << filearray.indexOf(QString("@@@@@@Version:"));
    ui->textBrowser->append(filearray.mid(filearray.indexOf(QString("{\"version_name\"")),64));

    file_out->write((const char*)&m_Header,sizeof(m_Header));
    file_out->write(filearray);
    file_out->flush();
    file_out->close();

    ui->textBrowser->append(QString("Bin size :").append(QString::number(file_out->size())));
    //filename = file_out-




    ConfigIni->setValue("inputpath",ui->inputEdit->text());
    ConfigIni->setValue("outputpath",ui->outputEdit->text());
    ConfigIni->deleteLater();

    file_out->open(QIODevice::ReadOnly);

    md5_init(&state);
    //md5_append(&state,(const unsigned char*)&m_Header,sizeof(m_Header));
    md5_append(&state,(const unsigned char*)file_out->readAll().data(),file_out->size());
    md5_finish(&state, digest);


    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(QString(filearray.mid(filearray.indexOf(QString("{\"version_name\"")),64)).toUtf8(), &json_error);
    if(json_error.error == QJsonParseError::NoError)
    {
        //qDebug()<<"xxxxxxxxx" <<__LINE__;
        if(parse_doucment.isObject())
        {
            //qDebug()<<"xxxxxxxxx" <<__LINE__;
            QJsonObject obj = parse_doucment.object();
            if(obj.contains("version_name"))
            {
                QJsonValue name_value = obj.take("version_name");
                if(name_value.isString())
                {
                    //qDebug()<<"xxxxxxxxx" <<__LINE__;
                    QString name = name_value.toString();
                    fwvsersionname = name;
                    ui->textBrowser->append(name.prepend(QString("XXXXXXXXXNAME:")));
                }
            }

            if(obj.contains("version"))
            {
                QJsonValue name_value = obj.take("version");
                //if(name_value.isDouble())
                {
                    //qDebug()<<"xxxxxxxxx" <<__LINE__;
                    vsersionnum = name_value.toVariant().toInt();
                   // QString name = name_value.toString();
                   // ver = name;
                    //ui->textBrowser->append(name.prepend(QString("XXXXXXXXXNAME:")));
                }
            }

            //vsersionnum
           // qDebug()<<"xxxxxxxxx" <<__LINE__;
        }
    }
    //qDebug()<<"xxxxxxxxx" <<__LINE__ <<json_error.errorString()<<QString(filearray.mid(filearray.indexOf(QString("{\"version_name\"")),64));


    QJsonObject json;
    json.insert("version", vsersionnum);
    json.insert("version_name", fwvsersionname);
    if (ui->checkBox->isChecked())
        json.insert("mode", QString("auto"));
    else
        json.insert("mode", QString("manually"));
    json.insert("filename", QString("ct03_ota.bin"));
    json.insert("size", file_out->size());

    QString md5;
    QByteArray hashData;



    for (di = 0; di < 16; ++di)
    {
        char md5str[4];
        snprintf(md5str,sizeof(md5str),"%02x",digest[di]);
        strcat(hex_output,md5str);
    }

    hashData.setRawData(hex_output,strlen(hex_output));
    md5 = hashData;//.toHex();

    json.insert("checksum",md5);

    QJsonDocument document;
    document.setObject(json);
    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    QString json_str(byte_array);
    ui->textBrowser->append(json_str);
    QFile *json_out;
    json_out = new QFile(ui->outputEdit->text().append(".conf"));
    json_out->open(QIODevice::WriteOnly);
    json_out->write(json_str.toUtf8());
    json_out->flush();



    file->close();
    file_out->close();
    json_out->close();

}
