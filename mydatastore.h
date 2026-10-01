#ifndef MYDATASTORE_H
#define MYDATASTORE_H
#include <string>
#include <vector>
#include <set>
#include <map>
#include <deque>
#include <iostream>
#include "datastore.h"
#include "product.h"
#include "user.h"

class MyDataStore : public DataStore {
public:
  MyDataStore();
  virtual ~MyDataStore();
  void addProduct(Product* p);
  void addUser(User* u);
  std::vector<Product*> search(std::vector<std::string>& terms, int type);
  void dump(std::ostream& ofile);

  bool addToCart(const std::string& username, int hitIndex, std::vector<Product*>& hits);
  bool viewCart(const std::string& username);
  bool buyCart(const std::string& username);

private:
  std::vector<Product*> products_;
  std::map<std::string, User*> users_;
  std::map<std::string, std::set<Product*> > index_;
  std::map<std::string, std::deque<Product*> > carts_;
};
#endif