#include "user.h"

const char* USER_FILE = "data\\users.txt";

// ---------- 从文件读取所有用户 ----------
static vector<User> loadAllUsers() {
    vector<User> users;
    FILE* fp = fopen(USER_FILE, "rb");
    if (!fp) return users;  // 文件不存在，返回空列表

    // 跳过 UTF-8 BOM
    unsigned char bom[3];
    if (fread(bom, 1, 3, fp) == 3) {
        if (!(bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF))
            fseek(fp, 0, SEEK_SET);  // 没有 BOM，回到文件头
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        string s(line);
        // 去掉末尾换行符
        while (!s.empty() && (s.back() == '\n' || s.back() == '\r'))
            s.pop_back();
        if (s.empty()) continue;

        // 用 Tab 分割：用户名\t密码
        size_t tabPos = s.find('\t');
        if (tabPos == string::npos) continue;

        User u;
        u.name     = s.substr(0, tabPos);
        u.password = s.substr(tabPos + 1);
        users.push_back(u);
    }
    fclose(fp);
    return users;
}

// ---------- 把所有用户写入文件 ----------
static void saveAllUsers(const vector<User>& users) {
    FILE* fp = fopen(USER_FILE, "w");
    if (!fp) return;

    // 先写 UTF-8 BOM（让记事本能正确打开）
    unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
    fwrite(bom, 1, 3, fp);

    for (auto& u : users) {
        fprintf(fp, "%s\t%s\n", u.name.c_str(), u.password.c_str());
    }
    fclose(fp);
}

// ---------- 注册 ----------
bool registerUser(const string& username, const string& password) {
    // 用户名为空或密码为空 → 失败
    if (username.empty() || password.empty()) return false;

    // 用户名已存在 → 失败
    if (userExists(username)) return false;

    // 加入用户列表并保存
    vector<User> users = loadAllUsers();
    users.push_back({ username, password });
    saveAllUsers(users);
    return true;
}

// ---------- 登录 ----------
bool loginUser(const string& username, const string& password) {
    vector<User> users = loadAllUsers();
    for (auto& u : users) {
        if (u.name == username && u.password == password)
            return true;
    }
    return false;
}

// ---------- 检查用户是否存在 ----------
bool userExists(const string& username) {
    vector<User> users = loadAllUsers();
    for (auto& u : users) {
        if (u.name == username) return true;
    }
    return false;
}
