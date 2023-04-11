#ifndef GLOBAL_H
#define GLOBAL_H

#include <QApplication>
#include <QString>

#define APP_RESTART_CODE        31415

#define RELEASE_VERSION "3"
#define BUILD_NUMBER ".6" //.1
#define VERSION "49"
#define SOFTWARE_VERSION RELEASE_VERSION + QString(".23.03.") + VERSION + BUILD_NUMBER


#define DATABASE_INFO_TABLE                 "databaseInfo"
#define VCH_003_DATABASE_VERSION            "1.0"      // Это нужно для того, чтобы понять вдруг изменился формат таблиц. В этом случае нужно удалить старую базу



#define APPLICATION_PATH                    QApplication::applicationDirPath()

#define DEBUG_LOG_DIR_PATH                  APPLICATION_PATH + "/../debug_log"
#define CONFIG_DIR_PATH                     APPLICATION_PATH + "/../config"
#define DATA_DIR_PATH                       APPLICATION_PATH + "/../data"
#define LOG_DIR_PATH                        APPLICATION_PATH + "/../log"
#define DEVICE_BACKUP_DIR_PATH              APPLICATION_PATH + "/../backup"
#define MASK_DATA_DIR_PATH                  APPLICATION_PATH + "/../masks"
#define TEMP_DIR_PATH                       APPLICATION_PATH + "/../temp"

#define DATABASE_FILE_NAME                  "ssu.db"
#define SETTINGS_INI_FILE_NAME              "settings.ini"
#define DOWLONAD_TEMP_FILE_NAME             "download_temp"

// Device backup
#define DEVICE_CARD_CONFIG_FILE_NAME        "SSU.CardConfig.backup"
#define DEVICE_DATABASE_FILE_NAME           "SSU.Data.backup"
#define DEVICE_PANEL_CONFIG_FILE_NAME       "cards.config"

#define WINDOW_SETTINGS_GROUP                   "WINDOW_SETTINGS"
#define GENERAL_SETTINGS_GROUP                  "GENERAL_SETTINGS"
#define EVENTS_SETTINGS_GROUP                   "EVENTS_SETTINGS"
#define ALARM_COLOR_INDICATIONS_SETTINGS_GROUP  "ALARM_COLOR_INDICATIONS_SETTINGS_GROUP"

#define CONNECTION_SETTINGS_GROUP           "CONNECTIONS"
#define MASK_INFO_GROUP                     "MASK"
#define UPDATER_GROUP                       "UPDATER"

#define MASK_FILE_SUFFIX                    "mask"





#endif // GLOBAL_H
