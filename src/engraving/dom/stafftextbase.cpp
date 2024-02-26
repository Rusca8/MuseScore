/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "stafftextbase.h"

#include "types/typesconv.h"

#include "segment.h"
#include "staff.h"
#include "soundflag.h"
#include "score.h"

#include "log.h"

using namespace mu;
using namespace mu::engraving;

namespace mu::engraving {
//---------------------------------------------------------
//   StaffTextBase
//---------------------------------------------------------

StaffTextBase::StaffTextBase(const ElementType& type, Segment* parent, TextStyleType tid, ElementFlags flags)
    : TextBase(type, parent, tid, flags)
{
    setSwingParameters(Constants::DIVISION / 2, 60);
}

void StaffTextBase::scanElements(void* data, void (* func)(void*, EngravingItem*), bool all)
{
    for (EngravingObject* child: scanChildren()) {
        child->scanElements(data, func, all);
    }
    if (all || visible() || score()->isShowInvisible()) {
        func(data, this);
    }
}

EngravingObjectList StaffTextBase::scanChildren() const
{
    EngravingObjectList children;

    if (m_soundFlag) {
        children.push_back(m_soundFlag);
    }

    return children;
}

void StaffTextBase::clear()
{
    for (voice_idx_t voice = 0; voice < VOICES; ++voice) {
        m_channelNames[voice].clear();
    }
    clearAeolusStops();
}

//---------------------------------------------------------
//   clearAeolusStops
//---------------------------------------------------------

void StaffTextBase::clearAeolusStops()
{
    for (int i = 0; i < 4; ++i) {
        m_aeolusStops[i] = 0;
    }
}

//---------------------------------------------------------
//   setAeolusStop
//---------------------------------------------------------

void StaffTextBase::setAeolusStop(int group, int idx, bool val)
{
    if (val) {
        m_aeolusStops[group] |= (1 << idx);
    } else {
        m_aeolusStops[group] &= ~(1 << idx);
    }
}

void StaffTextBase::setAeolusStop(int group, int val)
{
    m_aeolusStops[group] = val;
}

//---------------------------------------------------------
//   getAeolusStop
//---------------------------------------------------------

bool StaffTextBase::getAeolusStop(int group, int idx) const
{
    return m_aeolusStops[group] & (1 << idx);
}

int StaffTextBase::aeolusStop(int group) const
{
    return m_aeolusStops[group];
}

//---------------------------------------------------------
//   segment
//---------------------------------------------------------

Segment* StaffTextBase::segment() const
{
    if (!explicitParent()->isSegment()) {
        LOGD("parent %s", explicitParent()->typeName());
        return 0;
    }
    Segment* s = toSegment(explicitParent());
    return s;
}

bool StaffTextBase::hasSoundFlag() const
{
    return m_soundFlag != nullptr;
}

SoundFlag* StaffTextBase::soundFlag() const
{
    return m_soundFlag;
}

void StaffTextBase::setSoundFlag(SoundFlag* flag)
{
    if (m_soundFlag == flag) {
        return;
    }

    m_soundFlag = flag;

    if (m_soundFlag) {
        m_soundFlag->setParent(this);
        m_soundFlag->setTrack(track());
    }
}

void StaffTextBase::setTrack(track_idx_t idx)
{
    TextBase::setTrack(idx);

    if (m_soundFlag) {
        m_soundFlag->setTrack(idx);
    }
}
}
