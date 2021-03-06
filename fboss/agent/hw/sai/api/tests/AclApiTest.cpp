/*
 *  Copyright (c) 2004-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#include "fboss/agent/hw/sai/api/AclApi.h"
#include "fboss/agent/hw/sai/api/SaiObjectApi.h"
#include "fboss/agent/hw/sai/fake/FakeSai.h"

#include <folly/logging/xlog.h>

#include <gtest/gtest.h>

#include <vector>

using namespace facebook::fboss;

class AclApiTest : public ::testing::Test {
 public:
  void SetUp() override {
    fs = FakeSai::getInstance();
    sai_api_initialize(0, nullptr);
    aclApi = std::make_unique<AclApi>();
  }

  sai_object_id_t kSwitchID() const {
    return 0;
  }

  sai_uint32_t kPriority() const {
    return 1;
  }

  sai_uint32_t kPriority2() const {
    return 2;
  }

  std::pair<folly::IPAddressV6, folly::IPAddressV6> kSrcIpV6() const {
    return std::make_pair(
        folly::IPAddressV6("2620:0:1cfe:face:b00c::3"),
        folly::IPAddressV6("2620:0:1cfe:face:b00c::3"));
  }

  std::pair<folly::IPAddressV6, folly::IPAddressV6> kSrcIpV6_2() const {
    return std::make_pair(
        folly::IPAddressV6("2620:0:1cfe:face:c00c::3"),
        folly::IPAddressV6("2620:0:1cfe:face:c00c::3"));
  }

  std::pair<folly::IPAddressV6, folly::IPAddressV6> kDstIpV6() const {
    return std::make_pair(
        folly::IPAddressV6("2620:0:1cfe:face:b00c::4"),
        folly::IPAddressV6("2620:0:1cfe:face:b00c::4"));
  }

  std::pair<folly::IPAddressV6, folly::IPAddressV6> kDstIpV6_2() const {
    return std::make_pair(
        folly::IPAddressV6("2620:0:1cfe:face:c00c::4"),
        folly::IPAddressV6("2620:0:1cfe:face:c00c::4"));
  }

  std::pair<sai_uint16_t, sai_uint16_t> kL4SrcPort() const {
    return std::make_pair(9001, 0xFFFF);
  }

  std::pair<sai_uint16_t, sai_uint16_t> kL4SrcPort2() const {
    return std::make_pair(10001, 0xFFFF);
  }

  std::pair<sai_uint16_t, sai_uint16_t> kL4DstPort() const {
    return std::make_pair(9002, 0xFFFF);
  }

  std::pair<sai_uint16_t, sai_uint16_t> kL4DstPort2() const {
    return std::make_pair(10002, 0xFFFF);
  }

  std::pair<sai_uint8_t, sai_uint8_t> kIpProtocol() const {
    return std::make_pair(6, 0xFF);
  }

  std::pair<sai_uint8_t, sai_uint8_t> kIpProtocol2() const {
    return std::make_pair(17, 0xFF);
  }

  std::pair<sai_uint8_t, sai_uint8_t> kTcpFlags() const {
    return std::make_pair(1, 0xFF);
  }

  std::pair<sai_uint8_t, sai_uint8_t> kTcpFlags2() const {
    return std::make_pair(2, 0xFF);
  }

  std::pair<sai_uint8_t, sai_uint8_t> kDscp() const {
    // TOS is 8-bits: 6-bit DSCP followed by 2-bit ECN.
    // mask of 0xFC to match on 6-bit DSCP
    return std::make_pair(10, 0xFC);
  }

  std::pair<sai_uint8_t, sai_uint8_t> kDscp2() const {
    // TOS is 8-bits: 6-bit DSCP followed by 2-bit ECN.
    // mask of 0xFC to match on 6-bit DSCP
    return std::make_pair(20, 0xFC);
  }

  std::pair<sai_uint8_t, sai_uint8_t> kTtl() const {
    return std::make_pair(128, 128);
  }

  std::pair<sai_uint8_t, sai_uint8_t> kTtl2() const {
    return std::make_pair(64, 64);
  }

  std::pair<sai_uint32_t, sai_uint32_t> kFdbDstUserMeta() const {
    return std::make_pair(11, 0xFFFFFFFF);
  }

  std::pair<sai_uint32_t, sai_uint32_t> kFdbDstUserMeta2() const {
    return std::make_pair(12, 0xFFFFFFFF);
  }
  std::pair<sai_uint32_t, sai_uint32_t> kRouteDstUserMeta() const {
    return std::make_pair(11, 0xFFFFFFFF);
  }

  std::pair<sai_uint32_t, sai_uint32_t> kRouteDstUserMeta2() const {
    return std::make_pair(12, 0xFFFFFFFF);
  }

  std::pair<sai_uint32_t, sai_uint32_t> kNeighborDstUserMeta() const {
    return std::make_pair(11, 0xFFFFFFFF);
  }
  std::pair<sai_uint32_t, sai_uint32_t> kNeighborDstUserMeta2() const {
    return std::make_pair(12, 0xFFFFFFFF);
  }

  sai_uint8_t kSetTC() const {
    return 1;
  }

  sai_uint8_t kSetTC2() const {
    return 2;
  }

  const std::vector<sai_int32_t>& kActionTypeList() const {
    static const std::vector<sai_int32_t> actionTypeList = {
        SAI_ACL_ACTION_TYPE_PACKET_ACTION,
        SAI_ACL_ACTION_TYPE_MIRROR_INGRESS,
        SAI_ACL_ACTION_TYPE_MIRROR_EGRESS,
        SAI_ACL_ACTION_TYPE_SET_TC,
        SAI_ACL_ACTION_TYPE_SET_DSCP};

    return actionTypeList;
  }

  AclTableSaiId createAclTable() const {
    SaiAclTableTraits::Attributes::Stage aclTableStageAttribute{
        SAI_ACL_STAGE_INGRESS};
    std::vector<sai_int32_t> aclTableBindPointTypeListAttribute{
        SAI_ACL_BIND_POINT_TYPE_PORT};

    return aclApi->create<SaiAclTableTraits>(
        {
            aclTableStageAttribute,
            aclTableBindPointTypeListAttribute,
            kActionTypeList(),
            true, // srcIpv6
            true, // dstIpv6
            true, // l4SrcPort
            true, // l4DstPort
            true, // ipProtocol
            true, // tcpFlags
            true, // inPort
            true, // outPort
            true, // ipFrag
            true, // dscp
            true, // dstMac
            true, // ipType
            true, // ttl
            true, // fdb meta
            true, // route meta
            true // neighbor meta
        },
        kSwitchID());
  }

  AclEntrySaiId createAclEntry(AclTableSaiId aclTableId) const {
    SaiAclEntryTraits::Attributes::TableId aclTableIdAttribute{aclTableId};
    SaiAclEntryTraits::Attributes::Priority aclPriorityAttribute{kPriority()};
    SaiAclEntryTraits::Attributes::FieldSrcIpV6 aclFieldSrcIpV6{
        AclEntryFieldIpV6(kSrcIpV6())};
    SaiAclEntryTraits::Attributes::FieldDstIpV6 aclFieldDstIpV6{
        AclEntryFieldIpV6(kDstIpV6())};
    SaiAclEntryTraits::Attributes::FieldL4SrcPort aclFieldL4SrcPortAttribute{
        AclEntryFieldU16(kL4SrcPort())};
    SaiAclEntryTraits::Attributes::FieldL4DstPort aclFieldL4DstPortAttribute{
        AclEntryFieldU16(kL4DstPort())};
    SaiAclEntryTraits::Attributes::FieldIpProtocol aclFieldIpProtocolAttribute{
        AclEntryFieldU8(kIpProtocol())};
    SaiAclEntryTraits::Attributes::FieldTcpFlags aclFieldTcpFlagsAttribute{
        AclEntryFieldU8(kTcpFlags())};
    SaiAclEntryTraits::Attributes::FieldDscp aclFieldDscpAttribute{
        AclEntryFieldU8(kDscp())};
    SaiAclEntryTraits::Attributes::FieldTtl aclFieldTtlAttribute{
        AclEntryFieldU8(kTtl())};
    SaiAclEntryTraits::Attributes::FieldFdbDstUserMeta
        aclFieldFdbDstUserMetaAttribute{AclEntryFieldU32(kFdbDstUserMeta())};
    SaiAclEntryTraits::Attributes::FieldRouteDstUserMeta
        aclFieldRouteDstUserMetaAttribute{
            AclEntryFieldU32(kRouteDstUserMeta())};
    SaiAclEntryTraits::Attributes::FieldNeighborDstUserMeta
        aclFieldNeighborDstUserMetaAttribute{
            AclEntryFieldU32(kNeighborDstUserMeta())};
    SaiAclEntryTraits::Attributes::ActionSetTC aclActionSetTC{
        AclEntryActionU8(kSetTC())};

    return aclApi->create<SaiAclEntryTraits>(
        {aclTableIdAttribute,
         aclPriorityAttribute,
         aclFieldSrcIpV6,
         aclFieldDstIpV6,
         aclFieldL4SrcPortAttribute,
         aclFieldL4DstPortAttribute,
         aclFieldIpProtocolAttribute,
         aclFieldTcpFlagsAttribute,
         aclFieldDscpAttribute,
         aclFieldTtlAttribute,
         aclFieldFdbDstUserMetaAttribute,
         aclFieldRouteDstUserMetaAttribute,
         aclFieldNeighborDstUserMetaAttribute,
         aclActionSetTC},
        kSwitchID());
  }

  void checkAclTable(AclTableSaiId aclTableId) const {
    EXPECT_EQ(aclTableId, fs->aclTableManager.get(aclTableId).id);
  }

  void checkAclEntry(AclTableSaiId aclTableId, AclEntrySaiId aclEntryId) const {
    EXPECT_EQ(aclEntryId, fs->aclTableManager.getMember(aclEntryId).id);
    EXPECT_EQ(aclTableId, fs->aclTableManager.getMember(aclEntryId).tableId);
  }

  AclTableGroupSaiId createAclTableGroup(
      const std::vector<sai_int32_t>& aclTableGroupBindPointTypeListAttribute =
          std::vector<sai_int32_t>{SAI_ACL_BIND_POINT_TYPE_PORT}) const {
    SaiAclTableGroupTraits::Attributes::Stage aclTableGroupStageAttribute{
        SAI_ACL_STAGE_INGRESS};
    SaiAclTableGroupTraits::Attributes::Type aclTableGroupTypeAttribute{
        SAI_ACL_TABLE_GROUP_TYPE_SEQUENTIAL};

    return aclApi->create<SaiAclTableGroupTraits>(
        {aclTableGroupStageAttribute,
         aclTableGroupBindPointTypeListAttribute,
         aclTableGroupTypeAttribute},
        kSwitchID());
  }

  AclTableGroupMemberSaiId createAclTableGroupMember(
      AclTableGroupSaiId aclTableGroupId,
      AclTableSaiId aclTableId) const {
    SaiAclTableGroupMemberTraits::Attributes::TableGroupId
        aclTableGroupTableGroupIdAttribute{aclTableGroupId};
    SaiAclTableGroupMemberTraits::Attributes::TableId
        aclTableGroupTableIdAttribute{aclTableId};
    SaiAclTableGroupMemberTraits::Attributes::Priority
        aclTableGroupPriorityAttribute{kPriority()};

    return aclApi->create<SaiAclTableGroupMemberTraits>(
        {aclTableGroupTableGroupIdAttribute,
         aclTableGroupTableIdAttribute,
         aclTableGroupPriorityAttribute},
        kSwitchID());
  }

  void checkAclTableGroup(AclTableGroupSaiId aclTableGroupId) const {
    EXPECT_EQ(
        aclTableGroupId, fs->aclTableGroupManager.get(aclTableGroupId).id);
  }

  void checkAclTableGroupMember(
      AclTableGroupSaiId aclTableGroupId,
      AclTableGroupMemberSaiId aclTableGroupMemberId) {
    EXPECT_EQ(
        aclTableGroupMemberId,
        fs->aclTableGroupManager.getMember(aclTableGroupMemberId).id);
    EXPECT_EQ(
        aclTableGroupId,
        fs->aclTableGroupManager.getMember(aclTableGroupMemberId).tableGroupId);
  }

  void getAndVerifyAclEntryAttribute(
      AclEntrySaiId aclEntryId,
      sai_uint32_t priority,
      const std::pair<folly::IPAddressV6, folly::IPAddressV6>& srcIpV6,
      const std::pair<folly::IPAddressV6, folly::IPAddressV6>& dstIpV6,
      const std::pair<sai_uint16_t, sai_uint16_t>& l4SrcPort,
      const std::pair<sai_uint16_t, sai_uint16_t>& L4DstPort,
      const std::pair<sai_uint8_t, sai_uint8_t>& ipProtocol,
      const std::pair<sai_uint8_t, sai_uint8_t>& tcpFlags,
      const std::pair<sai_uint8_t, sai_uint8_t>& dscp,
      const std::pair<sai_uint8_t, sai_uint8_t>& ttl,
      const std::pair<sai_uint32_t, sai_uint32_t>& fdbDstUserMeta,
      const std::pair<sai_uint32_t, sai_uint32_t>& routeDstUserMeta,
      const std::pair<sai_uint32_t, sai_uint32_t>& neighborDstUserMeta,
      sai_uint8_t setTC) const {
    auto aclPriorityGot = aclApi->getAttribute(
        aclEntryId, SaiAclEntryTraits::Attributes::Priority());

    auto aclFieldSrcIpV6Got = aclApi->getAttribute(
        aclEntryId, SaiAclEntryTraits::Attributes::FieldSrcIpV6());
    auto aclFieldDstIpV6Got = aclApi->getAttribute(
        aclEntryId, SaiAclEntryTraits::Attributes::FieldDstIpV6());
    auto aclFieldL4SrcPortGot = aclApi->getAttribute(
        aclEntryId, SaiAclEntryTraits::Attributes::FieldL4SrcPort());
    auto aclFieldL4DstPortGot = aclApi->getAttribute(
        aclEntryId, SaiAclEntryTraits::Attributes::FieldL4DstPort());
    auto aclFieldIpProtocolGot = aclApi->getAttribute(
        aclEntryId, SaiAclEntryTraits::Attributes::FieldIpProtocol());
    auto aclFieldTcpFlagsGot = aclApi->getAttribute(
        aclEntryId, SaiAclEntryTraits::Attributes::FieldTcpFlags());
    auto aclFieldDscpGot = aclApi->getAttribute(
        aclEntryId, SaiAclEntryTraits::Attributes::FieldDscp());
    auto aclFieldTtlGot = aclApi->getAttribute(
        aclEntryId, SaiAclEntryTraits::Attributes::FieldTtl());
    auto aclFieldFdbDstUserMetaGot = aclApi->getAttribute(
        aclEntryId, SaiAclEntryTraits::Attributes::FieldFdbDstUserMeta());
    auto aclFieldRouteDstUserMetaGot = aclApi->getAttribute(
        aclEntryId, SaiAclEntryTraits::Attributes::FieldRouteDstUserMeta());
    auto aclFieldNeighborDstUserMetaGot = aclApi->getAttribute(
        aclEntryId, SaiAclEntryTraits::Attributes::FieldNeighborDstUserMeta());

    auto aclActionSetTCGot = aclApi->getAttribute(
        aclEntryId, SaiAclEntryTraits::Attributes::ActionSetTC());

    EXPECT_EQ(aclPriorityGot, priority);

    EXPECT_EQ(aclFieldSrcIpV6Got.getDataAndMask(), srcIpV6);
    EXPECT_EQ(aclFieldDstIpV6Got.getDataAndMask(), dstIpV6);
    EXPECT_EQ(aclFieldL4SrcPortGot.getDataAndMask(), l4SrcPort);
    EXPECT_EQ(aclFieldL4DstPortGot.getDataAndMask(), L4DstPort);
    EXPECT_EQ(aclFieldIpProtocolGot.getDataAndMask(), ipProtocol);
    EXPECT_EQ(aclFieldTcpFlagsGot.getDataAndMask(), tcpFlags);
    EXPECT_EQ(aclFieldDscpGot.getDataAndMask(), dscp);
    EXPECT_EQ(aclFieldTtlGot.getDataAndMask(), ttl);
    EXPECT_EQ(aclFieldFdbDstUserMetaGot.getDataAndMask(), fdbDstUserMeta);
    EXPECT_EQ(aclFieldRouteDstUserMetaGot.getDataAndMask(), routeDstUserMeta);
    EXPECT_EQ(
        aclFieldNeighborDstUserMetaGot.getDataAndMask(), neighborDstUserMeta);

    EXPECT_EQ(aclActionSetTCGot.getData(), setTC);
  }

  std::shared_ptr<FakeSai> fs;
  std::unique_ptr<AclApi> aclApi;
};

TEST_F(AclApiTest, createAclTable) {
  auto aclTableId = createAclTable();
  checkAclTable(aclTableId);
}

TEST_F(AclApiTest, removeAclTable) {
  auto aclTableId = createAclTable();
  checkAclTable(aclTableId);
  aclApi->remove(aclTableId);
}

TEST_F(AclApiTest, createAclEntry) {
  auto aclTableId = createAclTable();
  checkAclTable(aclTableId);

  auto aclEntryId = createAclEntry(aclTableId);
  checkAclEntry(aclTableId, aclEntryId);
}

TEST_F(AclApiTest, mulipleAclTables) {
  auto aclTableId1 = createAclTable();
  auto aclTableId2 = createAclTable();
  checkAclTable(aclTableId1);
  checkAclTable(aclTableId2);
  EXPECT_NE(aclTableId1, aclTableId2);

  auto aclEntryId1 = createAclEntry(aclTableId1);
  auto aclEntryId2 = createAclEntry(aclTableId2);
  checkAclEntry(aclTableId1, aclEntryId1);
  checkAclEntry(aclTableId2, aclEntryId2);
}

TEST_F(AclApiTest, removeAclEntry) {
  auto aclTableId = createAclTable();
  checkAclTable(aclTableId);

  auto aclEntryId = createAclEntry(aclTableId);
  checkAclEntry(aclTableId, aclEntryId);

  aclApi->remove(aclEntryId);
}

TEST_F(AclApiTest, multipleAclEntries) {
  auto aclTableId = createAclTable();
  checkAclTable(aclTableId);

  auto aclEntryId1 = createAclEntry(aclTableId);
  auto aclEntryId2 = createAclEntry(aclTableId);

  checkAclEntry(aclTableId, aclEntryId1);
  checkAclEntry(aclTableId, aclEntryId2);
  EXPECT_NE(aclEntryId1, aclEntryId2);
}

TEST_F(AclApiTest, getAclTableAttribute) {
  auto aclTableId = createAclTable();
  checkAclTable(aclTableId);

  auto aclEntryId = createAclEntry(aclTableId);
  checkAclEntry(aclTableId, aclEntryId);

  auto aclTableStageGot =
      aclApi->getAttribute(aclTableId, SaiAclTableTraits::Attributes::Stage());
  auto aclTableBindPointTypeListGot = aclApi->getAttribute(
      aclTableId, SaiAclTableTraits::Attributes::BindPointTypeList());
  auto aclTableActionTypeListGot = aclApi->getAttribute(
      aclTableId, SaiAclTableTraits::Attributes::ActionTypeList());
  auto aclTableEntryListGot = aclApi->getAttribute(
      aclTableId, SaiAclTableTraits::Attributes::EntryList());

  auto aclTableFieldSrcIpV6Got = aclApi->getAttribute(
      aclTableId, SaiAclTableTraits::Attributes::FieldSrcIpV6());
  auto aclTableFieldDstIpV6Got = aclApi->getAttribute(
      aclTableId, SaiAclTableTraits::Attributes::FieldDstIpV6());
  auto aclTableFieldL4SrcPortGot = aclApi->getAttribute(
      aclTableId, SaiAclTableTraits::Attributes::FieldL4SrcPort());
  auto aclTableFieldL4DstPortGot = aclApi->getAttribute(
      aclTableId, SaiAclTableTraits::Attributes::FieldL4DstPort());
  auto aclTableFieldIpProtocolGot = aclApi->getAttribute(
      aclTableId, SaiAclTableTraits::Attributes::FieldIpProtocol());
  auto aclTableFieldTcpFlagsGot = aclApi->getAttribute(
      aclTableId, SaiAclTableTraits::Attributes::FieldTcpFlags());
  auto aclTableFieldInPortGot = aclApi->getAttribute(
      aclTableId, SaiAclTableTraits::Attributes::FieldInPort());
  auto aclTableFieldOutPortGot = aclApi->getAttribute(
      aclTableId, SaiAclTableTraits::Attributes::FieldOutPort());
  auto aclTableFieldIpFragGot = aclApi->getAttribute(
      aclTableId, SaiAclTableTraits::Attributes::FieldIpFrag());
  auto aclTableFieldDscpGot = aclApi->getAttribute(
      aclTableId, SaiAclTableTraits::Attributes::FieldDscp());
  auto aclTableFieldDstMacGot = aclApi->getAttribute(
      aclTableId, SaiAclTableTraits::Attributes::FieldDstMac());
  auto aclTableFieldIpTypeGot = aclApi->getAttribute(
      aclTableId, SaiAclTableTraits::Attributes::FieldIpType());
  auto aclTableFieldTtlGot = aclApi->getAttribute(
      aclTableId, SaiAclTableTraits::Attributes::FieldTtl());
  auto aclTableFieldFdbDstUserMetaGot = aclApi->getAttribute(
      aclTableId, SaiAclTableTraits::Attributes::FieldFdbDstUserMeta());
  auto aclTableFieldRouteDstUserMetaGot = aclApi->getAttribute(
      aclTableId, SaiAclTableTraits::Attributes::FieldRouteDstUserMeta());
  auto aclTableFieldNeighborDstUserMetaGot = aclApi->getAttribute(
      aclTableId, SaiAclTableTraits::Attributes::FieldNeighborDstUserMeta());

  EXPECT_EQ(aclTableStageGot, SAI_ACL_STAGE_INGRESS);
  EXPECT_EQ(aclTableBindPointTypeListGot.size(), 1);
  EXPECT_EQ(aclTableBindPointTypeListGot[0], SAI_ACL_BIND_POINT_TYPE_PORT);
  EXPECT_TRUE(aclTableActionTypeListGot == kActionTypeList());

  EXPECT_EQ(aclTableEntryListGot.size(), 1);
  EXPECT_EQ(aclTableEntryListGot[0], static_cast<uint32_t>(aclEntryId));

  EXPECT_EQ(aclTableFieldSrcIpV6Got, true);
  EXPECT_EQ(aclTableFieldDstIpV6Got, true);
  EXPECT_EQ(aclTableFieldL4SrcPortGot, true);
  EXPECT_EQ(aclTableFieldL4DstPortGot, true);
  EXPECT_EQ(aclTableFieldIpProtocolGot, true);
  EXPECT_EQ(aclTableFieldTcpFlagsGot, true);
  EXPECT_EQ(aclTableFieldInPortGot, true);
  EXPECT_EQ(aclTableFieldOutPortGot, true);
  EXPECT_EQ(aclTableFieldIpFragGot, true);
  EXPECT_EQ(aclTableFieldDscpGot, true);
  EXPECT_EQ(aclTableFieldDstMacGot, true);
  EXPECT_EQ(aclTableFieldIpTypeGot, true);
  EXPECT_EQ(aclTableFieldTtlGot, true);
  EXPECT_EQ(aclTableFieldFdbDstUserMetaGot, true);
  EXPECT_EQ(aclTableFieldRouteDstUserMetaGot, true);
  EXPECT_EQ(aclTableFieldNeighborDstUserMetaGot, true);
}

TEST_F(AclApiTest, getAclEntryAttribute) {
  auto aclTableId = createAclTable();
  checkAclTable(aclTableId);

  auto aclEntryId = createAclEntry(aclTableId);
  checkAclEntry(aclTableId, aclEntryId);

  getAndVerifyAclEntryAttribute(
      aclEntryId,
      kPriority(),
      kSrcIpV6(),
      kDstIpV6(),
      kL4SrcPort(),
      kL4DstPort(),
      kIpProtocol(),
      kTcpFlags(),
      kDscp(),
      kTtl(),
      kFdbDstUserMeta(),
      kRouteDstUserMeta(),
      kNeighborDstUserMeta(),
      kSetTC());
}

TEST_F(AclApiTest, setAclTableAttribute) {
  auto aclTableId = createAclTable();
  checkAclTable(aclTableId);

  auto aclEntryId = createAclEntry(aclTableId);
  checkAclEntry(aclTableId, aclEntryId);

  // SAI spec does not support setting any attribute for ACL table post
  // creation.
  SaiAclTableTraits::Attributes::Stage stage{SAI_ACL_STAGE_EGRESS};
  SaiAclTableTraits::Attributes::BindPointTypeList bindPointTypeList{};
  SaiAclTableTraits::Attributes::ActionTypeList actionTypeList{};
  SaiAclTableTraits::Attributes::EntryList entryList{};

  SaiAclTableTraits::Attributes::FieldSrcIpV6 fieldSrcIpv6{false};
  SaiAclTableTraits::Attributes::FieldDstIpV6 fieldDstIpV6{false};
  SaiAclTableTraits::Attributes::FieldL4SrcPort fieldL4SrcPort{false};
  SaiAclTableTraits::Attributes::FieldL4DstPort fieldL4DstPort{false};
  SaiAclTableTraits::Attributes::FieldIpProtocol fieldIpProtocol{false};
  SaiAclTableTraits::Attributes::FieldTcpFlags fieldTcpFlags{false};
  SaiAclTableTraits::Attributes::FieldInPort fieldInPort{false};
  SaiAclTableTraits::Attributes::FieldOutPort fieldOutPort{false};
  SaiAclTableTraits::Attributes::FieldIpFrag fieldIpFrag{false};
  SaiAclTableTraits::Attributes::FieldDscp fieldDscp{false};
  SaiAclTableTraits::Attributes::FieldDstMac fieldDstMac{false};
  SaiAclTableTraits::Attributes::FieldIpType fieldIpType{false};
  SaiAclTableTraits::Attributes::FieldTtl fieldTtl{false};
  SaiAclTableTraits::Attributes::FieldFdbDstUserMeta fieldFdbDstUserMeta{false};
  SaiAclTableTraits::Attributes::FieldRouteDstUserMeta fieldRouteDstUserMeta{
      false};
  SaiAclTableTraits::Attributes::FieldNeighborDstUserMeta
      fieldNeieghborDstUserMeta{false};

  EXPECT_THROW(aclApi->setAttribute(aclTableId, stage), SaiApiError);
  EXPECT_THROW(
      aclApi->setAttribute(aclTableId, bindPointTypeList), SaiApiError);
  EXPECT_THROW(aclApi->setAttribute(aclTableId, actionTypeList), SaiApiError);
  EXPECT_THROW(aclApi->setAttribute(aclTableId, entryList), SaiApiError);

  EXPECT_THROW(aclApi->setAttribute(aclTableId, fieldSrcIpv6), SaiApiError);
  EXPECT_THROW(aclApi->setAttribute(aclTableId, fieldDstIpV6), SaiApiError);
  EXPECT_THROW(aclApi->setAttribute(aclTableId, fieldL4SrcPort), SaiApiError);
  EXPECT_THROW(aclApi->setAttribute(aclTableId, fieldL4DstPort), SaiApiError);
  EXPECT_THROW(aclApi->setAttribute(aclTableId, fieldIpProtocol), SaiApiError);
  EXPECT_THROW(aclApi->setAttribute(aclTableId, fieldTcpFlags), SaiApiError);
  EXPECT_THROW(aclApi->setAttribute(aclTableId, fieldInPort), SaiApiError);
  EXPECT_THROW(aclApi->setAttribute(aclTableId, fieldOutPort), SaiApiError);
  EXPECT_THROW(aclApi->setAttribute(aclTableId, fieldIpFrag), SaiApiError);
  EXPECT_THROW(aclApi->setAttribute(aclTableId, fieldDscp), SaiApiError);
  EXPECT_THROW(aclApi->setAttribute(aclTableId, fieldDstMac), SaiApiError);
  EXPECT_THROW(aclApi->setAttribute(aclTableId, fieldIpType), SaiApiError);
  EXPECT_THROW(aclApi->setAttribute(aclTableId, fieldTtl), SaiApiError);
  EXPECT_THROW(
      aclApi->setAttribute(aclTableId, fieldFdbDstUserMeta), SaiApiError);
  EXPECT_THROW(
      aclApi->setAttribute(aclTableId, fieldRouteDstUserMeta), SaiApiError);
  EXPECT_THROW(
      aclApi->setAttribute(aclTableId, fieldNeieghborDstUserMeta), SaiApiError);
}

TEST_F(AclApiTest, setAclEntryAttribute) {
  auto aclTableId = createAclTable();
  checkAclTable(aclTableId);

  auto aclEntryId = createAclEntry(aclTableId);
  checkAclEntry(aclTableId, aclEntryId);

  // SAI spec does not support setting tableId for ACL entry post
  // creation.
  SaiAclEntryTraits::Attributes::TableId aclTableIdAttribute2{2};
  EXPECT_THROW(
      aclApi->setAttribute(aclEntryId, aclTableIdAttribute2), SaiApiError);

  // SAI spec supports setting priority and fieldDscp for ACL entry post
  // creation.
  SaiAclEntryTraits::Attributes::Priority aclPriorityAttribute2{kPriority2()};

  SaiAclEntryTraits::Attributes::FieldSrcIpV6 aclFieldSrcIpV6Attribute2{
      AclEntryFieldIpV6(kSrcIpV6_2())};
  SaiAclEntryTraits::Attributes::FieldDstIpV6 aclFieldDstIpV6Attribute2{
      AclEntryFieldIpV6(kDstIpV6_2())};
  SaiAclEntryTraits::Attributes::FieldL4SrcPort aclFieldL4SrcPortAttribute2{
      AclEntryFieldU16(kL4SrcPort2())};
  SaiAclEntryTraits::Attributes::FieldL4DstPort aclFieldL4DstPortAttribute2{
      AclEntryFieldU16(kL4DstPort2())};
  SaiAclEntryTraits::Attributes::FieldIpProtocol aclFieldIpProtocolAttribute2{
      AclEntryFieldU8(kIpProtocol2())};
  SaiAclEntryTraits::Attributes::FieldTcpFlags aclFieldTcpFlagsAttribute2{
      AclEntryFieldU8(kTcpFlags2())};
  SaiAclEntryTraits::Attributes::FieldDscp aclFieldDscpAttribute2{
      AclEntryFieldU8(kDscp2())};
  SaiAclEntryTraits::Attributes::FieldTtl aclFieldTtlAttribute2{
      AclEntryFieldU8(kTtl2())};
  SaiAclEntryTraits::Attributes::FieldFdbDstUserMeta
      aclFieldFdbDstUserMetaAttribute2{AclEntryFieldU32(kFdbDstUserMeta2())};
  SaiAclEntryTraits::Attributes::FieldRouteDstUserMeta
      aclFieldRouteDstUserMetaAttribute2{
          AclEntryFieldU32(kRouteDstUserMeta2())};
  SaiAclEntryTraits::Attributes::FieldNeighborDstUserMeta
      aclFieldNeighborDstUserMetaAttribute2{
          AclEntryFieldU32(kNeighborDstUserMeta2())};
  SaiAclEntryTraits::Attributes::ActionSetTC aclActionSetTC2{
      AclEntryActionU8(kSetTC2())};

  aclApi->setAttribute(aclEntryId, aclPriorityAttribute2);

  aclApi->setAttribute(aclEntryId, aclFieldSrcIpV6Attribute2);
  aclApi->setAttribute(aclEntryId, aclFieldDstIpV6Attribute2);
  aclApi->setAttribute(aclEntryId, aclFieldL4SrcPortAttribute2);
  aclApi->setAttribute(aclEntryId, aclFieldL4DstPortAttribute2);
  aclApi->setAttribute(aclEntryId, aclFieldIpProtocolAttribute2);
  aclApi->setAttribute(aclEntryId, aclFieldTcpFlagsAttribute2);
  aclApi->setAttribute(aclEntryId, aclFieldDscpAttribute2);
  aclApi->setAttribute(aclEntryId, aclFieldTtlAttribute2);
  aclApi->setAttribute(aclEntryId, aclFieldFdbDstUserMetaAttribute2);
  aclApi->setAttribute(aclEntryId, aclFieldRouteDstUserMetaAttribute2);
  aclApi->setAttribute(aclEntryId, aclFieldNeighborDstUserMetaAttribute2);
  aclApi->setAttribute(aclEntryId, aclActionSetTC2);

  getAndVerifyAclEntryAttribute(
      aclEntryId,
      kPriority2(),
      kSrcIpV6_2(),
      kDstIpV6_2(),
      kL4SrcPort2(),
      kL4DstPort2(),
      kIpProtocol2(),
      kTcpFlags2(),
      kDscp2(),
      kTtl2(),
      kFdbDstUserMeta2(),
      kRouteDstUserMeta2(),
      kNeighborDstUserMeta2(),
      kSetTC2());
}

TEST_F(AclApiTest, formatAclTableStageAttribute) {
  SaiAclTableTraits::Attributes::Stage stage{0};
  std::string expected("Stage: 0");
  EXPECT_EQ(expected, fmt::format("{}", stage));
}

TEST_F(AclApiTest, formatAclEntryAttribute) {
  SaiAclEntryTraits::Attributes::TableId tableId{0};
  std::string expected("TableId: 0");
  EXPECT_EQ(expected, fmt::format("{}", tableId));
}

TEST_F(AclApiTest, createAclTableGroup) {
  auto aclTableGroupId = createAclTableGroup();
  checkAclTableGroup(aclTableGroupId);
}

TEST_F(AclApiTest, removeAclTableGroup) {
  auto aclTableGroupId = createAclTableGroup();
  checkAclTableGroup(aclTableGroupId);
  aclApi->remove(aclTableGroupId);
}

TEST_F(AclApiTest, createAclTableGroupMember) {
  auto aclTableGroupId = createAclTableGroup();
  checkAclTableGroup(aclTableGroupId);
  auto aclTableId = createAclTable();
  checkAclTable(aclTableId);

  auto aclTableGroupMemberId =
      createAclTableGroupMember(aclTableGroupId, aclTableId);
  checkAclTableGroupMember(aclTableGroupId, aclTableGroupMemberId);
}

TEST_F(AclApiTest, removeAclTableGroupMember) {
  auto aclTableGroupId = createAclTableGroup();
  checkAclTableGroup(aclTableGroupId);
  auto aclTableId = createAclTable();
  checkAclTable(aclTableId);

  auto aclTableGroupMemberId =
      createAclTableGroupMember(aclTableGroupId, aclTableId);
  checkAclTableGroupMember(aclTableGroupId, aclTableGroupMemberId);

  aclApi->remove(aclTableGroupMemberId);
}

TEST_F(AclApiTest, multipleAclTableGroupMember) {
  auto aclTableGroupId = createAclTableGroup();
  checkAclTableGroup(aclTableGroupId);

  auto aclTableId1 = createAclTable();
  checkAclTable(aclTableId1);
  auto aclTableId2 = createAclTable();
  checkAclTable(aclTableId2);

  auto aclTableGroupMemberId1 =
      createAclTableGroupMember(aclTableGroupId, aclTableId1);
  checkAclTableGroupMember(aclTableGroupId, aclTableGroupMemberId1);
  auto aclTableGroupMemberId2 =
      createAclTableGroupMember(aclTableGroupId, aclTableId1);
  checkAclTableGroupMember(aclTableGroupId, aclTableGroupMemberId2);
}

TEST_F(AclApiTest, getAclTableGroupAttribute) {
  auto aclTableGroupId = createAclTableGroup();
  checkAclTableGroup(aclTableGroupId);
  auto aclTableId = createAclTable();
  checkAclTable(aclTableId);

  auto aclTableGroupMemberId =
      createAclTableGroupMember(aclTableGroupId, aclTableId);
  checkAclTableGroupMember(aclTableGroupId, aclTableGroupMemberId);

  auto aclTableGroupStageGot = aclApi->getAttribute(
      aclTableGroupId, SaiAclTableGroupTraits::Attributes::Stage());
  auto aclTableGroupBindPointTypeListGot = aclApi->getAttribute(
      aclTableGroupId, SaiAclTableGroupTraits::Attributes::BindPointTypeList());
  auto aclTableGroupTypeGot = aclApi->getAttribute(
      aclTableGroupId, SaiAclTableGroupTraits::Attributes::Type());
  auto aclTableGroupMembersGot = aclApi->getAttribute(
      aclTableGroupId, SaiAclTableGroupTraits::Attributes::MemberList());

  EXPECT_EQ(aclTableGroupStageGot, SAI_ACL_STAGE_INGRESS);
  EXPECT_EQ(aclTableGroupBindPointTypeListGot.size(), 1);
  EXPECT_EQ(aclTableGroupBindPointTypeListGot[0], SAI_ACL_BIND_POINT_TYPE_PORT);
  EXPECT_EQ(aclTableGroupTypeGot, SAI_ACL_TABLE_GROUP_TYPE_SEQUENTIAL);
  EXPECT_EQ(aclTableGroupMembersGot.size(), 1);
  EXPECT_EQ(aclTableGroupMembersGot[0], aclTableGroupMemberId);
}

TEST_F(AclApiTest, getAclTableGroupMemberAttribute) {
  auto aclTableGroupId = createAclTableGroup();
  checkAclTableGroup(aclTableGroupId);
  auto aclTableId = createAclTable();
  checkAclTable(aclTableId);

  auto aclTableGroupMemberId =
      createAclTableGroupMember(aclTableGroupId, aclTableId);
  checkAclTableGroupMember(aclTableGroupId, aclTableGroupMemberId);

  auto aclTableGroupIdGot = aclApi->getAttribute(
      aclTableGroupMemberId,
      SaiAclTableGroupMemberTraits::Attributes::TableGroupId());
  auto aclTableIdGot = aclApi->getAttribute(
      aclTableGroupMemberId,
      SaiAclTableGroupMemberTraits::Attributes::TableId());
  auto aclPriorityGot = aclApi->getAttribute(
      aclTableGroupMemberId,
      SaiAclTableGroupMemberTraits::Attributes::Priority());

  EXPECT_EQ(aclTableGroupIdGot, aclTableGroupId);
  EXPECT_EQ(aclTableIdGot, aclTableId);
  EXPECT_EQ(aclPriorityGot, 1);
}

TEST_F(AclApiTest, setAclTableGroupAttribute) {
  auto aclTableGroupId = createAclTableGroup();
  checkAclTableGroup(aclTableGroupId);

  // SAI spec does not support setting any attribute for ACL table group post
  // creation.
  SaiAclTableGroupTraits::Attributes::Stage stage{SAI_ACL_STAGE_EGRESS};
  SaiAclTableGroupTraits::Attributes::BindPointTypeList bindPointTypeList{};
  SaiAclTableGroupTraits::Attributes::Type type{
      SAI_ACL_TABLE_GROUP_TYPE_PARALLEL};
  SaiAclTableGroupTraits::Attributes::MemberList memberList{};

  EXPECT_THROW(aclApi->setAttribute(aclTableGroupId, stage), SaiApiError);
  EXPECT_THROW(
      aclApi->setAttribute(aclTableGroupId, bindPointTypeList), SaiApiError);
  EXPECT_THROW(aclApi->setAttribute(aclTableGroupId, type), SaiApiError);
  EXPECT_THROW(aclApi->setAttribute(aclTableGroupId, memberList), SaiApiError);
}

TEST_F(AclApiTest, setAclTableGroupMemberAttribute) {
  auto aclTableGroupId = createAclTableGroup();
  checkAclTableGroup(aclTableGroupId);
  auto aclTableId = createAclTable();
  checkAclTable(aclTableId);
  auto aclTableGroupMemberId =
      createAclTableGroupMember(aclTableGroupId, aclTableId);
  checkAclTableGroupMember(aclTableGroupId, aclTableGroupMemberId);

  // SAI spec does not support setting any attribute for ACL table group member
  // post creation.
  SaiAclTableGroupMemberTraits::Attributes::TableGroupId tableGroupId{1};
  SaiAclTableGroupMemberTraits::Attributes::TableId tableId{1};
  SaiAclTableGroupMemberTraits::Attributes::Priority priority{kPriority()};

  EXPECT_THROW(
      aclApi->setAttribute(aclTableGroupMemberId, tableGroupId), SaiApiError);
  EXPECT_THROW(
      aclApi->setAttribute(aclTableGroupMemberId, tableId), SaiApiError);
  EXPECT_THROW(
      aclApi->setAttribute(aclTableGroupMemberId, priority), SaiApiError);
}

TEST_F(AclApiTest, formatAclTableGroupStageAttribute) {
  SaiAclTableGroupTraits::Attributes::Stage stage{0};
  std::string expected("Stage: 0");
  EXPECT_EQ(expected, fmt::format("{}", stage));
}

TEST_F(AclApiTest, formatAclTableGroupMemberAttribute) {
  SaiAclTableGroupMemberTraits::Attributes::TableGroupId tableGroupId{0};
  std::string expected("TableGroupId: 0");
  EXPECT_EQ(expected, fmt::format("{}", tableGroupId));
}
