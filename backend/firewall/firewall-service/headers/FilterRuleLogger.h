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
    static FilterRuleLogger& getInstance();
    void log(std::shared_ptr<FilterRule> rule);
    std::vector<std::shared_ptr<FilterRule>> selectAllRules();
    std::vector<std::shared_ptr<FilterRule>> findRulesByProperties(std::shared_ptr<FilterRule> rule);
    std::shared_ptr<FilterRule> findRulesByID(int ID);
    void removeRuleByID(std::shared_ptr<FilterRule> rule);
private:
    FilterRuleLogger(const std::string& dbName,const std::string& schema_) : schema(schema_ ) {
        this->openDB(dbName);
    }
    void openDB(const std::string& dbName);
    void insertL2Rule(std::shared_ptr<FilterRule> l2rule);
    void insertL3Rule(std::shared_ptr<FilterRule> l2rule);
    void insertL4SimpleRule(std::shared_ptr<FilterRule> l2rule);
    std::vector<std::shared_ptr<FilterRule>> findL2RuleByAction(bool permit);
    std::vector<std::shared_ptr<FilterRule>> findL3RuleByAction(bool permit);
    std::vector<std::shared_ptr<FilterRule>> findL4SimpleRuleByAction(bool permit);
    void setupTables();
    std::vector<std::shared_ptr<FilterRule>> selectL2Rules();
    std::vector<std::shared_ptr<FilterRule>> selectL3Rules();
    std::vector<std::shared_ptr<FilterRule>> selectL4SimpleRules();
    sqlite3* db;
    std::string schema;
};