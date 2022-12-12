#pragma once
#include <string>

struct ST_REPORT;

bool sendResultToServer(const char* pipe, const ST_REPORT report);