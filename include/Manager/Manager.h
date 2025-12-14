//#pragma once
#ifndef MANAGER_H
#define MANAGER_H


#include "Manager/Database.h"
#include "Manager/Parser.h"


class Manager {
    private:
        Database db;
        Parser parser;

    public:
        Manager(const std::string& conn_str, const std::string& data_dir = "./");

        void clearDatabase();
        void loadData();
        void analytics();
};
#endif