#include <iostream>
#include <boost/lockfree/queue.hpp>
#include <memory>
#include <utility>
#include "OrderStruct.h"
#include "RiskSettingStruct.h"
#include "RiskTriggerLogData.h"
#include "SystemContext.h"
#include "RiskTriggerLogThread.h"


int main(int argc, char* argv[]) {
  SystemContext ctx(&argc, &argv, argv[0], "./logs");

  CRiskTriggerLogThread trigger_log_thread;
  
  CRiskItemInfo stRiskItemInfo;
  ST_INNER_ORDER _{};
  ST_INNER_ORDER *pCurOrder = &_;
  auto pTriggerLogData = std::make_unique<CRiskTriggerLogData>(stRiskItemInfo, pCurOrder);
  trigger_log_thread.PushToQueue(std::move(pTriggerLogData));

  return 0;
}