#include <iostream>
#include <iomanip>
#include "mydatastore.h"
#include "util.h"

using namespace std;

MyDataStore::MyDataStore()
{
}

MyDataStore::~MyDataStore()
{
    for (size_t i = 0; i < products_.size(); i++) {
        delete products_[i];
    }
    for (map<string, User*>::iterator it = users_.begin(); it != users_.end(); ++it) {
        delete it->second;
    }
}

void MyDataStore::addProduct(Product* p)
{
    products_.push_back(p);
    set<string> kw = p->keywords();
    for (set<string>::iterator it = kw.begin(); it != kw.end(); ++it) {
        index_[convToLower(*it)].insert(p);
    }
}

void MyDataStore::addUser(User* u)
{
    users_[convToLower(u->getName())] = u;
}

vector<Product*> MyDataStore::search(vector<string>& terms, int type)
{
    vector<Product*> hits;
    if (terms.empty()) {
        return hits;
    }
    set<Product*> result;
    for (size_t i = 0; i < terms.size(); i++) {
        set<Product*> current;
        map<string, set<Product*> >::iterator it = index_.find(convToLower(terms[i]));
        if (it != index_.end()) {
            current = it->second;
        }
        if (i == 0) {
            result = current;
        }
        else if (type == 0) {
            result = setIntersection(result, current);
        }
        else {
            result = setUnion(result, current);
        }
    }
    for (set<Product*>::iterator it = result.begin(); it != result.end(); ++it) {
        hits.push_back(*it);
    }
    return hits;
}

void MyDataStore::dump(ostream& ofile)
{
    ofile << "<products>" << endl;
    for (size_t i = 0; i < products_.size(); i++) {
        products_[i]->dump(ofile);
    }
    ofile << "</products>" << endl;
    ofile << "<users>" << endl;
    for (map<string, User*>::iterator it = users_.begin(); it != users_.end(); ++it) {
        it->second->dump(ofile);
    }
    ofile << "</users>" << endl;
}

bool MyDataStore::addToCart(const string& username, int hitIndex, vector<Product*>& hits)
{
    string key = convToLower(username);
    if (users_.find(key) == users_.end()) {
        return false;
    }
    if (hitIndex < 1 || hitIndex > (int)hits.size()) {
        return false;
    }
    carts_[key].push_back(hits[hitIndex - 1]);
    return true;
}

bool MyDataStore::viewCart(const string& username)
{
    string key = convToLower(username);
    if (users_.find(key) == users_.end()) {
        return false;
    }
    deque<Product*>& cart = carts_[key];
    int n = 1;
    for (deque<Product*>::iterator it = cart.begin(); it != cart.end(); ++it) {
        cout << "Item " << setw(3) << n << endl;
        cout << (*it)->displayString() << endl;
        cout << endl;
        n++;
    }
    return true;
}

bool MyDataStore::buyCart(const string& username)
{
    string key = convToLower(username);
    map<string, User*>::iterator uit = users_.find(key);
    if (uit == users_.end()) {
        return false;
    }
    User* user = uit->second;
    deque<Product*>& cart = carts_[key];
    deque<Product*> remaining;
    for (deque<Product*>::iterator it = cart.begin(); it != cart.end(); ++it) {
        Product* p = *it;
        if (p->getQty() > 0 && user->getBalance() >= p->getPrice()) {
            p->subtractQty(1);
            user->deductAmount(p->getPrice());
        }
        else {
            remaining.push_back(p);
        }
    }
    cart = remaining;
    return true;
}