#pragma once

#include <vector>

void SetupPythonEnvironment();
const std::vector<CStringW> &GetUsdPathList();
const std::vector<CStringW> &GetUsdPythonPathList();
const CStringW &GetUsdPath();
const CStringW &GetUsdPythonPath();