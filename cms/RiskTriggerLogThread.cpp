#include "RiskTriggerLogThread.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_int32(LogQueueLen, 102400, "触发日志队列长度");


CRiskTriggerLogThread::CRiskTriggerLogThread()
    : m_qTriggerLogData(FLAGS_LogQueueLen) {
  m_Terminated = false;
}

CRiskTriggerLogThread::~CRiskTriggerLogThread(void) {
  Stop();
  // 清空队列资源
  while (!m_qTriggerLogData.empty()) {
    CRiskTriggerLogData *p = nullptr;
    if (m_qTriggerLogData.pop(p)) {
      if (p) {
        delete p;
        p = nullptr;
      }
    }
  }
}

bool CRiskTriggerLogThread::InitInstance() {
  // QUESTION 这事什么实现?
  return true;
}

void CRiskTriggerLogThread::Run()
{
    while (!m_Terminated)
    {
        while (!m_qTriggerLogData.empty())
        {
            // 该无锁队列多入多出, 但是仅支持平凡类型, 故使用裸指针处理
            CRiskTriggerLogData *p = nullptr;
            if (m_qTriggerLogData.pop(p))
            {
                ExecuteRequest(p);
                if (p)
                {
                    delete p;
                    p = nullptr;
                }
            }
        }

        usleep(500);
    }
}

void CRiskTriggerLogThread::ExitInstance()
{
}

void CRiskTriggerLogThread::Publish(std::vector<std::unique_ptr<proto::risk::RiskTriggerLog>> vecTriggerLog)
{
    if (vecTriggerLog.empty())
    {
        return;
    }

#ifdef UNIT_TEST
    for (auto &triggerLog : vecTriggerLog)
    {
        LOG_INFO << "Test triggerLog print: " << ShortUtf8DebugString(*triggerLog.get());
        g_testRiskTriggerLogs.push(std::move(vecTriggerLog));
    }
#else
    for (auto &&triggerLog : vecTriggerLog)
    {
        try
        {
            std::string buffer;
            buffer.clear();
            triggerLog->SerializeToString(&buffer);
            // 发送走nats, 目前nats仅支持同步接口
            // if (!g_SimpleContext.Send(CSimpleContext::trigger_log, "ficcc.risk.log.event.kingstar", buffer, true)) // ficc::risk::RiskEvent::RE_TRIGGER_LOG_EVENT
            // {
            //     LOG_WARNING << "RiskTriggerLog Sending failure!";
            // }
        }
        catch (const std::exception &e)
        {
            LOG(ERROR) << "[RiskTrigger]"
                      << " component=RiskTriggerLogThread"
                      << " operation=Polish"
                      << " status=fail"
                      << " error=" << e.what();
        }
    }
#endif
}

// 暂时先这么写, 后续优化到该线程中进行字段设置, 减少关键路径上赋值的耗时操作
void CRiskTriggerLogThread::SetRiskTriggerLog(const CRiskTriggerLogData *pTriggerLogData)
{
    auto& pRiskObjCtrl = pTriggerLogData->m_stRiskItemInfo.m_pRiskObject;
    auto& pRiskManageBase = pTriggerLogData->m_stRiskItemInfo.m_pRiskManageBase;
    auto pCurOrder = pTriggerLogData->m_pOrder;

    for (auto &triggerLog : pTriggerLogData->m_vecTriggerLog)
    {
        // 试算不写风控日志
        // if (pre_check_type != PreCheckType::PRE_CHECK_TYPE_YES)
        // {
        //     // 有触发风控, 记录风控触发日志
        //     // 场外基金转换业务
        //     if (pCurOrder->cSide == ficc::risk::BsFlag::SIDE_TRANSFER_IN || pCurOrder->cSide == ficc::risk::BsFlag::SIDE_TRANSFER_OUT)
        //     {
        //         triggerLog->set_bs_flag(ficc::risk::BsFlag::BSFLAG_TRANSFER);
        //     }
        //     else
        //     {
        //         triggerLog->set_bs_flag(pCurOrder->iBsFlag);
        //     }
        // }

        triggerLog->set_order_party_id(pCurOrder->strPartyID);
        triggerLog->set_portfolio_id(pCurOrder->strPortfolioId);
        triggerLog->set_post_id(pCurOrder->strPostID);
        triggerLog->set_user_id(pCurOrder->strUserID);
        triggerLog->set_product_id(pCurOrder->strProductID);
        triggerLog->set_object_id(pRiskObjCtrl->iObjectId);
        triggerLog->set_object_name(pRiskObjCtrl->strObjectName);
        triggerLog->set_risk_object_type(pRiskObjCtrl->iRiskObjectType);
        triggerLog->set_risk_object(pRiskObjCtrl->strRiskObject);
        triggerLog->set_party_id(pRiskObjCtrl->strPartyId);
        triggerLog->set_control_point(std::to_string(pCurOrder->iSourceType));
        triggerLog->set_ext_accnt_id(pCurOrder->strExtAcntId);

        triggerLog->mutable_instrument()->set_appl_id(pCurOrder->stInstrument.strAppId);
        triggerLog->mutable_instrument()->set_market_indicator(pCurOrder->stInstrument.strMarketIndicator);
        triggerLog->mutable_instrument()->set_market_group(pCurOrder->stInstrument.strGroup);
        triggerLog->mutable_instrument()->set_market(pCurOrder->stInstrument.strMarket);
        triggerLog->mutable_instrument()->set_security_id(pCurOrder->stInstrument.strSecurityId);
        triggerLog->mutable_instrument()->set_symbol(pCurOrder->stInstrument.strSecurityName);
        triggerLog->mutable_instrument()->set_security_type(pCurOrder->stInstrument.strSecurityType);
        triggerLog->mutable_instrument()->set_security_sub_type(pCurOrder->stInstrument.strSecuritySubType);
        triggerLog->mutable_instrument()->set_security_group(pCurOrder->stInstrument.strSecurityGroup);
        triggerLog->mutable_instrument()->set_currency_id(pCurOrder->stInstrument.strCurrencyId);
        triggerLog->mutable_instrument()->set_product(pCurOrder->stInstrument.strProduct);

        triggerLog->set_order_qty(pCurOrder->dOrderQty);
        triggerLog->set_order_amt(pCurOrder->dOrderAmt);
        triggerLog->set_price(pCurOrder->dPrice);

        if (pCurOrder->iSourceType == ficc::risk::RiskSourceType::kType_Order)
        {
            triggerLog->set_order_sno(pCurOrder->strOrderID);
        }
        else
        {
            // 指令号长度为18
            triggerLog->set_order_sno(pCurOrder->strOrderID.substr(0, 18));
        }

        triggerLog->set_risk_type(pRiskManageBase->risk_type);
        triggerLog->set_risk_id(pRiskManageBase->risk_id);
        triggerLog->set_risk_name(pRiskManageBase->risk_name);
        triggerLog->set_risk_summary(pRiskManageBase->risk_summary);
    }
}

void CRiskTriggerLogThread::PushToQueue(std::unique_ptr<CRiskTriggerLogData> p)
{
    // 该无锁队列多入多出, 但是仅支持平凡类型, 故使用裸指针处理
    while (!m_qTriggerLogData.push(p.get()));
    // 此处智能指针释放所有权
    p.release();
}

void CRiskTriggerLogThread::ExecuteRequest(CRiskTriggerLogData* pData)
{
    // 缺失的情况不能发送Publish
    if (pData && pData->m_stRiskItemInfo.m_pRiskObject != nullptr
        && pData->m_stRiskItemInfo.m_pRiskManageBase != nullptr
        && pData->m_pOrder != nullptr)
    {
        CRiskTriggerLogThread::SetRiskTriggerLog(pData);
        Publish(std::move(pData->m_vecTriggerLog));
    }
}



