#include <chrono>
#include <iostream>
#include <memory>
#include <nanobench.h>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include <glog/logging.h>
#include "OrderStruct.h"
#include "RiskSettingStruct.h"
#include "RiskTriggerLogData.h"
#include "SystemContext.h"
#include "RiskTriggerLogThread.h"

namespace {

CRiskItemInfo MakeRiskItemInfo() {
  CRiskItemInfo info;
  auto risk_object = std::make_shared<ST_RISK_OBJ_CTRL>();
  risk_object->iObjectId = 1001;
  risk_object->strObjectName = "portfolio_A";
  risk_object->iRiskObjectType = 1;
  risk_object->strRiskObject = "ACCNT_001";
  risk_object->strPartyId = "P001";
  info.m_pRiskObject = risk_object;

  auto risk_manage = std::make_shared<ST_RISK_MANAGE_BASE>();
  risk_manage->object_id = risk_object->iObjectId;
  risk_manage->risk_type = 10;
  risk_manage->risk_id = 20001;
  risk_manage->risk_name = "position_limit";
  risk_manage->risk_summary = "pos limit benchmark";
  risk_manage->control_point = "ORD";
  risk_manage->party_id = risk_object->strPartyId;
  risk_manage->status = 1;
  info.m_pRiskManageBase = risk_manage;

  return info;
}

ST_INNER_ORDER MakeOrder() {
  ST_INNER_ORDER order{};
  order.strOrderID = "ORD123456789";
  order.strPortfolioId = "PF001";
  order.strSubAcntId = "SUB001";
  order.strExtAcntId = "EXT001";
  order.strExtSecAcntId = "EXTSEC001";
  order.strPostID = "POST001";
  order.strPartyID = "PTY001";
  order.strProductID = "PRD001";
  order.strMarketGroup = "MK_GROUP_A";
  order.cSide = 'B';
  order.iBsFlag = 1;
  order.dPrice = 10.5;
  order.dOrderQty = 1000.0;
  order.dOrderAmt = order.dPrice * order.dOrderQty;
  order.cOrderType = '0';
  order.cTimeInForce = '0';
  order.iSourceType = ficc::risk::RiskSourceType::kType_Order;
  order.iPurpose = 0;
  order.iTransactTime = 93000000;
  order.iRiskExecTime = 93000000;
  order.iExecuteReportTime = 0;
  order.iCancelOrderTime = 0;
  order.iEntrustAction = 1;
  order.cPreOrderStatus = '0';
  order.cOrderStatus = '0';
  order.dCxlQty = 0.0;
  order.dCxlAmt = 0.0;
  order.dLastDoneQty = 0.0;
  order.dLastDonePrice = 0.0;
  order.dCumDoneQty = 0.0;
  order.dPreLeavesQty = 0.0;
  order.dLeavesQty = order.dOrderQty;
  order.dAvgDonePrice = 0.0;
  order.dDoneAmt = 0.0;
  order.iVolumeMultiple = 1;
  order.strUserID = "user_001";
  order.strDesignation = "benchmark";

  order.stInstrument.strAppId = "XSHG";
  order.stInstrument.strMarketIndicator = "A";
  order.stInstrument.strGroup = "EQ";
  order.stInstrument.strMarket = "SH";
  order.stInstrument.strSecurityId = "600000";
  order.stInstrument.strSecurityName = "PF_BANK";
  order.stInstrument.strSecurityType = "EQT";
  order.stInstrument.strSecuritySubType = "A";
  order.stInstrument.strSecurityGroup = "BANK";
  order.stInstrument.strCurrencyId = "CNY";
  order.stInstrument.strProduct = "STK";

  return order;
}

proto::risk::RiskTriggerLog MakeLogTemplate() {
  proto::risk::RiskTriggerLog log;
  log.set_sno(1);
  log.set_busi_date(20240101);
  log.set_busi_time(93000000);
  log.set_risk_summary("benchmark");
  log.set_control_point("ORD");
  log.set_calc_pos(1);
  log.set_before_value(0.0);
  log.set_real_value(0.0);
  log.set_set_value("limit=1000000");
  log.set_risk_action(1);
  log.set_remark("benchmark sample");
  log.set_nv_type(0);
  log.set_bs_flag(1);
  log.set_calc_target(0.0);
  return log;
}

std::unique_ptr<CRiskTriggerLogData> MakeLogData(const CRiskItemInfo &risk_info,
                                                 const ST_INNER_ORDER *order,
                                                 size_t log_count,
                                                 const proto::risk::RiskTriggerLog &tpl) {
  auto data = std::make_unique<CRiskTriggerLogData>(risk_info, order);
  data->m_vecTriggerLog.reserve(log_count);
  for (size_t i = 0; i < log_count; ++i) {
    data->m_vecTriggerLog.emplace_back(std::make_unique<proto::risk::RiskTriggerLog>(tpl));
  }
  return data;
}

} // namespace

int main(int argc, char* argv[]) {
  SystemContext ctx(&argc, &argv, argv[0], "./logs");
  FLAGS_minloglevel = google::GLOG_WARNING;
  FLAGS_alsologtostderr = false;

  auto risk_info = MakeRiskItemInfo();
  auto order = MakeOrder();
  auto log_tpl = MakeLogTemplate();

  CRiskTriggerLogThread trigger_log_thread;
  if (!trigger_log_thread.Launch()) {
    std::cerr << "Failed to launch RiskTriggerLogThread" << std::endl;
    return 1;
  }

  ankerl::nanobench::Bench bench;
  bench.title("RiskTriggerLogThread PushToQueue");
  bench.minEpochIterations(1000);

  bench.run("log_count_1", [&]() {
    auto data = MakeLogData(risk_info, &order, 1, log_tpl);
    trigger_log_thread.PushToQueue(std::move(data));
  });

  bench.run("log_count_5", [&]() {
    auto data = MakeLogData(risk_info, &order, 5, log_tpl);
    trigger_log_thread.PushToQueue(std::move(data));
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  trigger_log_thread.Stop();

  return 0;
}
