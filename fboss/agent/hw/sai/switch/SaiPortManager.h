/*
 *  Copyright (c) 2004-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#pragma once

#include "fboss/agent/hw/gen-cpp2/hardware_stats_types.h"
#include "fboss/agent/hw/sai/api/PortApi.h"
#include "fboss/agent/hw/sai/store/SaiObjectWithCounters.h"
#include "fboss/agent/hw/sai/switch/SaiBridgeManager.h"
#include "fboss/agent/hw/sai/switch/SaiQueueManager.h"
#include "fboss/agent/state/Port.h"
#include "fboss/agent/state/PortQueue.h"
#include "fboss/agent/state/StateDelta.h"
#include "fboss/agent/types.h"

#include "folly/container/F14Map.h"

namespace facebook::fboss {

class ConcurrentIndices;
class SaiManagerTable;
class SaiPlatform;
class HwPortFb303Stats;

using SaiPort = SaiObjectWithCounters<SaiPortTraits>;

struct SaiCpuPortHandle {
  PortSaiId cpuPortId;
  SaiQueueHandles queues;
};

struct SaiPortHandle {
  std::shared_ptr<SaiPort> port;
  std::shared_ptr<SaiBridgePort> bridgePort;
  SaiQueueHandles queues;
};

class SaiPortManager {
 public:
  SaiPortManager(
      SaiManagerTable* managerTable,
      SaiPlatform* platform,
      ConcurrentIndices* concurrentIndices_);
  ~SaiPortManager();
  PortSaiId addPort(const std::shared_ptr<Port>& swPort);
  void removePort(PortID id);
  void changePort(
      const std::shared_ptr<Port>& oldPort,
      const std::shared_ptr<Port>& newPort);

  SaiPortTraits::CreateAttributes attributesFromSwPort(
      const std::shared_ptr<Port>& swPort) const;

  const SaiPortHandle* getPortHandle(PortID swId) const;
  SaiPortHandle* getPortHandle(PortID swId);
  const SaiQueueHandle* getQueueHandle(
      PortID swId,
      const SaiQueueConfig& saiQueueConfig) const;
  SaiQueueHandle* getQueueHandle(
      PortID swId,
      const SaiQueueConfig& saiQueueConfig);
  void processPortDelta(const StateDelta& stateDelta, std::mutex& lock);
  void updateStats();
  std::map<PortID, HwPortStats> getPortStats() const;
  void changeQueue(
      PortID swId,
      const QueueConfig& oldQueueConfig,
      const QueueConfig& newQueueConfig);

  const HwPortFb303Stats* getLastPortStat(PortID port) const;

  cfg::PortSpeed getMaxSpeed(PortID port) const;

 private:
  const std::vector<sai_stat_id_t>& supportedStats() const;
  SaiPortHandle* getPortHandleImpl(PortID swId) const;
  SaiQueueHandle* getQueueHandleImpl(
      PortID swId,
      const SaiQueueConfig& saiQueueConfig) const;
  void loadPortQueues(SaiPortHandle* portHandle);
  SaiManagerTable* managerTable_;
  SaiPlatform* platform_;
  ConcurrentIndices* concurrentIndices_;
  folly::F14FastMap<PortID, std::unique_ptr<SaiPortHandle>> handles_;
  folly::F14FastMap<PortID, std::unique_ptr<HwPortFb303Stats>> portStats_;
};

} // namespace facebook::fboss
