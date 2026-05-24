#include "ui.h"
#include "user.h"
#include <iostream>

string g_currentUser = "";

// 控制台登录/注册（终端操作，文件保存到 data/users.txt）
static bool consoleLogin() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    while (true) {
        cout << "\n========================================" << endl;
        cout << "        背单词小程序 — 终端登录" << endl;
        cout << "========================================" << endl;
        cout << "  1. 登录" << endl;
        cout << "  2. 注册" << endl;
        cout << "  3. 退出" << endl;
        cout << "----------------------------------------" << endl;
        cout << "  请选择 (1-3): ";

        string choice;
        getline(cin, choice);

        if (choice == "1") {
            // ---- 登录 ----
            cout << "\n--- 登录 ---" << endl;
            cout << "用户名: ";
            string username;
            getline(cin, username);
            cout << "密  码: ";
            string password;
            getline(cin, password);

            if (username.empty() || password.empty()) {
                cout << "[错误] 用户名和密码不能为空！" << endl;
            } else if (loginUser(username, password)) {
                g_currentUser = username;
                cout << "[成功] 欢迎回来，" << username << "！" << endl;
                return true;
            } else {
                cout << "[错误] 用户名或密码错误！" << endl;
            }
        } else if (choice == "2") {
            // ---- 注册 ----
            cout << "\n--- 注册 ---" << endl;
            cout << "用户名: ";
            string username;
            getline(cin, username);
            cout << "密  码: ";
            string password;
            getline(cin, password);
            cout << "确认密码: ";
            string confirm;
            getline(cin, confirm);

            if (username.empty() || password.empty()) {
                cout << "[错误] 用户名和密码不能为空！" << endl;
            } else if (password != confirm) {
                cout << "[错误] 两次密码输入不一致！" << endl;
            } else if (registerUser(username, password)) {
                g_currentUser = username;
                cout << "[成功] 注册成功，欢迎，" << username << "！" << endl;
                return true;
            } else {
                cout << "[错误] 用户名已存在，请换一个！" << endl;
            }
        } else if (choice == "3") {
            cout << "再见！" << endl;
            return false;
        } else {
            cout << "[错误] 无效选项，请重新输入！" << endl;
        }
    }
}

int main() {
    // 1. 终端登录/注册
    if (!consoleLogin())
        return 0;

    // 2. 初始化 EasyX 图形窗口
    initgraph(WIN_W, WIN_H);
    SetWindowText(GetHWnd(), _T("背单词小程序"));

    // 3. 设置随机种子
    srand((unsigned)time(NULL));

    // 4. 文字透明背景
    setbkmode(TRANSPARENT);

    // 5. 页面路由（状态机）
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
