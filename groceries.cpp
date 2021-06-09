//William Martell: Receipts Project.
//16 April, 2021.

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "split.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
using namespace std;

struct Item {
    int item_id;
    string desc;
    double price;
    Item(int id, const string& d, double p) : desc(d) {
        item_id = id;
        price = p;
    }
};

vector<Item> items;

void read_items(const string& fname) {
    string line;
    ifstream f(fname);

    while (getline(f,line)) {
        auto three = split(line, ',');
        items.emplace_back(stoi(three[0]), three[1], stod(three[2]));
    }
}

class LineItem {
public:
    int item_id;
    int quantity;
    LineItem(int id, int q) {
        item_id = id;
        quantity = q;
    }
    
    double sub_total() {
        for (auto& item: items) {
            if (item_id == item.item_id) {
                double result;
                result = item.price * quantity;
                return result;
            }
        }
    }

    friend bool operator<(const LineItem& item1, const LineItem& item2) {
        return item1.item_id <item2.item_id;}
};





struct Customer {
    int cust_id;
    string name;
    string street;
    string city;
    string zip;
    string phone;
    string email;
    Customer(int id, const string& n,const string& s,const string& c, const string& cc, const string& z, const string& p, const string& e) : name(n)
    , street(s), city(c + ", " + cc), zip(z), phone(p), email(e) {
        cust_id = id;
    }
    
    string print_detail() const{
        string s;
        s = s + "Customer ID #" + to_string(cust_id) + ":\n" + name + ", ph. " + phone + ", email: " + email + "\n" + street + "\n" + city + " " + zip + "\n";
        return s;
    }
};

vector<Customer> customers;

void read_customers(const string& fname) {
    string line;
    ifstream f(fname);

    while (getline(f,line)) {
        auto seven = split(line, ',');
        customers.emplace_back(stoi(seven[0]), seven[1], seven[2], seven[3], seven[4], seven[5], seven[6], seven[7]);
    }
}





class Payment {
    double amount;
public:
    virtual string print_detail() const {
        return "";
    }
};

class Credit : public Payment {
    string card_number;
    string expiration;
public:
    Credit(const string& c, const string& e) {
        card_number = c;
        expiration = e;
    }

    string print_detail() const override {
        string s;
        s += "Paid by Credit card " + card_number + ", exp. " + expiration;
        return s;
    }
};

class Paypal : public Payment {
    string paypal_id;
public:   
    Paypal(const string& p) {
        paypal_id = p;
    }
    
    string print_detail() const override {
        string s;
        s += "Paid by Paypal ID: " + paypal_id;
        return s;
    }
};

class WireTransfer : public Payment {
    string bank_id;
    string account_id;
public:
    WireTransfer(const string& b, const string& a) {
        bank_id = b;
        account_id = a;
    }
    
    string print_detail() const override {
        string s;
        s += "Paid by Wire transfer from Bank ID " + bank_id + ", Account# " + account_id;
        return s;
    }
};





class Order {
    int cust_id;
    int order_id;
    string order_date;
    vector<LineItem> line_items;
    Payment* payment;
public:
    Order(int cust, int order, const string& o, vector<LineItem> v, Payment* p) {
        cust_id = cust;
        order_id = order;
        order_date = o;
        line_items = v;
        payment = p;
    }

    ~Order() {};
    
    double total() {
        double result;
        for (auto& line_item: line_items) {
            result += line_item.sub_total();
        }
        return result;
    }

    string print_order() {
        ostringstream OSS;
        string s;
        OSS.precision(2);
        OSS << "====================\n" << "Order #" << to_string(order_id) << ", Date: " << order_date + '\n';
        OSS << "Amount: $" << fixed << total() << ", " << payment->print_detail() << "\n\n";
        int i;
        for (i=0; i < customers.size(); i++) {
            if (cust_id == customers[i].cust_id) {
                OSS << customers[i].print_detail();
                break;
            }
        }

        OSS << "\n Order Detail: \n";
        for (auto& i: line_items) {    
            int j;
            for (j=0; j < items.size(); j++)  {
                if (i.item_id == items[j].item_id) {
                    OSS << "     Item " << to_string(items[j].item_id) << ": \"" << items[j].desc << "\", " << to_string(i.quantity) << " @ " << items[j].price <<'\n';
                }
            }
        }
        
        s = OSS.str();
        return s;
    }
};

vector<Order> orders;

void read_orders(const string& fname) {
    string line;
    ifstream f(fname);
    int count = 0;
    string savedline = "";
    
    while (getline(f,line)) {
        count++;
        if(count % 2 != 0){
            savedline = line;
            continue;
        } else {
            auto first_line = split(savedline, ',');
            int customer_id = stoi(first_line[0]);
            int order_id = stoi(first_line[1]);
            string order_date = first_line[2];
            vector<LineItem> line_items;
            Payment* payment;

            for(int i=3; i<first_line.size(); i++){
                auto id_and_quantity = split(first_line[i], '-');
                line_items.emplace_back(stoi(id_and_quantity[0]),stoi(id_and_quantity[1]));
            }
            sort(line_items.begin(), line_items.end());

            auto payment_field = split(line, ',');
            if (payment_field[0] == "1") {
                payment = new Credit(payment_field[1], payment_field[2]);
            }
            if (payment_field[0] == "2") {
                payment = new Paypal(payment_field[1]);
            }
            if (payment_field[0] == "3") {
                payment = new WireTransfer(payment_field[1],payment_field[2]);
            }
            orders.emplace_back(customer_id, order_id, order_date, line_items, payment);
        }
        
    }
}

int main() {
    read_customers("customers.txt");
    read_items("items.txt.");
    read_orders("orders.txt");
    
    for (auto& order: orders)
        cout << order.print_order() << endl;
    
    ofstream out("order_report.txt");
    for (auto& order: orders)
        out << order.print_order();
    out.close();

}