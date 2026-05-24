#pragma once
#include "common.h"

// 注册（成功返回 true，用户名已存在或为空返回 false）
bool registerUser(const string& username, const string& password);

// 登录（用户名密码匹配返回 true）
bool loginUser(const string& username, const string& password);

// 检查用户名是否已被注册
bool userExists(const string& username);
