#include "configurationdialog.h"
#include "ui_configurationdialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

ConfigurationDialog::ConfigurationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigurationDialog)
{
    ui->setupUi(this);

    const QString binaryPath = QDir::currentPath();
    const QString settingsPath = binaryPath + QDir::separator() + "TimeTracker.ini";
    QSettings settings(settingsPath, QSettings::IniFormat);
    std::string savePath = QDir::cleanPath(settings.value("Settings/saveDir").toString()).toStdString();

    ui->saveDirectoryEditBox->setText(savePath.c_str());
}

ConfigurationDialog::~ConfigurationDialog()
{
    delete ui;
}

void ConfigurationDialog::on_browseSaveDirectoryButton_clicked()
{
    QFileDialog directoryDialog;
    directoryDialog.setFileMode(QFileDialog::Directory);
    directoryDialog.setOption(QFileDialog::ShowDirsOnly);

    std::string saveDirectory = directoryDialog.getExistingDirectory(this, "Choose Save Directory").toStdString();
    if(saveDirectory.empty())
        return;

    ui->saveDirectoryEditBox->setText(saveDirectory.c_str());
}

void ConfigurationDialog::on_okCancelButtonBox_accepted()
{
    QString path = ui->saveDirectoryEditBox->text();
    if(path.isEmpty())
    {
        QMessageBox errorDialog;
        errorDialog.setText("Settings Error.");
        errorDialog.setInformativeText("The save path is empty.");
        errorDialog.setStandardButtons(QMessageBox::Ok);
        errorDialog.exec();

        return;
    }

    QDir checkPath(path);
    if(!checkPath.exists())
    {
        QMessageBox errorDialog;
        errorDialog.setText("Settings Error.");
        errorDialog.setInformativeText("The save path is invalid.");
        errorDialog.setStandardButtons(QMessageBox::Ok);
        errorDialog.exec();

        return;
    }

    const QString binaryPath = QDir::currentPath();
    const QString settingsPath = binaryPath + QDir::separator() + "TimeTracker.ini";

    QSettings settings(settingsPath, QSettings::IniFormat);
    settings.setValue("Settings/saveDir", path);
}
