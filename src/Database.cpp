#include "Manager/Database.h"
#include <iostream>


Database::Database(const std::string& conn_str) : connection_string(conn_str) {}


void Database::initializeDatabase() {
    try {
        pqxx::connection conn(connection_string);
        pqxx::work txn(conn);

        txn.exec (
            "CREATE TABLE IF NOT EXISTS products_dim ("
            "product_id INT PRIMARY KEY,"
            "product_name VARCHAR(200),"
            "category VARCHAR(100),"
            "price DECIMAL(10, 2)"
            ")"
        );

        txn.exec (
            "CREATE TABLE IF NOT EXISTS customers_dim ("
            "customer_id INT PRIMARY KEY,"
            "customer_name VARCHAR(100),"
            "region VARCHAR(100)"
            ")"
        );

        txn.exec (
            "CREATE TABLE IF NOT EXISTS time_dim ("
            "time_id SERIAL PRIMARY KEY,"
            "year INTEGER,"
            "quarter INTEGER,"
            "month INTEGER,"
            "day INTEGER,"
            "day_of_week VARCHAR(20),"
            "date DATE UNIQUE"
            ")"
        );

        txn.exec (
            "CREATE TABLE IF NOT EXISTS sales_fact("
            "sale_id INTEGER PRIMARY KEY,"
            "sale_date DATE,"
            "product_id INTEGER REFERENCES products_dim(product_id),"
            "customer_id INTEGER REFERENCES customers_dim(customer_id),"
            "quantity INTEGER,"
            "amount DECIMAL(10, 2)"
            ")"
        );

        txn.exec("CREATE INDEX IF NOT EXISTS idx_sales_fact_product ON sales_fact(product_id)");
        txn.exec("CREATE INDEX IF NOT EXISTS idx_sales_fact_customer ON sales_fact(customer_id)");
        txn.exec("CREATE INDEX IF NOT EXISTS idx_sales_fact_date ON sales_fact(sale_date)");
        txn.exec("CREATE INDEX IF NOT EXISTS idx_products_dim_category ON products_dim(category)");
        txn.exec("CREATE INDEX IF NOT EXISTS idx_customers_dim_region ON customers_dim(region)");
        
        txn.commit();
    } catch (const std::exception& e){
        std::cerr << "Error with initialize: " << e.what() << "\n";
    }
}


void Database::clearDatabase() {
    try {
        pqxx::connection conn(connection_string);
        pqxx::work txn(conn);

        txn.exec("DELETE FROM sales_fact");
        txn.exec("DELETE FROM time_dim");
        txn.exec("DELETE FROM customers_dim");
        txn.exec("DELETE FROM products_dim");

        txn.commit();
    } catch (const std::exception& e){
        std::cerr << "Error with clearing db: " << e.what() << "\n";
    }
}


void Database::TimeDimension(const std::vector<Sale>& sales) {
    try {
        pqxx::connection conn(connection_string);
        pqxx::work txn(conn);

        for (const auto& sale : sales) {
            txn.exec_params(
                "INSERT INTO time_dim (date, year, month, day, quarter, day_of_week) "
                "SELECT $1::DATE,"
                " EXTRACT(YEAR FROM $1::DATE), "
                " EXTRACT(MONTH FROM $1::DATE), "
                " EXTRACT(DAY FROM $1::DATE), "
                " EXTRACT(QUARTER FROM $1::DATE), "
                " TO_CHAR($1::DATE, 'Day') "
                "WHERE NOT EXISTS (SELECT 1 FROM time_dim WHERE date = $1::DATE)",
                sale.sale_date
            );
        }

        txn.commit();
    } catch (const std::exception& e){
        std::cerr << "Error with add: " << e.what() << "\n";
    }
}


void Database::loadProducts(const std::vector<Product>& products) {
    try {
        pqxx::connection conn(connection_string);
        pqxx::work txn(conn);

        for (const auto& prod : products){
            txn.exec_params(
                "INSERT INTO products_dim (product_id, product_name, category, price) "
                "VALUES ($1, $2, $3, $4) "
                "ON CONFLICT (product_id) DO UPDATE SET "
                "product_name = EXCLUDED.product_name, "
                "category = EXCLUDED.category, "
                "price = EXCLUDED.price",
                prod.product_id, prod.product_name, prod.category, prod.price
            );
        }
        
        txn.commit();
        std::cout << "Loaded " << products.size() << " products\n";
    } catch (const std::exception& e){
        std::cerr << "Error loading products: " << e.what() << "\n";
    }
}


void Database::loadCustomers(const std::vector<Customer>& customers){
    try {
        pqxx::connection conn(connection_string);
        pqxx::work txn(conn);
        
        for (const auto& cust : customers) {
            txn.exec_params(
                "INSERT INTO customers_dim (customer_id, customer_name, region) "
                "VALUES ($1, $2, $3) "
                "ON CONFLICT (customer_id) DO UPDATE SET "
                "customer_name = EXCLUDED.customer_name, "
                "region = EXCLUDED.region",
                cust.customer_id, cust.customer_name, cust.region
            );
        }
        
        txn.commit();
    } catch (const std::exception& e) {
        std::cerr << "Error with load: " << e.what() << "\n";
    }
}


void Database::loadSales(const std::vector<Sale>& sales) {
    try {
        pqxx::connection conn(connection_string);
        pqxx::work txn(conn);

        TimeDimension(sales);

        for (const auto& sale : sales) {
            txn.exec_params(
                "INSERT INTO sales_fact (sale_id, sale_date, product_id, customer_id, quantity, amount) "
                "VALUES ($1, $2::DATE, $3, $4, $5, $6) "
                "ON CONFLICT (sale_id) DO UPDATE SET "
                "sale_date = EXCLUDED.sale_date, "
                "product_id = EXCLUDED.product_id, "
                "customer_id = EXCLUDED.customer_id, "
                "quantity = EXCLUDED.quantity, "
                "amount = EXCLUDED.amount",
                sale.sale_id, sale.sale_date, sale.product_id, 
                sale.customer_id, sale.quantity, sale.amount
            );
        }
        
        txn.commit();
    } catch (const std::exception& e){
        std::cerr << "Error with load: " << e.what() << "\n";
    }
}


void Database::analytics() {
    try {
        pqxx::connection conn(connection_string);
        pqxx::work txn(conn);

        std::cout << "\n==== Analytics ====\n";

        std::cout << "1. Sales by category:\n";
        pqxx::result res = txn.exec(
            "SELECT p.category, SUM(s.amount) AS total "
            "FROM sales_fact s "
            "JOIN products_dim p ON s.product_id = p.product_id "
            "GROUP BY p.category "
            "ORDER BY total DESC"
        );

        for (const auto& row : res) {
            std::cout << "  " << row["category"].c_str() << ": " 
                      << row["total"].as<double>() << "\n";
        }

        std::cout << "\n2. Number of purchases by region:\n";
        pqxx::result amres = txn.exec(
            "SELECT c.region, COUNT(s.sale_id) AS number "
            "FROM sales_fact s "
            "JOIN customers_dim c ON s.customer_id = c.customer_id "
            "GROUP BY c.region "
            "ORDER BY number DESC" 
        );

        for (const auto& row : amres) {
            std::cout << "  " << row["region"].c_str() << ": " 
                      << row["number"].as<int>() << "\n";
        }

        std::cout << "\n3. Average ticket by month:\n";
        pqxx::result mres = txn.exec(
            "SELECT EXTRACT(YEAR FROM s.sale_date) AS year, "
            "EXTRACT(MONTH FROM s.sale_date) AS month, "
            "AVG(s.amount) AS average "
            "FROM sales_fact s "
            "GROUP BY year, month "
            "ORDER BY year, month"
        );

        for (const auto& row : mres) {
            std::cout << "  " << row["year"].as<int>() << "-" 
                      << row["month"].as<int>() << ": " 
                      << row["average"].as<double>() << "\n";
        }

        txn.commit();
    } catch (const std::exception& e){
        std::cerr << "Error in analytics: " << e.what() << "\n";
    }
}


pqxx::connection Database::getConnection() { return pqxx::connection(connection_string); }