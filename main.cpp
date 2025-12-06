#define ANKERL_NANOBENCH_IMPLEMENTATION
#include "nanobench.h"

#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <boost/lockfree/queue.hpp>
#include "lockfree_queue.h"
#include "lockfree_queue_no_block.h"
#include "SystemContext.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <nanobench.h>
#include <utility>
#include <functional>
#include "OrderStruct.h"
#include "RiskSettingStruct.h"
#include "RiskTriggerLogData.h"
#include "SystemContext.h"
#include "RiskTriggerLogThread.h"
#include <random>


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

// 全局benchmark配置参数
constexpr int count = 4096;
constexpr int producer_thread_cnt = 3;
constexpr int consumer_thread_cnt = 1;
constexpr int empty_item_no = count * producer_thread_cnt + 1;

template<typename QueueType>
auto createQueueBenchmark(const std::string& queue_name,
                         QueueType& queue,
                         const CRiskItemInfo& risk_info,
                         const ST_INNER_ORDER& order,
                         const proto::risk::RiskTriggerLog& log_tpl,
                         ankerl::nanobench::Bench& bench) {
  return [&bench, &queue, &risk_info, &order, &log_tpl, queue_name]() {
    bench.run(queue_name + " lockfree queue bench test", [&] {
      auto push_func = [&] {
        for (int i = 0; i < count; ++i) {
          auto data = MakeLogData(risk_info, &order, 5, log_tpl);
          if constexpr (std::is_same_v<QueueType, boost::lockfree::queue<CRiskTriggerLogData*>>) {
            queue.push(data.release());
          } else {
            queue.push(data.release(), true);
          }
        }
      };

      auto pop_func = [&] {
        int cnt = 1;
        CRiskTriggerLogData *tmp = nullptr;

        // 检查初始状态
        if (cnt == empty_item_no) {
          if (queue.empty()) {
            LOG(INFO) << queue_name << " 队列元素数量正常";
            return;
          } else {
            LOG(INFO) << queue_name << " 队列元素数量异常";
          }
        }

        while (true) {
          bool popped = false;
          if constexpr (std::is_same_v<QueueType, boost::lockfree::queue<CRiskTriggerLogData*>>) {
            popped = queue.pop(tmp);
          } else {
            popped = queue.pop(tmp, true);
          }
          
          if (!popped) {
            break;
          }
          
          delete tmp;
          tmp = nullptr;
          cnt++;
          LOG(INFO) << queue_name << "_que cnt: " << cnt;

          if (cnt == empty_item_no) {
            if (queue.empty()) {
              LOG(INFO) << queue_name << " 队列元素数量正常";
            } else {
              LOG(INFO) << queue_name << " 队列元素数量异常";
            }
            return;
          }
        }
      };

      std::vector<std::thread> producer_threads;
      std::vector<std::thread> consumer_threads;

      for (int i = 0; i < producer_thread_cnt; ++i) {
        producer_threads.emplace_back(push_func);
      }

      for (int i = 0; i < consumer_thread_cnt; ++i) {
        consumer_threads.emplace_back(pop_func);
      }

      for (auto &t : producer_threads) { t.join(); }
      for (auto &t : consumer_threads) { t.join();
      }
    });
  };
}

int main(int argc, char* argv[]) {
  SystemContext ctx(&argc, &argv, argv[0], "./logs");
  auto risk_info = MakeRiskItemInfo();
  auto order = MakeOrder();
  auto log_tpl = MakeLogTemplate();
  boost::lockfree::queue<CRiskTriggerLogData *> boost_que(count);
  lockfree_queue<CRiskTriggerLogData *> ks_no_block_que(count);
  lockfree_queue_no_block<CRiskTriggerLogData *> ks_que(count);
  ankerl::nanobench::Bench bench;
  bench.relative(true);
  bench.minEpochIterations(50);
  

  // 创建benchmark可调用对象
  auto boost_benchmark = createQueueBenchmark("boost", boost_que, risk_info, order, log_tpl, bench);
  auto ks_benchmark = createQueueBenchmark("ks", ks_que, risk_info, order, log_tpl, bench);
  auto ks_no_block_benchmark = createQueueBenchmark("ks_no_block", ks_no_block_que, risk_info, order, log_tpl, bench);
  
  // 将可调用对象存储在vector中
  std::vector<std::function<void()>> benchmarks = {boost_benchmark, ks_benchmark, ks_no_block_benchmark};
  
  // 使用均匀分布的随机数引擎进行随机打乱
  std::random_device rd;
  std::mt19937 gen(rd());
  std::shuffle(benchmarks.begin(), benchmarks.end(), gen);

  for (int i = 0; i < 30; ++i) {
    // 执行打乱后的benchmark
    for (auto& benchmark : benchmarks) {
      benchmark();
    }
  }

  return 0;
}