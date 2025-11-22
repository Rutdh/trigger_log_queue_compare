#pragma once

#include "SecurityInfo.h"
#include "RiskServerUtil.h"
#include "trade_event.pb.h"
#include "RiskDefine.h"

using risktime_t = int;

typedef struct st_inner_order {
    ST_INSTRUMENT stInstrument;
    std::string strOrderID;           // 系统内部委托号
    std::string strPortfolioId;       // 投资组合ID
    std::string strSubAcntId;         // 二级账户ID
    std::string strExtAcntId;         // 一级账户ID
    std::string strExtSecAcntId;      // 一级证券账号ID
    char cSide;                       // 买卖方向
    int iBsFlag;                      // 内部业务类型, 用买卖方向以及其他多个字段获取出来的
    double dPrice;                    // 价格
    double dOrderQty;                 // 订单数量
    char cOrderType;                  // 订单类型
    char cTimeInForce;                // 订单有效时期类型
    int iPurpose;                     // 投保标志
    risktime_t iTransactTime;         // 订单申报时间(交易所时间HHMMSSsss)
    risktime_t iRiskExecTime;         // 风控执行时间(交易所时间HHMMSSsss)
    risktime_t iExecuteReportTime;    // 执行报告时间(交易所时间HHMMSSsss)
    risktime_t iCancelOrderTime;      // 撤单时间(交易所时间HHMMSSsss)

    std::string strOrderDate;         // 交易日
    int iEntrustAction;               // 委托动作类型
    int iPositionEffect;              // 开平标志
    char cPreOrderStatus;             // 前委托状态
    char cOrderStatus;                // 委托状态
    double dOrderAmt;                 // 委托金额
    double dCxlQty;                   // 撤单数量
    double dCxlAmt;                   // 撤单金额
    double dLastDoneQty;              // 最新成交数量
    double dLastDonePrice;            // 最新成交价格
    double dCumDoneQty;               // 累计成交数量
    double dPreLeavesQty;             // 前剩余未完成数量
    double dLeavesQty;                // 剩余未完成数量
    double dAvgDonePrice;             // 成交均价
    double dDoneAmt;                  // 成交金额
    int iVolumeMultiple;              // 手数乘数
    std::string strUserID;            // 用户ID
    int iSourceType;                  // 订单源类型 1-指令 2-委托 3-监控(暂无)
    std::string strDesignation;       // 标签, 指定
    std::string strMarketGroup;       // 市场组

    // 指令用的字段?
    std::string strPostID;
    std::string strPartyID;
    std::string strProductID;

    void UpdateOrder(struct st_inner_order &other)
    {
        iExecuteReportTime = other.iExecuteReportTime;
        iCancelOrderTime = other.iCancelOrderTime;
        iEntrustAction = other.iEntrustAction;
        cPreOrderStatus = other.cPreOrderStatus;
        dCxlQty = other.dCxlQty;
        dCxlAmt = other.dCxlAmt;
        dLastDoneQty = other.dLastDoneQty;
        dLastDonePrice = other.dLastDonePrice;
        dCumDoneQty = other.dCumDoneQty;
        dPreLeavesQty = other.dPreLeavesQty;
        dLeavesQty = other.dLeavesQty;
        dAvgDonePrice = other.dAvgDonePrice;
        dDoneAmt = other.dDoneAmt;
    }

    void SetOrderByProto(const ::proto::OrderRecord &OrderRecord)
    {
        stInstrument.SetByPb(OrderRecord.instrument());
        strOrderID = OrderRecord.order_id();
        strPortfolioId = OrderRecord.portfolio_id();
        strSubAcntId = OrderRecord.sub_acnt_id();
        strExtAcntId = OrderRecord.ext_acnt_id();
        strExtSecAcntId = OrderRecord.ext_sec_acnt_id();

        cSide = OrderRecord.side();

        dPrice = OrderRecord.price();
        dOrderQty = OrderRecord.order_qty();
        cOrderType = OrderRecord.ord_type();
        cTimeInForce = OrderRecord.tif();
        iPurpose = OrderRecord.purpose();

        try
        {
            iTransactTime = utility::ConvertTimeToHHMMSSss(OrderRecord.ord_time());
            iRiskExecTime = utility::GetNowTimeHHMMSSss();
            if (OrderRecord.has_execution())
            {
                iExecuteReportTime = utility::ConvertTimeToHHMMSSss(OrderRecord.execution().exec_time());
            }
            iCancelOrderTime = utility::ConvertTimeToHHMMSSss(OrderRecord.cxl_time());
        }
        catch (const std::exception &e)
        {
            iTransactTime = 0;
            iRiskExecTime = 0;
            iExecuteReportTime = 0;
            iCancelOrderTime = 0;
        }

        iEntrustAction = OrderRecord.entrust_action();
        cPreOrderStatus = OrderRecord.pre_ord_status();
        cOrderStatus = OrderRecord.ord_status();
        dOrderAmt = OrderRecord.order_amount();
        dCxlQty = OrderRecord.cxl_qty();

        dCxlAmt = OrderRecord.cxl_amount();
        strOrderDate = OrderRecord.ord_date();

        if (OrderRecord.has_execution())
        {
            dLastDoneQty = OrderRecord.execution().last_qty();
            dLastDonePrice = OrderRecord.execution().last_px();
        }
        else
        {
            dLastDoneQty = 0;
            dLastDonePrice = 0;
        }

        dCumDoneQty = OrderRecord.cum_qty();
        dLeavesQty = OrderRecord.leaves_qty();
        dAvgDonePrice = OrderRecord.avg_px();
        dDoneAmt = OrderRecord.trade_amount();
        iVolumeMultiple = OrderRecord.volume_multiple();
        strUserID = OrderRecord.user_id();
        iSourceType = ficc::risk::RiskSourceType::kType_Order; // 是委托
        strDesignation = OrderRecord.designation();
        // HACK 这个文件太长了, 先随便填一个数字模拟吧
        // iBsFlag = ficc::risk::BsFlag::Convert(OrderRecord.instrument(), OrderRecord.side(), OrderRecord.position_effect());
        iBsFlag = 1;

        strPostID.clear();
        strPartyID.clear();
    }

/*     bool SetOrderByInstrRiskCheck(const ::proto::risk::InstructionRiskCheckReq &req, const proto::risk::RiskCheckElement &ele)
    {
        stInstrument.SetByPb(ele.instrument());
        strUserID = req.op_user().user_id();
        strPostID = req.op_user().post_id();
        strPartyID = req.risk_check().party_id();
        strProductID = req.risk_check().product_id();

        strPortfolioId = req.risk_check().portfolio_id();
        strExtAcntId = ele.ext_acnt_id();
        strSubAcntId = ele.sub_acnt_id();
        strExtSecAcntId = ele.ext_sec_acnt_id();

        cSide = (char)ele.side();
        dPrice = ele.order_price();
        dOrderQty = ele.order_qty();
        cOrderType = (char)ele.ord_type();
        dOrderAmt = ele.order_amt();
        strOrderDate = ele.trd_date();

        iBsFlag = ficc::risk::BsFlag::Convert(ele.instrument(), ele.side(), ele.position_effect());
        iEntrustAction = ficc::risk::EntrustAction::EN_ACTION_NEW_ORDER;
        iSourceType = ficc::risk::RiskSourceType::kType_Instruction;

        // 订单号填写 腿号 根据柯文涛确认格式固定, 指令号在腿号的前18位。
        strOrderID = ele.element_id();
        iPurpose = ele.sh_flag();
        if (iBsFlag <= 0)
        {
            return false;
        }

        return true;
    }

    bool SetOrderByPreCheckReqAndEle(const ::proto::risk::RiskPreCheckReq &req, const proto::risk::RiskCheckElement &ele)
    {
        stInstrument.SetByPb(ele.instrument());
        strUserID = req.op_user().user_id();
        strPostID = req.op_user().post_id();
        strPartyID = req.risk_check().party_id();
        strProductID = req.risk_check().product_id();
        strPortfolioId = req.risk_check().portfolio_id();
        strExtAcntId = ele.ext_acnt_id();
        strSubAcntId = ele.sub_acnt_id();
        strExtSecAcntId = ele.ext_sec_acnt_id();

        cSide = (char)ele.side();
        dPrice = ele.order_price();
        dOrderQty = ele.order_qty();
        dLeavesQty = ele.leaves_qty();
        cOrderType = (char)ele.ord_type();
        dOrderAmt = ele.order_amt();
        strOrderDate = ele.trd_date();

        iBsFlag = ficc::risk::BsFlag::Convert(ele.instrument(), ele.side(), ele.position_effect());
        iSourceType = ficc::risk::RiskSourceType::kType_Order;

        if (iBsFlag <= 0)
        {
            return false;
        }

        return true;
    } */

} ST_INNER_ORDER;