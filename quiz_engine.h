#pragma once
#include "common.h"

// 一道题的选项
struct Choice {
    string text;     // 选项文字
    bool   correct;  // 是否正确答案
};

// 开始一轮答题
void startQuiz(const QuizConfig& config);

// 获取当前题目的题干文字（英文或中文，取决于模式）
string getQuestion();

// 获取当前题目的音标
string getPhonetic();

// 获取 4 个选项
vector<Choice> getChoices();

// 当前题号（1开始）、总题数、对题数、错题数
int  getCurrentNum();
int  getTotalNum();
int  getCorrectNum();
int  getWrongNum();

// 答题是否结束
bool isQuizDone();

// 提交答案：传入选项索引 0-A, 1-B, 2-C, 3-D，返回 true=答对
bool answer(int optionIndex);

// 当前进度 0.0 ~ 1.0
double getProgress();
