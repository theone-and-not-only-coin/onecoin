// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 500;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        ( 0,   uint256("0x00000068ea1bda5d79b59679d475024f8bc3344a5571a2c00bed231c7495fae6") ) // genesis
        ( 5000,   uint256("0x000000000017501bf964656a97c4ba2657dd05b7bb52c4c17442966b11796989") )
        ( 10000,   uint256("0x0908e603384f625dc816a16dcc98a3c8a3183c759ec5e3d1bbef642642386a33") )
        ( 50000,   uint256("0x00000000001f1b1d5d043d65a52e53c460634275e33ef5bf308f8a362936c03e") )
        ( 100000,   uint256("0x000000000014e8ccf418d1293e8d2dda42abcbd5a2d335d2e035b2059c5b2d78") )
        ( 120000,   uint256("0x000000000003b1f965cefcbb3fb5eef2f19de55718e90fe35ddd752a142d6ce3") )
        ( 159264,   uint256("0x0000000002183e2d0574e1e1a7671794afe53ffa77ef302b0b5dd6412fc24575") )
        ( 163248,   uint256("0x0000000001c1ecaca0bfb1b38c63a0a9e978cdd5320bbfa93ea31a113954395d") )
        ( 168649,   uint256("0x0000000000050fc032c29d4e576df35ce3397ddde853103c8a4cea85226a2c36") )
        ( 180213,   uint256("0xc5afbc3c3365c551cf096a42109bdf933e138207f97db821e89eac0d9bef9412") )
        ( 195623,   uint256("0x91dd778923fce01170c838307a8ddeba345f2911f1443d8cf7e2c47d8d4debd1") )
        ( 226949,   uint256("0xd6b2010846578012561e9e6b55a6326f872d515471d0e0ba6957ee6f39e809fd") )
        ( 240442,   uint256("0x00000000000efd582bec2cfca4fe4604be9c16c31cc227d77594de5676b6efae") )
        ( 258737,   uint256("0x000000000023d644235e1a37be8f880a6ad72ac20dbda732386b6491a3de4fd5") )
        ( 299868,   uint256("0xc8c7a1208131b33716d1876fff0f920b08a04cbbfed86d444c258ef61668aa6b") )
    ;

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();

        if (nHeight <= pindexSync->nHeight)
            return false;
        return true;
    }
}
