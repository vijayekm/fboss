/*
 *  Copyright (c) 2004-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "fboss/agent/hw/test/dataplane_tests/HwTestQosUtils.h"

#include "fboss/agent/FbossError.h"

#include "fboss/agent/hw/sai/api/NextHopApi.h"
#include "fboss/agent/hw/sai/api/SaiApiTable.h"
#include "fboss/agent/hw/sai/switch/SaiManagerTable.h"
#include "fboss/agent/hw/sai/switch/SaiNextHopManager.h"
#include "fboss/agent/hw/sai/switch/SaiRouterInterfaceManager.h"
#include "fboss/agent/hw/sai/switch/SaiSwitch.h"

namespace facebook::fboss::utility {

void disableTTLDecrements(
    HwSwitch* hw,
    RouterID routerId,
    InterfaceID intf,
    const folly::IPAddress& nhopIp) {
#if SAI_API_VERSION >= SAI_VERSION(1, 6, 0)
  auto managerTable = static_cast<SaiSwitch*>(hw)->managerTable();
  auto rintfHandle =
      managerTable->routerInterfaceManager().getRouterInterfaceHandle(intf);
  SaiIpNextHopTraits::AdapterHostKey key(
      rintfHandle->routerInterface->adapterKey(), nhopIp);
  auto nhop = managerTable->nextHopManager().getNextHop(key);
  SaiIpNextHopTraits::Attributes::DisableTtlDecrement disableTtl{true};
  SaiApiTable::getInstance()->nextHopApi().setAttribute(
      nhop->getSaiObject()->adapterKey(), disableTtl);
#else
  throw FbossError("Disable TTL decrement for nhops is not supported");
#endif
}
} // namespace facebook::fboss::utility
