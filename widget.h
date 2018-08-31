#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QProcess>

#include "helper.h"
#include "../umu-firmware-updater/programengine.h"

namespace Ui
{
class Widget;
}

enum CheckFileHashError
{
    SuccessCheckFiles,
    FailureCheckFiles,
    NoCheckFile,
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget* parent = 0);
    ~Widget();

private:
    bool createBackUp();
    bool restoreFromBackUp();
    bool checkBackUpVersion();
    bool checkCurrentVersion();
    bool checkIsUsbFlashMount();
    bool checkUMUFlashFirmware();
    bool deletePreviousBackUp();
    bool checkAvailableVersion();
    bool updateMainApplication();
    bool checkAvailableOsUpdates();
    bool updateOsComponents();
    CheckFileHashError checkFileHash(QString path);

    void reboot();
    void poweroff();
    void startAutoReboot();
    void cancelAutoReboot();
    void resetProgressBar();
    void delay(int msToWait);
    void progress(int value);
    void clearMessageWidget();
    void blockControls(bool isBlock);
    void checkBackUpAndCurrentVersion();
    void execDetachProcessWithQuit(QString command);

    void printInfoForUpdatingMainApp();
    void abortUpdateMainApp();
    void abortUpdateOsComponentsApp();
    void browseLogDir();
    void umuReboot();

private slots:
    void on_rebootButton_released();
    void on_powerOffButton_released();
    void on_cancelAutoRebootButton_released();
    void onCheckSecondsToAutoReboot();
    void onCurrentTabChanged(int index);
    void on_checkUpdatesMainAppButton_released();
    void on_createBackUpButton_released();
    void on_restoreBackUpButton_released();
    void on_updateMainAppButton_released();

    void on_helpButton_released();
    void onMainAppTextEditChanged();
    void on_checkOsUpdatesButton_released();
    void on_updateOsButton_released();

    void on_viewLogButton_released();
    void on_copyLogsButton_released();

    void on_updateUMUFirmwareButton_released();

    void on_cancelUMUUpdateButton_released();

    void onUMUWriteMessage(QString s);
    void onUMUReWriteMessage(QString s);
    void onUMUUploadProgressChanged(qint64 progress, qint64 total);
    void onUMUInterfaceCommand(unsigned int command);
    void setUMUPower(bool value);


    void on_checkUMUUpdatesButton_released();

    void on_checkSettingsButton_released();
    void on_saveSettingsButton_released();
    void on_restoreSettingsButton_released();

private:
    Ui::Widget* ui;
    ProgramEngine* _UMUengine;

    int _countSecondsToAutoReboot;
    QTimer* _checkSecondsToAutoRebootTimer;
    QString _currentMainAppVersion;
    QString _currentBackUpMainAppVersion;
    QString _availableMainAppVersion;
    QString _flashDirPath;
    QString _homeTmpPath;
    Helper* _helper;
};

#endif  // WIDGET_H
