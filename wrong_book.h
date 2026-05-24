#pragma once
#include "common.h"

// 答错时调用：把单词加入当前用户的错题本
void addWrongWord(const string& username, Level level, int wordId);

// 获取当前用户的所有错词（返回 Word 列表，已去重）
vector<Word> getWrongWords(const string& username);

// 从错题本中删除某个单词（答对后可调用）
void removeWrongWord(const string& username, Level level, int wordId);

// 清空当前用户的所有错词
void clearWrongBook(const string& username);
