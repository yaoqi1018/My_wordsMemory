#include "ui.h"

// 全局变量：当前登录用户名
string g_currentUser = "";

int main() {
    // 1. 初始化 EasyX 图形窗口
    initgraph(WIN_W, WIN_H);
    SetWindowText(GetHWnd(), _T("背单词小程序"));

    // 2. 设置随机种子
    srand((unsigned)time(NULL));

    // 3. 文字透明背景
    setbkmode(TRANSPARENT);

    // 4. 页面路由（状态机）—— 暂时跳过登录，直接进主菜单
    g_currentUser = "test";
    Page page = PAGE_MENU;

    while (true) {
        switch (page) {
        case PAGE_LOGIN:    page = showLoginPage();    break;
        case PAGE_REGISTER: page = showRegisterPage(); break;
        case PAGE_MENU:     page = showMainMenu();     break;
        case PAGE_QUIZ:     page = showQuizPage();     break;
        case PAGE_SCORE:    page = showScorePage();    break;
        case PAGE_WRONG:    page = showWrongBookPage(); break;
        }
    }

    closegraph();
    return 0;
}
