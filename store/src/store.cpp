#include <store.hpp>

void store::reg(const name& acc, const uint8_t typ) {
    require_auth(get_self());

    if (typ == static_cast<uint8_t>(RegTyp::User)) {
        TABLE_USER users(get_self(), "store"_n.value);
        auto itr = users.find(acc.value);
        check( itr == users.end(), "acc already exist" );

        users.emplace( get_self(), [&]( auto& u ) {
            u.acc = acc;
            u.asset = asset{0, GetSymbol()};
        });
    }
    if (typ == static_cast<uint8_t>(RegTyp::Miner)) {
        TABLE_MINER miners(get_self(), "store"_n.value);
        auto itr = miners.find(acc.value);
        check( itr == miners.end(), "miner already exist" );

        miners.emplace( get_self(), [&]( auto& m ) {
            m.acc = acc;
            m.asset = asset{0, GetSymbol()};
        });
    }
}

void store::addtrust(const name& acc, const name& miner) {
    require_auth(acc);

    TABLE_TRUST trusts(get_self(), acc.value);
    auto idx = trusts.get_index<"acc"_n>();
    auto itr = idx.find(acc.value);

    trusts.emplace(get_self(), [&](auto &d) {
        d.id = trusts.available_primary_key();
        d.acc = acc;
        d.miner = miner;
        d.time = current_time_point();
        d.success = false;
    });
}

void store::recvtrust(const name& miner, uint64_t id, const name& acc) {
    require_auth(miner);

    TABLE_TRUST trusts(get_self(), acc.value);
    auto itr = trusts.find(id);
    check(itr != trusts.end(), "delegate id not exist");
    trusts.modify(itr, get_self(), [&](auto &d) {
        d.success = true;
    });
}

void store::canceltrust(const name& acc, uint64_t id) {
    require_auth(acc);

    TABLE_TRUST trusts(get_self(), acc.value);
    auto itr = trusts.find(id);
    check(itr != trusts.end(), "delegate id not exist");
    trusts.erase(itr);
}

void store::upload(const name& acc, const std::string& enc_hash, const asset& reward) {
    require_auth(acc);

    TABLE_FILE files(get_self(), acc.value);
    files.emplace(get_self(), [&](auto &f) {
        f.id = files.available_primary_key();
        f.enc_hash = enc_hash;
        f.acc = acc;
    });

    TABLE_ORDER orders(get_self(), "store"_n.value);
    orders.emplace(get_self(), [&](auto &o) {
        o.id = orders.available_primary_key();
        o.employer = acc;
        o.time = current_time_point();
        o.amount = reward;
    });
}

void store::cancelupload(const name& acc, const uint64_t orderId) {
    require_auth(acc);

    TABLE_ORDER orders(get_self(), "store"_n.value);
    auto itr = orders.find(orderId);
    check(itr != orders.end(), "orderId invalid");
    check(itr->taker.value == 0, "order already be taken");
    auto fId = itr->fileId;
    orders.erase(itr);

    TABLE_FILE files(get_self(), acc.value);
    auto fitr = files.find(fId);
    check(fitr == files.end(), "file id invalid");
    files.erase(fitr);
}

void store::takeorder(const name& miner, uint64_t orderId, std::string& pubKey) {
    require_auth(miner);

    TABLE_ORDER orders(get_self(), "store"_n.value);
    auto itr = orders.find(orderId);
    check(itr != orders.end(), "orderId invalid");
    check(itr->taker.value == 0, "order already be taken");
    orders.modify(itr, get_self(), [&](auto &o) {
        o.taker = miner;
    });

    TABLE_FILE files(get_self(), itr->employer.value);
    auto fitr = files.find(itr->fileId);
    check(fitr == files.end(), "file id invalid");
    files.modify(fitr, get_self(), [&](auto &f){
        f.pub_key = pubKey;
    });
}

void store::confirm(const name& acc, uint64_t orderId, uint8_t star) {
    require_auth(acc);

    TABLE_ORDER orders(get_self(), "store"_n.value);
    auto itr = orders.find(orderId);
    check(itr != orders.end(), "orderId invalid");
    check(itr->taker.value != 0, "order not be taken");
    //todo transfer asset
}

void store::share(const name& acc, const std::string& enc_hash) {
    require_auth(acc);
}