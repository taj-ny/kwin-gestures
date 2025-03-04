#pragma once

#include "libgestures/actions/command.h"
#include "libgestures/actions/input.h"
#include "libgestures/actions/plasmaglobalshortcut.h"
#include "libgestures/gestures/gesturerecognizer.h"

#include <QRegularExpression>
#include <QVector>

#include "yaml-cpp/yaml.h"

static const std::unordered_map<QString, uint32_t> s_keyboard = {
    {"RESERVED", KEY_RESERVED},
    {"ESC", KEY_ESC},
    {"1", KEY_1},
    {"2", KEY_2},
    {"3", KEY_3},
    {"4", KEY_4},
    {"5", KEY_5},
    {"6", KEY_6},
    {"7", KEY_7},
    {"8", KEY_8},
    {"9", KEY_9},
    {"0", KEY_0},
    {"MINUS", KEY_MINUS},
    {"EQUAL", KEY_EQUAL},
    {"BACKSPACE", KEY_BACKSPACE},
    {"TAB", KEY_TAB},
    {"Q", KEY_Q},
    {"W", KEY_W},
    {"E", KEY_E},
    {"R", KEY_R},
    {"T", KEY_T},
    {"Y", KEY_Y},
    {"U", KEY_U},
    {"I", KEY_I},
    {"O", KEY_O},
    {"P", KEY_P},
    {"LEFTBRACE", KEY_LEFTBRACE},
    {"RIGHTBRACE", KEY_RIGHTBRACE},
    {"ENTER", KEY_ENTER},
    {"LEFTCTRL", KEY_LEFTCTRL},
    {"A", KEY_A},
    {"S", KEY_S},
    {"D", KEY_D},
    {"F", KEY_F},
    {"G", KEY_G},
    {"H", KEY_H},
    {"J", KEY_J},
    {"K", KEY_K},
    {"L", KEY_L},
    {"SEMICOLON", KEY_SEMICOLON},
    {"APOSTROPHE", KEY_APOSTROPHE},
    {"GRAVE", KEY_GRAVE},
    {"LEFTSHIFT", KEY_LEFTSHIFT},
    {"BACKSLASH", KEY_BACKSLASH},
    {"Z", KEY_Z},
    {"X", KEY_X},
    {"C", KEY_C},
    {"V", KEY_V},
    {"B", KEY_B},
    {"N", KEY_N},
    {"M", KEY_M},
    {"COMMA", KEY_COMMA},
    {"DOT", KEY_DOT},
    {"SLASH", KEY_SLASH},
    {"RIGHTSHIFT", KEY_RIGHTSHIFT},
    {"KPASTERISK", KEY_KPASTERISK},
    {"LEFTALT", KEY_LEFTALT},
    {"SPACE", KEY_SPACE},
    {"CAPSLOCK", KEY_CAPSLOCK},
    {"F1", KEY_F1},
    {"F2", KEY_F2},
    {"F3", KEY_F3},
    {"F4", KEY_F4},
    {"F5", KEY_F5},
    {"F6", KEY_F6},
    {"F7", KEY_F7},
    {"F8", KEY_F8},
    {"F9", KEY_F9},
    {"F10", KEY_F10},
    {"NUMLOCK", KEY_NUMLOCK},
    {"SCROLLLOCK", KEY_SCROLLLOCK},
    {"KP7", KEY_KP7},
    {"KP8", KEY_KP8},
    {"KP9", KEY_KP9},
    {"KPMINUS", KEY_KPMINUS},
    {"KP4", KEY_KP4},
    {"KP5", KEY_KP5},
    {"KP6", KEY_KP6},
    {"KPPLUS", KEY_KPPLUS},
    {"KP1", KEY_KP1},
    {"KP2", KEY_KP2},
    {"KP3", KEY_KP3},
    {"KP0", KEY_KP0},
    {"KPDOT", KEY_KPDOT},
    {"ZENKAKUHANKAKU", KEY_ZENKAKUHANKAKU},
    {"102ND", KEY_102ND},
    {"F11", KEY_F11},
    {"F12", KEY_F12},
    {"RO", KEY_RO},
    {"KATAKANA", KEY_KATAKANA},
    {"HIRAGANA", KEY_HIRAGANA},
    {"HENKAN", KEY_HENKAN},
    {"KATAKANAHIRAGANA", KEY_KATAKANAHIRAGANA},
    {"MUHENKAN", KEY_MUHENKAN},
    {"KPJPCOMMA", KEY_KPJPCOMMA},
    {"KPENTER", KEY_KPENTER},
    {"RIGHTCTRL", KEY_RIGHTCTRL},
    {"KPSLASH", KEY_KPSLASH},
    {"SYSRQ", KEY_SYSRQ},
    {"RIGHTALT", KEY_RIGHTALT},
    {"LINEFEED", KEY_LINEFEED},
    {"HOME", KEY_HOME},
    {"UP", KEY_UP},
    {"PAGEUP", KEY_PAGEUP},
    {"LEFT", KEY_LEFT},
    {"RIGHT", KEY_RIGHT},
    {"END", KEY_END},
    {"DOWN", KEY_DOWN},
    {"PAGEDOWN", KEY_PAGEDOWN},
    {"INSERT", KEY_INSERT},
    {"DELETE", KEY_DELETE},
    {"MACRO", KEY_MACRO},
    {"MUTE", KEY_MUTE},
    {"VOLUMEDOWN", KEY_VOLUMEDOWN},
    {"VOLUMEUP", KEY_VOLUMEUP},
    {"POWER", KEY_POWER},
    {"KPEQUAL", KEY_KPEQUAL},
    {"KPPLUSMINUS", KEY_KPPLUSMINUS},
    {"PAUSE", KEY_PAUSE},
    {"SCALE", KEY_SCALE},
    {"KPCOMMA", KEY_KPCOMMA},
    {"HANGEUL", KEY_HANGEUL},
    {"HANJA", KEY_HANJA},
    {"YEN", KEY_YEN},
    {"LEFTMETA", KEY_LEFTMETA},
    {"RIGHTMETA", KEY_RIGHTMETA},
    {"COMPOSE", KEY_COMPOSE},
    {"STOP", KEY_STOP},
    {"AGAIN", KEY_AGAIN},
    {"PROPS", KEY_PROPS},
    {"UNDO", KEY_UNDO},
    {"FRONT", KEY_FRONT},
    {"COPY", KEY_COPY},
    {"OPEN", KEY_OPEN},
    {"PASTE", KEY_PASTE},
    {"FIND", KEY_FIND},
    {"CUT", KEY_CUT},
    {"HELP", KEY_HELP},
    {"MENU", KEY_MENU},
    {"CALC", KEY_CALC},
    {"SETUP", KEY_SETUP},
    {"SLEEP", KEY_SLEEP},
    {"WAKEUP", KEY_WAKEUP},
    {"FILE", KEY_FILE},
    {"SENDFILE", KEY_SENDFILE},
    {"DELETEFILE", KEY_DELETEFILE},
    {"XFER", KEY_XFER},
    {"PROG1", KEY_PROG1},
    {"PROG2", KEY_PROG2},
    {"WWW", KEY_WWW},
    {"MSDOS", KEY_MSDOS},
    {"COFFEE", KEY_COFFEE},
    {"ROTATE_DISPLAY", KEY_ROTATE_DISPLAY},
    {"CYCLEWINDOWS", KEY_CYCLEWINDOWS},
    {"MAIL", KEY_MAIL},
    {"BOOKMARKS", KEY_BOOKMARKS},
    {"COMPUTER", KEY_COMPUTER},
    {"BACK", KEY_BACK},
    {"FORWARD", KEY_FORWARD},
    {"CLOSECD", KEY_CLOSECD},
    {"EJECTCD", KEY_EJECTCD},
    {"EJECTCLOSECD", KEY_EJECTCLOSECD},
    {"NEXTSONG", KEY_NEXTSONG},
    {"PLAYPAUSE", KEY_PLAYPAUSE},
    {"PREVIOUSSONG", KEY_PREVIOUSSONG},
    {"STOPCD", KEY_STOPCD},
    {"RECORD", KEY_RECORD},
    {"REWIND", KEY_REWIND},
    {"PHONE", KEY_PHONE},
    {"ISO", KEY_ISO},
    {"CONFIG", KEY_CONFIG},
    {"HOMEPAGE", KEY_HOMEPAGE},
    {"REFRESH", KEY_REFRESH},
    {"EXIT", KEY_EXIT},
    {"MOVE", KEY_MOVE},
    {"EDIT", KEY_EDIT},
    {"SCROLLUP", KEY_SCROLLUP},
    {"SCROLLDOWN", KEY_SCROLLDOWN},
    {"KPLEFTPAREN", KEY_KPLEFTPAREN},
    {"KPRIGHTPAREN", KEY_KPRIGHTPAREN},
    {"NEW", KEY_NEW},
    {"REDO", KEY_REDO},
    {"F13", KEY_F13},
    {"F14", KEY_F14},
    {"F15", KEY_F15},
    {"F16", KEY_F16},
    {"F17", KEY_F17},
    {"F18", KEY_F18},
    {"F19", KEY_F19},
    {"F20", KEY_F20},
    {"F21", KEY_F21},
    {"F22", KEY_F22},
    {"F23", KEY_F23},
    {"F24", KEY_F24},
    {"PLAYCD", KEY_PLAYCD},
    {"PAUSECD", KEY_PAUSECD},
    {"PROG3", KEY_PROG3},
    {"PROG4", KEY_PROG4},
    {"ALL_APPLICATIONS", KEY_ALL_APPLICATIONS},
    {"SUSPEND", KEY_SUSPEND},
    {"CLOSE", KEY_CLOSE},
    {"PLAY", KEY_PLAY},
    {"FASTFORWARD", KEY_FASTFORWARD},
    {"BASSBOOST", KEY_BASSBOOST},
    {"PRINT", KEY_PRINT},
    {"HP", KEY_HP},
    {"CAMERA", KEY_CAMERA},
    {"SOUND", KEY_SOUND},
    {"QUESTION", KEY_QUESTION},
    {"EMAIL", KEY_EMAIL},
    {"CHAT", KEY_CHAT},
    {"SEARCH", KEY_SEARCH},
    {"CONNECT", KEY_CONNECT},
    {"FINANCE", KEY_FINANCE},
    {"SPORT", KEY_SPORT},
    {"SHOP", KEY_SHOP},
    {"ALTERASE", KEY_ALTERASE},
    {"CANCEL", KEY_CANCEL},
    {"BRIGHTNESSDOWN", KEY_BRIGHTNESSDOWN},
    {"BRIGHTNESSUP", KEY_BRIGHTNESSUP},
    {"MEDIA", KEY_MEDIA},
    {"SWITCHVIDEOMODE", KEY_SWITCHVIDEOMODE},
    {"KBDILLUMTOGGLE", KEY_KBDILLUMTOGGLE},
    {"KBDILLUMDOWN", KEY_KBDILLUMDOWN},
    {"KBDILLUMUP", KEY_KBDILLUMUP},
    {"SEND", KEY_SEND},
    {"REPLY", KEY_REPLY},
    {"FORWARDMAIL", KEY_FORWARDMAIL},
    {"SAVE", KEY_SAVE},
    {"DOCUMENTS", KEY_DOCUMENTS},
    {"BATTERY", KEY_BATTERY},
    {"BLUETOOTH", KEY_BLUETOOTH},
    {"WLAN", KEY_WLAN},
    {"UWB", KEY_UWB},
    {"UNKNOWN", KEY_UNKNOWN},
    {"VIDEO_NEXT", KEY_VIDEO_NEXT},
    {"VIDEO_PREV", KEY_VIDEO_PREV},
    {"BRIGHTNESS_CYCLE", KEY_BRIGHTNESS_CYCLE},
    {"BRIGHTNESS_AUTO", KEY_BRIGHTNESS_AUTO},
    {"DISPLAY_OFF", KEY_DISPLAY_OFF},
    {"WWAN", KEY_WWAN},
    {"RFKILL", KEY_RFKILL},
    {"MICMUTE", KEY_MICMUTE},
    {"OK", KEY_OK},
    {"SELECT", KEY_SELECT},
    {"GOTO", KEY_GOTO},
    {"CLEAR", KEY_CLEAR},
    {"POWER2", KEY_POWER2},
    {"OPTION", KEY_OPTION},
    {"INFO", KEY_INFO},
    {"TIME", KEY_TIME},
    {"VENDOR", KEY_VENDOR},
    {"ARCHIVE", KEY_ARCHIVE},
    {"PROGRAM", KEY_PROGRAM},
    {"CHANNEL", KEY_CHANNEL},
    {"FAVORITES", KEY_FAVORITES},
    {"EPG", KEY_EPG},
    {"PVR", KEY_PVR},
    {"MHP", KEY_MHP},
    {"LANGUAGE", KEY_LANGUAGE},
    {"TITLE", KEY_TITLE},
    {"SUBTITLE", KEY_SUBTITLE},
    {"ANGLE", KEY_ANGLE},
    {"FULL_SCREEN", KEY_FULL_SCREEN},
    {"MODE", KEY_MODE},
    {"KEYBOARD", KEY_KEYBOARD},
    {"ASPECT_RATIO", KEY_ASPECT_RATIO},
    {"PC", KEY_PC},
    {"TV", KEY_TV},
    {"TV2", KEY_TV2},
    {"VCR", KEY_VCR},
    {"VCR2", KEY_VCR2},
    {"SAT", KEY_SAT},
    {"SAT2", KEY_SAT2},
    {"CD", KEY_CD},
    {"TAPE", KEY_TAPE},
    {"RADIO", KEY_RADIO},
    {"TUNER", KEY_TUNER},
    {"PLAYER", KEY_PLAYER},
    {"TEXT", KEY_TEXT},
    {"DVD", KEY_DVD},
    {"AUX", KEY_AUX},
    {"MP3", KEY_MP3},
    {"AUDIO", KEY_AUDIO},
    {"VIDEO", KEY_VIDEO},
    {"DIRECTORY", KEY_DIRECTORY},
    {"LIST", KEY_LIST},
    {"MEMO", KEY_MEMO},
    {"CALENDAR", KEY_CALENDAR},
    {"RED", KEY_RED},
    {"GREEN", KEY_GREEN},
    {"YELLOW", KEY_YELLOW},
    {"BLUE", KEY_BLUE},
    {"CHANNELUP", KEY_CHANNELUP},
    {"CHANNELDOWN", KEY_CHANNELDOWN},
    {"FIRST", KEY_FIRST},
    {"LAST", KEY_LAST},
    {"AB", KEY_AB},
    {"NEXT", KEY_NEXT},
    {"RESTART", KEY_RESTART},
    {"SLOW", KEY_SLOW},
    {"SHUFFLE", KEY_SHUFFLE},
    {"BREAK", KEY_BREAK},
    {"PREVIOUS", KEY_PREVIOUS},
    {"DIGITS", KEY_DIGITS},
    {"TEEN", KEY_TEEN},
    {"TWEN", KEY_TWEN},
    {"VIDEOPHONE", KEY_VIDEOPHONE},
    {"GAMES", KEY_GAMES},
    {"ZOOMIN", KEY_ZOOMIN},
    {"ZOOMOUT", KEY_ZOOMOUT},
    {"ZOOMRESET", KEY_ZOOMRESET},
    {"WORDPROCESSOR", KEY_WORDPROCESSOR},
    {"EDITOR", KEY_EDITOR},
    {"SPREADSHEET", KEY_SPREADSHEET},
    {"GRAPHICSEDITOR", KEY_GRAPHICSEDITOR},
    {"PRESENTATION", KEY_PRESENTATION},
    {"DATABASE", KEY_DATABASE},
    {"NEWS", KEY_NEWS},
    {"VOICEMAIL", KEY_VOICEMAIL},
    {"ADDRESSBOOK", KEY_ADDRESSBOOK},
    {"MESSENGER", KEY_MESSENGER},
    {"DISPLAYTOGGLE", KEY_DISPLAYTOGGLE},
    {"SPELLCHECK", KEY_SPELLCHECK},
    {"LOGOFF", KEY_LOGOFF},
    {"DOLLAR", KEY_DOLLAR},
    {"EURO", KEY_EURO},
    {"FRAMEBACK", KEY_FRAMEBACK},
    {"FRAMEFORWARD", KEY_FRAMEFORWARD},
    {"CONTEXT_MENU", KEY_CONTEXT_MENU},
    {"MEDIA_REPEAT", KEY_MEDIA_REPEAT},
    {"10CHANNELSUP", KEY_10CHANNELSUP},
    {"10CHANNELSDOWN", KEY_10CHANNELSDOWN},
    {"IMAGES", KEY_IMAGES},
    {"NOTIFICATION_CENTER", KEY_NOTIFICATION_CENTER},
    {"PICKUP_PHONE", KEY_PICKUP_PHONE},
    {"HANGUP_PHONE", KEY_HANGUP_PHONE},
    {"DEL_EOL", KEY_DEL_EOL},
    {"DEL_EOS", KEY_DEL_EOS},
    {"INS_LINE", KEY_INS_LINE},
    {"DEL_LINE", KEY_DEL_LINE},
    {"FN", KEY_FN},
    {"FN_ESC", KEY_FN_ESC},
    {"FN_F1", KEY_FN_F1},
    {"FN_F2", KEY_FN_F2},
    {"FN_F3", KEY_FN_F3},
    {"FN_F4", KEY_FN_F4},
    {"FN_F5", KEY_FN_F5},
    {"FN_F6", KEY_FN_F6},
    {"FN_F7", KEY_FN_F7},
    {"FN_F8", KEY_FN_F8},
    {"FN_F9", KEY_FN_F9},
    {"FN_F10", KEY_FN_F10},
    {"FN_F11", KEY_FN_F11},
    {"FN_F12", KEY_FN_F12},
    {"FN_1", KEY_FN_1},
    {"FN_2", KEY_FN_2},
    {"FN_D", KEY_FN_D},
    {"FN_E", KEY_FN_E},
    {"FN_F", KEY_FN_F},
    {"FN_S", KEY_FN_S},
    {"FN_B", KEY_FN_B},
    {"FN_RIGHT_SHIFT", KEY_FN_RIGHT_SHIFT},
    {"BRL_DOT1", KEY_BRL_DOT1},
    {"BRL_DOT2", KEY_BRL_DOT2},
    {"BRL_DOT3", KEY_BRL_DOT3},
    {"BRL_DOT4", KEY_BRL_DOT4},
    {"BRL_DOT5", KEY_BRL_DOT5},
    {"BRL_DOT6", KEY_BRL_DOT6},
    {"BRL_DOT7", KEY_BRL_DOT7},
    {"BRL_DOT8", KEY_BRL_DOT8},
    {"BRL_DOT9", KEY_BRL_DOT9},
    {"BRL_DOT10", KEY_BRL_DOT10},
    {"NUMERIC_0", KEY_NUMERIC_0},
    {"NUMERIC_1", KEY_NUMERIC_1},
    {"NUMERIC_2", KEY_NUMERIC_2},
    {"NUMERIC_3", KEY_NUMERIC_3},
    {"NUMERIC_4", KEY_NUMERIC_4},
    {"NUMERIC_5", KEY_NUMERIC_5},
    {"NUMERIC_6", KEY_NUMERIC_6},
    {"NUMERIC_7", KEY_NUMERIC_7},
    {"NUMERIC_8", KEY_NUMERIC_8},
    {"NUMERIC_9", KEY_NUMERIC_9},
    {"NUMERIC_STAR", KEY_NUMERIC_STAR},
    {"NUMERIC_POUND", KEY_NUMERIC_POUND},
    {"NUMERIC_A", KEY_NUMERIC_A},
    {"NUMERIC_B", KEY_NUMERIC_B},
    {"NUMERIC_C", KEY_NUMERIC_C},
    {"NUMERIC_D", KEY_NUMERIC_D},
    {"CAMERA_FOCUS", KEY_CAMERA_FOCUS},
    {"WPS_BUTTON", KEY_WPS_BUTTON},
    {"TOUCHPAD_TOGGLE", KEY_TOUCHPAD_TOGGLE},
    {"TOUCHPAD_ON", KEY_TOUCHPAD_ON},
    {"TOUCHPAD_OFF", KEY_TOUCHPAD_OFF},
    {"CAMERA_ZOOMIN", KEY_CAMERA_ZOOMIN},
    {"CAMERA_ZOOMOUT", KEY_CAMERA_ZOOMOUT},
    {"CAMERA_UP", KEY_CAMERA_UP},
    {"CAMERA_DOWN", KEY_CAMERA_DOWN},
    {"CAMERA_LEFT", KEY_CAMERA_LEFT},
    {"CAMERA_RIGHT", KEY_CAMERA_RIGHT},
    {"ATTENDANT_ON", KEY_ATTENDANT_ON},
    {"ATTENDANT_OFF", KEY_ATTENDANT_OFF},
    {"ATTENDANT_TOGGLE", KEY_ATTENDANT_TOGGLE},
    {"LIGHTS_TOGGLE", KEY_LIGHTS_TOGGLE},
    {"ALS_TOGGLE", KEY_ALS_TOGGLE},
    {"ROTATE_LOCK_TOGGLE", KEY_ROTATE_LOCK_TOGGLE},
    {"REFRESH_RATE_TOGGLE", KEY_REFRESH_RATE_TOGGLE},
    {"BUTTONCONFIG", KEY_BUTTONCONFIG},
    {"TASKMANAGER", KEY_TASKMANAGER},
    {"JOURNAL", KEY_JOURNAL},
    {"CONTROLPANEL", KEY_CONTROLPANEL},
    {"APPSELECT", KEY_APPSELECT},
    {"SCREENSAVER", KEY_SCREENSAVER},
    {"VOICECOMMAND", KEY_VOICECOMMAND},
    {"ASSISTANT", KEY_ASSISTANT},
    {"KBD_LAYOUT_NEXT", KEY_KBD_LAYOUT_NEXT},
    {"EMOJI_PICKER", KEY_EMOJI_PICKER},
    {"DICTATE", KEY_DICTATE},
    {"CAMERA_ACCESS_ENABLE", KEY_CAMERA_ACCESS_ENABLE},
    {"CAMERA_ACCESS_DISABLE", KEY_CAMERA_ACCESS_DISABLE},
    {"CAMERA_ACCESS_TOGGLE", KEY_CAMERA_ACCESS_TOGGLE},
    {"BRIGHTNESS_MIN", KEY_BRIGHTNESS_MIN},
    {"BRIGHTNESS_MAX", KEY_BRIGHTNESS_MAX},
    {"KBDINPUTASSIST_PREV", KEY_KBDINPUTASSIST_PREV},
    {"KBDINPUTASSIST_NEXT", KEY_KBDINPUTASSIST_NEXT},
    {"KBDINPUTASSIST_PREVGROUP", KEY_KBDINPUTASSIST_PREVGROUP},
    {"KBDINPUTASSIST_NEXTGROUP", KEY_KBDINPUTASSIST_NEXTGROUP},
    {"KBDINPUTASSIST_ACCEPT", KEY_KBDINPUTASSIST_ACCEPT},
    {"KBDINPUTASSIST_CANCEL", KEY_KBDINPUTASSIST_CANCEL},
    {"RIGHT_UP", KEY_RIGHT_UP},
    {"RIGHT_DOWN", KEY_RIGHT_DOWN},
    {"LEFT_UP", KEY_LEFT_UP},
    {"LEFT_DOWN", KEY_LEFT_DOWN},
    {"ROOT_MENU", KEY_ROOT_MENU},
    {"MEDIA_TOP_MENU", KEY_MEDIA_TOP_MENU},
    {"NUMERIC_11", KEY_NUMERIC_11},
    {"NUMERIC_12", KEY_NUMERIC_12},
    {"AUDIO_DESC", KEY_AUDIO_DESC},
    {"3D_MODE", KEY_3D_MODE},
    {"NEXT_FAVORITE", KEY_NEXT_FAVORITE},
    {"STOP_RECORD", KEY_STOP_RECORD},
    {"PAUSE_RECORD", KEY_PAUSE_RECORD},
    {"VOD", KEY_VOD},
    {"UNMUTE", KEY_UNMUTE},
    {"FASTREVERSE", KEY_FASTREVERSE},
    {"SLOWREVERSE", KEY_SLOWREVERSE},
    {"DATA", KEY_DATA},
    {"ONSCREEN_KEYBOARD", KEY_ONSCREEN_KEYBOARD},
    {"PRIVACY_SCREEN_TOGGLE", KEY_PRIVACY_SCREEN_TOGGLE},
    {"SELECTIVE_SCREENSHOT", KEY_SELECTIVE_SCREENSHOT},
    {"NEXT_ELEMENT", KEY_NEXT_ELEMENT},
    {"PREVIOUS_ELEMENT", KEY_PREVIOUS_ELEMENT},
    {"AUTOPILOT_ENGAGE_TOGGLE", KEY_AUTOPILOT_ENGAGE_TOGGLE},
    {"MARK_WAYPOINT", KEY_MARK_WAYPOINT},
    {"SOS", KEY_SOS},
    {"NAV_CHART", KEY_NAV_CHART},
    {"FISHING_CHART", KEY_FISHING_CHART},
    {"SINGLE_RANGE_RADAR", KEY_SINGLE_RANGE_RADAR},
    {"DUAL_RANGE_RADAR", KEY_DUAL_RANGE_RADAR},
    {"RADAR_OVERLAY", KEY_RADAR_OVERLAY},
    {"TRADITIONAL_SONAR", KEY_TRADITIONAL_SONAR},
    {"CLEARVU_SONAR", KEY_CLEARVU_SONAR},
    {"SIDEVU_SONAR", KEY_SIDEVU_SONAR},
    {"NAV_INFO", KEY_NAV_INFO},
    {"BRIGHTNESS_MENU", KEY_BRIGHTNESS_MENU},
    {"MACRO1", KEY_MACRO1},
    {"MACRO2", KEY_MACRO2},
    {"MACRO3", KEY_MACRO3},
    {"MACRO4", KEY_MACRO4},
    {"MACRO5", KEY_MACRO5},
    {"MACRO6", KEY_MACRO6},
    {"MACRO7", KEY_MACRO7},
    {"MACRO8", KEY_MACRO8},
    {"MACRO9", KEY_MACRO9},
    {"MACRO10", KEY_MACRO10},
    {"MACRO11", KEY_MACRO11},
    {"MACRO12", KEY_MACRO12},
    {"MACRO13", KEY_MACRO13},
    {"MACRO14", KEY_MACRO14},
    {"MACRO15", KEY_MACRO15},
    {"MACRO16", KEY_MACRO16},
    {"MACRO17", KEY_MACRO17},
    {"MACRO18", KEY_MACRO18},
    {"MACRO19", KEY_MACRO19},
    {"MACRO20", KEY_MACRO20},
    {"MACRO21", KEY_MACRO21},
    {"MACRO22", KEY_MACRO22},
    {"MACRO23", KEY_MACRO23},
    {"MACRO24", KEY_MACRO24},
    {"MACRO25", KEY_MACRO25},
    {"MACRO26", KEY_MACRO26},
    {"MACRO27", KEY_MACRO27},
    {"MACRO28", KEY_MACRO28},
    {"MACRO29", KEY_MACRO29},
    {"MACRO30", KEY_MACRO30},
    {"MACRO_RECORD_START", KEY_MACRO_RECORD_START},
    {"MACRO_RECORD_STOP", KEY_MACRO_RECORD_STOP},
    {"MACRO_PRESET_CYCLE", KEY_MACRO_PRESET_CYCLE},
    {"MACRO_PRESET1", KEY_MACRO_PRESET1},
    {"MACRO_PRESET2", KEY_MACRO_PRESET2},
    {"MACRO_PRESET3", KEY_MACRO_PRESET3},
    {"KBD_LCD_MENU1", KEY_KBD_LCD_MENU1},
    {"KBD_LCD_MENU2", KEY_KBD_LCD_MENU2},
    {"KBD_LCD_MENU3", KEY_KBD_LCD_MENU3},
    {"KBD_LCD_MENU4", KEY_KBD_LCD_MENU4},
    {"KBD_LCD_MENU5", KEY_KBD_LCD_MENU5},
    {"MAX", KEY_MAX},
};

// https://invent.kde.org/plasma/kwin/-/blob/cc4d99ae/src/mousebuttons.cpp#L14
static const std::unordered_map<QString, uint32_t> s_mouse = {
    {"LEFT", BTN_LEFT},
    {"MIDDLE", BTN_MIDDLE},
    {"RIGHT", BTN_RIGHT},

    // Those 5 button are supposed to be like this (I think)
    {"BACK", BTN_SIDE},
    {"FORWARD", BTN_EXTRA},
    {"TASK", BTN_FORWARD},
    {"SIDE", BTN_BACK},
    {"EXTRA", BTN_TASK},

    {"EXTRA6", 0x118},
    {"EXTRA7", 0x119},
    {"EXTRA8", 0x11a},
    {"EXTRA9", 0x11b},
    {"EXTRA10", 0x11c},
    {"EXTRA11", 0x11d},
    {"EXTRA12", 0x11e},
    {"EXTRA13", 0x11f},
};

static const std::unordered_map<QString, Qt::KeyboardModifier> s_keyboardModifiers = {
    {"alt", Qt::KeyboardModifier::AltModifier},
    {"ctrl", Qt::KeyboardModifier::ControlModifier},
    {"meta", Qt::KeyboardModifier::MetaModifier},
    {"shift", Qt::KeyboardModifier::ShiftModifier},
};

namespace YAML
{

struct range
{
    qreal min = 0.0;
    qreal max = 0.0;

    range() = default;
    range(qreal min, qreal max)
    {
        this->min = min;
        this->max = max;
    }
};

template<>
struct convert<range>
{
    static bool decode(const Node &node, range &range)
    {
        const auto rangeRaw = node.as<QString>();
        if (rangeRaw.contains("-")) {
            const auto split = rangeRaw.split("-");
            range.min = split[0].toDouble();
            range.max = split[1].toDouble();
        } else {
            range.min = range.max = rangeRaw.toDouble();
        }

        return true;
    }
};

template<>
struct convert<std::shared_ptr<libgestures::Condition>>
{
    static bool decode(const Node &node, std::shared_ptr<libgestures::Condition> &condition)
    {
        condition = std::make_shared<libgestures::Condition>();

        const auto negatedNode = node["negate"];
        if (negatedNode.IsDefined()) {
            for (const auto &negatedRaw : negatedNode.as<QStringList>(QStringList())) {
                if (negatedRaw.contains("window_class")) {
                    condition->setNegateWindowClass(true);
                } else if (negatedRaw.contains("window_state")) {
                    condition->setNegateWindowState(true);
                } else {
                    throw Exception(node.Mark(), "Invalid negated condition property");
                }
            }
        }

        const auto windowClass = node["window_class"];
        if (windowClass.IsDefined()) {
            condition->setWindowClass(windowClass.as<QRegularExpression>());
        }
        condition->setWindowState(node["window_state"].as<libgestures::WindowStates>(libgestures::WindowState::All));

        return true;
    }
};

template<>
struct convert<std::shared_ptr<libgestures::Gesture>>
{
    static bool decode(const Node &node, std::shared_ptr<libgestures::Gesture> &gesture)
    {
        const auto type = node["type"].as<QString>();
        if (type == "hold") {
            gesture = std::make_shared<libgestures::HoldGesture>();
        } else if (type == "pinch") {
            auto pinchGesture = new libgestures::PinchGesture;
            pinchGesture->setDirection(node["direction"].as<libgestures::PinchDirection>());
            gesture.reset(pinchGesture);
        } else if (type == "swipe") {
            auto swipeGesture = new libgestures::SwipeGesture;
            swipeGesture->setDirection(node["direction"].as<libgestures::SwipeDirection>());
            gesture.reset(swipeGesture);
        } else if (type == "rotate") {
            auto rotateGesture = new libgestures::RotateGesture;
            rotateGesture->setDirection(node["direction"].as<libgestures::RotateDirection>());
            gesture.reset(rotateGesture);
        } else {
            throw Exception(node.Mark(), "Invalid gesture type");
        }

        const auto fingersNode = node["fingers"];
        if (!fingersNode) {
            throw Exception(node.Mark(), "Finger count not specified");
        }
        const auto fingersRaw = fingersNode.as<QString>();
        range fingers;
        if (fingersRaw.contains("-")) {
            const auto split = fingersRaw.split("-");
            fingers.min = split[0].toUInt();
            fingers.max = split[1].toUInt();
        } else {
            fingers.min = fingers.max = fingersRaw.toUInt();
        }

        const auto thresholdRaw = node["threshold"].as<QString>("");
        range threshold(0, 0);
        if (thresholdRaw.contains("-")) {
            const auto split = thresholdRaw.split("-");
            threshold.min = split[0].toFloat();
            threshold.max = split[1].toFloat();
        } else {
            threshold.min = thresholdRaw.toFloat();
        }

        gesture->setFingers(fingers.min, fingers.max);
        gesture->setSpeed(node["speed"].as<libgestures::GestureSpeed>(libgestures::GestureSpeed::Any));
        gesture->setThresholds(threshold.min, threshold.max);

        if (const auto modifiersNode = node["keyboard_modifiers"]) {
            if (modifiersNode.IsSequence()) {
                if (const auto modifiers = modifiersNode.as<Qt::KeyboardModifiers>(Qt::KeyboardModifier::NoModifier)) {
                    gesture->setKeyboardModifiers(modifiers);
                }
            } else {
                const auto modifierMatchingMode = modifiersNode.as<QString>();
                if (modifierMatchingMode == "any") {
                    gesture->setKeyboardModifiers(std::nullopt);
                } else if (modifierMatchingMode == "none") {
                    gesture->setKeyboardModifiers(Qt::KeyboardModifier::NoModifier);
                } else {
                    throw Exception(node.Mark(), "Invalid keyboard modifier");
                }
            }
        }

        for (const auto &conditionNode : node["conditions"]) {
            gesture->addCondition(conditionNode.as<std::shared_ptr<libgestures::Condition>>());
        }
        for (const auto &actionNode : node["actions"]) {
            gesture->addAction(actionNode.as<std::shared_ptr<libgestures::GestureAction>>());
        }

        return true;
    }
};

template<>
struct convert<std::shared_ptr<libgestures::GestureAction>>
{
    static bool decode(const Node &node, std::shared_ptr<libgestures::GestureAction> &action)
    {
        if (node["command"].IsDefined()) {
            auto commandAction = new libgestures::CommandGestureAction();
            commandAction->setCommand(node["command"].as<QString>());
            action.reset(commandAction);
        } else if (node["input"].IsDefined()) {
            auto inputAction = new libgestures::InputGestureAction;
            inputAction->setSequence(node["input"].as<std::vector<libgestures::InputAction>>());
            action.reset(inputAction);
        } else if (node["keyboard"].IsDefined()) {
            Node keyboardNode;
            keyboardNode["keyboard"] = node["keyboard"];
            Node inputNode;
            inputNode.push_back(keyboardNode);

            auto inputAction = new libgestures::InputGestureAction;
            inputAction->setSequence(inputNode.as<std::vector<libgestures::InputAction>>());
            action.reset(inputAction);
        } else if (node["plasma_shortcut"].IsDefined()) {
            auto plasmaShortcutAction = new libgestures::PlasmaGlobalShortcutGestureAction;
            const auto split = node["plasma_shortcut"].as<QString>().split(",");
            plasmaShortcutAction->setComponent(split[0]);
            plasmaShortcutAction->setShortcut(split[1]);
            action.reset(plasmaShortcutAction);
        } else {
            throw Exception(node.Mark(), "Action has no valid action property");
        }

        const auto thresholdRaw = node["threshold"].as<QString>("");
        range threshold(0, 0);
        if (!thresholdRaw.isEmpty()) {
            if (thresholdRaw.contains("-")) {
                const auto split = thresholdRaw.split("-");
                threshold.min = split[0].toFloat();
                threshold.max = split[1].toFloat();
            } else {
                threshold.min = thresholdRaw.toFloat();
            }
        }
        const auto on = node["on"].as<libgestures::On>(libgestures::On::End);
        if (on == libgestures::On::Begin && (threshold.min != 0 || threshold.max != 0)) {
            throw Exception(node.Mark(), "Begin actions can't have thresholds");
        }

        action->setOn(on);
        action->setThresholds(threshold.min, threshold.max);
        action->setRepeatInterval(node["interval"].as<qreal>(0));
        action->setBlockOtherActions(node["block_other"].as<bool>(false));
        for (const auto &conditionNode : node["conditions"]) {
            action->addCondition(conditionNode.as<std::shared_ptr<libgestures::Condition>>());
        }

        return true;
    }
};

template<>
struct convert<std::shared_ptr<libgestures::GestureRecognizer>>
{
    static bool decode(const Node &node, std::shared_ptr<libgestures::GestureRecognizer> &gestureRecognizer)
    {
        const auto gesturesNode = node["gestures"];
        if (!gesturesNode.IsDefined()) {
            throw Exception(node.Mark(), "No gestures specified");
        }

        gestureRecognizer = std::make_unique<libgestures::GestureRecognizer>();
        for (const auto gestureNode : gesturesNode) {
            gestureRecognizer->registerGesture(gestureNode.as<std::shared_ptr<libgestures::Gesture>>());
        }

        gestureRecognizer->setDeltaMultiplier(node["delta_multiplier"].as<qreal>(gestureRecognizer->m_deltaMultiplier));
        const auto speedNode = node["speed"];
        if (speedNode.IsDefined()) {
            gestureRecognizer->setInputEventsToSample(
                speedNode["events"].as<uint8_t>(gestureRecognizer->m_inputEventsToSample));
            gestureRecognizer->setSwipeFastThreshold(
                speedNode["swipe_threshold"].as<qreal>(gestureRecognizer->m_swipeGestureFastThreshold));
            gestureRecognizer->setPinchInFastThreshold(
                speedNode["pinch_in_threshold"].as<qreal>(gestureRecognizer->m_pinchInFastThreshold));
            gestureRecognizer->setPinchOutFastThreshold(
                speedNode["pinch_out_threshold"].as<qreal>(gestureRecognizer->m_pinchOutFastThreshold));
            gestureRecognizer->setRotateFastThreshold(
                speedNode["rotate_threshold"].as<qreal>(gestureRecognizer->m_rotateFastThreshold));
        }

        return true;
    }
};

template<>
struct convert<libgestures::GestureSpeed>
{
    static bool decode(const Node &node, libgestures::GestureSpeed &speed)
    {
        const auto speedRaw = node.as<QString>();
        if (speedRaw == "fast") {
            speed = libgestures::GestureSpeed::Fast;
        } else if (speedRaw == "slow") {
            speed = libgestures::GestureSpeed::Slow;
        } else if (speedRaw == "any") {
            speed = libgestures::GestureSpeed::Any;
        } else {
            throw Exception(node.Mark(), "Invalid gesture speed");
        }

        return true;
    }
};

template<>
struct convert<libgestures::PinchDirection>
{
    static bool decode(const Node &node, libgestures::PinchDirection &direction)
    {
        const auto directionRaw = node.as<QString>();
        if (directionRaw == "in") {
            direction = libgestures::PinchDirection::In;
        } else if (directionRaw == "out") {
            direction = libgestures::PinchDirection::Out;
        } else if (directionRaw == "any") {
            direction = libgestures::PinchDirection::Any;
        } else {
            throw Exception(node.Mark(), "Invalid pinch direction");
        }

        return true;
    }
};

template<>
struct convert<libgestures::RotateDirection>
{
    static bool decode(const Node &node, libgestures::RotateDirection &direction)
    {
        const auto directionRaw = node.as<QString>();
        if (directionRaw == "any") {
            direction = libgestures::RotateDirection::Any;
        } else if (directionRaw == "clockwise") {
            direction = libgestures::RotateDirection::Clockwise;
        } else if (directionRaw == "counterclockwise") {
            direction = libgestures::RotateDirection::Counterclockwise;
        } else {
            throw Exception(node.Mark(), "Invalid rotate direction");
        }

        return true;
    }
};

template<>
struct convert<libgestures::SwipeDirection>
{
    static bool decode(const Node &node, libgestures::SwipeDirection &direction)
    {
        const auto directionRaw = node.as<QString>();
        if (directionRaw == "left") {
            direction = libgestures::SwipeDirection::Left;
        } else if (directionRaw == "right") {
            direction = libgestures::SwipeDirection::Right;
        } else if (directionRaw == "left_right") {
            direction = libgestures::SwipeDirection::LeftRight;
        } else if (directionRaw == "up") {
            direction = libgestures::SwipeDirection::Up;
        } else if (directionRaw == "down") {
            direction = libgestures::SwipeDirection::Down;
        } else if (directionRaw == "up_down") {
            direction = libgestures::SwipeDirection::UpDown;
        } else if (directionRaw == "any") {
            direction = libgestures::SwipeDirection::Any;
        } else {
            throw Exception(node.Mark(), "Invalid swipe direction");
        }

        return true;
    }
};

template<>
struct convert<libgestures::WindowStates>
{
    static bool decode(const Node &node, libgestures::WindowStates &windowStates)
    {
        windowStates = static_cast<libgestures::WindowState>(0);
        for (const auto &stateRaw : node.as<QStringList>()) {
            if (stateRaw == "maximized") {
                windowStates |= libgestures::WindowState::Maximized;
            } else if (stateRaw == "fullscreen") {
                windowStates |= libgestures::WindowState::Fullscreen;
            } else {
                throw Exception(node.Mark(), "Invalid window state");
            }
        }

        return true;
    }
};

template<>
struct convert<libgestures::On>
{
    static bool decode(const Node &node, libgestures::On &when)
    {
        const auto raw = node.as<QString>();
        if (raw == "begin") {
            when = libgestures::On::Begin;
        } else if (raw == "update") {
            when = libgestures::On::Update;
        } else if (raw == "cancel") {
            when = libgestures::On::Cancel;
        } else if (raw == "end") {
            when = libgestures::On::End;
        } else if (raw == "end_cancel") {
            when = libgestures::On::EndOrCancel;
        } else {
            throw Exception(node.Mark(), "Invalid action event ('on')");
        }

        return true;
    }
};

template<>
struct convert<std::vector<libgestures::InputAction>>
{
    static bool decode(const Node &node, std::vector<libgestures::InputAction> &actions)
    {
        for (const auto &device : node) {
            if (device["keyboard"].IsDefined()) {
                for (auto &actionRaw : device["keyboard"].as<QStringList>()) {
                    actionRaw = actionRaw.toUpper();
                    libgestures::InputAction action;
                    if (actionRaw.startsWith("+") || actionRaw.startsWith("-")) {
                        const auto key = actionRaw.mid(1);
                        if (!s_keyboard.contains(key)) {
                            throw Exception(node.Mark(), ("Invalid keyboard key ('" + key + "')").toStdString());
                        }

                        if (actionRaw[0] == '+') {
                            action.keyboardPress.push_back(s_keyboard.at(key));
                        } else {
                            action.keyboardRelease.push_back(s_keyboard.at(key));
                        }
                    } else {
                        for (const auto &keyRaw : actionRaw.split("+")) {
                            if (!s_keyboard.contains(keyRaw)) {
                                throw Exception(node.Mark(), ("Invalid keyboard key ('" + keyRaw + "')").toStdString());
                            }

                            const auto key = s_keyboard.at(keyRaw);
                            action.keyboardPress.push_back(key);
                            action.keyboardRelease.insert(action.keyboardRelease.begin(), key);
                        }
                    }
                    actions.push_back(action);
                }
            } else if (device["mouse"].IsDefined()) {
                for (auto &actionRaw : device["mouse"].as<QStringList>()) {
                    actionRaw = actionRaw.toUpper();
                    libgestures::InputAction action;
                    if (actionRaw.startsWith("+") || actionRaw.startsWith("-")) {
                        const auto button = actionRaw.mid(1);
                        if (!s_mouse.contains(button)) {
                            throw Exception(node.Mark(), ("Invalid mouse button ('" + button + "')").toStdString());
                        }

                        if (actionRaw[0] == '+') {
                            action.mousePress.push_back(s_mouse.at(button));
                        } else {
                            action.mouseRelease.push_back(s_mouse.at(button));
                        }
                    } else if (actionRaw.startsWith("MOVE_BY_DELTA")) {
                        action.mouseMoveRelativeByDelta = true;
                    } else if (actionRaw.startsWith("MOVE_BY")) {
                        const auto split = actionRaw.split(" ");
                        action.mouseMoveRelative = QPointF(split[1].toFloat(), split[2].toFloat());
                    } else if (actionRaw.startsWith("MOVE_TO")) {
                        const auto split = actionRaw.split(" ");
                        action.mouseMoveAbsolute = QPointF(split[1].toFloat(), split[2].toFloat());
                    } else {
                        for (const auto &buttonRaw : actionRaw.split("+")) {
                            if (!s_mouse.contains(buttonRaw)) {
                                throw Exception(node.Mark(), ("Invalid mouse button ('" + buttonRaw + "')").toStdString());
                            }

                            const auto button = s_mouse.at(buttonRaw);
                            action.mousePress.push_back(button);
                            action.mouseRelease.insert(action.mouseRelease.begin(), button);
                        }
                    }
                    actions.push_back(action);
                }
            }
        }

        return true;
    }
};

template<>
struct convert<QString>
{
    static bool decode(const Node &node, QString &s)
    {
        s = QString::fromStdString(node.as<std::string>());
        return true;
    }
};

template<>
struct convert<QStringList>
{
    static bool decode(const Node &node, QStringList &list)
    {
        for (const auto &s : node.as<std::vector<QString>>()) {
            list << s;
        }
        return true;
    }
};

template<>
struct convert<QRegularExpression>
{
    static bool decode(const Node &node, QRegularExpression &regex)
    {
        regex = QRegularExpression(node.as<QString>());
        return true;
    }
};

template<>
struct convert<Qt::KeyboardModifiers>
{
    static bool decode(const Node &node, Qt::KeyboardModifiers &modifiers)
    {
        for (const auto &modifierRaw : node.as<QStringList>()) {
            if (s_keyboardModifiers.contains(modifierRaw)) {
                modifiers |= s_keyboardModifiers.at(modifierRaw);
            } else {
                throw Exception(node.Mark(), "Invalid keyboard modifier");
            }
        }

        return true;
    }
};



}