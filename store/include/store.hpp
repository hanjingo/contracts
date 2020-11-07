#include <set>
#include <string>
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/time.hpp>
#include <eosio/transaction.hpp>

using namespace eosio;
using eosio::symbol;
using eosio::current_time_point;

CONTRACT store : public contract 
{
using contract::contract;

public:
   enum class RegTyp : uint8_t {
      User = 0, // 普通用户
      Miner,    // 矿工
   };

   // 货币符号
   static symbol GetSymbol()
   {
      return symbol(symbol_code("EOS"), 4);
   }

public:
   /**
    * @brief 注册 用户/矿工
    * @param acc 账号
    * @param typ 类型
    */
   ACTION reg(const name& acc, uint8_t typ);

   /**
    * @brief 信任矿工
    * @param acc 账号
    * @param miner 矿工
    */
   ACTION addtrust(const name& acc, const name& miner);

   /**
    * @brief 接受信任
    * @param miner 矿工
    * @param id 委托id

    */
   ACTION recvtrust(const name& miner, uint64_t id, const name& acc);

   /**
    * @brief 取消委托
    * @param acc 用户
    * @param id 委托id
    */
   ACTION canceltrust(const name& acc, uint64_t id);

   /**
    * @brief 上传
    * @param acc 用户
    * @param enc_hash 文件加密的文件hash
    * @param reward 上传奖励
    */
   ACTION upload(const name& acc, const std::string& enc_hash, const asset& reward);

   /**
    * @brief 取消上传
    * @param acc 用户
    * @param orderId 订单id
    */
   ACTION cancelupload(const name& acc, const uint64_t orderId);

   /**
    * @brief 接受订单
    * @param miner 矿工
    * @param orderId 订单id
    * @param pub_key 公钥
    */
   ACTION takeorder(const name& miner, uint64_t orderId, std::string& pubKey);

   /**
    * @brief 确认上传成功
    * @param acc 用户
    * @param orderId 订单id
    * @param star 打分
    */
   ACTION confirm(const name& acc, uint64_t orderId, uint8_t star);

   /**
    * @brief 分享文件
    * @param acc 账号
    * @param enc_hash 文件hash
    */
   ACTION share(const name& acc, const std::string& enc_hash);

private:
   // 用户表
   TABLE user
   {
      name acc;    // 用户账号
      asset asset; // 资产

      uint64_t primary_key() const { return acc.value; }
   };
   typedef eosio::multi_index<
      "user"_n, user
   > TABLE_USER;

   // 矿工表
   TABLE miner
   {
      name acc;     // 矿工账号
      asset asset;  // 资产
      uint8_t star; // 服务评分

      uint64_t primary_key() const { return acc.value; }
   };
   typedef eosio::multi_index<
      "miner"_n, miner
   > TABLE_MINER;

   // 文件表
   TABLE file
   {
      uint64_t id;          // 自增id
      std::string enc_hash; // 加密文件hash
      name acc;             // 所属人
      std::string pub_key;  // 加密公钥

      uint64_t primary_key() const { return id; }
      uint64_t byacc() const { return acc.value; }
   };
   typedef eosio::multi_index<
      "file"_n, file,
      indexed_by<"acc"_n, const_mem_fun<file, uint64_t, &file::byacc>>
   > TABLE_FILE;

   // 信任表
   TABLE trust
   {
      uint64_t id;         // 申请id
      name acc;            // 用户账号
      name miner;          // 矿工账号
      time_point_sec time; // 申请时间
      bool success;        // 委托是否成功

      uint64_t primary_key() const { return id; }
      uint64_t byacc() const { return acc.value; }
      uint64_t byminer() const { return miner.value; }
   };
   typedef eosio::multi_index<
      "trust"_n, trust,
      indexed_by<"acc"_n, const_mem_fun<trust, uint64_t, &trust::byacc>>,
      indexed_by<"miner"_n, const_mem_fun<trust, uint64_t, &trust::byminer>>
   > TABLE_TRUST;

   // 订单表
   TABLE order
   {
      uint64_t id;         // 记录id
      name employer;       // 订单发起人
      time_point_sec time; // 创建时间
      asset amount;        // 订单金额
      name taker;          // 接单人
      uint64_t fileId;     // 文件id

      uint64_t primary_key() const { return id; }
      uint64_t byemployer() const { return employer.value; }
      uint64_t bytaker() const { return taker.value; }
   };
   typedef eosio::multi_index<
      "order"_n, order,
      indexed_by<"byemployer"_n, const_mem_fun<order, uint64_t, &order::byemployer>>,
      indexed_by<"bytaker"_n, const_mem_fun<order, uint64_t, &order::bytaker>>
   > TABLE_ORDER;
};