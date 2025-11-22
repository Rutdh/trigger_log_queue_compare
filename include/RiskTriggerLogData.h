#pragma once
#include "RiskSettingStruct.h"
#include "risk_log.pb.h"
#include "OrderStruct.h"

class CRiskTriggerLogData {
public:
  CRiskItemInfo m_stRiskItemInfo;
  // 订单对象
  const ST_INNER_ORDER *m_pOrder = nullptr;

  std::vector<std::unique_ptr<proto::risk::RiskTriggerLog>> m_vecTriggerLog;

public:
  // 移动构造 (转移资源, 无拷贝)
  CRiskTriggerLogData(CRiskTriggerLogData &&other) noexcept
      : m_vecTriggerLog(std::move(other.m_vecTriggerLog)) {}

  // 移动赋值 (同上)
  CRiskTriggerLogData &operator=(CRiskTriggerLogData &&other) noexcept {
    if (this != &other) {
      m_vecTriggerLog = std::move(other.m_vecTriggerLog);
    }
    return *this;
  }

  CRiskTriggerLogData(const CRiskItemInfo &stRiskItemInfo,
                      const ST_INNER_ORDER *pOrder) {
    m_stRiskItemInfo = stRiskItemInfo;
    m_pOrder = pOrder;
  }

  ~CRiskTriggerLogData() { m_pOrder = nullptr; }
};