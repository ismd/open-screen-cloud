#ifndef SCREENSHOTGUN_SETTINGSFORM_H
#define SCREENSHOTGUN_SETTINGSFORM_H

#include <QDialog>
#include "Settings.h"

class App;

namespace Ui {
    class Settings;
}

class SettingsForm : public QDialog {
    Q_OBJECT

public:
    explicit SettingsForm(App&);
    ~SettingsForm();

    bool valid() const;
    void saveValues();
    void setError(const QString&);
    void show();

private slots:
    void accept();

private:
    Ui::Settings* ui;
    App& app_;
    Settings settings;
};

#endif // SCREENSHOTGUN_SETTINGSFORM_H
