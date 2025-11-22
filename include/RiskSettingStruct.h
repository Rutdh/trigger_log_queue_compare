#pragma once

#include <iomanip>
#include <memory>
#include <string>
#include <sstream>
#include "risk_settings.pb.h"

typedef struct st_risk_obj_ctrl
{
    // 对像序号
    int iObjectId;
    // 对像名称
    std::string strObjectName;
    // 风控对像类型
    int iRiskObjectType;
    // 风控对像(和序号谁是唯一键?)
    std::string strRiskObject;
    // 所属机构
    std::string strPartyId;
    // 风控状态
    int iStatus;

    st_risk_obj_ctrl()
    {
        iObjectId = 0;
        strObjectName.clear();
        iRiskObjectType = 0;
        strRiskObject.clear();
        strPartyId.clear();
        iStatus = 0;
    }

    void Clear()
    {
        iObjectId = 0;
        strObjectName.clear();
        iRiskObjectType = 0;
        strRiskObject.clear();
        strPartyId.clear();
        iStatus = 0;
    }

    void SetByPbRiskObjectControl(const proto::risk::RiskObjectControl &pb)
    {
        iObjectId = pb.object_id();
        strObjectName = pb.object_name();
        iRiskObjectType = pb.risk_object_type();
        strRiskObject = pb.risk_object();
        strPartyId = pb.party_id();
        iStatus = pb.status();
    }

    std::string ToString() const
    {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(4);
        ss << iObjectId << ",";
        ss << strObjectName << ",";
        ss << iRiskObjectType << ",";
        ss << strRiskObject << ",";
        ss << strPartyId << ",";
        ss << iStatus;

        return ss.str();
    }
} ST_RISK_OBJ_CTRL;


typedef struct st_risk_manage_base {
    int32_t object_id;
    int32_t risk_type;
    int32_t risk_id;
    std::string risk_name;
    std::string risk_summary;
    int32_t layer_ctrl_dim;
    int32_t risk_mark;
    int32_t risk_level;
    int32_t control_level;
    std::string control_point;
    int32_t status;
    int32_t interval_time;
    int32_t calc_pos;
    int32_t date_switch;
    int32_t begin_date;
    int32_t end_date;
    int32_t time_switch;
    int32_t begin_time;
    int32_t end_time;
    int32_t condition_switch;
    std::string condition_param;
    int32_t include_flag;
    double min_param_value;
    double max_param_value;
    int32_t template_flag;
    std::string template_name;
    std::string party_id;
    std::string template_remark;
    uint64_t time_stamp;
    int32_t user_switch;
    std::string user_list;

    st_risk_manage_base() {
        object_id = 0;
        risk_type = 0;
        risk_id = 0;
        risk_name = "";
        risk_summary = "";
        layer_ctrl_dim = 0;
        risk_mark = 0;
        risk_level = 0;
        control_level = 0;
        control_point = "";
        status = 0;
        interval_time = 0;
        calc_pos = 0;
        date_switch = 0;
        begin_date = 0;
        end_date = 0;
        time_switch = 0;
        begin_time = 0;
        end_time = 0;
        condition_switch = 0;
        condition_param = "";
        include_flag = 0;
        min_param_value = 0.0;
        max_param_value = 0.0;
        template_flag = 0;
        template_name = "";
        party_id = "";
        template_remark = "";
        time_stamp = 0;
        user_switch = 0;
        user_list = "";
    }

    void SetByPbRiskManageBase(const proto::risk::RiskManageBase &pb) {
        object_id = pb.object_id();
        risk_type = pb.risk_type();
        risk_id = pb.risk_id();
        risk_name = pb.risk_name();
        risk_summary = pb.risk_summary();
        layer_ctrl_dim = pb.layer_ctrl_dim();
        risk_mark = pb.risk_mark();
        risk_level = pb.risk_level();
        control_level = pb.control_level();
        control_point = pb.control_point();
        status = pb.status();
        interval_time = pb.interval_time();
        calc_pos = pb.calc_pos();
        date_switch = pb.date_switch();
        begin_date = pb.begin_date();
        end_date = pb.end_date();
        time_switch = pb.time_switch();
        begin_time = pb.begin_time();
        end_time = pb.end_time();
        condition_switch = pb.condition_switch();
        condition_param = pb.condition_param();
        include_flag = pb.include_flag();
        min_param_value = pb.min_param_value();
        max_param_value = pb.max_param_value();
        template_flag = pb.template_flag();
        template_name = pb.template_name();
        party_id = pb.party_id();
        template_remark = pb.template_remark();
        time_stamp = pb.time_stamp();
        user_switch = pb.user_switch();
        user_list = pb.user_list();
    }

    std::string ToString() const {
        char buf[4096] = {0};
        snprintf(buf, sizeof(buf),
            "object_id:%d|risk_type:%d|risk_id:%d|risk_name:%s|risk_summary:%s|"
            "layer_ctrl_dim:%d|risk_mark:%d|risk_level:%d|control_level:%d|"
            "control_point:%s|status:%d|interval_time:%d|calc_pos:%d|"
            "date_switch:%d|begin_date:%d|end_date:%d|time_switch:%d|"
            "begin_time:%d|end_time:%d|condition_switch:%d|condition_param:%s|"
            "include_flag:%d|min_param_value:%f|max_param_value:%f|"
            "template_flag:%d|template_name:%s|party_id:%s|template_remark:%s|"
            "time_stamp:%llu|user_switch:%d|user_list:%s",
            object_id, risk_type, risk_id, risk_name.c_str(), risk_summary.c_str(),
            layer_ctrl_dim, risk_mark, risk_level, control_level, control_point.c_str(),
            status, interval_time, calc_pos, date_switch, begin_date, end_date,
            time_switch, begin_time, end_time, condition_switch, condition_param.c_str(),
            include_flag, min_param_value, max_param_value,
            template_flag, template_name.c_str(), party_id.c_str(), template_remark.c_str(),
            (unsigned long long)time_stamp, user_switch, user_list.c_str());
        return std::string(buf);
    }
} ST_RISK_MANAGE_BASE;

class CRiskItemInfo {
  public:
    std::shared_ptr<ST_RISK_OBJ_CTRL> m_pRiskObject;
    std::shared_ptr<ST_RISK_MANAGE_BASE> m_pRiskManageBase;
};