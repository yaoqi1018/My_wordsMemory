#include "ui.h"
#include "word.h"
#include "user.h"
#include "wrong_book.h"
#include "quiz_engine.h"

// ================================================================
// 通用绘制元件
// ================================================================

// 圆角矩形
static void fillRoundRect(int x, int y, int w, int h, int r, COLORREF color) {
    setfillcolor(color);
    setlinecolor(color);
    fillroundrect(x, y, x + w, y + h, r, r);
}

// 按钮（支持 hover 变色）
static void drawButton(int x, int y, int w, int h, const string& text, bool hover) {
    COLORREF bg = hover ? CLR_BTN_HOVER : CLR_BTN;
    fillRoundRect(x, y, w, h, 8, bg);
    settextcolor(CLR_BTN_TEXT);
    settextstyle(22, 0, _T("微软雅黑"));
    setbkmode(TRANSPARENT);
    wstring ws = to_wstr(text);
    int tw = textwidth(ws.c_str());
    int th = textheight(ws.c_str());
    outtextxy(x + (w - tw) / 2, y + (h - th) / 2, ws.c_str());
}

// 单选按钮（空心圆 + 选中实心 + 标签文字）
static void drawRadio(int x, int y, const string& text, bool selected) {
    setlinecolor(CLR_RADIO);
    setfillcolor(CLR_WHITE);
    fillcircle(x, y, 10);
    circle(x, y, 10);
    if (selected) {
        setfillcolor(CLR_RADIO);
        solidcircle(x, y, 6);
    }
    settextcolor(CLR_TEXT);
    settextstyle(20, 0, _T("微软雅黑"));
    setbkmode(TRANSPARENT);
    wstring ws = to_wstr(text);
    outtextxy(x + 22, y - 10, ws.c_str());
}

// 输入框
static void drawInputBox(int x, int y, int w, int h, const string& text,
                          bool focused, bool isPassword) {
    setfillcolor(CLR_INPUT_BG);
    setlinecolor(focused ? CLR_BTN : CLR_INPUT_BD);
    fillrectangle(x, y, x + w, y + h);
    rectangle(x, y, x + w, y + h);

    settextstyle(18, 0, _T("微软雅黑"));
    setbkmode(TRANSPARENT);
    if (isPassword && !text.empty()) {
        string dots(text.size(), '*');
        settextcolor(CLR_TEXT);
        wstring ws = to_wstr(dots);
        outtextxy(x + 8, y + (h - textheight(ws.c_str())) / 2, ws.c_str());
    } else {
        settextcolor(CLR_TEXT);
        wstring ws = to_wstr(text);
        outtextxy(x + 8, y + (h - textheight(ws.c_str())) / 2, ws.c_str());
    }
}

// 清屏并填充背景色
static void drawBackground() {
    setbkcolor(CLR_BG);
    cleardevice();
}

// ================================================================
// 1. 登录页面
// ================================================================
Page showLoginPage() {
    string username, password;
    int    focus = 0;     // 0=无, 1=用户名, 2=密码
    string error;

    // 控件位置
    const int ix = 310, iy1 = 160, iy2 = 230, iw = 280, ih = 38;
    const int bx1 = 250, by = 320, bw = 130, bh = 42;
    const int bx2 = 420;

    BeginBatchDraw();
    while (true) {
        drawBackground();

        // 标题
        settextcolor(CLR_TITLE);
        settextstyle(34, 0, _T("微软雅黑"));
        draw_text_center(60, "背单词小程序");
        settextstyle(16, 0, _T("微软雅黑"));
        settextcolor(CLR_TEXT);
        draw_text_center(108, "英语词汇学习助手");

        // 标签
        settextstyle(20, 0, _T("微软雅黑"));
        settextcolor(CLR_TEXT);
        draw_text(210, iy1 + 6, "用户名:");
        draw_text(210, iy2 + 6, "密  码:");

        // 输入框占位提示
        if (username.empty() && focus != 1) {
            settextcolor(RGB(180, 180, 180));
            settextstyle(18, 0, _T("微软雅黑"));
            setbkmode(TRANSPARENT);
            outtextxy(ix + 8, iy1 + 8, to_wstr("请输入用户名").c_str());
        }
        drawInputBox(ix, iy1, iw, ih, username, focus == 1, false);

        if (password.empty() && focus != 2) {
            settextcolor(RGB(180, 180, 180));
            settextstyle(18, 0, _T("微软雅黑"));
            setbkmode(TRANSPARENT);
            outtextxy(ix + 8, iy2 + 8, to_wstr("请输入密码").c_str());
        }
        drawInputBox(ix, iy2, iw, ih, password, focus == 2, true);

        // ---- 鼠标处理 ----
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(GetHWnd(), &pt);

        bool hoverLogin = in_rect(pt.x, pt.y, bx1, by, bw, bh);
        bool hoverReg   = in_rect(pt.x, pt.y, bx2, by, bw, bh);

        while (MouseHit()) {
            MOUSEMSG msg = GetMouseMsg();
            if (msg.uMsg == WM_LBUTTONDOWN) {
                int mx = msg.x, my = msg.y;
                if (in_rect(mx, my, ix, iy1, iw, ih))
                    { focus = 1; error.clear(); }
                else if (in_rect(mx, my, ix, iy2, iw, ih))
                    { focus = 2; error.clear(); }
                else if (in_rect(mx, my, bx1, by, bw, bh)) {
                    if (username.empty() || password.empty())
                        error = "用户名和密码不能为空";
                    else if (loginUser(username, password)) {
                        g_currentUser = username;
                        EndBatchDraw();
                        return PAGE_MENU;
                    } else
                        error = "用户名或密码错误";
                }
                else if (in_rect(mx, my, bx2, by, bw, bh)) {
                    EndBatchDraw();
                    return PAGE_REGISTER;
                }
                else
                    focus = 0;
            }
        }

        // ---- 键盘输入（EasyX 文字输入必须用 WM_CHAR） ----
        ExMessage kmsg;
        while (peekmessage(&kmsg, EX_CHAR)) {
            if (kmsg.message != WM_CHAR) continue;
            char c = (char)kmsg.ch;
            string* target = (focus == 1) ? &username : (focus == 2) ? &password : nullptr;
            if (target) {
                if (c == '\b') {  // 退格
                    if (!target->empty()) target->pop_back();
                } else if (c >= 32) {
                    *target += c;
                }
            }
            // 回车快捷登录
            if (c == '\r' && focus > 0) {
                if (username.empty() || password.empty())
                    error = "用户名和密码不能为空";
                else if (loginUser(username, password)) {
                    g_currentUser = username;
                    EndBatchDraw();
                    return PAGE_MENU;
                } else
                    error = "用户名或密码错误";
            }
        }

        // 绘制按钮
        drawButton(bx1, by, bw, bh, "登  录", hoverLogin);
        drawButton(bx2, by, bw, bh, "注  册", hoverReg);

        // 错误提示
        if (!error.empty()) {
            settextcolor(CLR_WRONG);
            settextstyle(16, 0, _T("微软雅黑"));
            draw_text_center(395, error);
        }

        FlushBatchDraw();
        Sleep(16);
    }
    EndBatchDraw();
}

// ================================================================
// 2. 注册页面
// ================================================================
Page showRegisterPage() {
    string username, password, confirm;
    int    focus = 0;
    string error;

    const int ix = 310, iy1 = 155, iy2 = 220, iy3 = 285, iw = 280, ih = 38;
    const int bx1 = 290, by = 355, bw = 100, bh = 40;
    const int bx2 = 410;

    BeginBatchDraw();
    while (true) {
        drawBackground();

        settextcolor(CLR_TITLE);
        settextstyle(34, 0, _T("微软雅黑"));
        draw_text_center(60, "用户注册");

        // 标签
        settextstyle(20, 0, _T("微软雅黑"));
        settextcolor(CLR_TEXT);
        draw_text(210, iy1 + 6, "用户名:");
        draw_text(210, iy2 + 6, "密  码:");
        draw_text(190, iy3 + 6, "确认密码:");

        // 占位提示
        if (username.empty() && focus != 1) {
            settextcolor(RGB(180, 180, 180)); settextstyle(18, 0, _T("微软雅黑"));
            setbkmode(TRANSPARENT);
            outtextxy(ix + 8, iy1 + 8, to_wstr("请输入用户名").c_str());
        }
        drawInputBox(ix, iy1, iw, ih, username, focus == 1, false);

        if (password.empty() && focus != 2) {
            settextcolor(RGB(180, 180, 180)); settextstyle(18, 0, _T("微软雅黑"));
            setbkmode(TRANSPARENT);
            outtextxy(ix + 8, iy2 + 8, to_wstr("请输入密码").c_str());
        }
        drawInputBox(ix, iy2, iw, ih, password, focus == 2, true);

        if (confirm.empty() && focus != 3) {
            settextcolor(RGB(180, 180, 180)); settextstyle(18, 0, _T("微软雅黑"));
            setbkmode(TRANSPARENT);
            outtextxy(ix + 8, iy3 + 8, to_wstr("请再次输入密码").c_str());
        }
        drawInputBox(ix, iy3, iw, ih, confirm, focus == 3, true);

        // ---- 鼠标处理 ----
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(GetHWnd(), &pt);

        bool hoverReg  = in_rect(pt.x, pt.y, bx1, by, bw, bh);
        bool hoverBack = in_rect(pt.x, pt.y, bx2, by, bw, bh);

        while (MouseHit()) {
            MOUSEMSG msg = GetMouseMsg();
            if (msg.uMsg == WM_LBUTTONDOWN) {
                int mx = msg.x, my = msg.y;
                if (in_rect(mx, my, ix, iy1, iw, ih))      { focus = 1; error.clear(); }
                else if (in_rect(mx, my, ix, iy2, iw, ih)) { focus = 2; error.clear(); }
                else if (in_rect(mx, my, ix, iy3, iw, ih)) { focus = 3; error.clear(); }
                else if (in_rect(mx, my, bx1, by, bw, bh)) {
                    if (username.empty() || password.empty())
                        error = "用户名和密码不能为空";
                    else if (password != confirm)
                        error = "两次密码输入不一致";
                    else if (registerUser(username, password)) {
                        g_currentUser = username;
                        EndBatchDraw();
                        return PAGE_MENU;
                    } else
                        error = "用户名已存在，请换一个";
                }
                else if (in_rect(mx, my, bx2, by, bw, bh)) {
                    EndBatchDraw();
                    return PAGE_LOGIN;
                }
                else
                    focus = 0;
            }
        }

        // ---- 键盘输入（EasyX 文字输入必须用 WM_CHAR） ----
        ExMessage kmsg;
        while (peekmessage(&kmsg, EX_CHAR)) {
            if (kmsg.message != WM_CHAR) continue;
            char c = (char)kmsg.ch;
            string* target = nullptr;
            if (focus == 1) target = &username;
            else if (focus == 2) target = &password;
            else if (focus == 3) target = &confirm;

            if (target) {
                if (c == '\b') { if (!target->empty()) target->pop_back(); }
                else if (c >= 32) { *target += c; }
            }
            if (c == '\r' && focus > 0) {
                if (username.empty() || password.empty())
                    error = "用户名和密码不能为空";
                else if (password != confirm)
                    error = "两次密码输入不一致";
                else if (registerUser(username, password)) {
                    g_currentUser = username;
                    EndBatchDraw();
                    return PAGE_MENU;
                } else
                    error = "用户名已存在，请换一个";
            }
        }

        drawButton(bx1, by, bw, bh, "注  册", hoverReg);
        drawButton(bx2, by, bw, bh, "返  回", hoverBack);

        if (!error.empty()) {
            settextcolor(CLR_WRONG);
            settextstyle(16, 0, _T("微软雅黑"));
            draw_text_center(420, error);
        }

        FlushBatchDraw();
        Sleep(16);
    }
    EndBatchDraw();
}

// ================================================================
// 3. 主菜单页面
// ================================================================
Page showMainMenu() {
    Level  selLevel = LV_PRIMARY;
    Order  selOrder = ORD_RANDOM;
    Mode   selMode  = MODE_EN2CN;
    string countStr = "20";
    bool   inputFocus = false;

    // 获取错题本可用词数
    vector<Word> wrongWords = getWrongWords(g_currentUser);

    const char* levelNames[] = { "小学词汇", "初中词汇", "高中词汇", "错题本" };
    const char* orderNames[] = { "随机出现", "字典顺序" };
    const char* modeNames[]  = { "看英文选中文", "看中文选英文" };

    BeginBatchDraw();
    while (true) {
        drawBackground();

        // 顶部用户信息
        settextcolor(CLR_TEXT);
        settextstyle(18, 0, _T("微软雅黑"));
        draw_text(30, 12, "用户: " + g_currentUser);

        // 退出按钮
        int logoutX = 700, logoutY = 8, logoutW = 70, logoutH = 28;
        int startX = 270, startY = 450, startW = 160, startH = 46;
        int wrongBtnX = 460;
        int countX = 220, countY = 340, countW = 80, countH = 34;

        // ---- 鼠标处理 ----
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(GetHWnd(), &pt);

        bool hoverLogout = in_rect(pt.x, pt.y, logoutX, logoutY, logoutW, logoutH);
        bool hoverStart  = in_rect(pt.x, pt.y, startX, startY, startW, startH);
        bool hoverWrong  = in_rect(pt.x, pt.y, wrongBtnX, startY, startW, startH);

        while (MouseHit()) {
            MOUSEMSG msg = GetMouseMsg();
            if (msg.uMsg == WM_LBUTTONDOWN) {
                int mx = msg.x, my = msg.y;
                // 退出
                if (in_rect(mx, my, logoutX, logoutY, logoutW, logoutH)) {
                    g_currentUser = "";
                    EndBatchDraw();
                    return PAGE_LOGIN;
                }
                // 数量输入框
                if (in_rect(mx, my, countX, countY, countW, countH)) {
                    inputFocus = true;
                } else {
                    inputFocus = false;
                }
                // 词汇范围单选
                for (int i = 0; i < 4; i++) {
                    int rx = 180 + (i % 2) * 230;
                    int ry = 75 + (i / 2) * 42;
                    if (in_rect(mx, my, rx - 15, ry - 15, 180, 30)) {
                        if (i == 3 && wrongWords.empty()) { /* 错题本为空，禁止选 */ }
                        else selLevel = (Level)i;
                    }
                }
                // 出题顺序
                for (int i = 0; i < 2; i++) {
                    int rx = 230 + i * 200, ry = 195;
                    if (in_rect(mx, my, rx - 15, ry - 15, 160, 30))
                        selOrder = (Order)i;
                }
                // 背诵模式
                for (int i = 0; i < 2; i++) {
                    int rx = 230 + i * 200, ry = 265;
                    if (in_rect(mx, my, rx - 15, ry - 15, 180, 30))
                        selMode = (Mode)i;
                }
                // 开始背诵
                if (in_rect(mx, my, startX, startY, startW, startH)) {
                    int cnt = atoi(countStr.c_str());
                    if (cnt <= 0) cnt = 10;
                    int avail;
                    if (selLevel == LV_WRONG) avail = (int)wrongWords.size();
                    else avail = getWordCount(selLevel);
                    if (avail > 0) {
                        if (cnt > avail) cnt = avail;
                        QuizConfig cfg;
                        cfg.level = selLevel; cfg.order = selOrder;
                        cfg.mode = selMode;   cfg.count = cnt;
                        startQuiz(cfg);
                        if (getTotalNum() > 0) {
                            Page result = showQuizPage();
                            if (result == PAGE_SCORE) {
                                EndBatchDraw();
                                return PAGE_SCORE;
                            }
                        }
                    }
                }
                // 查看错题本
                if (in_rect(mx, my, wrongBtnX, startY, startW, startH)) {
                    EndBatchDraw();
                    return PAGE_WRONG;
                }
            }
        }

        // 绘制退出按钮
        drawButton(logoutX, logoutY, logoutW, logoutH, "退出", hoverLogout);

        // 分隔线
        setlinecolor(CLR_LINE);
        line(20, 46, 780, 46);

        // 词汇范围
        settextstyle(22, 0, _T("微软雅黑"));
        settextcolor(CLR_TITLE);
        draw_text(60, 75, "词汇范围:");
        for (int i = 0; i < 4; i++) {
            int rx = 180 + (i % 2) * 230;
            int ry = 75 + (i / 2) * 42;
            string label = levelNames[i];
            if (i == 3) label += " (" + to_string(wrongWords.size()) + "词)";
            drawRadio(rx, ry, label, selLevel == (Level)i);
        }

        // 出题顺序
        settextstyle(22, 0, _T("微软雅黑"));
        settextcolor(CLR_TITLE);
        draw_text(60, 195, "出题顺序:");
        for (int i = 0; i < 2; i++)
            drawRadio(230 + i * 200, 195, orderNames[i], selOrder == (Order)i);

        // 背诵模式
        settextstyle(22, 0, _T("微软雅黑"));
        settextcolor(CLR_TITLE);
        draw_text(60, 265, "背诵模式:");
        for (int i = 0; i < 2; i++)
            drawRadio(230 + i * 200, 265, modeNames[i], selMode == (Mode)i);

        // 答题数量
        settextstyle(22, 0, _T("微软雅黑"));
        settextcolor(CLR_TITLE);
        draw_text(60, 345, "答题数量:");
        drawInputBox(countX, countY, countW, countH, countStr, inputFocus, false);
        draw_text(310, 347, "题");

        // 按钮
        drawButton(startX, startY, startW, startH, "开始背诵", hoverStart);
        drawButton(wrongBtnX, startY, startW, startH, "查看错题本", hoverWrong);

        // 键盘输入（答题数量）
        ExMessage kmsg;
        while (peekmessage(&kmsg, EX_CHAR)) {
            if (kmsg.message != WM_CHAR) continue;
            char c = (char)kmsg.ch;
            if (inputFocus) {
                if (c == '\b') { if (!countStr.empty()) countStr.pop_back(); }
                else if (c >= '0' && c <= '9') { if (countStr.size() < 4) countStr += c; }
            }
        }

        FlushBatchDraw();
        Sleep(16);
    }
    EndBatchDraw();
}

// ================================================================
// 4. 答题页面
// ================================================================
Page showQuizPage() {
    bool waiting = false;      // 是否在等待跳下一题
    int  picked  = -1;         // 用户选中的选项索引
    DWORD waitStart = 0;

    // 加载第一题
    vector<Choice> choices = getChoices();
    string qText = getQuestion();
    string phonetic = getPhonetic();

    const char* labels[] = { "A", "B", "C", "D" };

    BeginBatchDraw();
    while (true) {
        drawBackground();

        // 没题了 → 成绩页
        if (isQuizDone() && !waiting) { EndBatchDraw(); return PAGE_SCORE; }

        // ---- 顶部进度条 ----
        int barX = 10, barY = 10, barW = 630, barH = 18;
        setfillcolor(CLR_BAR_BG);
        setlinecolor(CLR_BAR_BG);
        fillrectangle(barX, barY, barX + barW, barY + barH);
        int done = getCurrentNum() - (waiting ? 0 : 1);
        if (getTotalNum() > 0) {
            setfillcolor(CLR_BTN);
            fillrectangle(barX, barY, barX + (int)(barW * done / getTotalNum()), barY + barH);
        }

        settextstyle(16, 0, _T("微软雅黑"));
        settextcolor(CLR_TEXT);
        char buf[120];
        sprintf(buf, "进度: %d/%d   正确: %d   错误: %d",
            min(done + 1, getTotalNum()), getTotalNum(),
            getCorrectNum(), getWrongNum());
        outtextxy(650, barY, to_wstr(buf).c_str());

        // 分隔线
        setlinecolor(CLR_LINE);
        line(10, 38, 790, 38);

        // ---- 题目 ----
        settextcolor(CLR_TITLE);
        settextstyle(28, 0, _T("微软雅黑"));
        draw_text_center(75, qText);

        if (!phonetic.empty()) {
            settextcolor(CLR_TEXT);
            settextstyle(18, 0, _T("微软雅黑"));
            draw_text_center(113, phonetic);
        }

        // ---- 鼠标处理 ----
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(GetHWnd(), &pt);

        int hovered = -1;
        bool hoverBack = false;
        int optStartY = 160;
        int backX = 20, backY = 520, backW = 120, backH = 36;

        if (!waiting) {
            hoverBack = in_rect(pt.x, pt.y, backX, backY, backW, backH);
            for (int i = 0; i < 4 && i < (int)choices.size(); i++) {
                int oy = optStartY + i * 68;
                if (in_rect(pt.x, pt.y, 140, oy, 520, 54))
                    hovered = i;
            }

            while (MouseHit()) {
                MOUSEMSG msg = GetMouseMsg();
                if (msg.uMsg == WM_LBUTTONDOWN) {
                    int mx = msg.x, my = msg.y;
                    if (in_rect(mx, my, backX, backY, backW, backH)) {
                        EndBatchDraw();
                        return PAGE_MENU;
                    }
                    for (int i = 0; i < 4 && i < (int)choices.size(); i++) {
                        int oy = optStartY + i * 68;
                        if (in_rect(mx, my, 140, oy, 520, 54)) {
                            picked = i;
                            answer(i);
                            waiting = true;
                            waitStart = GetTickCount();
                            break;
                        }
                    }
                }
            }
        }

        // ---- 绘制 4 个选项 ----
        for (int i = 0; i < 4 && i < (int)choices.size(); i++) {
            int ox = 140, oy = optStartY + i * 68, ow = 520, oh = 54;

            COLORREF bg = CLR_WHITE;
            COLORREF bd = CLR_INPUT_BD;

            if (waiting && i == picked)
                bg = choices[i].correct ? CLR_CORRECT : CLR_WRONG;
            else if (hovered == i)
                { bg = RGB(235, 245, 255); bd = CLR_BTN; }

            // 圆角按钮：先画边框色，再画填充色（避免使用 roundrect）
            setfillcolor(bd);
            fillroundrect(ox, oy, ox + ow, oy + oh, 10, 10);
            setfillcolor(bg);
            fillroundrect(ox + 1, oy + 1, ox + ow - 1, oy + oh - 1, 9, 9);

            // 字母标号
            settextcolor(CLR_BTN);
            settextstyle(22, 0, _T("微软雅黑"));
            setbkmode(TRANSPARENT);
            outtextxy(ox + 20, oy + 14, to_wstr(labels[i]).c_str());

            // 选项文字
            settextcolor(CLR_TEXT);
            settextstyle(20, 0, _T("微软雅黑"));
            wstring optText = to_wstr(choices[i].text);
            outtextxy(ox + 55, oy + 14, optText.c_str());
        }

        // 返回按钮
        drawButton(backX, backY, backW, backH, "返回菜单", hoverBack);

        // 等待 1.2 秒自动跳下一题
        if (waiting && GetTickCount() - waitStart > 1200) {
            waiting = false;
            picked = -1;
            if (isQuizDone()) { EndBatchDraw(); return PAGE_SCORE; }
            choices  = getChoices();
            qText    = getQuestion();
            phonetic = getPhonetic();
        }

        FlushBatchDraw();
        Sleep(16);
    }
    EndBatchDraw();
}

// ================================================================
// 5. 成绩页面
// ================================================================
Page showScorePage() {
    int correct = getCorrectNum();
    int total   = getTotalNum();
    double pct  = (total > 0) ? (100.0 * correct / total) : 0;

    int backX = 310, backY = 450, backW = 180, backH = 46;

    BeginBatchDraw();
    while (true) {
        drawBackground();

        settextcolor(CLR_TITLE);
        settextstyle(34, 0, _T("微软雅黑"));
        draw_text_center(50, "答题完成");

        char buf[100];
        settextstyle(24, 0, _T("微软雅黑"));
        settextcolor(CLR_TEXT);
        sprintf(buf, "正确:  %d 题", correct);   draw_text_center(140, buf);
        sprintf(buf, "错误:  %d 题", total - correct); draw_text_center(185, buf);
        sprintf(buf, "正确率:  %.0f%%", pct);
        settextcolor(pct >= 60 ? CLR_CORRECT : CLR_WRONG);
        draw_text_center(240, buf);

        // 进度条
        int bx = 150, by = 310, bw = 500, bh = 36;
        setfillcolor(CLR_BAR_BG);
        setlinecolor(CLR_BAR_BG);
        fillroundrect(bx, by, bx + bw, by + bh, 10, 10);
        if (pct > 0) {
            setfillcolor(pct >= 60 ? CLR_CORRECT : CLR_WRONG);
            fillroundrect(bx, by, bx + (int)(bw * pct / 100), by + bh, 10, 10);
        }
        settextcolor(CLR_WHITE);
        settextstyle(20, 0, _T("微软雅黑"));
        setbkmode(TRANSPARENT);
        sprintf(buf, "%.0f%%", pct);
        wstring pws = to_wstr(buf);
        int tw = textwidth(pws.c_str());
        outtextxy(bx + (bw - tw) / 2, by + 6, pws.c_str());

        // ---- 鼠标 ----
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(GetHWnd(), &pt);
        bool hoverBack = in_rect(pt.x, pt.y, backX, backY, backW, backH);

        while (MouseHit()) {
            MOUSEMSG msg = GetMouseMsg();
            if (msg.uMsg == WM_LBUTTONDOWN) {
                if (in_rect(msg.x, msg.y, backX, backY, backW, backH)) {
                    EndBatchDraw();
                    return PAGE_MENU;
                }
            }
        }

        drawButton(backX, backY, backW, backH, "返回主菜单", hoverBack);
        FlushBatchDraw();
        Sleep(16);
    }
    EndBatchDraw();
}

// ================================================================
// 6. 错题本页面
// ================================================================
Page showWrongBookPage() {
    vector<Word> wrongWords = getWrongWords(g_currentUser);

    int scroll = 0;
    int maxScroll = max(0, (int)wrongWords.size() - 13) * 34;

    int reviewX = 250, reviewY = 530, rw = 160, rh = 42;
    int backX   = 430, backY   = 530, bw = 120, bh = 42;

    BeginBatchDraw();
    while (true) {
        drawBackground();

        char title[100];
        sprintf(title, "错题本 - 共 %d 个错词", (int)wrongWords.size());
        settextcolor(CLR_TITLE);
        settextstyle(28, 0, _T("微软雅黑"));
        draw_text_center(20, title);

        setlinecolor(CLR_LINE);
        line(30, 58, 770, 58);

        if (wrongWords.empty()) {
            settextcolor(CLR_TEXT);
            settextstyle(20, 0, _T("微软雅黑"));
            draw_text_center(220, "暂无错题，继续保持！");
        } else {
            // 表头
            settextcolor(CLR_TITLE);
            settextstyle(18, 0, _T("微软雅黑"));
            draw_text(60,  70, "序号");
            draw_text(140, 70, "英文");
            draw_text(340, 70, "音标");
            draw_text(580, 70, "中文");
            setlinecolor(CLR_LINE);
            line(40, 92, 760, 92);

            // 列表
            int startIdx = scroll / 34;
            int y = 98;
            for (int i = startIdx; i < (int)wrongWords.size() && y < 510; i++) {
                // 斑马纹
                if (i % 2 == 0) {
                    setfillcolor(CLR_ROW_EVEN);
                    fillrectangle(40, y - 2, 760, y + 30);
                }
                settextcolor(CLR_TEXT);
                settextstyle(16, 0, _T("微软雅黑"));
                setbkmode(TRANSPARENT);

                char idx[10]; sprintf(idx, "%d", i + 1);
                draw_text(60,  y + 5, idx);
                draw_text(140, y + 5, wrongWords[i].english);
                draw_text(340, y + 5, wrongWords[i].phonetic);
                draw_text(580, y + 5, wrongWords[i].chinese);
                y += 34;
            }
        }

        setlinecolor(CLR_LINE);
        line(30, 520, 770, 520);

        // ---- 鼠标 ----
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(GetHWnd(), &pt);

        bool hoverReview = in_rect(pt.x, pt.y, reviewX, reviewY, rw, rh);
        bool hoverBack   = in_rect(pt.x, pt.y, backX, backY, bw, bh);

        while (MouseHit()) {
            MOUSEMSG msg = GetMouseMsg();
            if (msg.uMsg == WM_LBUTTONDOWN) {
                if (!wrongWords.empty() && in_rect(msg.x, msg.y, reviewX, reviewY, rw, rh)) {
                    QuizConfig cfg;
                    cfg.level = LV_WRONG; cfg.order = ORD_RANDOM;
                    cfg.mode = MODE_EN2CN; cfg.count = (int)wrongWords.size();
                    startQuiz(cfg);
                    if (getTotalNum() > 0) {
                        Page result = showQuizPage();
                        if (result == PAGE_SCORE) {
                            EndBatchDraw();
                            return PAGE_SCORE;
                        }
                    }
                }
                if (in_rect(msg.x, msg.y, backX, backY, bw, bh)) {
                    EndBatchDraw();
                    return PAGE_MENU;
                }
            }
            if (msg.uMsg == WM_MOUSEWHEEL) {
                scroll -= msg.wheel / 10;
                if (scroll < 0) scroll = 0;
                if (scroll > maxScroll) scroll = maxScroll;
            }
        }

        if (!wrongWords.empty())
            drawButton(reviewX, reviewY, rw, rh, "用错题本背诵", hoverReview);
        drawButton(backX, backY, bw, bh, "返回菜单", hoverBack);

        FlushBatchDraw();
        Sleep(16);
    }
    EndBatchDraw();
}
