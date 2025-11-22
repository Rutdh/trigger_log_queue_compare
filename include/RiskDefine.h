#pragma once

namespace ficc {
namespace risk {
namespace RiskSourceType {
constexpr int kType_Instruction = 1; // 订单类型指令
constexpr int kType_Order = 2;       // 订单类型委托
constexpr int kType_Monitor = 3;     // 订单类型监控

static bool IsOrderType(const int source_type) {
  if (source_type == kType_Order) {
    return true;
  }
  return false;
}

static bool IsInstructionType(const int source_type) {
  if (source_type == kType_Instruction) {
    return true;
  }
  return false;
}
}; // namespace RiskSourceType
}; // namespace risk
} // namespace ficc