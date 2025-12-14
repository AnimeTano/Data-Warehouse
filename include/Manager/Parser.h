//#pragma once
#ifndef PARSER_H
#define PARSER_H


#include <string>
#include <vector>


struct Sale {
    int sale_id;
    std::string sale_date;
    int product_id;
    int customer_id;
    int quantity;
    double amount;
};


struct Product {
    int product_id;
    std::string product_name;
    std::string category;
    double price;
};


struct Customer {
    int customer_id;
    std::string customer_name;
    std::string region;
};


class Parser {
    private:
        std::string directory;

    public:
        Parser(const std::string& dir = "./");

        std::vector<std::string> splitCSV(const std::string& line, char del = ',');
        std::string convertToDate(const std::string& dateStr);
        
        std::vector<Sale> parseSales();
        std::vector<Product> parseProducts();
        std::vector<Customer> parseCustomers();

        std::string getDirectory() const { return directory; }
};
#endif