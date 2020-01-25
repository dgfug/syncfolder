#ifndef SETTINGS_DEF_H
#define SETTINGS_DEF_H

#include <QSettings>
#include <QString>

#define KEY_LAST_FILE "dmeditor/last_opened_file"
#define KEY_LAST_FOLDER "dmeditor/last_opened_folder"
#define KEY_LAST_WIN_WIDTH "dmeditor/last_win_width"
#define KEY_LAST_WIN_HEIGHT "dmeditor/last_win_height"
#define KEY_LAST_PRIMARY_FONT_SIZE "dmeditor/last_primary_font_size"

class DMSettings {
public:
    static inline QString getString(const QString &key) {
        QSettings settings;
        return settings.value(key).toString();
    }

    static inline int getInt(const QString &key) {
        QSettings settings;
        return settings.value(key, 0).toInt();
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
