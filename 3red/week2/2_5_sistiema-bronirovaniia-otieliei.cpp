#include <iomanip>
#include <iostream>
#include <queue>
#include <map>
#include <vector>
#include <string>
#include <utility>

using namespace std;

class Hotel;

class BookingManager {
public:
    Hotel& Get(const string& h);

    void Book(int64_t time, const string& h, uint32_t clid, uint16_t rooms);

    int64_t LastTime() const;

    uint32_t Clients(const string& h);

    uint64_t Rooms(const string& h);

private:
    map<string, Hotel> hotels_;
    int64_t last_time_ = 0;
};

class Hotel {
    struct BookNode {
        int64_t time;
        uint32_t clid;
        uint16_t rooms;
    };

public:
    explicit Hotel(const BookingManager& manager)
        : manager_(manager)
    {}

    void Book(int64_t time, uint32_t clid, uint16_t rooms) {
        queue_.push({time, clid, rooms});
        clients_[clid] += rooms;
        rooms_ += rooms;
    }

    uint32_t Clients() {
        CleanQueue();
        return clients_.size();
    }

    uint64_t Rooms() {
        CleanQueue();
        return rooms_;
    }

private:
    static const int PERIOD = 86400;

    const BookingManager& manager_;

    map<uint32_t, uint64_t> clients_;
    uint64_t rooms_ = 0;

    queue<BookNode> queue_;

    void CleanQueue() {
        while (CheckAndRemoveFirstBook()) {}
    }

    bool CheckAndRemoveFirstBook() {
        if (queue_.empty()) {
            return false;
        }

        auto book = queue_.front();
        if (book.time > (manager_.LastTime() - PERIOD)) {
            return false;
        }

        auto it = clients_.find(book.clid);
        it->second -= book.rooms;
        rooms_ -= book.rooms;

        if (it->second == 0) {
            clients_.erase(it);
        }

        queue_.pop();
        return true;
    }
};

Hotel& BookingManager::Get(const string& h) {
    auto it = hotels_.find(h);
    if (it == hotels_.end()) {
        it = hotels_.emplace(h, *this).first;
    }
    return it->second;
}

void BookingManager::Book(int64_t time, const string& h, uint32_t clid, uint16_t rooms) {
    last_time_ = time;
    Get(h).Book(time, clid, rooms);
}

int64_t BookingManager::LastTime() const {
    return last_time_;
}

uint32_t BookingManager::Clients(const string& h) {
    return Get(h).Clients();
}

uint64_t BookingManager::Rooms(const string& h) {
    return Get(h).Rooms();
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    BookingManager manager;

    int query_count;
    cin >> query_count;

    for (int query_id = 0; query_id < query_count; ++query_id) {
        string query_type;
        cin >> query_type;

        if (query_type == "BOOK") {
            string hotel;
            int64_t time;
            uint32_t clid;
            uint16_t rooms;
            cin >> time >> hotel >> clid >> rooms;
            manager.Book(time, hotel, clid, rooms);
        } else if (query_type == "CLIENTS") {
            string hotel;
            cin >> hotel;
            cout << manager.Clients(hotel) << '\n';
        } else if (query_type == "ROOMS") {
            string hotel;
            cin >> hotel;
            cout << manager.Rooms(hotel) << '\n';
        }
    }

    return 0;
}