/****************************************************************************
*                                                                           *
* Project64 - A Nintendo 64 emulator.                                      *
* http://www.pj64-emu.com/                                                  *
* Copyright (C) 2012 Project64. All rights reserved.                        *
*                                                                           *
* License:                                                                  *
* GNU/GPLv2 http://www.gnu.org/licenses/gpl-2.0.html                        *
*                                                                           *
****************************************************************************/

#include "stdafx.h"
#include "Breakpoints.h"

#include <Project64-core/N64System/Mips/RegisterClass.h>
#include <Project64-core/N64System/SystemGlobals.h>
#include <Project64-core/N64System/Mips/OpcodeName.h>
#include <Project64-core/N64System/N64Class.h>

//BOOL CBreakpoints::m_Debugging = FALSE;
//BOOL CBreakpoints::m_Skipping = FALSE;
//
//std::vector<uint32_t> CBreakpoints::m_RBP;
//std::vector<uint32_t> CBreakpoints::m_WBP;
//std::vector<uint32_t> CBreakpoints::m_EBP;
//
//int CBreakpoints::m_nRBP = 0;
//int CBreakpoints::m_nWBP = 0;
//int CBreakpoints::m_nEBP = 0;

CBreakpoints::CBreakpoints()
{
    m_Debugging = FALSE;
    m_Skipping = FALSE;
}

void CBreakpoints::Pause()
{
    KeepDebugging();
    g_System->Pause();
}

void CBreakpoints::Resume()
{
    g_System->ExternalEvent(SysEvent_ResumeCPU_FromMenu);
}

bool CBreakpoints::isDebugging()
{
    return m_Debugging;
}

void CBreakpoints::KeepDebugging()
{
    m_Debugging = true;
}

void CBreakpoints::StopDebugging()
{
    m_Debugging = false;
}

bool CBreakpoints::isSkipping()
{
    bool ret = m_Skipping;
    m_Skipping = FALSE;
    return ret;
}
void CBreakpoints::Skip()
{
    m_Skipping = true;
}

bool CBreakpoints::RBPAdd(uint32_t address, bool bTemporary)
{
    if (!ReadBPExists(address))
    {
        m_ReadMem.insert(breakpoints_t::value_type(address, bTemporary));
        return true;
    }
    return false;
}

bool CBreakpoints::WBPAdd(uint32_t address, bool bTemporary)
{
    if (!WriteBPExists(address))
    {
        m_WriteMem.insert(breakpoints_t::value_type(address, bTemporary));
        return true;
    }
    return false;
}

bool CBreakpoints::EBPAdd(uint32_t address, bool bTemporary)
{
    if (!ExecutionBPExists(address))
    {
        m_Execution.insert(breakpoints_t::value_type(address, bTemporary));
        return true;
    }
    return false;
}

void CBreakpoints::RBPRemove(uint32_t address)
{
    breakpoints_t::iterator itr = m_ReadMem.find(address);
    if (itr != m_ReadMem.end())
    {
        m_ReadMem.erase(itr);
    }
}

void CBreakpoints::WBPRemove(uint32_t address)
{
    breakpoints_t::iterator itr = m_WriteMem.find(address);
    if (itr != m_WriteMem.end())
    {
        m_WriteMem.erase(itr);
    }
}

void CBreakpoints::EBPRemove(uint32_t address)
{
    breakpoints_t::iterator itr = m_Execution.find(address);
    if (itr != m_Execution.end())
    {
        m_Execution.erase(itr);
    }
}

void CBreakpoints::RBPToggle(uint32_t address, bool bTemporary)
{
    if (RBPAdd(address, bTemporary) == false)
    {
        RBPRemove(address);
    }
}

void CBreakpoints::WBPToggle(uint32_t address, bool bTemporary)
{
    if (WBPAdd(address, bTemporary) == false)
    {
        WBPRemove(address);
    }
}

void CBreakpoints::EBPToggle(uint32_t address, bool bTemporary)
{
    if (EBPAdd(address, bTemporary) == false)
    {
        EBPRemove(address);
    }
}

void CBreakpoints::RBPClear()
{
    m_ReadMem.clear();
}

void CBreakpoints::WBPClear()
{
    m_WriteMem.clear();
}

void CBreakpoints::EBPClear()
{
    m_Execution.clear();
}

void CBreakpoints::BPClear()
{
    RBPClear();
    WBPClear();
    EBPClear();
}

CBreakpoints::BPSTATE CBreakpoints::ReadBPExists(uint32_t address, bool bRemoveTemp)
{
    breakpoints_t::const_iterator itr = m_ReadMem.find(address);
    if (itr != m_ReadMem.end())
    {
        if (itr->second)
        {
            if (bRemoveTemp)
            {
                m_ReadMem.erase(itr);
            }
            return BP_SET_TEMP;
        }
        return BP_SET;
    }
    return BP_NOT_SET;
}

CBreakpoints::BPSTATE CBreakpoints::WriteBPExists(uint32_t address, bool bRemoveTemp)
{
    breakpoints_t::const_iterator itr = m_WriteMem.find(address);
    if (itr != m_WriteMem.end())
    {
        if (itr->second)
        {
            if (bRemoveTemp)
            {
                m_ReadMem.erase(itr);
            }
            return BP_SET_TEMP;
        }
        return BP_SET;
    }
    return BP_NOT_SET;
}

CBreakpoints::BPSTATE CBreakpoints::ExecutionBPExists(uint32_t address, bool bRemoveTemp)
{
    breakpoints_t::const_iterator itr = m_Execution.find(address);
    if (itr != m_Execution.end())
    {
        if (itr->second)
        {
            if (bRemoveTemp)
            {
                m_ReadMem.erase(itr);
            }
            return BP_SET_TEMP;
        }
        return BP_SET;
    }
    return BP_NOT_SET;
}