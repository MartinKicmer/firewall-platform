#include "../headers/FilterRuleLogger.h"
#include <fstream>
#include <memory>
#include <sqlite3.h>
#include <sstream>
#include <stdexcept>
#include <tuple>
#include <vector>


void FilterRuleLogger::removeRuleByID(std::shared_ptr<FilterRule> rule) {
    sqlite3_stmt* stmt;
    std::string tableName;
    std::shared_ptr<RemoveRule> rmRule = std::dynamic_pointer_cast<RemoveRule>(rule->getRule());
    if(rmRule->layer == "L2") {
        tableName = "l2_rules";
    }
    if(rmRule->layer == "L3") {
        tableName = "l3_rules";
    }
    std::string sql = "DELETE FROM " + tableName + " WHERE id = ?;";
    if (sqlite3_prepare_v2(this->db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, rule->getID());
        
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            std::cout << "Rule " << rule->getID() << " deleted from " << tableName << std::endl;
        }
        sqlite3_finalize(stmt);
    }

}



std::vector<std::shared_ptr<FilterRule>> FilterRuleLogger::findL2RuleByAction(bool permit_) {
    std::vector<std::shared_ptr<FilterRule>> results;
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, permit, limit_count, source_mac, dest_mac FROM l2_rules WHERE permit = ?;";

    if (sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "SQL error (prepare): " << sqlite3_errmsg(this->db) << std::endl;
        return results; 
    }

    sqlite3_bind_int(stmt, 1, permit_ ? 1 : 0);

    while (sqlite3_step(stmt) == SQLITE_ROW) {    
        int ID = sqlite3_column_int(stmt, 0);
        bool permit = (sqlite3_column_int(stmt, 1) != 0); 
        int limitCount = sqlite3_column_int(stmt, 2);
        
        std::string sourceMAC = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string destMAC = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        
        auto rule = std::make_shared<L2Rule>(permit,limitCount,sourceMAC,destMAC);
        results.push_back(std::make_shared<FilterRule>(rule,ID));
    }

    sqlite3_finalize(stmt);
    
    return results;
}

std::vector<std::shared_ptr<FilterRule>> FilterRuleLogger::findL3RuleByAction(bool permit) {
    std::vector<std::shared_ptr<FilterRule>> results;
    sqlite3_stmt* stmt;
    
    const char* sql = "SELECT id, permit, limit_count, source_ip, source_prefix, "
                      "dest_ip, dest_prefix, ttl_max, ttl_min, protocol, tos, allow_fragments "
                      "FROM l3_rules WHERE permit = ?;";

    if (sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "SQL error (prepare L3): " << sqlite3_errmsg(this->db) << std::endl;
        return results; 
    }

    sqlite3_bind_int(stmt, 1, permit ? 1 : 0);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int ID = sqlite3_column_int(stmt, 0);
        bool p = sqlite3_column_int(stmt, 1) != 0;
        int limitCount = sqlite3_column_int(stmt, 2);
        
        const char* srcIpPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        const char* destIpPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        
        std::string sourceIP = srcIpPtr ? srcIpPtr : "";
        int sourcePref = sqlite3_column_int(stmt, 4);
        std::string destIP = destIpPtr ? destIpPtr : "";
        int destPref = sqlite3_column_int(stmt, 6);
        
        int ttlMax = sqlite3_column_int(stmt, 7);
        int ttlMin = sqlite3_column_int(stmt, 8);
        int protocol = sqlite3_column_int(stmt, 9);
        int tos = sqlite3_column_int(stmt, 10);
        bool allowedFrag = sqlite3_column_int(stmt, 11) != 0;

        std::tuple<std::string, int> source{sourceIP, sourcePref};
        std::tuple<std::string, int> dest{destIP, destPref};
        
        auto rule = std::make_shared<L3Rule>(
            p, limitCount, source, dest, ttlMax, ttlMin, protocol, tos, allowedFrag
        );

        results.push_back(std::make_shared<FilterRule>(rule, ID));
    }

    sqlite3_finalize(stmt);
    return results;
}


std::shared_ptr<FilterRule> FilterRuleLogger::findRulesByID(int ID) {
    auto allRules = this->selectAllRules();
    for(auto rule : allRules) {
        if(rule->getID() == ID) {
            return rule;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<FilterRule>> FilterRuleLogger::findRulesByProperties(std::shared_ptr<FilterRule> filterRule) {
    std::vector<std::shared_ptr<FilterRule>> results;
    
    auto selectRule = std::dynamic_pointer_cast<SelectRule>(filterRule->getRule());
    if(selectRule->layer == "L2") {
        results = this->findL2RuleByAction(selectRule->permit);
    }
    if(selectRule->layer == "L3") {
        results = this->findL3RuleByAction(selectRule->permit);
    }
    return results;

}

FilterRuleLogger& FilterRuleLogger::getInstance() {
    static FilterRuleLogger logger("../rules.db","../schema.sql");

    return logger;
}

void FilterRuleLogger::openDB(const std::string& dbName) {
    if(!sqlite3_open(dbName.c_str(), &this->db)) {
         this->setupTables();
    } else {
        throw std::runtime_error("Could not open SQLLTIE DBb\n");
    }
}

void FilterRuleLogger::setupTables() {
    std::ifstream istream(this->schema);
    if(istream.is_open()) {
        std::stringstream ss;
        ss << istream.rdbuf();

        std::string script = ss.str();
        char* errorMessage = nullptr;
        int result = sqlite3_exec(db, script.c_str(), nullptr, nullptr, &errorMessage);

        if (result != SQLITE_OK) {
            std::string err = "Chyba pri provadeni schematu: " + std::string(errorMessage);
            sqlite3_free(errorMessage); 
            throw std::runtime_error(err);
        }

        std::cout << "Loaded schema file: " << this->schema << std::endl;
    } else {
        throw std::runtime_error("Could not open starting SQL script\n");
    }   
}

void FilterRuleLogger::log(std::shared_ptr<FilterRule> rule) {
    auto currentRule = rule->getRule();
    if(auto l2rule = std::dynamic_pointer_cast<L2Rule>(currentRule)) {
        this->insertL2Rule(rule);
    }
    if(auto l3rule = std::dynamic_pointer_cast<L3Rule>(currentRule)) {
        this->insertL3Rule(rule);
    }

}


void FilterRuleLogger::insertL3Rule(std::shared_ptr<FilterRule> rule) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO l3_rules (id,permit,limit_count,source_ip,source_prefix,dest_ip,dest_prefix,ttl_max,ttl_min,protocol,tos,allow_fragments) VALUES (?,?, ?, ?, ?,?, ?, ?, ?,?, ?, ?);";
    if(sqlite3_prepare_v2(this->db, sql, -1,&stmt,nullptr) != SQLITE_OK) {
        throw std::runtime_error("Error while inserting into L3RULES\n");
    }
    auto l3rule = std::dynamic_pointer_cast<L3Rule>(rule->getRule());
    const auto& [source,sourcePref] = l3rule->source;
    const auto& [dest,destPref] = l3rule->dest;

    sqlite3_bind_int(stmt,1,rule->getID());
    sqlite3_bind_int(stmt, 2, l3rule->permit ? 1 : 0);
    sqlite3_bind_int(stmt, 3, l3rule->limitCount);
    sqlite3_bind_text(stmt, 4, source.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, sourcePref);
    sqlite3_bind_text(stmt, 6, dest.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 7, destPref);
    sqlite3_bind_int(stmt, 8, l3rule->ttlMax);
    sqlite3_bind_int(stmt, 9, l3rule->ttlMin);
    sqlite3_bind_int(stmt, 10, l3rule->protocol);
    sqlite3_bind_int(stmt, 11, l3rule->tos);
    sqlite3_bind_int(stmt,12,l3rule->allowFragments);

    if(sqlite3_step(stmt) != SQLITE_DONE) {
        std::runtime_error("Could not finish l3 rule insert statement\n");
    }
    std::cout << "New L3 rule saved into DB " << std::endl;
    sqlite3_finalize(stmt);
}


void FilterRuleLogger::insertL2Rule(std::shared_ptr<FilterRule> rule) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO l2_rules (id,permit, limit_count, source_mac, dest_mac) VALUES (?,?, ?, ?, ?);";
    if (sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Error while inserting into L2RULES\n");
    }

    auto l2rule = std::dynamic_pointer_cast<L2Rule>(rule->getRule());
    sqlite3_bind_int(stmt, 1, rule->getID());
    sqlite3_bind_int(stmt, 2, l2rule->permit ? 1 : 0);
    sqlite3_bind_int(stmt, 3, l2rule->limitCount);
    sqlite3_bind_text(stmt, 4, l2rule->source.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, l2rule->dest.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::runtime_error("Error while writing into L2RULES\n");
    } 
    std::cout << "New L2 rule saved into DB " << std::endl;
    sqlite3_finalize(stmt);
}


std::vector<std::shared_ptr<FilterRule>> FilterRuleLogger::selectL3Rules() {
    std::vector<std::shared_ptr<FilterRule>> rules;
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id,permit,limit_count,source_ip,source_prefix,dest_ip,dest_prefix,ttl_max,ttl_min,protocol,tos,allow_fragments FROM l3_rules;";

    if (sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::runtime_error("Error while preparing sql select for l3 rules\n");
    }

     while (sqlite3_step(stmt) == SQLITE_ROW) {
        int ID = sqlite3_column_int(stmt, 0);
        bool permit = sqlite3_column_int(stmt, 1) != 0;
        int limitCount = sqlite3_column_int(stmt, 2);
        std::string sourceIP = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        int sourcePref = sqlite3_column_int(stmt, 4);
        std::string destIP = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        int destPref = sqlite3_column_int(stmt, 6);
        int ttlMax = sqlite3_column_int(stmt, 7);
        int ttlMin = sqlite3_column_int(stmt, 8);
        int protocol = sqlite3_column_int(stmt, 9);
        int tos = sqlite3_column_int(stmt, 10);
        bool allowedFrag = sqlite3_column_int(stmt, 11) != 0;

        std::tuple<std::string,int> source{sourceIP,sourcePref};
        std::tuple<std::string,int> dest{destIP,destPref};
        auto rule = std::make_shared<L3Rule>(
            permit, limitCount, source, dest,ttlMax,ttlMin,protocol,tos,allowedFrag);
        auto filterRule = std::make_shared<FilterRule>(rule,ID);
        rules.push_back(filterRule);
    }
    
    sqlite3_finalize(stmt);
    return rules;

}

std::vector<std::shared_ptr<FilterRule>> FilterRuleLogger::selectL2Rules() {
    std::vector<std::shared_ptr<FilterRule>> rules;
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id,permit, limit_count, source_mac, dest_mac FROM l2_rules;";

    if (sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
         std::runtime_error("Error while preparing sql select for l3 rules\n");
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        bool permit = sqlite3_column_int(stmt, 1) != 0;
        int limitCount = sqlite3_column_int(stmt, 2);
        std::string srcMac = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string destMac = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        auto rule = std::make_shared<L2Rule>(permit, limitCount, srcMac, destMac);
        auto filterRule = std::make_shared<FilterRule>(rule,id);
    }

    sqlite3_finalize(stmt);
    return rules;
}

std::vector<std::shared_ptr<FilterRule>> FilterRuleLogger::selectAllRules() {
    auto l2rules = this->selectL2Rules();
    auto l3rules = this->selectL3Rules();

    std::vector<std::shared_ptr<FilterRule>> allRules(l2rules);
    allRules.insert(allRules.end(), l2rules.begin(), l2rules.end());
    allRules.insert(allRules.end(), l3rules.begin(), l3rules.end());
    return allRules;
}