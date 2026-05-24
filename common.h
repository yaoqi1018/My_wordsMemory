#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX          // 阻止 windows.h 定义 min/max 宏，避免跟 std::min/max 冲突
#include <windows.h>
#include <graphics.h>     // EasyX 图形库


#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <random>
#include <ctime>
#include <cstdio>
#include <cstdlib>        // atoi()
#include <cctype>         // isalpha()
using namespace std;

// ---------- 窗口 ----------
const int WIN_W = 800;
const int WIN_H = 600;

// ---------- 颜色 ----------
const COLORREF CLR_BG        = RGB(245, 245, 247);
const COLORREF CLR_TITLE     = RGB(44,  62,  80);
const COLORREF CLR_TEXT      = RGB(52,  73,  94);
const COLORREF CLR_BTN       = RGB(52,  152, 219);
const COLORREF CLR_BTN_HOVER = RGB(41,  128, 185);
const COLORREF CLR_BTN_TEXT  = RGB(255, 255, 255);
const COLORREF CLR_INPUT_BG  = RGB(255, 255, 255);
const COLORREF CLR_INPUT_BD  = RGB(189, 195, 199);
const COLORREF CLR_CORRECT   = RGB(46,  204, 113);
const COLORREF CLR_WRONG     = RGB(231, 76,  60);
const COLORREF CLR_WHITE     = RGB(255, 255, 255);
const COLORREF CLR_RADIO     = RGB(52,  152, 219);
const COLORREF CLR_BAR_BG    = RGB(220, 220, 220);
const COLORREF CLR_LINE      = RGB(200, 200, 200);
const COLORREF CLR_ROW_EVEN  = RGB(245, 248, 252);

// ---------- 枚举 ----------
enum Level { LV_PRIMARY = 0, LV_JUNIOR = 1, LV_SENIOR = 2, LV_WRONG = 3 };
enum Order { ORD_RANDOM = 0, ORD_DICT = 1 };
enum Mode  { MODE_EN2CN = 0, MODE_CN2EN = 1 };
enum Page  { PAGE_LOGIN, PAGE_REGISTER, PAGE_MENU, PAGE_QUIZ, PAGE_SCORE, PAGE_WRONG };


// 一个单词
struct Word {
    int    id;
    string english;   // 英文
    string phonetic;  // 音标
    string chinese;   // 中文释义
};

// 用户
struct User {
    string name;
    string password;
};

// 答题配置
struct QuizConfig {
    Level level;
    Order order;
    Mode  mode;
    int   count;
};

// 答题会话
struct QuizSession {
    QuizConfig   config;
    vector<Word> words;
    int          currentIndex;  // 当前题号（0开始）
    int          correctCount;  // 答对
    int          wrongCount;    // 答错
};

// ---------- 全局变量 ----------
extern string g_currentUser;  // 当前登录用户名

// ---------- 工具函数 ----------

// UTF-8 → 宽字符串（EasyX 在 Unicode 项目里需要 wchar_t*）
inline wstring to_wstr(const string& utf8) {
    if (utf8.empty()) return L"";
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
    if (len <= 0) return L"";
    wstring result(len - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &result[0], len);
    return result;
}

// 画文字（自动 UTF-8 → 宽字符）
inline void draw_text(int x, int y, const string& text) {
    wstring ws = to_wstr(text);
    outtextxy(x, y, ws.c_str());
}

// 居中画文字
inline void draw_text_center(int y, const string& text) {
    wstring ws = to_wstr(text);
    int w = textwidth(ws.c_str());
    int x = (WIN_W - w) / 2;
    if (x < 0) x = 0;
    outtextxy(x, y, ws.c_str());
}

// 鼠标是否在矩形区域内
inline bool in_rect(int mx, int my, int x, int y, int w, int h) {
    return mx >= x && mx <= x + w && my >= y && my <= y + h;
}
