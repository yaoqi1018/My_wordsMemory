#pragma once
#include "common.h"

// 加载指定级别的词汇（首次调用时自动从 .doc 转 .txt）
vector<Word> loadWords(Level level);

// 获取某级别词汇总数
int getWordCount(Level level);

// 根据 ID 查找单词
Word getWordById(Level level, int id);
