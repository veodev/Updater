#include "widget.h"
#include "ui_widget.h"

#include <QTime>
#include <QDir>
#include <QCryptographicHash>
#include <unistd.h>
#include <QScrollBar>
#include <QtMath>

const int CHECK_TIMER_INTERVAL_SEC = 1;
const int SEC_TO_AUTO_REBOOT = 20;

const QString PROGRAM_NAME = "avicon31";
const QString MD5SUM_FILE_NAME = "md5sum";
const QString PROGRAM_NAME_BACKUP = "avicon31.backUp";
const QString PROGRAM_DIR = "/usr/local/avicon-31";
const QString TMP_PROGRAM_DIR = "/usr/local/avicon-31/tmp";
const QString COMPONENTS_QT_DIR = "/usr/local";
const QString SYSTEM_UPDATE_NAME = "avicon31_patch.tar";
const QString SYSTEM_UPDATE_TMP_SUBDIR = "avicon31_patch";
const QString SYSTEM_UPDATE_SCRIPT_NAME = "setup.sh";
const QString PROGRAM_LOGS_DIR = QDir::homePath() + QString("/.avicon-31/log/");
const QString UMU_FIRMWARE_PATH = "/root/umu-firmware";
const QString UMU_FTP_URL = "ftp://anonimous:@192.168.100.100/";

/*Write "echo Done" at the and of setup.sh*/

Widget::Widget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , _countSecondsToAutoReboot(0)
    , _checkSecondsToAutoRebootTimer(Q_NULLPTR)
    , _currentMainAppVersion(QString())
    , _currentBackUpMainAppVersion(QString())
    , _availableMainAppVersion(QString())
    , _flashDirPath(QString())
    , _homeTmpPath(QDir::homePath() + QString("/tmp"))
{
    ui->setupUi(this);
    ui->logFilesListWidget->verticalScrollBar()->setStyleSheet("width: 30px;");
    ui->tabWidget->setCurrentIndex(0);
    startAutoReboot();
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onCurrentTabChanged(int)));

    _helper = new Helper(this);
    _helper->hide();
    ui->helpButton->hide();

    connect(ui->mainAppTextEdit, SIGNAL(textChanged()), this, SLOT(onMainAppTextEditChanged()));

    QFile versionFile("/etc/os-version");
    if (versionFile.exists()) {
        versionFile.open(QIODevice::ReadOnly);
        QTextStream in(&versionFile);
        QString line = in.readLine();
        ui->osVersionLabel->setText(line);
    }
    else {
        ui->osVersionLabel->setText("1.0 (Factory default)");
    }
}

Widget::~Widget()
{
    QDir(_homeTmpPath).removeRecursively();
    delete ui;
}

void Widget::startAutoReboot()
{
    ui->progressBar->setMaximum(SEC_TO_AUTO_REBOOT);
    ui->messageLabel->setText(QString(tr("Rebooting after: %1 sec ...")).arg(QString::number(SEC_TO_AUTO_REBOOT)));
    _checkSecondsToAutoRebootTimer = new QTimer(this);
    _checkSecondsToAutoRebootTimer->setInterval(CHECK_TIMER_INTERVAL_SEC * 1000);
    connect(_checkSecondsToAutoRebootTimer, SIGNAL(timeout()), this, SLOT(onCheckSecondsToAutoReboot()));
    _checkSecondsToAutoRebootTimer->start();
}

void Widget::cancelAutoReboot()
{
    if (_checkSecondsToAutoRebootTimer->isActive()) {
        _checkSecondsToAutoRebootTimer->stop();
    }
    clearMessageWidget();
}

void Widget::reboot()
{
    QProcess* process = new QProcess(this);
    process->startDetached("reboot");
    qApp->quit();
}

void Widget::poweroff()
{
    QProcess* process = new QProcess(this);
    process->startDetached("poweroff");
    qApp->quit();
}

void Widget::execDetachProcessWithQuit(QString command)
{
    QProcess* process = new QProcess(this);
    process->startDetached(command);
    qApp->quit();
}

void Widget::printInfoForUpdatingMainApp()
{
    _helper->insertText(QString(tr("Instructions for work with main application:")) + QString("\n"));
    _helper->insertText(QString(tr("1. To back up the current version, press button \"Back up\".\n")));
    _helper->insertText(QString(tr("2. To restore the current version from the backup, press button \"Restore\".\n")));

    _helper->insertText(QString(tr("3. To update main program make sure, that the USB flash drive must be formatted in FAT32 and files \"avikon31\" and \"md5sum\" should be in the root of the file system.\n")));
    _helper->insertText(QString(tr("3.1 Insert the USB flash drive if it is not already connected.\n")));
    _helper->insertText(QString(tr("3.2 Press button \"Check updates\" to verify the new version without installing.\n")));
    _helper->insertText(QString(tr("3.3 Press button \"Update\" to update to the available on the USB flash drive version.\n")));
}

void Widget::abortUpdateMainApp()
{
    ui->progressBar->hide();
    ui->mainAppTextEdit->insertPlainText(QString(tr("=== ABORTED UPDATE!!! ===")) + QString("\n"));
    ui->messageLabel->setText(QString(tr("Aborted!")));
    blockControls(false);
}

void Widget::abortUpdateOsComponentsApp()
{
    ui->progressBar->hide();
    ui->osComponentsTextEdit->insertPlainText(QString(tr("=== ABORTED UPDATE!!! ===")) + QString("\n"));
    ui->messageLabel->setText(QString(tr("Aborted!")));
    blockControls(false);
}

void Widget::browseLogDir()
{
    ui->logFilesListWidget->clear();
    QDir dir(PROGRAM_LOGS_DIR);
    if (dir.exists()) {
        dir.refresh();
        QFileInfoList fileList = dir.entryInfoList(QDir::Files);
        foreach (const QFileInfo& fileInfo, fileList) {
            QListWidgetItem* newItem = new QListWidgetItem();
            newItem->setText(fileInfo.fileName());
            ui->logFilesListWidget->addItem(newItem);
        }
    }
}


void Widget::clearMessageWidget()
{
    ui->progressBar->hide();
    ui->progressBar->setValue(0);
    ui->messageLabel->clear();
}

bool Widget::checkCurrentVersion()
{
    QString command = PROGRAM_DIR + QString("/") + PROGRAM_NAME + QString(" -v");

    QProcess process;
    process.start("sh", QStringList() << "-c" << command);
    process.waitForStarted();
    process.waitForFinished();
    if (process.exitCode() == 0) {
        QString output = process.readAllStandardOutput();
        QStringList list = output.split(QChar('\n'));
        if (list.isEmpty() == false) {
            _currentMainAppVersion = list.first();
            ui->currentVersionLineEdit->setText(_currentMainAppVersion);
            ui->createBackUpButton->setEnabled(true);
            return true;
        }
    }
    else {
        ui->mainAppTextEdit->insertPlainText(QString(tr("Application Avicon-31 not found!")) + QString("\n"));
    }
    ui->createBackUpButton->setEnabled(false);
    return false;
}

bool Widget::checkBackUpVersion()
{
    QString command = PROGRAM_DIR + QString("/") + PROGRAM_NAME_BACKUP + QString(" -v");
    QProcess process;
    process.start("sh", QStringList() << "-c" << command);
    process.waitForStarted();
    process.waitForFinished();
    if (process.exitCode() == 0) {
        QString output = process.readAllStandardOutput();
        QStringList list = output.split(QChar('\n'));
        if (list.isEmpty() == false) {
            _currentBackUpMainAppVersion = list.first();
            ui->backUpVersionLineEdit->setText(_currentBackUpMainAppVersion);
            return true;
        }
    }
    return false;
}

bool Widget::checkAvailableVersion()
{
    bool res1 = QDir(_homeTmpPath).removeRecursively();
    bool res2 = QDir().mkdir(_homeTmpPath);
    if ((res1 && res2) == false) {
        ui->mainAppTextEdit->insertPlainText(QString(tr("Can't create tmp directory!.")) + QString("\n"));
        return false;
    }

    QDir dir(_homeTmpPath);
    if (dir.exists()) {
        if (QFile(_homeTmpPath + QString("/") + PROGRAM_NAME).exists() == false) {
            QFileInfo fileInfo(_flashDirPath, PROGRAM_NAME);
            QString sourcePath = fileInfo.absoluteFilePath();
            QString destinationPath = _homeTmpPath + QString("/") + fileInfo.fileName();
            QFile::copy(sourcePath, destinationPath);
        }
        QFile file(_homeTmpPath + QString("/") + PROGRAM_NAME);
        file.setPermissions(QFile::ReadOwner | QFile::ExeOwner | QFile::WriteOwner);

        QString command = _homeTmpPath + QString("/") + PROGRAM_NAME + QString(" -v");
        QProcess process;
        process.start("sh", QStringList() << "-c" << command);
        process.waitForStarted();
        process.waitForFinished();
        if (process.exitCode() == 0) {
            QString output = process.readAllStandardOutput();
            QStringList list = output.split(QChar('\n'));
            if (list.isEmpty() == false) {
                _availableMainAppVersion = list.first();
                ui->availableVersionLineEdit->setText(_availableMainAppVersion);
                ui->mainAppTextEdit->insertPlainText(QString(tr("Found.")) + QString("\n"));
                return true;
            }
        }
    }
    ui->mainAppTextEdit->insertPlainText(QString(tr("NOT found!")) + QString("\n"));
    return false;
}

bool Widget::checkAvailableOsUpdates()
{
    bool res1 = QDir(_homeTmpPath).removeRecursively();
    bool res2 = QDir().mkdir(_homeTmpPath);
    if ((res1 && res2) == false) {
        ui->mainAppTextEdit->insertPlainText(QString(tr("Can't create tmp directory!.")) + QString("\n"));
        return false;
    }

    bool res = false;
    QDir dir(_homeTmpPath);
    if (dir.exists()) {
        bool res3 = false;
        if (QFile(_flashDirPath + QString("/") + SYSTEM_UPDATE_NAME).exists() == true) {
            QFileInfo fileInfo(_flashDirPath, SYSTEM_UPDATE_NAME);
            QString sourcePath = fileInfo.absoluteFilePath();
            QString destinationPath = _homeTmpPath + QString("/") + fileInfo.fileName();
            res3 = QFile::copy(sourcePath, destinationPath);
        }
        bool res4 = dir.mkdir(SYSTEM_UPDATE_TMP_SUBDIR);
        QProcess process;
        QString command = QString("tar -xvf ") + _homeTmpPath + QString("/") + SYSTEM_UPDATE_NAME + QString(" -C") + _homeTmpPath + QString("/");
        process.start("sh", QStringList() << "-c" << command);
        process.waitForStarted();
        process.waitForFinished();
        bool res5 = false;
        QString output = process.readAllStandardOutput();
        (process.exitCode() == 0 && output.contains(QString("setup.sh"))) ? res5 = true : res5 = false;

        QProcess process2;
        QString command2 = QString("chown -R root:root ") + _homeTmpPath + QString("/") + SYSTEM_UPDATE_TMP_SUBDIR + QString(" | chmod -R +x ") + _homeTmpPath + QString("/") + SYSTEM_UPDATE_TMP_SUBDIR;
        process2.start("sh", QStringList() << "-c" << command2);
        process2.waitForStarted();
        process2.waitForFinished();
        bool res6 = false;
        (process2.exitCode() == 0) ? res6 = true : res6 = false;
        res = res3 && res4 && res5 && res6;
    }
    res ? ui->osComponentsTextEdit->insertPlainText(QString(tr("Found.")) + QString("\n")) : ui->osComponentsTextEdit->insertPlainText(QString(tr("NOT found!")) + QString("\n"));
    return res;
}

bool Widget::updateOsComponents()
{
    ui->osComponentsTextEdit->insertPlainText(QString(tr("Updating files ... ")) + QString("\n"));
    delay(100);
    QProcess process;
    QString command = _homeTmpPath + QString("/") + SYSTEM_UPDATE_TMP_SUBDIR + QString("/") + SYSTEM_UPDATE_SCRIPT_NAME;
    process.start("sh", QStringList() << "-c" << command);
    process.waitForStarted();
    QString output;
    while (1) {
        if (process.waitForFinished() == true) {
            output = process.readAllStandardOutput();
            break;
        }
    }
    bool res = output.contains(QString("Done"));
    res ? ui->osComponentsTextEdit->insertPlainText(QString(tr("Success.")) + QString("\n")) : ui->osComponentsTextEdit->insertPlainText(QString(tr("Failure!")) + QString("\n"));
    return res;
}

bool Widget::updateMainApplication()
{
    ui->mainAppTextEdit->insertPlainText(QString(tr("Updating files ... ")) + QString("\n"));
    delay(100);
    bool res = false;
    if (QFile(_homeTmpPath + QString("/") + PROGRAM_NAME).exists()) {
        QString sourcePath = PROGRAM_DIR + "/" + PROGRAM_NAME;
        bool res1 = true;
        if (QFile(sourcePath).exists()) {
            res1 = QFile::remove(sourcePath);
            sync();
        }
        sourcePath = _homeTmpPath + "/" + PROGRAM_NAME;
        QString destinationPath = PROGRAM_DIR + "/" + PROGRAM_NAME;
        bool res2 = QFile::copy(sourcePath, destinationPath);
        sync();
        res = res1 && res2;
    }

    res ? ui->mainAppTextEdit->insertPlainText(QString(tr("Success.")) + QString("\n")) : ui->mainAppTextEdit->insertPlainText(QString(tr("Failure!")) + QString("\n"));
    return res;
}

CheckFileHashError Widget::checkFileHash(QString path)
{
    QByteArray readMd5sum;
    QByteArray checkMd5sum;
    QFile* md5sumFile = new QFile(_flashDirPath + QString("/") + MD5SUM_FILE_NAME);
    if (md5sumFile->open(QFile::ReadOnly)) {
        QList<QByteArray> list = md5sumFile->readAll().split('\n');
        if (list.isEmpty() == false) {
            readMd5sum = list.first();
        }
        md5sumFile->close();
    }
    delete md5sumFile;

    QFile* file = new QFile(path + QString("/") + PROGRAM_NAME);
    if (file->open(QFile::ReadWrite)) {
        QCryptographicHash hash(QCryptographicHash::Md5);
        hash.addData(file);
        checkMd5sum = hash.result().toHex();
        file->close();
    }
    delete file;

    if (readMd5sum.isEmpty()) {
        return NoCheckFile;
    }
    else if (readMd5sum == checkMd5sum) {
        return SuccessCheckFiles;
    }
    return FailureCheckFiles;
}

void Widget::checkBackUpAndCurrentVersion()
{
    checkBackUpVersion();
    checkCurrentVersion();
}

bool Widget::checkIsUsbFlashMount()
{
    QPlainTextEdit* textEdit = Q_NULLPTR;
    switch (ui->tabWidget->currentIndex()) {
    case 0:
        break;
    case 1:
        textEdit = ui->mainAppTextEdit;
        break;
    case 2:
        textEdit = ui->osComponentsTextEdit;
        break;
    case 3:
        break;
    }
    if (textEdit != Q_NULLPTR) {
        textEdit->insertPlainText(QString(tr("Checking USB flash ... ")) + QString("\n"));
    }
    delay(100);
    QString command = QString("df");
    QProcess process;
    process.start("sh", QStringList() << "-c" << command);
    process.waitForStarted();
    process.waitForFinished();
    QString output = process.readAllStandardOutput();

    _flashDirPath = QString();
    QStringList list = output.split(QChar('\n'));
    QString strFromDfCommand = QString();
    if (list.isEmpty() == false) {
        foreach (QString str, list) {
            if (str.contains("sda")) {
                strFromDfCommand = str;
                break;
            }
        }
        QStringList list1 = strFromDfCommand.split(QChar(' '));
        if (list1.isEmpty() == false) {
            _flashDirPath = list1.last();
        }
    }
    bool res = !_flashDirPath.isEmpty();
    if (textEdit != Q_NULLPTR) {
        res ? textEdit->insertPlainText(QString(tr("Found.")) + QString("\n")) : textEdit->insertPlainText(QString(tr("NOT found! Check the connection or formatting.")) + QString("\n"));
    }
    return res;
}

bool Widget::checkUMUFlashFirmware() {}

void Widget::delay(int msToWait)
{
    QTime stopTime = QTime::currentTime().addMSecs(msToWait);
    while (QTime::currentTime() < stopTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

void Widget::blockControls(bool isBlock)
{
    ui->tabWidget->setEnabled(!isBlock);
    ui->createBackUpButton->setEnabled(!isBlock);
    ui->restoreBackUpButton->setEnabled(!isBlock);
    ui->updateMainAppButton->setEnabled(!isBlock);
    ui->checkUpdatesMainAppButton->setEnabled(!isBlock);
    ui->checkOsUpdatesButton->setEnabled(!isBlock);
    ui->updateOsButton->setEnabled(!isBlock);
    ui->viewLogButton->setEnabled(!isBlock);
    ui->copyLogsButton->setEnabled(!isBlock);
}

void Widget::resetProgressBar()
{
    ui->progressBar->setMaximum(10);
    ui->progressBar->setValue(0);
    ui->progressBar->show();
}

void Widget::progress(int value)
{
    ui->progressBar->setValue(value);
}

bool Widget::deletePreviousBackUp()
{
    ui->mainAppTextEdit->insertPlainText(QString(tr("Deleting current backup ...")) + QString("\n"));
    delay(100);
    bool res = QDir(PROGRAM_DIR).remove(PROGRAM_DIR + QString("/") + PROGRAM_NAME_BACKUP);
    if (res) {
        ui->backUpVersionLineEdit->clear();
        delay(100);
        ui->mainAppTextEdit->insertPlainText(QString(tr("Success.")) + QString("\n"));
    }
    else {
        ui->mainAppTextEdit->insertPlainText(QString(tr("Failure!")) + QString("\n"));
    }
    return res;
}

bool Widget::createBackUp()
{
    ui->mainAppTextEdit->insertPlainText(QString(tr("Creating backup ...")) + QString("\n"));
    delay(100);
    bool res = false;
    bool res1 = QDir().mkdir(TMP_PROGRAM_DIR);
    QDir dir(TMP_PROGRAM_DIR);
    if (dir.exists()) {
        QFileInfo fileInfo(PROGRAM_DIR, PROGRAM_NAME);
        QString sourcePath = fileInfo.absoluteFilePath();
        QString destinationPath = dir.absolutePath() + QString("/") + fileInfo.fileName();
        bool res2 = QFile::copy(sourcePath, destinationPath);
        bool res3 = QFile(destinationPath).rename(destinationPath + QString(".backUp"));
        QFileInfo fileInfoBackUp(destinationPath, PROGRAM_NAME_BACKUP);
        sourcePath = dir.absolutePath() + QString("/") + fileInfoBackUp.fileName();
        destinationPath = PROGRAM_DIR + QString("/") + fileInfoBackUp.fileName();
        bool res4 = QFile::copy(sourcePath, destinationPath);
        bool res5 = dir.removeRecursively();
        res = res1 && res2 && res3 && res4 && res5;
    }

    res ? ui->mainAppTextEdit->insertPlainText(QString(tr("Success.")) + QString("\n")) : ui->mainAppTextEdit->insertPlainText(QString(tr("Failure!")) + QString("\n"));
    return res;
}

bool Widget::restoreFromBackUp()
{
    ui->mainAppTextEdit->insertPlainText(QString(tr("Restoring from backup ...")) + QString("\n"));
    delay(100);
    bool res = false;
    bool res1 = QDir().mkdir(TMP_PROGRAM_DIR);
    QDir dir(TMP_PROGRAM_DIR);
    if (dir.exists()) {
        QFileInfo fileInfoBackUp(PROGRAM_DIR, PROGRAM_NAME_BACKUP);
        QString sourcePath = fileInfoBackUp.absoluteFilePath();
        QString destinationPath = dir.absolutePath() + QString("/") + fileInfoBackUp.fileName();
        bool res2 = QFile::copy(sourcePath, destinationPath);
        bool res3 = QFile(destinationPath).rename(TMP_PROGRAM_DIR + QString("/") + PROGRAM_NAME);
        bool res4 = QDir(PROGRAM_DIR).remove(PROGRAM_DIR + QString("/") + PROGRAM_NAME);
        QFileInfo fileInfo(destinationPath, PROGRAM_NAME);
        sourcePath = dir.absolutePath() + QString("/") + fileInfo.fileName();
        destinationPath = PROGRAM_DIR + QString("/") + fileInfo.fileName();
        bool res5 = QFile::copy(sourcePath, destinationPath);
        bool res6 = dir.removeRecursively();
        res = res1 && res2 && res3 && res4 && res5 && res6;
    }
    res ? ui->mainAppTextEdit->insertPlainText(QString(tr("Success.")) + QString("\n")) : ui->mainAppTextEdit->insertPlainText(QString(tr("Failure!")) + QString("\n"));
    return res;
}

void Widget::on_powerOffButton_released()
{
    _checkSecondsToAutoRebootTimer->stop();
    ui->progressBar->hide();
    ui->messageLabel->setText(QString(tr("Shutdown ...")));
    execDetachProcessWithQuit(QString("poweroff"));
}

void Widget::on_rebootButton_released()
{
    _checkSecondsToAutoRebootTimer->stop();
    ui->progressBar->hide();
    ui->messageLabel->setText(QString(tr("Rebooting ...")));
    execDetachProcessWithQuit(QString("reboot"));
}

void Widget::onCheckSecondsToAutoReboot()
{
    if (_countSecondsToAutoReboot < SEC_TO_AUTO_REBOOT) {
        ++_countSecondsToAutoReboot;
        progress(_countSecondsToAutoReboot);
        ui->messageLabel->setText(QString(tr("Rebooting after: %1 sec ...")).arg(QString::number(SEC_TO_AUTO_REBOOT - _countSecondsToAutoReboot)));
    }
    else {
        _checkSecondsToAutoRebootTimer->stop();
        ui->progressBar->hide();
        ui->messageLabel->setText(QString(tr("Rebooting ...")));
        execDetachProcessWithQuit(QString("reboot"));
    }
}

void Widget::onCurrentTabChanged(int index)
{
    cancelAutoReboot();
    switch (index) {
    case 0:
        ui->helpButton->hide();
        break;
    case 1:
        ui->backUpVersionLineEdit->clear();
        ui->currentVersionLineEdit->clear();
        ui->helpButton->show();
        _helper->setTitle(QString(QObject::tr("Help")));
        delay(100);
        checkBackUpAndCurrentVersion();
        break;
    case 2:
        ui->helpButton->hide();
        break;
    case 5:
        ui->helpButton->hide();
        browseLogDir();
        break;
    }
}

void Widget::on_checkUpdatesMainAppButton_released()
{
    blockControls(true);
    clearMessageWidget();
    ui->mainAppTextEdit->clear();
    ui->availableVersionLineEdit->clear();
    delay(100);
    if (checkIsUsbFlashMount() == false) {
        blockControls(false);
        return;
    }
    delay(100);
    ui->mainAppTextEdit->insertPlainText(QString(tr("Searching updates ...")) + QString("\n"));
    delay(100);
    if (checkAvailableVersion() == false) {
        blockControls(false);
        return;
    }
    delay(100);
    ui->mainAppTextEdit->insertPlainText(QString(tr("Checking FIRMWARE integrity ...")) + QString("\n"));
    delay(100);
    switch (checkFileHash(_flashDirPath)) {
    case SuccessCheckFiles:
        ui->mainAppTextEdit->insertPlainText(QString(tr("Success.")) + QString("\n"));
        break;
    case FailureCheckFiles:
        ui->mainAppTextEdit->insertPlainText(QString(tr("Failure!")) + QString("\n"));
        break;
    case NoCheckFile:
        ui->mainAppTextEdit->insertPlainText(QString(tr("Failure! File \"md5sum\" not found!")) + QString("\n"));
        break;
    }

    blockControls(false);
    sync();
}

void Widget::on_updateMainAppButton_released()
{
    blockControls(true);
    ui->mainAppTextEdit->clear();
    ui->currentVersionLineEdit->clear();
    resetProgressBar();
    ui->messageLabel->setText(QString(tr("Updating ...")));
    ui->mainAppTextEdit->insertPlainText(QString(tr("=== START UPDATE ===")) + QString("\n"));
    delay(100);
    progress(1);
    delay(100);
    if (checkIsUsbFlashMount() == false) {
        abortUpdateMainApp();
        return;
    }
    delay(100);
    progress(3);
    ui->mainAppTextEdit->insertPlainText(QString(tr("Searching updates ...")) + QString("\n"));
    delay(100);
    progress(4);
    checkAvailableVersion();
    ui->mainAppTextEdit->insertPlainText(QString(tr("Checking FIRMWARE integrity ...")) + QString("\n"));
    delay(100);
    progress(5);
    switch (checkFileHash(_flashDirPath)) {
    case SuccessCheckFiles:
        ui->mainAppTextEdit->insertPlainText(QString(tr("Success.")) + QString("\n"));
        break;
    case FailureCheckFiles:
        ui->mainAppTextEdit->insertPlainText(QString(tr("Failure!")) + QString("\n"));
        abortUpdateMainApp();
        return;
        break;
    case NoCheckFile:
        ui->mainAppTextEdit->insertPlainText(QString(tr("Failure! File \"md5sum\" not found!")) + QString("\n"));
        abortUpdateMainApp();
        return;
        break;
    }
    progress(7);
    if (updateMainApplication() == false) {
        abortUpdateMainApp();
        return;
    }
    ui->mainAppTextEdit->insertPlainText(QString(tr("Checking FIRMWARE integrity after update ... ")) + QString("\n"));
    delay(100);
    switch (checkFileHash(PROGRAM_DIR)) {
    case SuccessCheckFiles:
        ui->mainAppTextEdit->insertPlainText(QString(tr("Success.")) + QString("\n"));
        break;
    case FailureCheckFiles:
        ui->mainAppTextEdit->insertPlainText(QString(tr("Failure!")) + QString("\n"));
        break;
    case NoCheckFile:
        ui->mainAppTextEdit->insertPlainText(QString(tr("Failure! File \"md5sum\" not found!")) + QString("\n"));
        break;
    }
    delay(100);
    progress(8);
    checkCurrentVersion();
    progress(9);
    delay(1000);
    progress(10);
    delay(1000);
    ui->mainAppTextEdit->insertPlainText(QString(tr("=== FINISH UPDATE ===")) + QString("\n"));
    ui->progressBar->hide();
    ui->messageLabel->setText(QString(tr("Updated.")));
    blockControls(false);
    sync();
}

void Widget::on_createBackUpButton_released()
{
    blockControls(true);
    ui->mainAppTextEdit->clear();
    resetProgressBar();
    ui->messageLabel->setText(QString(tr("Creating backup ...")));
    ui->mainAppTextEdit->insertPlainText(QString(tr("=== START CREATING BACKUP ===")) + QString("\n"));
    delay(100);
    progress(1);
    ui->mainAppTextEdit->insertPlainText(QString(tr("Searching current backup ...")) + QString("\n"));
    delay(100);
    progress(2);
    if (checkBackUpVersion()) {
        ui->mainAppTextEdit->insertPlainText(QString(tr("Found.")) + QString("\n"));
        if (deletePreviousBackUp() == false) {
            ui->mainAppTextEdit->insertPlainText(QString(tr("=== ABORTED CREATING BACKUP!!! ===")) + QString("\n"));
            blockControls(false);
            return;
        }
    }
    else {
        ui->mainAppTextEdit->insertPlainText(QString(tr("NOT found!")) + QString("\n"));
    }
    delay(100);
    progress(3);
    if (createBackUp() == false) {
        ui->mainAppTextEdit->insertPlainText(QString(tr("=== ABORTED CREATING BACKUP!!! ===")) + QString("\n"));
        blockControls(false);
        return;
    }
    progress(7);
    delay(100);
    checkBackUpVersion();
    progress(9);
    delay(1000);
    progress(10);
    delay(1000);
    ui->mainAppTextEdit->insertPlainText(QString(tr("=== FINISH CREATING BACKUP ===")) + QString("\n"));
    ui->progressBar->hide();
    ui->messageLabel->setText(QString(tr("Backup was created.")));
    blockControls(false);
    sync();
}

void Widget::on_restoreBackUpButton_released()
{
    blockControls(true);
    ui->mainAppTextEdit->clear();
    resetProgressBar();
    ui->messageLabel->setText(QString(tr("Restoring ...")));
    ui->mainAppTextEdit->insertPlainText(QString(tr("=== START RESTORING ===")) + QString("\n"));
    delay(100);
    progress(1);
    ui->mainAppTextEdit->insertPlainText(QString(tr("Checking BACKUP file ...")) + QString("\n"));
    delay(100);
    progress(3);
    QFile file(PROGRAM_DIR + QString("/") + PROGRAM_NAME_BACKUP);
    if (file.exists()) {
        ui->mainAppTextEdit->insertPlainText(QString(tr("Found.")) + QString("\n"));
    }
    else {
        ui->mainAppTextEdit->insertPlainText(QString(tr("Not found.")) + QString("\n"));
        ui->mainAppTextEdit->insertPlainText(QString(tr("=== ABORTED RESTORING!!! ===")) + QString("\n"));
        blockControls(false);
        return;
    }
    delay(100);
    progress(5);
    ui->currentVersionLineEdit->clear();
    progress(6);
    delay(100);
    if (restoreFromBackUp() == false) {
        ui->mainAppTextEdit->insertPlainText(QString(tr("=== ABORTED RESTORING!!! ===")) + QString("\n"));
        blockControls(false);
        return;
    }
    progress(7);
    checkCurrentVersion();
    progress(8);
    delay(1000);
    progress(10);
    delay(1000);
    ui->mainAppTextEdit->insertPlainText(QString(tr("=== FINISH RESTORING ===")) + QString("\n"));
    ui->progressBar->hide();
    ui->messageLabel->setText(QString(tr("Restored.")));
    blockControls(false);
    sync();
}

void Widget::on_cancelAutoRebootButton_released()
{
    cancelAutoReboot();
}

void Widget::on_helpButton_released()
{
    _helper->reset();
    switch (ui->tabWidget->currentIndex()) {
    case 0:
        break;
    case 1:
        printInfoForUpdatingMainApp();
        break;
    case 2:
        break;
    case 3:
        break;
    }
    _helper->show();
}

void Widget::onMainAppTextEditChanged()
{
    QScrollBar* scrollBar = ui->mainAppTextEdit->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void Widget::on_checkOsUpdatesButton_released()
{
    blockControls(true);
    ui->osComponentsTextEdit->clear();
    delay(100);
    if (checkIsUsbFlashMount() == false) {
        blockControls(false);
        return;
    }
    delay(100);
    ui->osComponentsTextEdit->insertPlainText(QString(tr("Searching updates ...")) + QString("\n"));
    delay(100);
    checkAvailableOsUpdates();
    delay(100);
    sync();
    blockControls(false);
}

void Widget::on_updateOsButton_released()
{
    blockControls(true);
    ui->osComponentsTextEdit->clear();
    resetProgressBar();
    ui->messageLabel->setText(QString(tr("Updating ...")));
    ui->osComponentsTextEdit->insertPlainText(QString(tr("=== START UPDATE ===")) + QString("\n"));
    delay(100);
    progress(1);
    delay(100);
    if (checkIsUsbFlashMount() == false) {
        abortUpdateOsComponentsApp();
        return;
    }
    delay(100);
    ui->osComponentsTextEdit->insertPlainText(QString(tr("Searching updates ...")) + QString("\n"));
    delay(100);
    checkAvailableOsUpdates();
    delay(100);
    progress(3);
    if (updateOsComponents() == false) {
        abortUpdateOsComponentsApp();
        return;
    }
    delay(100);
    progress(6);
    delay(100);
    progress(9);
    progress(10);
    sync();
    ui->osComponentsTextEdit->insertPlainText(QString(tr("=== FINISH UPDATE ===")) + QString("\n"));
    ui->progressBar->hide();
    ui->messageLabel->setText(QString(tr("Updated.")));
    blockControls(false);
}

void Widget::on_viewLogButton_released()
{
    clearMessageWidget();
    if (ui->logFilesListWidget->currentRow() >= 0) {
        _helper->reset();
        QString fileName = ui->logFilesListWidget->currentItem()->text();
        QFile file(PROGRAM_LOGS_DIR + fileName);
        if (file.open(QIODevice::Text | QIODevice::ReadOnly)) {
            while (!file.atEnd()) {
                QString line(file.readLine());
                _helper->insertText(line);
            }
        }
        _helper->setTitle(fileName);
        file.close();
        _helper->show();
    }
}

void Widget::on_copyLogsButton_released()
{
    blockControls(true);
    if (checkIsUsbFlashMount() == false) {
        blockControls(false);
        return;
    }

    QString logsFlashPath = _flashDirPath + QString("/avicon31_logs");
    if (QDir(logsFlashPath).exists() == false) {
        if (QDir().mkdir(logsFlashPath) == false) {
            blockControls(false);
            return;
        }
    }

    QDir dir(PROGRAM_LOGS_DIR);

    if (dir.exists()) {
        dir.refresh();
        QFileInfoList fileList = dir.entryInfoList(QDir::Files);
        int count = 0;
        ui->progressBar->setMaximum(fileList.count() - 1);
        ui->progressBar->setValue(0);
        ui->progressBar->show();
        ui->messageLabel->setText(QString(tr("Copying files ...")));
        foreach (const QFileInfo& fileInfo, fileList) {
            QString sourcePath = fileInfo.absoluteFilePath();
            QString destinationPath = logsFlashPath + "/" + fileInfo.fileName();
            QFile::copy(sourcePath, destinationPath);
            ++count;
            progress(count);
            delay(10);
        }
        sync();
        ui->progressBar->hide();
        ui->messageLabel->setText(QString(tr("Copied.")));
    }
    blockControls(false);
}

void Widget::on_updateUMUFirmwareButton_released()
{
    ui->umuInfoLabel->setText(tr("UMU update in progress..."));
    ui->progressBar->show();
    ui->progressBar->setMaximum(100);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(2);
    qDebug() << "UMU power disabled...";
    setUMUPower(false);
    ui->umuLogTextEdit->appendPlainText(tr("UMU power disabled...") + "\n");
    delay(2000);

    ui->progressBar->setValue(12);

    update();
    delay(100);
    ui->progressBar->setValue(15);


    ui->progressBar->setValue(17);
    delay(100);
    ui->progressBar->setValue(20);
    setUMUPower(true);
    ui->umuLogTextEdit->appendPlainText(tr("UMU power enabled...") + "\n");
    ui->progressBar->setValue(30);
    for (int i = 0; i < 6; ++i) {
        delay(1000);
        ui->progressBar->setValue(30 + i);
        update();
    }


    ui->progressBar->setValue(72);
    update();
    qDebug() << "Starting program engine...";
    _UMUengine = new ProgramEngine(UMU_FTP_URL, UMU_FIRMWARE_PATH);
    connect(_UMUengine, SIGNAL(writeMessage(QString)), this, SLOT(onUMUWriteMessage(QString)));
    connect(_UMUengine, SIGNAL(reWriteMessage(QString)), this, SLOT(onUMUReWriteMessage(QString)));
    connect(_UMUengine, SIGNAL(sUploadProgressChanged(qint64, qint64)), this, SLOT(onUMUUploadProgressChanged(qint64, qint64)));
    connect(_UMUengine, SIGNAL(sInterfaceCommand(unsigned int)), this, SLOT(onUMUInterfaceCommand(unsigned int)));
    _UMUengine->start();
    ui->progressBar->setValue(100);
    ui->updateUMUFirmwareButton->setEnabled(false);
}

void Widget::on_cancelUMUUpdateButton_released()
{
    ui->progressBar->hide();
    ui->progressBar->setMaximum(100);
    ui->progressBar->setMinimum(0);

    if (_UMUengine != 0) {
        _UMUengine->stop();
        disconnect(_UMUengine, SIGNAL(writeMessage(QString)), this, SLOT(onUMUWriteMessage(QString)));
        disconnect(_UMUengine, SIGNAL(reWriteMessage(QString)), this, SLOT(onUMUReWriteMessage(QString)));
        disconnect(_UMUengine, SIGNAL(sUploadProgressChanged(qint64, qint64)), this, SLOT(onUMUUploadProgressChanged(qint64, qint64)));
        disconnect(_UMUengine, SIGNAL(sInterfaceCommand(unsigned int)), this, SLOT(onUMUInterfaceCommand(unsigned int)));
        delete _UMUengine;
        _UMUengine = 0;
    }
    ui->updateUMUFirmwareButton->setEnabled(true);
}

void Widget::onUMUWriteMessage(QString s)
{
    ui->umuLogTextEdit->appendPlainText(s);
}

void Widget::onUMUReWriteMessage(QString s)
{
    ui->umuLogTextEdit->appendPlainText(s);
}

void Widget::onUMUUploadProgressChanged(qint64 progress, qint64 total)
{
    ui->progressBar->setValue(qFloor(progress * 100 / total));
}

void Widget::onUMUInterfaceCommand(unsigned int command)
{
    switch (command) {
    case ProgramEngine::DisableUserCancellation: {
        ui->cancelUMUUpdateButton->setEnabled(false);
        break;
    }
    case ProgramEngine::EnableUserCancellation: {
        ui->cancelUMUUpdateButton->setEnabled(true);
        break;
    }
    case ProgramEngine::GoToNextStep: {
        umuReboot();
        break;
    }
    default:
        break;
    }
}

void Widget::setUMUPower(bool value)
{
    qDebug() << "UMU power:" << value;
    QString umuPowerGPIO("/proc/gpio/gpo-3");
    QFile file(umuPowerGPIO);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text) == false) {
        qWarning() << "Error of open file '" << umuPowerGPIO << "':" << file.errorString();
        return;
    }

    file.putChar(value ? '1' : '0');

    file.close();
    delay(100);
    sync();
}

void Widget::on_checkUMUUpdatesButton_released()
{
    ui->progressBar->show();
    ui->progressBar->setMaximum(100);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(3);
    sync();
    delay(100);
    ui->progressBar->setValue(9);
    ui->umuLogTextEdit->insertPlainText(QString(tr("Searching for USB device ...")) + QString("\n"));
    if (checkIsUsbFlashMount() == false) {
        ui->umuLogTextEdit->insertPlainText(QString(tr("USB device not found ...")) + QString("\n"));
        ui->checkUMUUpdatesButton->setEnabled(true);
        ui->updateUMUFirmwareButton->setEnabled(false);
        ui->umuInfoLabel->setText(tr("USB device not found!"));
        ui->progressBar->setValue(100);
        sync();
        delay(100);
        ui->progressBar->hide();
        return;
    }
    ui->progressBar->setValue(20);

    delay(100);
    ui->umuLogTextEdit->insertPlainText(QString(tr("Searching firmware ...")) + QString("\n"));
    delay(100);

    QString firmwareOnUsb = _flashDirPath + "/umu-firmware";
    QDir dir(firmwareOnUsb);
    dir.setFilter(QDir::Files);
    const auto& list = dir.entryList();
    for (const QString& filename : list) {
        qDebug() << "Found file" << filename;
    }
    ui->progressBar->setValue(25);

    sync();
    QDir oldDirectory("/root/umu-firmware");
    oldDirectory.removeRecursively();
    sync();
    ui->progressBar->setValue(30);

    QDir outDirectory("/root");
    outDirectory.mkdir("umu-firmware");
    ui->progressBar->setValue(35);

    for (const QString& filename : list) {
        qDebug() << "Copying: from" << firmwareOnUsb + "/" + filename << " to " << outDirectory.path() + "/umu-firmware/" + filename;
        QFile::copy(firmwareOnUsb + "/" + filename, outDirectory.path() + "/umu-firmware/" + filename);
        ui->umuLogTextEdit->insertPlainText(tr("File ") + filename + tr(" found") + "\n");
    }
    ui->progressBar->setValue(80);

    delay(100);
    sync();
    ui->progressBar->setValue(90);
    ui->umuLogTextEdit->insertPlainText(tr("Ready for update!\n"));
    ui->umuInfoLabel->setText(tr("UMU firmware found! Ready for update!"));
    ui->updateUMUFirmwareButton->setEnabled(true);
    ui->progressBar->hide();
    ui->progressBar->setValue(100);
}

void Widget::umuReboot()
{
    ui->progressBar->show();
    ui->progressBar->setMaximum(100);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(8);
    ui->umuLogTextEdit->appendPlainText(tr("Upload finished, rebooting...") + "\n");
    delay(100);
    ui->progressBar->setValue(15);
    setUMUPower(false);
    ui->umuLogTextEdit->appendPlainText(tr("UMU power disabled...") + "\n");
    delay(1000);
    ui->progressBar->setValue(23);
    setUMUPower(true);
    delay(100);
    ui->progressBar->setValue(26);
    ui->umuLogTextEdit->appendPlainText(tr("UMU power enabled...") + "\n");
    delay(1000);
    ui->progressBar->setValue(30);
    ui->umuLogTextEdit->appendPlainText(tr("Please wait...") + "\n");

    for (int i = 0; i < 60; ++i) {
        ui->progressBar->setValue(30 + i);
        update();
        delay(1000);
    }

    ui->progressBar->setValue(100);
    ui->umuLogTextEdit->appendPlainText(tr("Finished!") + "\n");
    setUMUPower(false);
    delay(100);
    ui->umuInfoLabel->setText(tr("UMU firmware updated successfully!"));
    ui->progressBar->hide();
}

void Widget::on_checkSettingsButton_released()
{
    ui->settingsLogTextEdit->clear();
    ui->progressBar->show();
    ui->progressBar->setMaximum(100);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(10);
    ui->settingsLogTextEdit->appendPlainText(tr("Searching for settings..."));
    sync();
    delay(100);
    ui->progressBar->setValue(30);

    if (checkIsUsbFlashMount() == false) {
        ui->saveSettingsButton->setEnabled(false);
        ui->restoreSettingsButton->setEnabled(false);
        ui->settingsLogTextEdit->appendPlainText(tr("USB drive not found!"));
        ui->progressBar->setValue(100);
        sync();
        delay(100);
        ui->progressBar->hide();
        return;
    }
    ui->settingsLogTextEdit->appendPlainText(tr("USB drive found!"));
    ui->saveSettingsButton->setEnabled(true);
    ui->progressBar->setValue(50);

    QDir dir(_flashDirPath);
    dir.setFilter(QDir::Files);
    const auto& list = dir.entryList();
    bool hasSettings = false;
    for (const QString& filename : list) {
        qDebug() << "Found file" << filename;
        if (filename == QString("Settings.backup")) {
            hasSettings = true;
        }
    }
    ui->progressBar->setValue(75);
    if (hasSettings) {
        ui->settingsLogTextEdit->appendPlainText(tr("Settings found!"));
        ui->restoreSettingsButton->setEnabled(true);
    }
    else {
        ui->settingsLogTextEdit->appendPlainText(tr("Backup not found!"));
        ui->restoreSettingsButton->setEnabled(false);
    }
    ui->progressBar->setValue(100);
    delay(100);
    sync();
    ui->progressBar->hide();
}

void Widget::on_saveSettingsButton_released()
{
    // zip --password qwerty Settings.zip umu-firmware
    ui->settingsLogTextEdit->clear();
    ui->progressBar->show();
    ui->progressBar->setMaximum(100);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(10);
    ui->settingsLogTextEdit->appendPlainText(tr("Saving backup..."));
    sync();
    QDir oldDirectory("/tmp/settings-backup");
    oldDirectory.removeRecursively();
    sync();
    QDir outDirectory("/tmp");
    outDirectory.mkdir("settings-backup");
    ui->progressBar->setValue(35);
    sync();
    QFile oldCalibration("/tmp/settings-backup/calibration.dat");
    oldCalibration.remove();
    sync();
    QFile::copy("/root/.avicon-31/calibration.dat", "/tmp/settings-backup/calibration.dat");
    ui->progressBar->setValue(45);
    QFile oldSettings("/tmp/settings-backup/avicon-31.conf");
    oldSettings.remove();
    sync();
    QFile::copy("/root/.config/Radioavionica/avicon-31.conf", "/tmp/settings-backup/avicon-31.conf");
    ui->progressBar->setValue(55);
    sync();
    ui->progressBar->setValue(60);

    QFile oldArchive("/tmp/Settings.zip");
    oldArchive.remove();
    sync();
    ui->progressBar->setValue(65);

    QString command("cd /tmp; zip -r --password z8u20Yk01Qw524 Settings.zip settings-backup");
    QProcess process;
    process.start("sh", QStringList() << "-c" << command);
    process.waitForStarted();
    if (process.waitForFinished()) {
        ui->settingsLogTextEdit->appendPlainText(tr("Settings packed!"));
    }
    else {
        ui->settingsLogTextEdit->appendPlainText(tr("Error! Cannot pack settings!"));
        ui->progressBar->setValue(100);
        sync();
        delay(100);
        return;
    }
    ui->progressBar->setValue(70);
    sync();
    if (checkIsUsbFlashMount()) {
        QFile checkExistFile(_flashDirPath + "/Settings.backup");
        if (checkExistFile.exists()) {
            checkExistFile.remove();
            sync();
        }
        QFile::copy("/tmp/Settings.zip", _flashDirPath + "/Settings.backup");
        ui->settingsLogTextEdit->appendPlainText(tr("Backup saved!"));
    }
    else {
        ui->settingsLogTextEdit->appendPlainText(tr("Error! Backup not saved!"));
    }
    ui->progressBar->setValue(80);
    sync();

    ui->settingsLogTextEdit->appendPlainText(tr("Finished!"));
    ui->progressBar->setValue(100);
    ui->progressBar->hide();
}

void Widget::on_restoreSettingsButton_released()
{
    // unzip -P z8u20Yk01Qw524 Settings.zip
    ui->settingsLogTextEdit->clear();
    ui->progressBar->show();
    ui->progressBar->setMaximum(100);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(10);
    ui->settingsLogTextEdit->appendPlainText(tr("Restoring settings..."));
    sync();
    QFile oldArchive("/tmp/Settings.zip");
    oldArchive.remove();
    sync();
    ui->progressBar->setValue(13);
    QDir oldBackupDirectory("/tmp/settings-backup");
    oldBackupDirectory.removeRecursively();
    ui->progressBar->setValue(15);
    sync();
    if (QFile("/tmp/Settings.zip").exists() == false) {
        ui->settingsLogTextEdit->appendPlainText(tr("Error! Settings not found!"));
        ui->progressBar->hide();
        return;
    }
    sync();
    ui->progressBar->setValue(20);

    ui->settingsLogTextEdit->appendPlainText(tr("Unpacking settings..."));

    QString command("cd /tmp; unzip -P z8u20Yk01Qw524 Settings.zip");
    QProcess process;
    process.start("sh", QStringList() << "-c" << command);
    process.waitForStarted();
    if (process.waitForFinished()) {
        ui->settingsLogTextEdit->appendPlainText(tr("Settings unpacked!"));
    }
    else {
        ui->settingsLogTextEdit->appendPlainText(tr("Error! Cannot unpack settings!"));
        ui->progressBar->setValue(100);
        sync();
        delay(100);
        return;
    }
    ui->progressBar->setValue(30);


    sync();
    QFile oldCalibration("/root/.avicon-31/calibration.dat");
    oldCalibration.remove();
    ui->progressBar->setValue(55);
    sync();
    QFile::copy("/tmp/settings-backup/calibration.dat", "/root/.avicon-31/calibration.dat");
    ui->progressBar->setValue(65);

    sync();
    QFile oldSettings("/root/.config/Radioavionica/avicon-31.conf");
    oldSettings.remove();
    ui->progressBar->setValue(75);
    sync();
    QFile::copy("/tmp/settings-backup/avicon-31.conf", "/root/.config/Radioavionica/avicon-31.conf");
    ui->progressBar->setValue(85);

    ui->settingsLogTextEdit->appendPlainText(tr("Settings restored!"));
    sync();
    ui->settingsLogTextEdit->appendPlainText(tr("Finished!"));
    ui->progressBar->setValue(100);
    ui->progressBar->hide();
}
