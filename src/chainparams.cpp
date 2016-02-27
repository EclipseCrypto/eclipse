// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "assert.h"

#include "chainparams.h"
#include "main.h"
#include "util.h"

#include <boost/assign/list_of.hpp>

using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

#define FIND_GENESIS 0

int64_t CChainParams::GetProofOfWorkReward(int nHeight, int64_t nFees) const
{
    // miner's coin base reward
    int64_t nSubsidy = 0;

    if (nHeight <= 0)
        nSubsidy = 0;
    else
    if (nHeight <= nLastPOWBlock) {
        int halvings = (2 * nHeight) / 675;  // 675/2 blocks/day
        nSubsidy = 4027 * COIN;
        nSubsidy >>= halvings;
    }

    if (fDebug && GetBoolArg("-printcreation"))
        LogPrintf("GetProofOfWorkReward() : create=%s nSubsidy=%d\n", FormatMoney(nSubsidy).c_str(), nSubsidy);

    return nSubsidy + nFees;
};


int64_t CChainParams::GetProofOfStakeReward(int nHeight) const
{
    int64_t nSubsidy = 10 * COIN;
    // 28 day HiPoS, starts at 300 EC, decreases by 10 EC every day, last day is 20 EC
    if (nHeight < 9450) {
        int nDays = 2 * nHeight / 675;
        nSubsidy = (30 - nDays) * nSubsidy;
    }

    if (fDebug && GetBoolArg("-printcreation"))
        LogPrintf("GetProofOfStakeReward(): create=%s\n", FormatMoney(nSubsidy).c_str());

    return nSubsidy;
}

//
// Main network
//

// Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress> &vSeedsOut, const SeedSpec6 *data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7*24*60*60;
    for (unsigned int i = 0; i < count; i++)
    {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

// Convert the pnSeeds array into usable address objects.
static void convertSeeds(std::vector<CAddress> &vSeedsOut, const unsigned int *data, unsigned int count, int port)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7*24*60*60;
    for (unsigned int k = 0; k < count; ++k)
    {
        struct in_addr ip;
        unsigned int i = data[k], t;

        // -- convert to big endian
        t =   (i & 0x000000ff) << 24u
            | (i & 0x0000ff00) << 8u
            | (i & 0x00ff0000) >> 8u
            | (i & 0xff000000) >> 24u;

        memcpy(&ip, &t, sizeof(ip));

        CAddress addr(CService(ip, port));
        addr.nTime = GetTime()-GetRand(nOneWeek)-nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

class CBaseChainParams : public CChainParams {
public:
    CBaseChainParams() {
        const char* pszTimestamp = "At Supreme Court, Unity in Grief Gives Way to Conflict";
        CTransaction txNew;
        txNew.nTime = GENESIS_BLOCK_TIME;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 0 << CBigNum(42) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].SetEmpty();
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime    = GENESIS_BLOCK_TIME;

        nLastPOWBlock = 4725;
    }
    virtual const CBlock& GenesisBlock() const { return genesis; }
    virtual const std::vector<CAddress>& FixedSeeds() const {
        return vFixedSeeds;
    }
protected:
    CBlock genesis;
    std::vector<CAddress> vFixedSeeds;
};

class CMainParams : public CBaseChainParams {
public:
    CMainParams() {
        strNetworkID = "main";

        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 0xe9;
        pchMessageStart[1] = 0xf7;
        pchMessageStart[2] = 0xdd;
        pchMessageStart[3] = 0xfb;

        vAlertPubKey = ParseHex("0414b9da469dda94915d1417aa05040f7aca71d7713fe39c6ac6bd23367f4187f35519ad4300a4863d3d162ecc59602bcef7d76212082af9654ae16abb01ba22e5");

        nDefaultPort = 35914;
        nRPCPort = 45914;
        nBIP44ID = 0x80000001;

        nFirstPosv2Block = 1;

        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 22);
        bnProofOfStakeLimit = CBigNum(~uint256(0) >> 20);
        bnProofOfStakeLimitV2 = CBigNum(~uint256(0) >> 20);

        genesis.nBits    = bnProofOfWorkLimit.GetCompact();
        genesis.nNonce   = 392503;
        hashGenesisBlock = uint256("0x000001e79ad85e44c7b1b72f416a70556834499141252f2cb13e9ff5d213d18d");
        uint256 hashGenesisMerkleRoot("0xddfa27510d5a293c083bbf95e82ba458179b858399465d0ab5197a5da95cc1f9");

// asdf fix me
// set conditional to true to find the genesis nNonce
#if FIND_GENESIS
        uint256 hashTarget;
        if (genesis.GetHash() != hashGenesisBlock) {
             printf("Searching for a genesis block........\n");
             hashTarget = CBigNum().SetCompact(genesis.nBits).getuint256();
             while (genesis.GetHash() > hashTarget) {
                 ++genesis.nNonce;
                 if (genesis.nNonce == 0)
                 {
                     printf("NONCE WRAPPED, incrementing time");
                     ++genesis.nTime;
                 }
             }
             printf("hashTarget == %s\n--\n", hashTarget.ToString().c_str());
        }

        printf("genesis.GetHash() == %s\n", genesis.GetHash().ToString().c_str());
        printf("genesis.hashMerkleRoot == %s\n", genesis.hashMerkleRoot.ToString().c_str());
        printf("genesis.nTime = %u \n", genesis.nTime);
        printf("genesis.nNonce = %u \n", genesis.nNonce);
#endif

        assert(genesis.GetHash() == hashGenesisBlock);
        assert(genesis.hashMerkleRoot == hashGenesisMerkleRoot);

        base58Prefixes[PUBKEY_ADDRESS]      = list_of(33).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[SCRIPT_ADDRESS]      = list_of(125).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[SECRET_KEY]          = list_of(191).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[STEALTH_ADDRESS]     = list_of(40).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_PUBLIC_KEY]      = list_of(0xEE)(0x80)(0x28)(0x6A).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY]      = list_of(0xEE)(0x80)(0x31)(0xE8).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_KEY_HASH]        = list_of(137).convert_to_container<std::vector<unsigned char> >();         // x
        base58Prefixes[EXT_ACC_HASH]        = list_of(83).convert_to_container<std::vector<unsigned char> >();          // a
        base58Prefixes[EXT_PUBLIC_KEY_BTC]  = list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >(); // xprv
        base58Prefixes[EXT_SECRET_KEY_BTC]  = list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >(); // xpub

        vSeeds.push_back(CDNSSeedData("main.eclipse.crypto",  "seed.eclipse.crypto"));

        //convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));
        convertSeeds(vFixedSeeds, pnSeed, ARRAYLEN(pnSeed), nDefaultPort);
    }

    virtual Network NetworkID() const { return CChainParams::MAIN; }
};
static CMainParams mainParams;

//
// Testnet
//

class CTestNetParams : public CBaseChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        strDataDir = "testnet";

        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 0xe4;
        pchMessageStart[1] = 0xf8;
        pchMessageStart[2] = 0xf6;
        pchMessageStart[3] = 0xde;

        vAlertPubKey = ParseHex("0414b9da469dda94915d1417aa05040f7aca71d7713fe39c6ac6bd23367f4187f35519ad4300a4863d3d162ecc59602bcef7d76212082af9654ae16abb01ba22e5");

        nDefaultPort = 35915;
        nRPCPort = 45915;
        nBIP44ID = 0x80000001;

        nFirstPosv2Block = 1;

        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 16);
        bnProofOfStakeLimit = CBigNum(~uint256(0) >> 20);
        bnProofOfStakeLimitV2 = CBigNum(~uint256(0) >> 16);

        genesis.nBits  = bnProofOfWorkLimit.GetCompact();
        genesis.nNonce = 32093;
        hashGenesisBlock = genesis.GetHash();


        hashGenesisBlock = uint256("0x00009254e9a563e136ff335277505959f5dd986ae130a371146cf25ee5b7bad3");
        uint256 hashGenesisMerkleRoot("0xddfa27510d5a293c083bbf95e82ba458179b858399465d0ab5197a5da95cc1f9");

// asdf fix me
// set conditional to true to find the genesis nNonce
#if FIND_GENESIS
        uint256 hashTarget;
        if (genesis.GetHash() != hashGenesisBlock) {
             printf("Searching for a genesis block........\n");
             hashTarget = CBigNum().SetCompact(genesis.nBits).getuint256();
             while (genesis.GetHash() > hashTarget) {
                 ++genesis.nNonce;
                 if (genesis.nNonce == 0)
                 {
                     printf("NONCE WRAPPED, incrementing time");
                     ++genesis.nTime;
                 }
             }
             printf("hashTarget == %s\n--\n", hashTarget.ToString().c_str());
        }

        printf("genesis.GetHash() == %s\n", genesis.GetHash().ToString().c_str());
        printf("genesis.hashMerkleRoot == %s\n", genesis.hashMerkleRoot.ToString().c_str());
        printf("genesis.nTime = %u \n", genesis.nTime);
        printf("genesis.nNonce = %u \n", genesis.nNonce);
#endif

        assert(genesis.GetHash() == hashGenesisBlock);
        assert(genesis.hashMerkleRoot == hashGenesisMerkleRoot);

        base58Prefixes[PUBKEY_ADDRESS]      = list_of(93).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[SCRIPT_ADDRESS]      = list_of(196).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[SECRET_KEY]          = list_of(255).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[STEALTH_ADDRESS]     = list_of(40).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_PUBLIC_KEY]      = list_of(0x76)(0xC0)(0xFD)(0xFB).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY]      = list_of(0x76)(0xC1)(0x07)(0x7A).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_KEY_HASH]        = list_of(75).convert_to_container<std::vector<unsigned char> >();          // X
        base58Prefixes[EXT_ACC_HASH]        = list_of(23).convert_to_container<std::vector<unsigned char> >();          // A
        base58Prefixes[EXT_PUBLIC_KEY_BTC]  = list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >(); // tprv
        base58Prefixes[EXT_SECRET_KEY_BTC]  = list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >(); // tpub

        //convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));
        convertSeeds(vFixedSeeds, pnTestnetSeed, ARRAYLEN(pnTestnetSeed), nDefaultPort);
    }
    virtual Network NetworkID() const { return CChainParams::TESTNET; }
};
static CTestNetParams testNetParams;


//
// Regression test
//
class CRegTestParams : public CTestNetParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        strDataDir = "regtest";

        pchMessageStart[0] = 0xcf;
        pchMessageStart[1] = 0xa3;
        pchMessageStart[2] = 0xbd;
        pchMessageStart[3] = 0xa9;
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 1);
        genesis.nTime = 1411111111;
        genesis.nBits  = bnProofOfWorkLimit.GetCompact();
        genesis.nNonce = 0;
        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 54612;


        hashGenesisBlock = uint256("0x7fbffdda4d07b4dbb3956c11a264661a885da7820a3fd9823f9a14eb8b06c17b");
        uint256 hashGenesisMerkleRoot("0xddfa27510d5a293c083bbf95e82ba458179b858399465d0ab5197a5da95cc1f9");

// asdf fix me
// set conditional to true to find the genesis nNonce
#if FIND_GENESIS
        uint256 hashTarget;
        if (genesis.GetHash() != hashGenesisBlock) {
             printf("Searching for a genesis block........\n");
             hashTarget = CBigNum().SetCompact(genesis.nBits).getuint256();
             while (genesis.GetHash() > hashTarget) {
                 ++genesis.nNonce;
                 if (genesis.nNonce == 0)
                 {
                     printf("NONCE WRAPPED, incrementing time");
                     ++genesis.nTime;
                 }
             }
             printf("hashTarget == %s\n--\n", hashTarget.ToString().c_str());
        }

        printf("genesis.GetHash() == %s\n", genesis.GetHash().ToString().c_str());
        printf("genesis.hashMerkleRoot == %s\n", genesis.hashMerkleRoot.ToString().c_str());
        printf("genesis.nTime = %u \n", genesis.nTime);
        printf("genesis.nNonce = %u \n", genesis.nNonce);
#endif

        assert(genesis.GetHash() == hashGenesisBlock);
        assert(genesis.hashMerkleRoot == hashGenesisMerkleRoot);


        vSeeds.clear();  // Regtest mode doesn't have any DNS seeds.
    }

    virtual bool RequireRPCPassword() const { return false; }
    virtual Network NetworkID() const { return CChainParams::REGTEST; }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = &mainParams;

const CChainParams &Params() {
    return *pCurrentParams;
}

const CChainParams &TestNetParams() {
    return testNetParams;
}

const CChainParams &MainNetParams() {
    return mainParams;
}

void SelectParams(CChainParams::Network network)
{
    switch (network)
    {
        case CChainParams::MAIN:
            pCurrentParams = &mainParams;
            break;
        case CChainParams::TESTNET:
            pCurrentParams = &testNetParams;
            break;
        case CChainParams::REGTEST:
            pCurrentParams = &regTestParams;
            break;
        default:
            assert(false && "Unimplemented network");
            return;
    };
};

bool SelectParamsFromCommandLine()
{
    bool fRegTest = GetBoolArg("-regtest", false);
    bool fTestNet = GetBoolArg("-testnet", false);

    if (fTestNet && fRegTest)
    {
        return false;
    };

    if (fRegTest)
    {
        SelectParams(CChainParams::REGTEST);
    } else
    if (fTestNet)
    {
        SelectParams(CChainParams::TESTNET);
    } else
    {
        SelectParams(CChainParams::MAIN);
    };

    return true;
}
