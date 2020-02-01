#ifndef SETTINGS_DEF_H
#define SETTINGS_DEF_H

#include <QSettings>
#include <QString>
#include <QDateTime>
#include <QDebug>

#define KEY_LAST_FILE "syncfolder/last_opened_file"
#define KEY_LAST_FOLDER "syncfolder/last_opened_folder"
#define KEY_LAST_WIN_WIDTH "syncfolder/last_win_width"
#define KEY_LAST_WIN_HEIGHT "syncfolder/last_win_height"
#define KEY_LAST_PRIMARY_FONT_SIZE "syncfolder/last_primary_font_size"
#define KEY_LAST_CHECK_UPDATE "syncfolder/last_check_update"
#define KEY_API_ENDPOINT "syncfolder/api_endpoint"
#define KEY_API_USERNAME "syncfolder/api_username"
#define KEY_API_PASSWORD "syncfolder/api_password"

class SyncFolderSettings {
public:
    static inline QString getString(const QString &key) {
        QSettings settings;
        return settings.value(key).toString();
    }

    static inline int getInt(const QString &key) {
        QSettings settings;
        return settings.value(key, 0).toInt();
    }

    static inline QDateTime getDateTime(const QString &key) {
        QSettings settings;
        return settings.value(key, QDateTime::fromMSecsSinceEpoch(0)).toDateTime();
    }

    static inline void setDateTime(const QString &key, const QDateTime time) {
        QSettings settings;
        settings.setValue(key, time);
    }

    static inline int getInt(const QString &key, int def) {
        QSettings settings;
        return settings.value(key, def).toInt();
    }

    static inline void setString(const QString &key, const QString &val) {
        QSettings settings;
        settings.setValue(key, val);
    }

    static inline void setInt(const QString &key, int val) {
        QSettings settings;
        settings.setValue(key, val);
    }
};

#endif
