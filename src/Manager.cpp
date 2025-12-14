#include "Manager/Manager.h"
#include <iostream>


Manager::Manager(const std::string& conn_str, const std::string& dir) : db(conn_str), parser(dir) { db.initializeDatabase(); }


void Manager::clearDatabase() { db.clearDatabase(); }


void Manager::loadData() {
    auto prod = parser.parseProducts();
    auto cust = parser.parseCustomers();
    auto sales = parser.parseSales();

    std::cout << "Parsed " << prod.size() << " products, " 
    << cust.size() << " customers, " << sales.size() << " sales\n";

    db.loadProducts(prod);
    db.loadCustomers(cust);
    db.loadSales(sales);

    std::cout << "Data loaded!\n";
}


void Manager::analytics() { db.analytics(); }