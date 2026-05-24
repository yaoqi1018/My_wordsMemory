#include "word.h"

// ---------- 词汇缓存 ----------
static vector<Word> g_words[3];
static bool g_loaded[3] = { false, false, false };

// ---------- 文件路径 ----------
static const char* getTxtPath(Level level) {
    switch (level) {
    case LV_PRIMARY: return "data\\words_primary.txt";
    case LV_JUNIOR:  return "data\\words_junior.txt";
    case LV_SENIOR:  return "data\\words_senior.txt";
    default:         return "";
    }
}

// ---------- 辅助 ----------

static vector<string> split(const string& s, char delim) {
    vector<string> result;
    string token;
    for (char c : s) {
        if (c == delim) { result.push_back(token); token.clear(); }
        else token += c;
    }
    result.push_back(token);
    return result;
}

// ---------- 公开接口 ----------

vector<Word> loadWords(Level level) {
    if (level < 0 || level > 2) return {};

    if (g_loaded[level]) return g_words[level];

    const char* filename = getTxtPath(level);
    FILE* fp = fopen(filename, "rb");
    if (!fp) return {};

    vector<Word> words;
    char buf[2048];
    int id = 0;

    while (fgets(buf, sizeof(buf), fp)) {
        string s(buf);
        while (!s.empty() && (s.back() == '\n' || s.back() == '\r'))
            s.pop_back();
        if (s.empty()) continue;

        vector<string> parts = split(s, '\t');
        if (parts.size() < 3) continue;

        Word w;
        w.id       = id++;
        w.english  = parts[0];
        w.phonetic = parts[1];
        w.chinese  = parts[2];
        words.push_back(w);
    }
    fclose(fp);

    g_words[level] = words;
    g_loaded[level] = true;
    return words;
}

int getWordCount(Level level) {
    if (level < 0 || level > 2) return 0;
    if (!g_loaded[level]) loadWords(level);
    return (int)g_words[level].size();
}

Word getWordById(Level level, int id) {
    if (level < 0 || level > 2) return { -1, "", "", "" };
    if (!g_loaded[level]) loadWords(level);
    for (auto& w : g_words[level]) {
        if (w.id == id) return w;
    }
    return { -1, "", "", "" };
}
