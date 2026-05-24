#include "wrong_book.h"
#include "word.h"

const char* WRONG_FILE = "data\\wrong.txt";

// 错题本的一条记录
struct WrongEntry {
    string username;
    int    level;   // 0=小学, 1=初中, 2=高中
    int    wordId;
};

// ---------- 读取所有错题记录 ----------
static vector<WrongEntry> loadAllEntries() {
    vector<WrongEntry> entries;
    FILE* fp = fopen(WRONG_FILE, "rb");
    if (!fp) return entries;

    // 跳过 UTF-8 BOM
    unsigned char bom[3];
    if (fread(bom, 1, 3, fp) == 3) {
        if (!(bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF))
            fseek(fp, 0, SEEK_SET);
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        string s(line);
        while (!s.empty() && (s.back() == '\n' || s.back() == '\r'))
            s.pop_back();
        if (s.empty()) continue;

        // 格式：用户名\t级别\t单词ID
        // 手动按 \t 分割
        vector<string> parts;
        string token;
        for (char c : s) {
            if (c == '\t') { parts.push_back(token); token.clear(); }
            else token += c;
        }
        parts.push_back(token);

        if (parts.size() >= 3) {
            WrongEntry e;
            e.username = parts[0];
            e.level    = atoi(parts[1].c_str());
            e.wordId   = atoi(parts[2].c_str());
            entries.push_back(e);
        }
    }
    fclose(fp);
    return entries;
}

// ---------- 添加错词 ----------
void addWrongWord(const string& username, Level level, int wordId) {
    if (level < 0 || level > 2) return;

    // 检查是否已经存在（不重复添加）
    vector<WrongEntry> entries = loadAllEntries();
    for (auto& e : entries) {
        if (e.username == username && e.level == (int)level && e.wordId == wordId)
            return;  // 已存在，跳过
    }

    // 追加到文件末尾
    FILE* fp = fopen(WRONG_FILE, "a");
    if (!fp) fp = fopen(WRONG_FILE, "w");
    if (!fp) return;

    // 如果是新建文件，先写 BOM
    fseek(fp, 0, SEEK_END);
    if (ftell(fp) == 0) {
        unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
        fwrite(bom, 1, 3, fp);
    }

    fprintf(fp, "%s\t%d\t%d\n", username.c_str(), (int)level, wordId);
    fclose(fp);
}

// ---------- 获取用户的所有错词（去重后返回 Word 列表）----------
vector<Word> getWrongWords(const string& username) {
    vector<Word> result;
    vector<WrongEntry> entries = loadAllEntries();

    // 用 seen 数组去重（同级别、同 ID 只取一次）
    bool seen[3][10000] = {};

    for (auto& e : entries) {
        if (e.username != username) continue;
        if (e.level < 0 || e.level > 2) continue;

        if (e.wordId < 10000 && seen[e.level][e.wordId]) continue;
        if (e.wordId < 10000) seen[e.level][e.wordId] = true;

        Word w = getWordById((Level)e.level, e.wordId);
        if (w.id >= 0) {
            result.push_back(w);
        }
    }
    return result;
}

// ---------- 删除某个错词 ----------
void removeWrongWord(const string& username, Level level, int wordId) {
    vector<WrongEntry> entries = loadAllEntries();

    FILE* fp = fopen(WRONG_FILE, "w");
    if (!fp) return;

    unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
    fwrite(bom, 1, 3, fp);

    for (auto& e : entries) {
        // 跳过要删除的那条
        if (e.username == username && e.level == (int)level && e.wordId == wordId)
            continue;
        fprintf(fp, "%s\t%d\t%d\n", e.username.c_str(), e.level, e.wordId);
    }
    fclose(fp);
}

// ---------- 清空用户的所有错词 ----------
void clearWrongBook(const string& username) {
    vector<WrongEntry> entries = loadAllEntries();

    FILE* fp = fopen(WRONG_FILE, "w");
    if (!fp) return;

    unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
    fwrite(bom, 1, 3, fp);

    for (auto& e : entries) {
        if (e.username == username) continue;  // 跳过当前用户
        fprintf(fp, "%s\t%d\t%d\n", e.username.c_str(), e.level, e.wordId);
    }
    fclose(fp);
}
