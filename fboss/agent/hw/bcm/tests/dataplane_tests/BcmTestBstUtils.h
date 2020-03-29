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

#include <vector>

#include "fboss/agent/types.h"

namespace facebook::fboss {

class HwSwitch;
namespace utility {

std::vector<uint64_t>
getQueueWaterMarks(const HwSwitch* hw, PortID port, int highestQueueId);

} // namespace utility
} // namespace facebook::fboss
