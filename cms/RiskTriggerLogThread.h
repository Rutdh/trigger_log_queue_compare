#pragma once

#include <boost/lockfree/queue.hpp>
#include <memory>
#include <vector>
#include "threadbase.h"
#include "risk_log.pb.h"
#include "RiskTriggerLogData.h"

class CRiskTriggerLogThread : public CThreadBase {
public:
  CRiskTriggerLogThread();
  ~CRiskTriggerLogThread();

public:
  virtual bool InitInstance();
  virtual void Run();
  virtual void ExitInstance();

  // 获取风控流水数据队列
  void Publish(std::vector<std::unique_ptr<proto::risk::RiskTriggerLog>> vecTriggerLog);
  static void SetRiskTriggerLog(const CRiskTriggerLogData *pTriggerLogData);

  void ExecuteRequest(CRiskTriggerLogData *pData);

  void PushToQueue(std::unique_ptr<CRiskTriggerLogData> p);

private:
  boost::lockfree::queue<CRiskTriggerLogData *> m_qTriggerLogData;
};