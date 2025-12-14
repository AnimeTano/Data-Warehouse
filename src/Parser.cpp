#include "Manager/Parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <ctime>


Parser::Parser(const std::string& dir) : directory(dir) { directory = "bin/"; }


std::vector<std::string> Parser::splitCSV(const std::string& line, char del){
    std::vector<std::string> token;
    std::string stroka;
    std::stringstream ss(line);

    while (std::getline(ss, stroka, del)) {
        token.push_back(stroka);
    }

    return token;
}


std::string Parser::convertToDate(const std::string& dateStr) {
    std::tm tm = {};
    std::stringstream ss(dateStr);

    ss >> std::get_time(&tm, "%Y-%m-%d");

    if (ss.fail()) {
        ss.clear();
        ss.str(dateStr);

        ss >> std::get_time(&tm, "%d.%m.%Y");
    }

    if (ss.fail()) { 
        return dateStr; 
    }

    std::stringstream res;
    res << std::put_time(&tm, "%Y-%m-%d");

    return res.str();
}


std::vector<Sale> Parser::parseSales() {
    std::vector<Sale> sales;
    std::string line;
    std::string filename = "../bin/sales.csv";
    
    std::ifstream file(filename);
    if (file.fail()){
        std::cerr << "Error with opening!\n";
        return sales;
    }

    std::getline(file, line);

    while (std::getline(file, line)) {
        
        auto token = splitCSV(line);

        try {
            Sale sale;

            sale.sale_id = std::stoi(token[0]);
            sale.sale_date = convertToDate(token[1]);
            sale.product_id = std::stoi(token[2]);
            sale.customer_id = std::stoi(token[3]);
            sale.quantity = std::stoi(token[4]);
            sale.amount = std::stod(token[5]);

            sales.push_back(sale);
        } catch (const std::exception& e) {
            std::cerr << "Error with parsing sales: " << e.what() << "\n";
        }
    }

    return sales;
}


std::vector<Product> Parser::parseProducts() {
    std::vector<Product> products;
    std::string line;
    std::string filename = ".,/bin/products.csv";
    
    std::ifstream file(filename);
    if (file.fail()){
        std::cerr << "Error with opening!\n";
        return products;
    }

    std::getline(file, line);

    while (std::getline(file, line)) {
        auto token = splitCSV(line);

        try {
            Product product;

            product.product_id = std::stoi(token[0]);
            product.product_name = token[1];
            product.category = token[2];
            product.price = std::stod(token[3]);

            products.push_back(product);
        } catch (const std::exception& e) {
            std::cerr << "Error with parsing products: " << e.what() << "\n";
        }
    }

    return products;
}


std::vector<Customer> Parser::parseCustomers() {
    std::vector<Customer> customers;
    std::string line;
    std::string filename = "../bin/customers.csv";
    
    std::ifstream file(filename);
    if (file.fail()){
        std::cerr << "Error with opening!\n";
        return customers;
    }

    std::getline(file, line);

    while (std::getline(file, line)) {        
        auto token = splitCSV(line);

        try {
            Customer customer;

            customer.customer_id = std::stoi(token[0]);
            customer.customer_name = token[1];
            customer.region = token[2];

            customers.push_back(customer);
        } catch (const std::exception& e) {
            std::cerr << "Error with parsing customers: " << e.what() << "\n";
        }
    }

    return customers;
}