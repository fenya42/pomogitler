#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <windows.h>

using namespace std;

#define MAX_DISCOUNT 20
#define SERVICE_FEE_PERCENT 5

struct Item {
    string name;
    double price;
    int quantity;
};

struct Order {
    int id;
    string customer;
    vector<Item> items;
};

struct Store {
    vector<Order> orders;
};

bool checkPrice(double price) {
    return (price >= 0 && price < 1000000);
}

double calculateTotal(Order order, double discountPercent = 0) {
    double subtotal = 0;
    for (int i = 0; i < order.items.size(); i++) {
        subtotal = subtotal + (order.items[i].price * order.items[i].quantity);
    }
    double discount = subtotal * discountPercent / 100;
    double service = subtotal * SERVICE_FEE_PERCENT / 100;
    return subtotal - discount + service;
}

double operator+(Order order, double discountPercent) {
    return calculateTotal(order, discountPercent);
}

void printOrder(Order order) {
    cout << "заказ " << order.id << ", клиент: " << order.customer << endl;
    for (int i = 0; i < order.items.size(); i++) {
        cout << "  - " << order.items[i].name << ": " << order.items[i].price << " шт " << order.items[i].quantity << " моры " << endl;
    }
}

void printOrder(Order order, bool showTotal) {
    printOrder(order);
    if (showTotal) {
        cout << "сумма к оплате: " << calculateTotal(order) << endl;
    }
}

void saveToFile(Store store) {
    ofstream file("orders.txt");
    for (int i = 0; i < store.orders.size(); i++) {
        file << store.orders[i].id << endl;
        file << store.orders[i].customer << endl;
        file << store.orders[i].items.size() << endl;
        for (int j = 0; j < store.orders[i].items.size(); j++) {
            file << store.orders[i].items[j].name << endl;
            file << store.orders[i].items[j].price << endl;
            file << store.orders[i].items[j].quantity << endl;
        }
        file << "#" << endl;
    }
    file.close();
    cout << "сохранено" << endl;
}

Store loadFromFile() {
    Store store;
    ifstream file("orders.txt");
    if (!file.is_open()) {
        cout << "нот фаунд" << endl;
        return store;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        Order order;
        order.id = stoi(line);

        getline(file, order.customer);

        int itemsCount;
        getline(file, line);
        itemsCount = stoi(line);

        for (int i = 0; i < itemsCount; i++) {
            Item item;
            getline(file, item.name);
            getline(file, line);
            item.price = stod(line);
            getline(file, line);
            item.quantity = stoi(line);
            order.items.push_back(item);
        }

        getline(file, line);
        store.orders.push_back(order);
    }

    file.close();
    cout << "загруженно" << endl;
    return store;
}

void removeEmptyOrders(Store& store) {
    for (int i = 0; i < store.orders.size(); i++) {
        if (store.orders[i].items.size() == 0) {
            store.orders.erase(store.orders.begin() + i);
            i--;
        }
    }
    cout << "пустые заказы снесены" << endl;
}

bool sortByTotal(Order a, Order b) {
    return calculateTotal(a) < calculateTotal(b);
}

bool sortByCustomer(Order a, Order b) {
    return a.customer < b.customer;
}

bool sortByItemCount(Order a, Order b) {
    int countA = 0;
    for (int i = 0; i < a.items.size(); i++) {
        countA = countA + a.items[i].quantity;
    }

    int countB = 0;
    for (int i = 0; i < b.items.size(); i++) {
        countB = countB + b.items[i].quantity;
    }

    return countA < countB;
}

void sortOrders(Store& store, bool (*cmp)(Order, Order)) {
    for (int i = 0; i < store.orders.size() - 1; i++) {
        for (int j = 0; j < store.orders.size() - i - 1; j++) {
            if (cmp(store.orders[j + 1], store.orders[j])) {
                Order temp = store.orders[j];
                store.orders[j] = store.orders[j + 1];
                store.orders[j + 1] = temp;
            }
        }
    }
}

string mostPopularItem(Store store) {
    vector<string> itemNames;
    vector<int> itemQuantities;

    for (int i = 0; i < store.orders.size(); i++) {
        for (int j = 0; j < store.orders[i].items.size(); j++) {
            string currentName = store.orders[i].items[j].name;
            int currentQuantity = store.orders[i].items[j].quantity;

            bool found = false;
            for (int k = 0; k < itemNames.size(); k++) {
                if (itemNames[k] == currentName) {
                    itemQuantities[k] = itemQuantities[k] + currentQuantity;
                    found = true;
                    break;
                }
            }

            if (!found) {
                itemNames.push_back(currentName);
                itemQuantities.push_back(currentQuantity);
            }
        }
    }

    if (itemNames.size() == 0) {
        return "";
    }

    int maxIndex = 0;
    for (int i = 1; i < itemQuantities.size(); i++) {
        if (itemQuantities[i] > itemQuantities[maxIndex]) {
            maxIndex = i;
        }
    }

    return itemNames[maxIndex];
}

void salesMatrix(Store store) {
    vector<string> allItems;
    for (int i = 0; i < store.orders.size(); i++) {
        for (int j = 0; j < store.orders[i].items.size(); j++) {
            string currentName = store.orders[i].items[j].name;
            bool found = false;
            for (int k = 0; k < allItems.size(); k++) {
                if (allItems[k] == currentName) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                allItems.push_back(currentName);
            }
        }
    }

    if (allItems.size() == 0) {
        cout << "тут пусто" << endl;
        return;
    }

    cout << "Заказ\\Товар";
    for (int i = 0; i < allItems.size(); i++) {
        cout << "\t" << allItems[i];
    }
    cout << endl;

    for (int i = 0; i < store.orders.size(); i++) {
        cout << "Заказ " << store.orders[i].id;

        vector<double> row;
        for (int j = 0; j < allItems.size(); j++) {
            row.push_back(0);
        }

        for (int j = 0; j < store.orders[i].items.size(); j++) {
            for (int k = 0; k < allItems.size(); k++) {
                if (store.orders[i].items[j].name == allItems[k]) {
                    row[k] = store.orders[i].items[j].price * store.orders[i].items[j].quantity;
                    break;
                }
            }
        }

        for (int j = 0; j < row.size(); j++) {
            cout << "\t" << row[j];
        }
        cout << endl;
    }
}

void findOrdersBySubstring(Store store) {
    string substr;
    cout << "подстрока для поиска: ";
    cin >> substr;

    for (int i = 0; i < store.orders.size(); i++) {
        for (int j = 0; j < store.orders[i].items.size(); j++) {
            if (store.orders[i].items[j].name.find(substr) != -1) {
                printOrder(store.orders[i], true);
                cout  << endl;
                break;
            }
        }
    }
}

void printStatistics(Store store) {
    if (store.orders.size() == 0) {
        cout << "тут пусто" << endl;
        return;
    }

    double totalRevenue = 0;
    double maxOrder = 0;
    int maxOrderIndex = 0;

    vector<string> customers;
    vector<double> customerSpending;

    for (int i = 0; i < store.orders.size(); i++) {
        double orderTotal = calculateTotal(store.orders[i]);
        totalRevenue = totalRevenue + orderTotal;

        if (orderTotal > maxOrder) {
            maxOrder = orderTotal;
            maxOrderIndex = i;
        }

        bool found = false;
        for (int j = 0; j < customers.size(); j++) {
            if (customers[j] == store.orders[i].customer) {
                customerSpending[j] = customerSpending[j] + orderTotal;
                found = true;
                break;
            }
        }

        if (!found) {
            customers.push_back(store.orders[i].customer);
            customerSpending.push_back(orderTotal);
        }
    }

    double avgCheck = totalRevenue / store.orders.size();

    int maxCustomerIndex = 0;
    for (int i = 1; i < customerSpending.size(); i++) {
        if (customerSpending[i] > customerSpending[maxCustomerIndex]) {
            maxCustomerIndex = i;
        }
    }

    cout << "общая выручка: " << totalRevenue << endl;
    cout << "авг чек: " << avgCheck << endl;
    cout << "самый дорогой заказ: " << maxOrder << " (заказ #" << store.orders[maxOrderIndex].id << ")" << endl;
    cout << "клиент с максимальными тратами: " << customers[maxCustomerIndex] << " (" << customerSpending[maxCustomerIndex] << ")" << endl;
    cout << "самый популярный товар: " << mostPopularItem(store) << endl;
}

void addOrder(Store& store) {
    Order newOrder;

    cout << "ID заказа: ";
    cin >> newOrder.id;

    cout << "имя клиента: ";
    cin >> newOrder.customer;

    int numItems;
    cout << "кол=во товаров: ";
    cin >> numItems;

    for (int i = 0; i < numItems; i++) {
        Item newItem;
        cout << "товар " << (i + 1) << endl;
        cout << "название: ";
        cin >> newItem.name;

        do {
            cout << "цена: ";
            cin >> newItem.price;
            if (!checkPrice(newItem.price)) {
                cout << "унга бунга циферку проверь" << endl;
            }
        } while (!checkPrice(newItem.price));

        cout << "кол-во: ";
        cin >> newItem.quantity;

        newOrder.items.push_back(newItem);
    }

    store.orders.push_back(newOrder);
    cout << "добавлено" << endl;
}

void showAllOrders(Store store) {
    if (store.orders.size() == 0) {
        cout << "заказов нет" << endl;
        return;
    }

    for (int i = 0; i < store.orders.size(); i++) {
        printOrder(store.orders[i], true);
        cout  << endl;
    }
}

void sortingMenu(Store& store) {
    int choice;
    cout << "1. сортировать по сумме" << endl;
    cout << "2. сортировать по имени клиента" << endl;
    cout << "3. сортировать по количеству товаров" << endl;
    cout << "цифру: ";
    cin >> choice;

    if (choice == 1) {
        sortOrders(store, sortByTotal);
        cout << "сортед по сумме" << endl;
    }
    else if (choice == 2) {
        sortOrders(store, sortByCustomer);
        cout << "сортед по имени клиента" << endl;
    }
    else if (choice == 3) {
        sortOrders(store, sortByItemCount);
        cout << "сортед по количеству товаров" << endl;
    }
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    Store store = loadFromFile();
    int choice;

    do {
        cout  << endl;
        cout << "1. добавить заказ" << endl;
        cout << "2. показать все заказы" << endl;
        cout << "3. снести пустого" << endl;
        cout << "4. сортировака" << endl;
        cout << "5. показать матрицу продаж" << endl;
        cout << "6. поиск заказов по подстроке" << endl;
        cout << "7. показать статистику" << endl;
        cout << "8. сохранить" << endl;
        cout << "0. покинуть дурку" << endl;
        cout << "цифру сюда: ";
        cin >> choice;

        switch (choice) {
        case 1:
            addOrder(store);
            break;
        case 2:
            showAllOrders(store);
            break;
        case 3:
            removeEmptyOrders(store);
            break;
        case 4:
            sortingMenu(store);
            break;
        case 5:
            salesMatrix(store);
            break;
        case 6:
            findOrdersBySubstring(store);
            break;
        case 7:
            printStatistics(store);
            break;
        case 8:
            saveToFile(store);
            break;
        case 0:
            cout << "не возвращайся" << endl;
            break;
        default:
            cout << "не та цифра" << endl;
        }
    } while (choice != 0);

    return 0;
}