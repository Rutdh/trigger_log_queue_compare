#pragma once

#include "component.pb.h"
#include "RiskServerIndex.h"

typedef struct st_instrument {
    std::string strMarket;
    std::string strSecurityId;
    std::string strSecurityName;
    std::string strSecurityType;   // 对应::base::SecType的内容
    std::string strSecuritySubType;
    std::string strSecurityGroup;
    std::string strGroup;
    std::string strAppId;
    int iDayTrading;
    std::string strMarketIndicator;
    std::string strCurrencyId;
    std::string strProduct;
    u64 uSecKey; // 证券主键(市场.证券代码)

    void Clear()
    {
        strGroup.clear();
        strMarket.clear();
        strSecurityId.clear();
        strSecurityName.clear();
        strSecurityType.clear();
        strSecurityGroup.clear();
        strAppId.clear();
        iDayTrading = 0;
        strMarketIndicator.clear();
        strCurrencyId.clear();
        strProduct.clear();
    }

    void SetByPb(const proto::Instrument &pb)
    {
        strGroup = pb.market_group();
        strMarket = pb.market();
        strSecurityId = pb.security_id();
        strSecurityName = pb.symbol();
        strSecurityType = pb.security_type();
        strSecurityGroup = pb.security_group();
        strSecuritySubType = pb.security_sub_type();
        strAppId = pb.appl_id();
        iDayTrading = 0;
        strMarketIndicator = pb.market_indicator();
        strCurrencyId = pb.currency_id();
        strProduct = pb.product();
        uSecKey = SecCodeKey::GetKey(strMarket.c_str(), strSecurityId.c_str());
    }

    std::string ToString() const
    {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(4);
        ss
            << strMarket << ","
            << strSecurityId << ","
            << strSecurityName << ","
            << strSecurityType << ","
            << strSecuritySubType << ","
            << strSecurityGroup << ","
            << strGroup << ","
            << strAppId << ","
            << iDayTrading << ","
            << strMarketIndicator << ","
            << strCurrencyId << ","
            << strProduct;
        return ss.str();
    }

} ST_INSTRUMENT;

// 本结构对标招商 ficc::risk::Prices
typedef struct st_prices {
    double dAsk;
    double dBid;
    double dPrevAsk;
    double dPrevBid;
    int64_t llUpdatedTime; // milliseconds
    // ... (后续字段未在图中显示)
} ST_PRICES;