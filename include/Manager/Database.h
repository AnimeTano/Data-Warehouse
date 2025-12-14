//#pragma once
#ifndef DATABASE_H
#define DATABASE_H


#include <string>
#include <pqxx/pqxx>
#include <vector>
#include "Parser.h"


class Database {
    private:
        std::string connection_string;

        void TimeDimension(const std::vector<Sale>& sales);
    
    public:
        Database(const std::string& conn_str);

        void initializeDatabase();
        void clearDatabase();

        void loadProducts(const std::vector<Product>& products);
        void loadCustomers(const std::vector<Customer>& customers);
        void loadSales(const std::vector<Sale>& sales);

        void analytics();

        pqxx::connection getConnection();
};
#endif