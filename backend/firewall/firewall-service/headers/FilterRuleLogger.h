#pragma once
#include <memory>
#include <sqlite3.h>
#include <string>
#include "Rules.h"
#include <stdexcept>
#include <vector>
#include "FilterRule.h"
class FilterRuleLogger {
public:
    FilterRuleLogger(const std::string& dbName,const std::string& schema_) : schema(schema_ ) {
        this->openDB(dbName);
    }
    void log(std::shared_ptr<FilterRule> rule);
    std::vector<std::shared_ptr<FilterRule>> selectAllRules();
private:
    void openDB(const std::string& dbName);
    void insertL2Rule(std::shared_ptr<FilterRule> l2rule);
    void insertL3Rule(std::shared_ptr<FilterRule> l2rule);
    void setupTables();
    std::vector<std::shared_ptr<FilterRule>> selectL2Rules();
    std::vector<std::shared_ptr<FilterRule>> selectL3Rules();
    
    sqlite3* db;
    std::string schema;
};