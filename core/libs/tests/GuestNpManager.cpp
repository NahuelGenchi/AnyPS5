#include "prx/libc/include/general/VabiMacros.hpp"
#include "SceTypes.hpp"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
int APS5_VABI sceNpGetNpId(int user_id, NpId* np_id);
int APS5_VABI sceNpCheckNpAvailability(int req_id, const NpOnlineId* online_id);
int APS5_VABI sceNpGetUserIdByAccountId(std::uint64_t account_id, int* user_id);
int APS5_VABI sceNpSetContentRestriction(const NpContentRestriction* restriction);
void APS5_VABI sceNpRegisterGamePresenceCallback(void* callback, void* userdata);
}

namespace {

constexpr int InvalidArgument = static_cast<int>(0x80550003u);
constexpr int SignedOut = static_cast<int>(0x80550006u);
constexpr int UserNotFound = static_cast<int>(0x80550007u);
constexpr int InvalidSize = static_cast<int>(0x80550011u);

void Require(bool condition, const char* message) {
    if (!condition) {
        std::fprintf(stderr, "NpManager: %s\n", message);
        std::abort();
    }
}

}

int main() {
    NpId npId{};
    std::memset(&npId, 0x5a, sizeof(npId));
    NpId untouched{};
    std::memset(&untouched, 0x5a, sizeof(untouched));
    Require(sceNpGetNpId(0x10000, &npId) == SignedOut, "sceNpGetNpId must report the user as signed out");
    Require(std::memcmp(&npId, &untouched, sizeof(npId)) == 0, "sceNpGetNpId must leave the NpId untouched");
    Require(sceNpGetNpId(0x10000, nullptr) == InvalidArgument, "sceNpGetNpId must reject a null NpId");

    NpOnlineId onlineId{};
    std::strcpy(onlineId.data, "SomePlayer");
    Require(sceNpCheckNpAvailability(1, &onlineId) == UserNotFound, "sceNpCheckNpAvailability must not find a user for an online id");
    Require(sceNpCheckNpAvailability(1, nullptr) == InvalidArgument, "sceNpCheckNpAvailability must reject a null online id");

    int userId = 0x5a5a;
    Require(sceNpGetUserIdByAccountId(0x1122334455667788ull, &userId) == SignedOut, "sceNpGetUserIdByAccountId must report signed out");
    Require(userId == 0x5a5a, "sceNpGetUserIdByAccountId must leave the user id untouched");
    Require(sceNpGetUserIdByAccountId(0, &userId) == InvalidArgument, "sceNpGetUserIdByAccountId must reject account 0");
    Require(sceNpGetUserIdByAccountId(1, nullptr) == InvalidArgument, "sceNpGetUserIdByAccountId must reject a null user id");

    NpAgeRestriction ages[2]{};
    NpContentRestriction restriction{};
    restriction.size = sizeof(restriction);
    restriction.default_age_restriction = 12;
    Require(sceNpSetContentRestriction(&restriction) == 0, "sceNpSetContentRestriction must accept a default age only");
    restriction.age_restriction_count = 2;
    restriction.age_restriction = ages;
    Require(sceNpSetContentRestriction(&restriction) == 0, "sceNpSetContentRestriction must accept per country ages");
    restriction.age_restriction = nullptr;
    Require(sceNpSetContentRestriction(&restriction) == InvalidArgument, "sceNpSetContentRestriction must reject a missing age list");
    restriction.age_restriction_count = 0x101;
    restriction.age_restriction = ages;
    Require(sceNpSetContentRestriction(&restriction) == InvalidArgument, "sceNpSetContentRestriction must reject more than 256 ages");
    restriction.age_restriction_count = -1;
    Require(sceNpSetContentRestriction(&restriction) == InvalidArgument, "sceNpSetContentRestriction must reject a negative count");
    restriction.age_restriction_count = 0;
    restriction.default_age_restriction = -1;
    Require(sceNpSetContentRestriction(&restriction) == InvalidArgument, "sceNpSetContentRestriction must reject a negative default age");
    restriction.default_age_restriction = 0;
    restriction.size = sizeof(restriction) - 1;
    Require(sceNpSetContentRestriction(&restriction) == InvalidSize, "sceNpSetContentRestriction must reject a wrong size");
    Require(sceNpSetContentRestriction(nullptr) == InvalidArgument, "sceNpSetContentRestriction must reject a null structure");

    sceNpRegisterGamePresenceCallback(reinterpret_cast<void*>(1), nullptr);
    sceNpRegisterGamePresenceCallback(nullptr, nullptr);
    return 0;
}
