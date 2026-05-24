#include "quiz_engine.h"
#include "word.h"
#include "wrong_book.h"

// ---------- 答题会话（全局单例）----------
static QuizSession g_ses;
static vector<Word> g_choices;   // 当前题的 4 个选项单词
static int          g_answer;    // 正确答案在 g_choices 中的索引
static bool         g_ready = false;

// ---------- 随机数 ----------
static mt19937 g_rng(random_device{}());

// 打乱 vector（Fisher-Yates 洗牌）
template<typename T>
static void shuffleVec(vector<T>& v) {
    for (int i = (int)v.size() - 1; i > 0; i--) {
        uniform_int_distribution<int> dist(0, i);
        swap(v[i], v[dist(g_rng)]);
    }
}

// ---------- 开始一轮答题 ----------
void startQuiz(const QuizConfig& config) {
    g_ses.config       = config;
    g_ses.currentIndex = 0;
    g_ses.correctCount = 0;
    g_ses.wrongCount   = 0;

    // 1. 加载词汇
    if (config.level == LV_WRONG) {
        g_ses.words = getWrongWords(g_currentUser);
    } else {
        g_ses.words = loadWords(config.level);
    }

    if (g_ses.words.empty()) {
        g_ready = true;
        return;
    }

    // 2. 排序或随机
    if (config.order == ORD_DICT) {
        sort(g_ses.words.begin(), g_ses.words.end(),
            [](const Word& a, const Word& b) { return a.english < b.english; });
    } else {
        shuffleVec(g_ses.words);
    }

    // 3. 截取答题数量（不能超过词库总数）
    int cnt = config.count;
    if (cnt <= 0 || cnt > (int)g_ses.words.size())
        cnt = (int)g_ses.words.size();
    g_ses.words.resize(cnt);

    g_ready = true;
}

// ---------- 获取题干 ----------
string getQuestion() {
    if (!g_ready || g_ses.words.empty()) return "";
    if (g_ses.currentIndex >= (int)g_ses.words.size()) return "";

    const Word& w = g_ses.words[g_ses.currentIndex];
    if (g_ses.config.mode == MODE_EN2CN)
        return w.english;   // 看英文选中文
    else
        return w.chinese;   // 看中文选英文
}

// ---------- 获取音标 ----------
string getPhonetic() {
    if (!g_ready || g_ses.words.empty()) return "";
    if (g_ses.currentIndex >= (int)g_ses.words.size()) return "";
    return g_ses.words[g_ses.currentIndex].phonetic;
}

// ---------- 生成 4 个选项 ----------
vector<Choice> getChoices() {
    if (!g_ready || g_ses.words.empty()) return {};

    const Word& current = g_ses.words[g_ses.currentIndex];

    // 1. 收集干扰词汇（排除当前词）
    vector<Word> pool;
    if (g_ses.config.level == LV_WRONG) {
        // 错题本模式：从全部级别中找干扰项
        for (int lv = 0; lv < 3; lv++) {
            vector<Word> all = loadWords((Level)lv);
            for (auto& w : all) {
                if (w.english != current.english)
                    pool.push_back(w);
            }
        }
    } else {
        vector<Word> all = loadWords(g_ses.config.level);
        for (auto& w : all) {
            if (w.id != current.id)
                pool.push_back(w);
        }
    }

    // 2. 随机选 3 个干扰项
    shuffleVec(pool);
    if (pool.size() > 3) pool.resize(3);

    // 3. 组合 4 个选项：1 正确 + 3 干扰
    g_choices.clear();
    g_choices.push_back(current);
    for (auto& w : pool) g_choices.push_back(w);

    // 4. 随机排列
    shuffleVec(g_choices);

    // 5. 找到正确答案的新位置
    g_answer = -1;
    for (int i = 0; i < (int)g_choices.size(); i++) {
        if (g_choices[i].id == current.id && g_choices[i].english == current.english) {
            g_answer = i;
            break;
        }
    }
    if (g_answer < 0) g_answer = 0;

    // 6. 生成选项文字
    vector<Choice> result;
    for (auto& w : g_choices) {
        Choice c;
        if (g_ses.config.mode == MODE_EN2CN)
            c.text = w.chinese;   // 题干是英文，选项显示中文
        else
            c.text = w.english;   // 题干是中文，选项显示英文
        c.correct = (w.id == current.id && w.english == current.english);
        result.push_back(c);
    }
    return result;
}

// ---------- 答题 ----------
bool answer(int optionIndex) {
    if (!g_ready) return false;

    bool correct = (optionIndex == g_answer);
    if (correct) {
        g_ses.correctCount++;
    } else {
        g_ses.wrongCount++;
        // 错题本模式下不再重复加入
        if (g_ses.config.level != LV_WRONG) {
            const Word& w = g_ses.words[g_ses.currentIndex];
            addWrongWord(g_currentUser, g_ses.config.level, w.id);
        }
    }

    g_ses.currentIndex++;  // 进入下一题
    return correct;
}

// ---------- 简单查询 ----------
int  getCurrentNum() { return min(g_ses.currentIndex + 1, (int)g_ses.words.size()); }
int  getTotalNum()   { return (int)g_ses.words.size(); }
int  getCorrectNum() { return g_ses.correctCount; }
int  getWrongNum()   { return g_ses.wrongCount; }

bool isQuizDone() {
    if (!g_ready) return true;
    if (g_ses.words.empty()) return true;
    return g_ses.currentIndex >= (int)g_ses.words.size();
}

double getProgress() {
    if (g_ses.words.empty()) return 0.0;
    return (double)g_ses.currentIndex / g_ses.words.size();
}
