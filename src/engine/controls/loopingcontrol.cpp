#include "engine/controls/loopingcontrol.h"

#include <QtDebug>

#include "control/controlobject.h"
#include "control/controlpushbutton.h"
#include "engine/controls/bpmcontrol.h"
#include "engine/controls/enginecontrol.h"
#include "engine/enginebuffer.h"
#include "moc_loopingcontrol.cpp"
#include "preferences/usersettings.h"
#include "track/track.h"
#include "util/compatibility.h"
#include "util/math.h"
#include "util/sample.h"

namespace {
constexpr mixxx::audio::FrameDiff_t kMinimumAudibleLoopSizeFrames = 150;
}

double LoopingControl::s_dBeatSizes[] = { 0.03125, 0.0625, 0.125, 0.25, 0.5,
                                          1, 2, 4, 8, 16, 32, 64, 128, 256, 512 };

// Used to generate the beatloop_%SIZE, beatjump_%SIZE, and loop_move_%SIZE CO
// ConfigKeys.
ConfigKey keyForControl(const QString& group, const QString& ctrlName, double num) {
    ConfigKey key;
    key.group = group;
    key.item = ctrlName.arg(num);
    return key;
}

// static
QList<double> LoopingControl::getBeatSizes() {
    QList<double> result;
    for (unsigned int i = 0; i < (sizeof(s_dBeatSizes) / sizeof(s_dBeatSizes[0])); ++i) {
        result.append(s_dBeatSizes[i]);
    }
    return result;
}

LoopingControl::LoopingControl(const QString& group,
        UserSettingsPointer pConfig)
        : EngineControl(group, pConfig),
          m_bLoopingEnabled(false),
          m_bLoopRollActive(false),
          m_bAdjustingLoopIn(false),
          m_bAdjustingLoopOut(false),
          m_bAdjustingLoopInOld(false),
          m_bAdjustingLoopOutOld(false),
          m_bLoopOutPressedWhileLoopDisabled(false) {
    m_oldLoopInfo = {mixxx::audio::kInvalidFramePos,
            mixxx::audio::kInvalidFramePos,
            LoopSeekMode::MovedOut};
    m_loopInfo.setValue(m_oldLoopInfo);
    m_currentPosition.setValue(mixxx::audio::kStartFramePos);
    m_pActiveBeatLoop = nullptr;
    m_pRateControl = nullptr;
    //Create loop-in, loop-out, loop-exit, and reloop/exit ControlObjects
    m_pLoopInButton = new ControlPushButton(ConfigKey(group, "loop_in"));
    connect(m_pLoopInButton, &ControlObject::valueChanged,
            this, &LoopingControl::slotLoopIn,
            Qt::DirectConnection);
    m_pLoopInButton->set(0);

    m_pLoopInGotoButton = new ControlPushButton(ConfigKey(group, "loop_in_goto"));
    connect(m_pLoopInGotoButton, &ControlObject::valueChanged,
            this, &LoopingControl::slotLoopInGoto);

    m_pLoopOutButton = new ControlPushButton(ConfigKey(group, "loop_out"));
    connect(m_pLoopOutButton, &ControlObject::valueChanged,
            this, &LoopingControl::slotLoopOut,
            Qt::DirectConnection);
    m_pLoopOutButton->set(0);

    m_pLoopOutGotoButton = new ControlPushButton(ConfigKey(group, "loop_out_goto"));
    connect(m_pLoopOutGotoButton, &ControlObject::valueChanged,
            this, &LoopingControl::slotLoopOutGoto);


    m_pLoopExitButton = new ControlPushButton(ConfigKey(group, "loop_exit"));
    connect(m_pLoopExitButton, &ControlObject::valueChanged,
            this, &LoopingControl::slotLoopExit,
            Qt::DirectConnection);
    m_pLoopExitButton->set(0);

    m_pReloopToggleButton = new ControlPushButton(ConfigKey(group, "reloop_toggle"));
    connect(m_pReloopToggleButton, &ControlObject::valueChanged,
            this, &LoopingControl::slotReloopToggle,
            Qt::DirectConnection);
    m_pReloopToggleButton->set(0);
    // The old reloop_exit name was confusing. This CO does both entering and exiting.
    ControlDoublePrivate::insertAlias(ConfigKey(group, "reloop_exit"),
                                      ConfigKey(group, "reloop_toggle"));

    m_pReloopAndStopButton = new ControlPushButton(ConfigKey(group, "reloop_andstop"));
    connect(m_pReloopAndStopButton, &ControlObject::valueChanged,
            this, &LoopingControl::slotReloopAndStop,
            Qt::DirectConnection);

    m_pCOLoopEnabled = new ControlObject(ConfigKey(group, "loop_enabled"));
    m_pCOLoopEnabled->set(0.0);
    m_pCOLoopEnabled->connectValueChangeRequest(this,
            &LoopingControl::slotLoopEnabledValueChangeRequest,
            Qt::DirectConnection);

    m_pCOLoopStartPosition =
            new ControlObject(ConfigKey(group, "loop_start_position"));
    m_pCOLoopStartPosition->set(kNoTrigger);
    connect(m_pCOLoopStartPosition, &ControlObject::valueChanged,
            this, &LoopingControl::slotLoopStartPos,
            Qt::DirectConnection);

    m_pCOLoopEndPosition =
            new ControlObject(ConfigKey(group, "loop_end_position"));
    m_pCOLoopEndPosition->set(kNoTrigger);
    connect(m_pCOLoopEndPosition, &ControlObject::valueChanged,
            this, &LoopingControl::slotLoopEndPos,
            Qt::DirectConnection);

    m_pQuantizeEnabled = ControlObject::getControl(ConfigKey(group, "quantize"));
    m_pNextBeat = ControlObject::getControl(ConfigKey(group, "beat_next"));
    m_pPreviousBeat = ControlObject::getControl(ConfigKey(group, "beat_prev"));
    m_pClosestBeat = ControlObject::getControl(ConfigKey(group, "beat_closest"));
    m_pTrackSamples = ControlObject::getControl(ConfigKey(group, "track_samples"));
    m_pSlipEnabled = ControlObject::getControl(ConfigKey(group, "slip_enabled"));

    // DEPRECATED: Use beatloop_size and beatloop_set instead.
    // Activates a beatloop of a specified number of beats.
    m_pCOBeatLoop = new ControlObject(ConfigKey(group, "beatloop"), false);
    connect(m_pCOBeatLoop, &ControlObject::valueChanged, this,
            [=](double value){slotBeatLoop(value);}, Qt::DirectConnection);

    m_pCOBeatLoopSize = new ControlObject(ConfigKey(group, "beatloop_size"),
                                          true, false, false, 4.0);
    m_pCOBeatLoopSize->connectValueChangeRequest(this,
            &LoopingControl::slotBeatLoopSizeChangeRequest, Qt::DirectConnection);
    m_pCOBeatLoopActivate = new ControlPushButton(ConfigKey(group, "beatloop_activate"));
    connect(m_pCOBeatLoopActivate, &ControlObject::valueChanged,
            this, &LoopingControl::slotBeatLoopToggle);
    m_pCOBeatLoopRollActivate = new ControlPushButton(ConfigKey(group, "beatlooproll_activate"));
    connect(m_pCOBeatLoopRollActivate, &ControlObject::valueChanged,
            this, &LoopingControl::slotBeatLoopRollActivate);

    // Here we create corresponding beatloop_(SIZE) CO's which all call the same
    // BeatControl, but with a set value.
    for (unsigned int i = 0; i < (sizeof(s_dBeatSizes) / sizeof(s_dBeatSizes[0])); ++i) {
        BeatLoopingControl* pBeatLoop = new BeatLoopingControl(group, s_dBeatSizes[i]);
        connect(pBeatLoop, &BeatLoopingControl::activateBeatLoop,
                this, &LoopingControl::slotBeatLoopActivate,
                Qt::DirectConnection);
        connect(pBeatLoop,  &BeatLoopingControl::activateBeatLoopRoll,
                this, &LoopingControl::slotBeatLoopActivateRoll,
                Qt::DirectConnection);
        connect(pBeatLoop,  &BeatLoopingControl::deactivateBeatLoop,
                this, &LoopingControl::slotBeatLoopDeactivate,
                Qt::DirectConnection);
        connect(pBeatLoop,  &BeatLoopingControl::deactivateBeatLoopRoll,
                this, &LoopingControl::slotBeatLoopDeactivateRoll,
                Qt::DirectConnection);
        m_beatLoops.append(pBeatLoop);
    }

    m_pCOBeatJump = new ControlObject(ConfigKey(group, "beatjump"), false);
    connect(m_pCOBeatJump, &ControlObject::valueChanged,
            this, &LoopingControl::slotBeatJump, Qt::DirectConnection);
    m_pCOBeatJumpSize = new ControlObject(ConfigKey(group, "beatjump_size"),
                                          true, false, false, 4.0);
    m_pCOBeatJumpForward = new ControlPushButton(ConfigKey(group, "beatjump_forward"));
    connect(m_pCOBeatJumpForward, &ControlObject::valueChanged,
            this, &LoopingControl::slotBeatJumpForward);
    m_pCOBeatJumpBackward = new ControlPushButton(ConfigKey(group, "beatjump_backward"));
    connect(m_pCOBeatJumpBackward, &ControlObject::valueChanged,
            this, &LoopingControl::slotBeatJumpBackward);

    // Create beatjump_(SIZE) CO's which all call beatjump, but with a set
    // value.
    for (unsigned int i = 0; i < (sizeof(s_dBeatSizes) / sizeof(s_dBeatSizes[0])); ++i) {
        BeatJumpControl* pBeatJump = new BeatJumpControl(group, s_dBeatSizes[i]);
        connect(pBeatJump, &BeatJumpControl::beatJump,
                this, &LoopingControl::slotBeatJump,
                Qt::DirectConnection);
        m_beatJumps.append(pBeatJump);
    }

    m_pCOLoopMove = new ControlObject(ConfigKey(group, "loop_move"), false);
    connect(m_pCOLoopMove, &ControlObject::valueChanged,
            this, &LoopingControl::slotLoopMove, Qt::DirectConnection);

    // Create loop_move_(SIZE) CO's which all call loop_move, but with a set
    // value.
    for (unsigned int i = 0; i < (sizeof(s_dBeatSizes) / sizeof(s_dBeatSizes[0])); ++i) {
        LoopMoveControl* pLoopMove = new LoopMoveControl(group, s_dBeatSizes[i]);
        connect(pLoopMove, &LoopMoveControl::loopMove,
                this, &LoopingControl::slotLoopMove,
                Qt::DirectConnection);
        m_loopMoves.append(pLoopMove);
    }

    m_pCOLoopScale = new ControlObject(ConfigKey(group, "loop_scale"), false);
    connect(m_pCOLoopScale, &ControlObject::valueChanged,
            this, &LoopingControl::slotLoopScale);
    m_pLoopHalveButton = new ControlPushButton(ConfigKey(group, "loop_halve"));
    connect(m_pLoopHalveButton, &ControlObject::valueChanged,
            this, &LoopingControl::slotLoopHalve);
    m_pLoopDoubleButton = new ControlPushButton(ConfigKey(group, "loop_double"));
    connect(m_pLoopDoubleButton, &ControlObject::valueChanged,
            this, &LoopingControl::slotLoopDouble);

    m_pPlayButton = ControlObject::getControl(ConfigKey(group, "play"));
}

LoopingControl::~LoopingControl() {
    delete m_pLoopOutButton;
    delete m_pLoopOutGotoButton;
    delete m_pLoopInButton;
    delete m_pLoopInGotoButton;
    delete m_pLoopExitButton;
    delete m_pReloopToggleButton;
    delete m_pReloopAndStopButton;
    delete m_pCOLoopEnabled;
    delete m_pCOLoopStartPosition;
    delete m_pCOLoopEndPosition;
    delete m_pCOLoopScale;
    delete m_pLoopHalveButton;
    delete m_pLoopDoubleButton;

    delete m_pCOBeatLoop;
    while (!m_beatLoops.isEmpty()) {
        BeatLoopingControl* pBeatLoop = m_beatLoops.takeLast();
        delete pBeatLoop;
    }
    delete m_pCOBeatLoopSize;
    delete m_pCOBeatLoopActivate;
    delete m_pCOBeatLoopRollActivate;

    delete m_pCOBeatJump;
    delete m_pCOBeatJumpSize;
    delete m_pCOBeatJumpForward;
    delete m_pCOBeatJumpBackward;
    while (!m_beatJumps.isEmpty()) {
        BeatJumpControl* pBeatJump = m_beatJumps.takeLast();
        delete pBeatJump;
    }

    delete m_pCOLoopMove;
    while (!m_loopMoves.isEmpty()) {
        LoopMoveControl* pLoopMove = m_loopMoves.takeLast();
        delete pLoopMove;
    }
}

void LoopingControl::slotLoopScale(double scaleFactor) {
    LoopInfo loopInfo = m_loopInfo.getValue();
    if (!loopInfo.startPosition.isValid() || !loopInfo.endPosition.isValid()) {
        return;
    }

    const mixxx::audio::FrameDiff_t loopLength =
            (loopInfo.endPosition - loopInfo.startPosition) * scaleFactor;
    const auto trackEndPosition =
            mixxx::audio::FramePos::fromEngineSamplePosMaybeInvalid(
                    m_pTrackSamples->get());
    if (!trackEndPosition.isValid()) {
        return;
    }

    // Abandon loops that are too short of extend beyond the end of the file.
    if (loopLength < kMinimumAudibleLoopSizeFrames ||
            loopInfo.startPosition + loopLength > trackEndPosition) {
        return;
    }

    loopInfo.endPosition = loopInfo.startPosition + loopLength;

    // TODO(XXX) we could be smarter about taking the active beatloop, scaling
    // it by the desired amount and trying to find another beatloop that matches
    // it, but for now we just clear the active beat loop if somebody scales.
    clearActiveBeatLoop();

    // Don't allow 0 samples loop, so one can still manipulate it
    if (loopInfo.endPosition == loopInfo.startPosition) {
        if ((loopInfo.endPosition + 1) >= trackEndPosition) {
            loopInfo.startPosition -= 1;
        } else {
            loopInfo.endPosition += 1;
        }
    }
    // Do not allow loops to go past the end of the song
    else if (loopInfo.endPosition > trackEndPosition) {
        loopInfo.endPosition = trackEndPosition;
    }

    // Reseek if the loop shrank out from under the playposition.
    loopInfo.seekMode = (m_bLoopingEnabled && scaleFactor < 1.0)
            ? LoopSeekMode::Changed
            : LoopSeekMode::MovedOut;

    m_loopInfo.setValue(loopInfo);
    emit loopUpdated(loopInfo.startPosition, loopInfo.endPosition);

    // Update CO for loop end marker
    m_pCOLoopEndPosition->set(loopInfo.endPosition.toEngineSamplePos());
}

void LoopingControl::slotLoopHalve(double pressed) {
    if (pressed <= 0.0) {
        return;
    }

    slotBeatLoop(m_pCOBeatLoopSize->get() / 2.0, true, false);
}

void LoopingControl::slotLoopDouble(double pressed) {
    if (pressed <= 0.0) {
        return;
    }

    slotBeatLoop(m_pCOBeatLoopSize->get() * 2.0, true, false);
}

void LoopingControl::process(const double dRate,
        mixxx::audio::FramePos currentPosition,
        const int iBufferSize) {
    Q_UNUSED(iBufferSize);
    Q_UNUSED(dRate);

    const auto previousPosition = m_currentPosition.getValue();

    if (previousPosition != currentPosition) {
        m_currentPosition.setValue(currentPosition);
    } else {
        // no transport, so we have to do scheduled seeks here
        LoopInfo loopInfo = m_loopInfo.getValue();
        if (m_bLoopingEnabled &&
                !m_bAdjustingLoopIn && !m_bAdjustingLoopOut &&
                loopInfo.startPosition.isValid() &&
                loopInfo.endPosition.isValid()) {
            if (loopInfo.startPosition != m_oldLoopInfo.startPosition ||
                    loopInfo.endPosition != m_oldLoopInfo.endPosition) {
                // bool seek is only valid after the loop has changed
                if (loopInfo.seekMode == LoopSeekMode::Changed) {
                    // here the loop has changed and the play position
                    // should be moved with it
                    const auto targetPosition =
                            seekInsideAdjustedLoop(currentPosition,
                                    m_oldLoopInfo.startPosition,
                                    loopInfo.startPosition,
                                    loopInfo.endPosition);
                    if (targetPosition.isValid()) {
                        // jump immediately
                        seekAbs(targetPosition);
                    }
                }
                m_oldLoopInfo = loopInfo;
            }
        }
    }

    if (m_bAdjustingLoopIn) {
        setLoopInToCurrentPosition();
    } else if (m_bAdjustingLoopOut) {
        setLoopOutToCurrentPosition();
    }
}

mixxx::audio::FramePos LoopingControl::nextTrigger(bool reverse,
        mixxx::audio::FramePos currentPosition,
        mixxx::audio::FramePos* pTargetPosition) {
    *pTargetPosition = mixxx::audio::kInvalidFramePos;

    LoopInfo loopInfo = m_loopInfo.getValue();

    // m_bAdjustingLoopIn is true while the LoopIn button is pressed while a loop is active (slotLoopIn)
    if (m_bAdjustingLoopInOld != m_bAdjustingLoopIn) {
        m_bAdjustingLoopInOld = m_bAdjustingLoopIn;

        // When the LoopIn button is released in reverse mode we jump to the end of the loop to not fall out and disable the active loop
        // This must not happen in quantized mode. The newly set start is always ahead (in time, but behind spacially) of the current position so we don't jump.
        // Jumping to the end is then handled when the loop's start is reached later in this function.
        if (reverse && !m_bAdjustingLoopIn && !m_pQuantizeEnabled->toBool()) {
            m_oldLoopInfo = loopInfo;
            *pTargetPosition = loopInfo.endPosition;
            return currentPosition;
        }
    }

    // m_bAdjustingLoopOut is true while the LoopOut button is pressed while a loop is active (slotLoopOut)
    if (m_bAdjustingLoopOutOld != m_bAdjustingLoopOut) {
        m_bAdjustingLoopOutOld = m_bAdjustingLoopOut;

        // When the LoopOut button is released in forward mode we jump to the start of the loop to not fall out and disable the active loop
        // This must not happen in quantized mode. The newly set end is always ahead of the current position so we don't jump.
        // Jumping to the start is then handled when the loop's end is reached later in this function.
        if (!reverse && !m_bAdjustingLoopOut && !m_pQuantizeEnabled->toBool()) {
            m_oldLoopInfo = loopInfo;
            *pTargetPosition = loopInfo.startPosition;
            return currentPosition;
        }
    }

    if (m_bLoopingEnabled &&
            !m_bAdjustingLoopIn && !m_bAdjustingLoopOut &&
            loopInfo.startPosition.isValid() &&
            loopInfo.endPosition.isValid()) {
        if (loopInfo.startPosition != m_oldLoopInfo.startPosition ||
                loopInfo.endPosition != m_oldLoopInfo.endPosition) {
            // bool seek is only valid after the loop has changed
            switch (loopInfo.seekMode) {
            case LoopSeekMode::Changed:
                // here the loop has changed and the play position
                // should be moved with it
                *pTargetPosition = seekInsideAdjustedLoop(currentPosition,
                        m_oldLoopInfo.startPosition,
                        loopInfo.startPosition,
                        loopInfo.endPosition);
                break;
            case LoopSeekMode::MovedOut: {
                bool movedOut = false;
                // Check if we have moved out of the loop, before we could enable it
                if (reverse) {
                    if (loopInfo.startPosition > currentPosition) {
                        movedOut = true;
                    }
                } else {
                    if (loopInfo.endPosition < currentPosition) {
                        movedOut = true;
                    }
                }
                if (movedOut) {
                    *pTargetPosition = seekInsideAdjustedLoop(currentPosition,
                            loopInfo.startPosition,
                            loopInfo.startPosition,
                            loopInfo.endPosition);
                }
                break;
            }
            case LoopSeekMode::None:
                // Nothing to do here. This is used for enabling saved loops
                // which we want to do without jumping to the loop start
                // position.
                break;
            }
            m_oldLoopInfo = loopInfo;
            if (pTargetPosition->isValid()) {
                // jump immediately
                return currentPosition;
            }
        }

        if (reverse) {
            *pTargetPosition = loopInfo.endPosition;
            return loopInfo.startPosition;
        } else {
            *pTargetPosition = loopInfo.startPosition;
            return loopInfo.endPosition;
        }
    }
    return mixxx::audio::kInvalidFramePos;
}

void LoopingControl::hintReader(HintVector* pHintList) {
    LoopInfo loopInfo = m_loopInfo.getValue();
    Hint loop_hint;
    // If the loop is enabled, then this is high priority because we will loop
    // sometime potentially very soon! The current audio itself is priority 1,
    // but we will issue ourselves at priority 2.
    if (m_bLoopingEnabled) {
        // If we're looping, hint the loop in and loop out, in case we reverse
        // into it. We could save information from process to tell which
        // direction we're going in, but that this is much simpler, and hints
        // aren't that bad to make anyway.
        if (loopInfo.startPosition.isValid()) {
            loop_hint.priority = 2;
            loop_hint.frame = static_cast<SINT>(
                    loopInfo.startPosition.toLowerFrameBoundary().value());
            loop_hint.frameCount = Hint::kFrameCountForward;
            pHintList->append(loop_hint);
        }
        if (loopInfo.endPosition.isValid()) {
            loop_hint.priority = 10;
            loop_hint.frame = static_cast<SINT>(
                    loopInfo.endPosition.toUpperFrameBoundary().value());
            loop_hint.frameCount = Hint::kFrameCountBackward;
            pHintList->append(loop_hint);
        }
    } else {
        if (loopInfo.startPosition.isValid()) {
            loop_hint.priority = 10;
            loop_hint.frame = static_cast<SINT>(
                    loopInfo.startPosition.toLowerFrameBoundary().value());
            loop_hint.frameCount = Hint::kFrameCountForward;
            pHintList->append(loop_hint);
        }
    }
}

mixxx::audio::FramePos LoopingControl::getSyncPositionInsideLoop(
        mixxx::audio::FramePos requestedPlayPosition,
        mixxx::audio::FramePos syncedPlayPosition) {
    // no loop, no adjustment
    if (!m_bLoopingEnabled) {
        return syncedPlayPosition;
    }

    const LoopInfo loopInfo = m_loopInfo.getValue();

    // if the request itself is outside loop do nothing
    // loop will be disabled later by notifySeek(...) as is was explicitly requested by the user
    // if the requested position is the exact end of a loop it should also be disabled later by notifySeek(...)
    if (requestedPlayPosition < loopInfo.startPosition ||
            requestedPlayPosition >= loopInfo.endPosition) {
        return syncedPlayPosition;
    }

    // the requested position is inside the loop (e.g hotcue at start)
    const mixxx::audio::FrameDiff_t loopSizeFrames = loopInfo.endPosition - loopInfo.startPosition;

    // the synced position is in front of the loop
    // adjust the synced position to same amount in front of the loop end
    if (syncedPlayPosition < loopInfo.startPosition) {
        mixxx::audio::FrameDiff_t adjustment = loopInfo.startPosition - syncedPlayPosition;

        // prevents jumping in front of the loop if loop is smaller than adjustment
        adjustment = fmod(adjustment, loopSizeFrames);

        // if the synced position is exactly the start of the loop we would end up at the exact end
        // as this would disable the loop in notifySeek() replace it with the start of the loop
        if (adjustment == 0) {
            return loopInfo.startPosition;
        }
        return loopInfo.endPosition - adjustment;
    }

    // the synced position is behind the loop
    // adjust the synced position to same amount behind the loop start
    if (syncedPlayPosition >= loopInfo.endPosition) {
        mixxx::audio::FrameDiff_t adjustment = syncedPlayPosition - loopInfo.endPosition;

        // prevents jumping behind the loop if loop is smaller than adjustment
        adjustment = fmod(adjustment, loopSizeFrames);

        return loopInfo.startPosition + adjustment;
    }

    // both, requested and synced position are inside the loop -> do nothing
    return syncedPlayPosition;
}

void LoopingControl::setBeatLoop(mixxx::audio::FramePos startPosition, bool enabled) {
    VERIFY_OR_DEBUG_ASSERT(startPosition.isValid()) {
        return;
    }

    mixxx::BeatsPointer pBeats = m_pBeats;
    if (!pBeats) {
        return;
    }

    double beatloopSize = m_pCOBeatLoopSize->get();

    // TODO(XXX): This is not realtime safe. See this Zulip discussion for details:
    // https://mixxx.zulipchat.com/#narrow/stream/109171-development/topic/getting.20locks.20out.20of.20Beats
    const auto endPosition = pBeats->findNBeatsFromPosition(startPosition, beatloopSize);
    if (endPosition.isValid()) {
        setLoop(startPosition, endPosition, enabled);
    }
}

void LoopingControl::setLoop(mixxx::audio::FramePos startPosition,
        mixxx::audio::FramePos endPosition,
        bool enabled) {
    VERIFY_OR_DEBUG_ASSERT(startPosition.isValid() && endPosition.isValid() &&
            startPosition < endPosition) {
        return;
    }

    LoopInfo loopInfo = m_loopInfo.getValue();
    if (loopInfo.startPosition != startPosition || loopInfo.endPosition != endPosition) {
        // Copy saved loop parameters to active loop
        loopInfo.startPosition = startPosition;
        loopInfo.endPosition = endPosition;
        loopInfo.seekMode = LoopSeekMode::None;
        clearActiveBeatLoop();
        m_loopInfo.setValue(loopInfo);
        m_pCOLoopStartPosition->set(loopInfo.startPosition.toEngineSamplePos());
        m_pCOLoopEndPosition->set(loopInfo.endPosition.toEngineSamplePos());
    }
    setLoopingEnabled(enabled);

    // Seek back to loop in position if we're already behind the loop end.
    //
    // TODO(Holzhaus): This needs to be reverted as soon as GUI controls for
    // controlling saved loop behaviour are in place, because this change makes
    // saved loops very risky to use and might potentially mess up your mix.
    // See https://github.com/mixxxdj/mixxx/pull/2194#issuecomment-721847833
    // for details.
    if (enabled && m_currentPosition.getValue() > loopInfo.endPosition) {
        slotLoopInGoto(1);
    }

    m_pCOBeatLoopSize->setAndConfirm(findBeatloopSizeForLoop(startPosition, endPosition));
}

void LoopingControl::setLoopInToCurrentPosition() {
    // set loop-in position
    const mixxx::BeatsPointer pBeats = m_pBeats;
    LoopInfo loopInfo = m_loopInfo.getValue();
    mixxx::audio::FramePos quantizedBeatPosition;
    mixxx::audio::FramePos position = m_currentPosition.getValue();
    if (m_pQuantizeEnabled->toBool() && pBeats) {
        const auto closestBeatPosition =
                mixxx::audio::FramePos::fromEngineSamplePosMaybeInvalid(
                        m_pClosestBeat->get());
        if (m_bAdjustingLoopIn) {
            if (closestBeatPosition == m_currentPosition.getValue()) {
                quantizedBeatPosition = closestBeatPosition;
            } else {
                quantizedBeatPosition =
                        mixxx::audio::FramePos::fromEngineSamplePosMaybeInvalid(
                                m_pPreviousBeat->get());
            }
        } else {
            quantizedBeatPosition = closestBeatPosition;
        }
        if (quantizedBeatPosition.isValid()) {
            position = quantizedBeatPosition;
        }
    }

    // Reset the loop out position if it is before the loop in so that loops
    // cannot be inverted.
    if (loopInfo.endPosition.isValid() && loopInfo.endPosition < position) {
        loopInfo.endPosition = mixxx::audio::kInvalidFramePos;
        m_pCOLoopEndPosition->set(loopInfo.endPosition.toEngineSamplePosMaybeInvalid());
    }

    // If we're looping and the loop-in and out points are now so close
    //  that the loop would be inaudible, set the in point to the smallest
    //  pre-defined beatloop size instead (when possible)
    if (loopInfo.endPosition.isValid() &&
            (loopInfo.endPosition - position) < kMinimumAudibleLoopSizeFrames) {
        if (quantizedBeatPosition.isValid() && pBeats) {
            position = pBeats->findNthBeat(quantizedBeatPosition, -2);
            if (!position.isValid() ||
                    (loopInfo.endPosition - position) <
                            kMinimumAudibleLoopSizeFrames) {
                position = loopInfo.endPosition - kMinimumAudibleLoopSizeFrames;
            }
        } else {
            position = loopInfo.endPosition - kMinimumAudibleLoopSizeFrames;
        }
    }

    loopInfo.startPosition = position;

    m_pCOLoopStartPosition->set(loopInfo.startPosition.toEngineSamplePosMaybeInvalid());

    // start looping
    if (loopInfo.startPosition.isValid() && loopInfo.endPosition.isValid()) {
        setLoopingEnabled(true);
        loopInfo.seekMode = LoopSeekMode::Changed;
    } else {
        loopInfo.seekMode = LoopSeekMode::MovedOut;
    }

    if (m_pQuantizeEnabled->toBool() && loopInfo.startPosition.isValid() &&
            loopInfo.endPosition.isValid() &&
            loopInfo.startPosition < loopInfo.endPosition && pBeats) {
        m_pCOBeatLoopSize->setAndConfirm(pBeats->numBeatsInRange(
                loopInfo.startPosition, loopInfo.endPosition));
        updateBeatLoopingControls();
    } else {
        clearActiveBeatLoop();
    }

    m_loopInfo.setValue(loopInfo);
    //qDebug() << "set loop_in to " << loopInfo.startPosition;
}

void LoopingControl::slotLoopIn(double pressed) {
    if (!m_pTrack) {
        return;
    }

    // If loop is enabled, suspend looping and set the loop in point
    // when this button is released.
    if (m_bLoopingEnabled) {
        if (pressed > 0.0) {
            m_bAdjustingLoopIn = true;
            // Adjusting both the in and out point at the same time makes no sense
            m_bAdjustingLoopOut = false;
        } else {
            setLoopInToCurrentPosition();
            m_bAdjustingLoopIn = false;
            LoopInfo loopInfo = m_loopInfo.getValue();
            if (loopInfo.startPosition < loopInfo.endPosition) {
                emit loopUpdated(loopInfo.startPosition, loopInfo.endPosition);
            } else {
                emit loopReset();
            }
        }
    } else {
        emit loopReset();
        if (pressed > 0.0) {
            setLoopInToCurrentPosition();
        }
        m_bAdjustingLoopIn = false;
    }
}

void LoopingControl::slotLoopInGoto(double pressed) {
    if (pressed == 0.0) {
        return;
    }

    const auto loopInPosition = m_loopInfo.getValue().startPosition;
    if (loopInPosition.isValid()) {
        seekAbs(loopInPosition);
    }
}

void LoopingControl::setLoopOutToCurrentPosition() {
    mixxx::BeatsPointer pBeats = m_pBeats;
    LoopInfo loopInfo = m_loopInfo.getValue();
    mixxx::audio::FramePos quantizedBeatPosition;
    mixxx::audio::FramePos position = m_currentPosition.getValue();
    if (m_pQuantizeEnabled->toBool() && pBeats) {
        const auto closestBeatPosition =
                mixxx::audio::FramePos::fromEngineSamplePosMaybeInvalid(
                        m_pClosestBeat->get());
        if (m_bAdjustingLoopOut) {
            if (closestBeatPosition == m_currentPosition.getValue()) {
                quantizedBeatPosition = closestBeatPosition;
            } else {
                quantizedBeatPosition =
                        mixxx::audio::FramePos::fromEngineSamplePosMaybeInvalid(
                                m_pNextBeat->get());
            }
        } else {
            quantizedBeatPosition = closestBeatPosition;
        }
        if (quantizedBeatPosition.isValid()) {
            position = quantizedBeatPosition;
        }
    }

    // If the user is trying to set a loop-out before the loop in or without
    // having a loop-in, then ignore it.
    if (!loopInfo.startPosition.isValid() || position < loopInfo.startPosition) {
        return;
    }

    // If the loop-in and out points are set so close that the loop would be
    //  inaudible (which can happen easily with quantize-to-beat enabled,)
    //  use the smallest pre-defined beatloop instead (when possible)
    if ((position - loopInfo.startPosition) < kMinimumAudibleLoopSizeFrames) {
        if (quantizedBeatPosition.isValid() && pBeats) {
            position = pBeats->findNthBeat(quantizedBeatPosition, 2);
            if (!position.isValid() ||
                    (position - loopInfo.startPosition) <
                            kMinimumAudibleLoopSizeFrames) {
                position = loopInfo.startPosition + kMinimumAudibleLoopSizeFrames;
            }
        } else {
            position = loopInfo.startPosition + kMinimumAudibleLoopSizeFrames;
        }
    }

    // set loop out position
    loopInfo.endPosition = position;

    m_pCOLoopEndPosition->set(loopInfo.endPosition.toEngineSamplePosMaybeInvalid());

    // start looping
    if (loopInfo.startPosition.isValid() && loopInfo.endPosition.isValid()) {
        setLoopingEnabled(true);
        loopInfo.seekMode = LoopSeekMode::Changed;
    } else {
        loopInfo.seekMode = LoopSeekMode::MovedOut;
    }

    if (m_pQuantizeEnabled->toBool() && pBeats) {
        m_pCOBeatLoopSize->setAndConfirm(pBeats->numBeatsInRange(
                loopInfo.startPosition, loopInfo.endPosition));
        updateBeatLoopingControls();
    } else {
        clearActiveBeatLoop();
    }
    //qDebug() << "set loop_out to " << loopInfo.endPosition;

    m_loopInfo.setValue(loopInfo);
}

void LoopingControl::setRateControl(RateControl* rateControl) {
    m_pRateControl = rateControl;
}

void LoopingControl::slotLoopOut(double pressed) {
    if (m_pTrack == nullptr) {
        return;
    }

    // If loop is enabled, suspend looping and set the loop out point
    // when this button is released.
    if (m_bLoopingEnabled) {
        if (pressed > 0.0) {
            m_bAdjustingLoopOut = true;
            // Adjusting both the in and out point at the same time makes no sense
            m_bAdjustingLoopIn = false;
        } else {
            // If this button was pressed to set the loop out point when loop
            // was disabled, that will enable looping, so avoid moving the
            // loop out point when the button is released.
            if (!m_bLoopOutPressedWhileLoopDisabled) {
                setLoopOutToCurrentPosition();
                LoopInfo loopInfo = m_loopInfo.getValue();
                if (loopInfo.startPosition < loopInfo.endPosition) {
                    emit loopUpdated(loopInfo.startPosition, loopInfo.endPosition);
                } else {
                    emit loopReset();
                }
                m_bAdjustingLoopOut = false;
            } else {
                m_bLoopOutPressedWhileLoopDisabled = false;
            }
        }
    } else {
        emit loopReset();
        if (pressed > 0.0) {
            setLoopOutToCurrentPosition();
            m_bLoopOutPressedWhileLoopDisabled = true;
        }
        m_bAdjustingLoopOut = false;
    }
}

void LoopingControl::slotLoopOutGoto(double pressed) {
    if (pressed == 0.0) {
        return;
    }

    const auto loopOutPosition = m_loopInfo.getValue().endPosition;
    if (loopOutPosition.isValid()) {
        seekAbs(loopOutPosition);
    }
}

void LoopingControl::slotLoopExit(double val) {
    if (!m_pTrack || val <= 0.0) {
        return;
    }

    // If we're looping, stop looping
    if (m_bLoopingEnabled) {
        setLoopingEnabled(false);
    }
}

void LoopingControl::slotLoopEnabledValueChangeRequest(double value) {
    if (!m_pTrack) {
        return;
    }

    if (value > 0.0) {
        // Requested to set loop_enabled to 1
        if (m_bLoopingEnabled) {
            VERIFY_OR_DEBUG_ASSERT(m_pCOLoopEnabled->toBool()) {
                m_pCOLoopEnabled->setAndConfirm(1.0);
            }
        } else {
            // Looping is currently disabled, try to enable the loop. In
            // contrast to the reloop_toggle CO, we jump in no case.
            LoopInfo loopInfo = m_loopInfo.getValue();
            if (loopInfo.startPosition.isValid() &&
                    loopInfo.endPosition.isValid() &&
                    loopInfo.startPosition <= loopInfo.endPosition) {
                // setAndConfirm is called by setLoopingEnabled
                setLoopingEnabled(true);
            }
        }
    } else {
        // Requested to set loop_enabled to 0
        if (m_bLoopingEnabled) {
            // Looping is currently enabled, disable the loop. If loop roll
            // was active, also disable slip.
            if (m_bLoopRollActive) {
                m_pSlipEnabled->set(0);
                m_bLoopRollActive = false;
                m_activeLoopRolls.clear();
            }
            // setAndConfirm is called by setLoopingEnabled
            setLoopingEnabled(false);
        } else {
            VERIFY_OR_DEBUG_ASSERT(!m_pCOLoopEnabled->toBool()) {
                m_pCOLoopEnabled->setAndConfirm(0.0);
            }
        }
    }
}

void LoopingControl::slotReloopToggle(double val) {
    if (!m_pTrack || val <= 0.0) {
        return;
    }

    // If we're looping, stop looping
    if (m_bLoopingEnabled) {
        // If loop roll was active, also disable slip.
        if (m_bLoopRollActive) {
            m_pSlipEnabled->set(0);
            m_bLoopRollActive = false;
            m_activeLoopRolls.clear();
        }
        setLoopingEnabled(false);
        //qDebug() << "reloop_toggle looping off";
    } else {
        // If we're not looping, enable the loop. If the loop is ahead of the
        // current play position, do not jump to it.
        LoopInfo loopInfo = m_loopInfo.getValue();
        if (loopInfo.startPosition.isValid() &&
                loopInfo.endPosition.isValid() &&
                loopInfo.startPosition <= loopInfo.endPosition) {
            setLoopingEnabled(true);
            if (m_currentPosition.getValue() > loopInfo.endPosition) {
                slotLoopInGoto(1);
            }
        }
        //qDebug() << "reloop_toggle looping on";
    }
}

void LoopingControl::slotReloopAndStop(double pressed) {
    if (pressed == 0.0) {
        return;
    }

    m_pPlayButton->set(0.0);

    const auto loopInPosition = m_loopInfo.getValue().startPosition;
    if (loopInPosition.isValid()) {
        seekAbs(loopInPosition);
    }
    setLoopingEnabled(true);
}

void LoopingControl::slotLoopStartPos(double positionSamples) {
    // This slot is called before trackLoaded() for a new Track
    const auto position = mixxx::audio::FramePos::fromEngineSamplePosMaybeInvalid(positionSamples);

    LoopInfo loopInfo = m_loopInfo.getValue();
    if (loopInfo.startPosition == position) {
        //nothing to do
        return;
    }

    clearActiveBeatLoop();

    if (!position.isValid()) {
        emit loopReset();
        setLoopingEnabled(false);
    }

    loopInfo.seekMode = LoopSeekMode::MovedOut;
    loopInfo.startPosition = position;
    m_pCOLoopStartPosition->set(position.toEngineSamplePosMaybeInvalid());

    if (loopInfo.endPosition.isValid() && loopInfo.endPosition <= loopInfo.startPosition) {
        emit loopReset();
        loopInfo.endPosition = mixxx::audio::kInvalidFramePos;
        m_pCOLoopEndPosition->set(kNoTrigger);
        setLoopingEnabled(false);
    }
    m_loopInfo.setValue(loopInfo);
}

void LoopingControl::slotLoopEndPos(double positionSamples) {
    // This slot is called before trackLoaded() for a new Track
    const auto position = mixxx::audio::FramePos::fromEngineSamplePosMaybeInvalid(positionSamples);

    LoopInfo loopInfo = m_loopInfo.getValue();
    if (position.isValid() && loopInfo.endPosition == position) {
        //nothing to do
        return;
    }

    // Reject if the loop-in is not set, or if the new position is before the
    // start point (but not -1).
    if (!loopInfo.startPosition.isValid() ||
            (position.isValid() && position <= loopInfo.startPosition)) {
        m_pCOLoopEndPosition->set(loopInfo.endPosition.toEngineSamplePosMaybeInvalid());
        return;
    }

    clearActiveBeatLoop();

    if (!position.isValid()) {
        emit loopReset();
        setLoopingEnabled(false);
    }
    loopInfo.endPosition = position;
    loopInfo.seekMode = LoopSeekMode::MovedOut;
    m_pCOLoopEndPosition->set(position.toEngineSamplePosMaybeInvalid());
    m_loopInfo.setValue(loopInfo);
}

// This is called from the engine thread
void LoopingControl::notifySeek(mixxx::audio::FramePos position) {
    LoopInfo loopInfo = m_loopInfo.getValue();
    const auto currentPosition = m_currentPosition.getValue();
    if (m_bLoopingEnabled) {
        // Disable loop when we jumping out, or over a catching loop,
        // using hot cues or waveform overview.
        // Jumping to the exact end of a loop is considered jumping out.
        if (currentPosition >= loopInfo.startPosition &&
                currentPosition <= loopInfo.endPosition &&
                position < loopInfo.startPosition) {
            // jumping out of loop in backwards
            setLoopingEnabled(false);
        }
        if (currentPosition <= loopInfo.endPosition &&
                position >= loopInfo.endPosition) {
            // jumping out or to the exact end of a loop or over a catching loop forward
            setLoopingEnabled(false);
        }
    }
}

void LoopingControl::setLoopingEnabled(bool enabled) {
    if (m_bLoopingEnabled == enabled) {
        return;
    }

    m_bLoopingEnabled = enabled;
    m_pCOLoopEnabled->setAndConfirm(enabled ? 1.0 : 0.0);
    BeatLoopingControl* pActiveBeatLoop = atomicLoadRelaxed(m_pActiveBeatLoop);
    if (pActiveBeatLoop != nullptr) {
        if (enabled) {
            pActiveBeatLoop->activate();
        } else {
            pActiveBeatLoop->deactivate();
        }
    }

    emit loopEnabledChanged(enabled);
}

bool LoopingControl::isLoopingEnabled() {
    return m_bLoopingEnabled;
}

void LoopingControl::trackLoaded(TrackPointer pNewTrack) {
    m_pTrack = pNewTrack;
    mixxx::BeatsPointer pBeats;
    if (pNewTrack) {
        pBeats = pNewTrack->getBeats();
    }
    trackBeatsUpdated(pBeats);
}

void LoopingControl::trackBeatsUpdated(mixxx::BeatsPointer pBeats) {
    clearActiveBeatLoop();
    m_pBeats = pBeats;
    if (m_pBeats) {
        LoopInfo loopInfo = m_loopInfo.getValue();
        if (loopInfo.startPosition.isValid() && loopInfo.endPosition.isValid()) {
            double loaded_loop_size = findBeatloopSizeForLoop(
                    loopInfo.startPosition, loopInfo.endPosition);
            if (loaded_loop_size != -1) {
                m_pCOBeatLoopSize->setAndConfirm(loaded_loop_size);
            }
        }
    }
}

void LoopingControl::slotBeatLoopActivate(BeatLoopingControl* pBeatLoopControl) {
    if (!m_pTrack) {
        return;
    }

    // Maintain the current start point if there is an active loop currently
    // looping. slotBeatLoop will update m_pActiveBeatLoop if applicable. Note,
    // this used to only maintain the current start point if a beatloop was
    // enabled. See Bug #1159243.
    slotBeatLoop(pBeatLoopControl->getSize(), m_bLoopingEnabled, true);
}

void LoopingControl::slotBeatLoopActivateRoll(BeatLoopingControl* pBeatLoopControl) {
     if (!m_pTrack) {
         return;
     }

    // Disregard existing loops (except beatlooprolls).
    m_pSlipEnabled->set(1);
    slotBeatLoop(pBeatLoopControl->getSize(), m_bLoopRollActive, true);
    m_bLoopRollActive = true;
    m_activeLoopRolls.push(pBeatLoopControl->getSize());
}

void LoopingControl::slotBeatLoopDeactivate(BeatLoopingControl* pBeatLoopControl) {
    Q_UNUSED(pBeatLoopControl);
    setLoopingEnabled(false);
}

void LoopingControl::slotBeatLoopDeactivateRoll(BeatLoopingControl* pBeatLoopControl) {
    pBeatLoopControl->deactivate();
    const double size = pBeatLoopControl->getSize();
    auto* i = m_activeLoopRolls.begin();
    while (i != m_activeLoopRolls.end()) {
        if (size == *i) {
            i = m_activeLoopRolls.erase(i);
        } else {
            ++i;
        }
    }

    // Make sure slip mode is not turned off if it was turned on
    // by something that was not a rolling beatloop.
    if (m_bLoopRollActive && m_activeLoopRolls.empty()) {
        setLoopingEnabled(false);
        m_pSlipEnabled->set(0);
        m_bLoopRollActive = false;
    }

    // Return to the previous beatlooproll if necessary.
    if (!m_activeLoopRolls.empty()) {
        slotBeatLoop(m_activeLoopRolls.top(), m_bLoopRollActive, true);
    }
}

void LoopingControl::clearActiveBeatLoop() {
    BeatLoopingControl* pOldBeatLoop = m_pActiveBeatLoop.fetchAndStoreAcquire(nullptr);
    if (pOldBeatLoop != nullptr) {
        pOldBeatLoop->deactivate();
    }
}

bool LoopingControl::currentLoopMatchesBeatloopSize() {
    const mixxx::BeatsPointer pBeats = m_pBeats;
    if (!pBeats) {
        return false;
    }

    LoopInfo loopInfo = m_loopInfo.getValue();
    if (!loopInfo.startPosition.isValid()) {
        return false;
    }

    // Calculate where the loop out point would be if it is a beatloop
    const auto loopEndPosition = pBeats->findNBeatsFromPosition(
            loopInfo.startPosition, m_pCOBeatLoopSize->get());

    return loopInfo.endPosition.isValid() &&
            loopInfo.endPosition > loopEndPosition - 1 &&
            loopInfo.endPosition < loopEndPosition + 1;
}

double LoopingControl::findBeatloopSizeForLoop(
        mixxx::audio::FramePos startPosition,
        mixxx::audio::FramePos endPosition) const {
    const mixxx::BeatsPointer pBeats = m_pBeats;
    if (!pBeats) {
        return -1;
    }

    for (unsigned int i = 0; i < (sizeof(s_dBeatSizes) / sizeof(s_dBeatSizes[0])); ++i) {
        const auto loopEndPosition = pBeats->findNBeatsFromPosition(startPosition, s_dBeatSizes[i]);
        if (loopEndPosition.isValid()) {
            if (endPosition > (loopEndPosition - 1) && endPosition < (loopEndPosition + 1)) {
                return s_dBeatSizes[i];
            }
        }
    }
    return -1;
}

void LoopingControl::updateBeatLoopingControls() {
    // O(n) search, but there are only ~10-ish beatloop controls so this is
    // fine.
    double dBeatloopSize = m_pCOBeatLoopSize->get();
    for (BeatLoopingControl* pBeatLoopControl: qAsConst(m_beatLoops)) {
        if (pBeatLoopControl->getSize() == dBeatloopSize) {
            if (m_bLoopingEnabled) {
                pBeatLoopControl->activate();
            }
            BeatLoopingControl* pOldBeatLoop =
                    m_pActiveBeatLoop.fetchAndStoreRelease(pBeatLoopControl);
            if (pOldBeatLoop != nullptr && pOldBeatLoop != pBeatLoopControl) {
                pOldBeatLoop->deactivate();
            }
            return;
        }
    }
    // If the loop did not return from the function yet, dBeatloopSize does
    // not match any of the BeatLoopingControls' sizes.
    clearActiveBeatLoop();
}

void LoopingControl::slotBeatLoop(double beats, bool keepStartPoint, bool enable) {
    // If this is a "new" loop, stop tracking saved loop changes
    if (!keepStartPoint) {
        emit loopReset();
    }

    // if a seek was queued in the engine buffer move the current sample to its position
    const mixxx::audio::FramePos seekPosition = getEngineBuffer()->queuedSeekPosition();
    if (seekPosition.isValid()) {
        // seek position is already quantized if quantization is enabled
        m_currentPosition.setValue(seekPosition);
    }

    double maxBeatSize = s_dBeatSizes[sizeof(s_dBeatSizes)/sizeof(s_dBeatSizes[0]) - 1];
    double minBeatSize = s_dBeatSizes[0];
    if (beats < 0) {
        // For now we do not handle negative beatloops.
        clearActiveBeatLoop();
        return;
    } else if (beats > maxBeatSize) {
        beats = maxBeatSize;
    } else if (beats < minBeatSize) {
        beats = minBeatSize;
    }

    const auto trackEndPosition =
            mixxx::audio::FramePos::fromEngineSamplePosMaybeInvalid(
                    m_pTrackSamples->get());
    const mixxx::BeatsPointer pBeats = m_pBeats;
    if (!trackEndPosition.isValid() || !pBeats) {
        clearActiveBeatLoop();
        m_pCOBeatLoopSize->setAndConfirm(beats);
        return;
    }

    // Calculate the new loop start and end positions
    // give start and end defaults so we can detect problems
    LoopInfo newloopInfo = {mixxx::audio::kInvalidFramePos,
            mixxx::audio::kInvalidFramePos,
            LoopSeekMode::MovedOut};
    LoopInfo loopInfo = m_loopInfo.getValue();
    const auto currentPosition = m_currentPosition.getValue();

    // Start from the current position/closest beat and
    // create the loop around X beats from there.
    if (keepStartPoint) {
        if (loopInfo.startPosition.isValid()) {
            newloopInfo.startPosition = loopInfo.startPosition;
        } else {
            newloopInfo.startPosition = currentPosition;
        }
    } else {
        // loop_in is set to the closest beat if quantize is on and the loop size is >= 1 beat.
        // The closest beat might be ahead of play position and will cause a catching loop.
        mixxx::audio::FramePos prevBeatPosition;
        mixxx::audio::FramePos nextBeatPosition;
        pBeats->findPrevNextBeats(currentPosition, &prevBeatPosition, &nextBeatPosition, true);

        if (m_pQuantizeEnabled->toBool() && prevBeatPosition.isValid() &&
                nextBeatPosition.isValid()) {
            const mixxx::audio::FrameDiff_t beatLength = nextBeatPosition - prevBeatPosition;
            double loopLength = beatLength * beats;

            const mixxx::audio::FramePos closestBeatPosition =
                    pBeats->findClosestBeat(currentPosition);
            if (beats >= 1.0) {
                newloopInfo.startPosition = closestBeatPosition;
            } else {
                // In case of beat length less then 1 beat:
                // (| - beats, ^ - current track's position):
                //
                // ...|...................^........|...
                //
                // If we press 1/2 beatloop we want loop from 50% to 100%,
                // If I press 1/4 beatloop, we want loop from 50% to 75% etc
                const mixxx::audio::FrameDiff_t framesSinceLastBeat =
                        currentPosition - prevBeatPosition;

                // find the previous beat fraction and check if the current position is closer to this or the next one
                // place the new loop start to the closer one
                const mixxx::audio::FramePos previousFractionBeatPosition =
                        prevBeatPosition +
                        floor(framesSinceLastBeat / loopLength) * loopLength;
                double framesSinceLastFractionBeatPosition =
                        currentPosition - previousFractionBeatPosition;

                if (framesSinceLastFractionBeatPosition <= (loopLength / 2.0)) {
                    newloopInfo.startPosition = previousFractionBeatPosition;
                } else {
                    newloopInfo.startPosition = previousFractionBeatPosition + loopLength;
                }
            }

            // If running reverse, move the loop one loop size to the left.
            // Thus, the loops end will be closest to the current position
            bool reverse = false;
            if (m_pRateControl != nullptr) {
                reverse = m_pRateControl->isReverseButtonPressed();
            }
            if (reverse) {
                newloopInfo.startPosition -= loopLength;
            }
        } else {
            newloopInfo.startPosition = currentPosition;
        }
    }

    newloopInfo.endPosition = pBeats->findNBeatsFromPosition(newloopInfo.startPosition, beats);

    if (!newloopInfo.startPosition.isValid() ||
            !newloopInfo.endPosition.isValid() ||
            newloopInfo.startPosition >=
                    newloopInfo.endPosition // happens when the call above fails
            || newloopInfo.endPosition >
                    trackEndPosition) { // Do not allow beat loops to go beyond the end of the track
        // If a track is loaded with beatloop_size larger than
        // the distance between the loop in point and
        // the end of the track, let beatloop_size be set to
        // a smaller size, but not get larger.
        const double previousBeatloopSize = m_pCOBeatLoopSize->get();
        const mixxx::audio::FramePos previousLoopEndPosition =
                pBeats->findNBeatsFromPosition(newloopInfo.startPosition, previousBeatloopSize);
        if (previousLoopEndPosition < newloopInfo.startPosition && beats < previousBeatloopSize) {
            m_pCOBeatLoopSize->setAndConfirm(beats);
        }
        return;
    }

    // When loading a new track or after setting a manual loop without quantize,
    // do not resize the existing loop until beatloop_size matches
    // the size of the existing loop.
    // Do not return immediately so beatloop_size can be updated.
    bool omitResize = false;
    if (!currentLoopMatchesBeatloopSize() && !enable) {
        omitResize = true;
    }

    if (m_pCOBeatLoopSize->get() != beats) {
        m_pCOBeatLoopSize->setAndConfirm(beats);
    }

    // This check happens after setting m_pCOBeatLoopSize so
    // beatloop_size can be prepared without having a track loaded.
    if (!newloopInfo.startPosition.isValid() || !newloopInfo.endPosition.isValid()) {
        return;
    }

    if (omitResize) {
        return;
    }

    // If resizing an inactive loop by changing beatloop_size,
    // do not seek to the adjusted loop.
    newloopInfo.seekMode = (keepStartPoint && (enable || m_bLoopingEnabled))
            ? LoopSeekMode::Changed
            : LoopSeekMode::MovedOut;

    m_loopInfo.setValue(newloopInfo);
    emit loopUpdated(newloopInfo.startPosition, newloopInfo.endPosition);
    m_pCOLoopStartPosition->set(newloopInfo.startPosition.toEngineSamplePos());
    m_pCOLoopEndPosition->set(newloopInfo.endPosition.toEngineSamplePos());

    if (enable) {
        setLoopingEnabled(true);
    }
    updateBeatLoopingControls();
}

void LoopingControl::slotBeatLoopSizeChangeRequest(double beats) {
    // slotBeatLoop will call m_pCOBeatLoopSize->setAndConfirm if
    // new beatloop_size is valid
    slotBeatLoop(beats, true, false);
}

void LoopingControl::slotBeatLoopToggle(double pressed) {
    if (pressed > 0) {
        slotBeatLoop(m_pCOBeatLoopSize->get());
    }
}

void LoopingControl::slotBeatLoopRollActivate(double pressed) {
    if (pressed > 0.0) {
        if (m_bLoopingEnabled) {
            setLoopingEnabled(false);
            // Make sure slip mode is not turned off if it was turned on
            // by something that was not a rolling beatloop.
            if (m_bLoopRollActive) {
                m_pSlipEnabled->set(0.0);
                m_bLoopRollActive = false;
                m_activeLoopRolls.clear();
            }
        } else {
            m_pSlipEnabled->set(1.0);
            slotBeatLoop(m_pCOBeatLoopSize->get());
            m_bLoopRollActive = true;
        }
    } else {
        setLoopingEnabled(false);
        // Make sure slip mode is not turned off if it was turned on
        // by something that was not a rolling beatloop.
        if (m_bLoopRollActive) {
            m_pSlipEnabled->set(0.0);
            m_bLoopRollActive = false;
            m_activeLoopRolls.clear();
        }
    }
}

void LoopingControl::slotBeatJump(double beats) {
    const mixxx::BeatsPointer pBeats = m_pBeats;
    if (!pBeats) {
        return;
    }

    LoopInfo loopInfo = m_loopInfo.getValue();
    const auto currentPosition = m_currentPosition.getValue();

    if (m_bLoopingEnabled && !m_bAdjustingLoopIn && !m_bAdjustingLoopOut &&
            loopInfo.startPosition <= currentPosition &&
            loopInfo.endPosition >= currentPosition) {
        // If inside an active loop, move loop
        slotLoopMove(beats);
    } else {
        // seekExact bypasses Quantize, because a beat jump is implicit quantized
        const auto seekPosition = pBeats->findNBeatsFromPosition(currentPosition, beats);
        if (seekPosition.isValid()) {
            seekExact(seekPosition);
        }
    }
}

void LoopingControl::slotBeatJumpForward(double pressed) {
    if (pressed != 0) {
        slotBeatJump(m_pCOBeatJumpSize->get());
    }
}

void LoopingControl::slotBeatJumpBackward(double pressed) {
    if (pressed != 0) {
        slotBeatJump(-1.0 * m_pCOBeatJumpSize->get());
    }
}

void LoopingControl::slotLoopMove(double beats) {
    const mixxx::BeatsPointer pBeats = m_pBeats;
    if (!pBeats || beats == 0) {
        return;
    }
    LoopInfo loopInfo = m_loopInfo.getValue();
    if (!loopInfo.startPosition.isValid() || !loopInfo.endPosition.isValid()) {
        return;
    }

    if (BpmControl::getBeatContext(pBeats,
                m_currentPosition.getValue(),
                nullptr,
                nullptr,
                nullptr,
                nullptr)) {
        const auto newLoopStartPosition =
                pBeats->findNBeatsFromPosition(loopInfo.startPosition, beats);
        const auto newLoopEndPosition = currentLoopMatchesBeatloopSize()
                ? pBeats->findNBeatsFromPosition(newLoopStartPosition, m_pCOBeatLoopSize->get())
                : pBeats->findNBeatsFromPosition(loopInfo.endPosition, beats);

        // The track would stop as soon as the playhead crosses track end,
        // so we don't allow moving a loop beyond end.
        // https://bugs.launchpad.net/mixxx/+bug/1799574
        const auto trackEndPosition =
                mixxx::audio::FramePos::fromEngineSamplePosMaybeInvalid(
                        m_pTrackSamples->get());
        if (!trackEndPosition.isValid() || newLoopEndPosition > trackEndPosition) {
            return;
        }
        // If we are looping make sure that the play head does not leave the
        // loop as a result of our adjustment.
        loopInfo.seekMode = m_bLoopingEnabled ? LoopSeekMode::Changed : LoopSeekMode::MovedOut;

        loopInfo.startPosition = newLoopStartPosition;
        loopInfo.endPosition = newLoopEndPosition;
        m_loopInfo.setValue(loopInfo);
        emit loopUpdated(loopInfo.startPosition, loopInfo.endPosition);
        m_pCOLoopStartPosition->set(loopInfo.startPosition.toEngineSamplePosMaybeInvalid());
        m_pCOLoopEndPosition->set(loopInfo.endPosition.toEngineSamplePosMaybeInvalid());
    }
}

// Must be called from the engine thread only
mixxx::audio::FramePos LoopingControl::seekInsideAdjustedLoop(
        mixxx::audio::FramePos currentPosition,
        mixxx::audio::FramePos oldLoopStartPosition,
        mixxx::audio::FramePos newLoopStartPosition,
        mixxx::audio::FramePos newLoopEndPosition) {
    if (currentPosition >= newLoopStartPosition && currentPosition <= newLoopEndPosition) {
        // playposition already is inside the loop
        return mixxx::audio::kInvalidFramePos;
    }
    if (oldLoopStartPosition.isValid() &&
            currentPosition < oldLoopStartPosition &&
            currentPosition <= newLoopEndPosition) {
        // Playposition was before a catching loop and is still a catching loop
        // nothing to do
        return mixxx::audio::kInvalidFramePos;
    }

    const mixxx::audio::FrameDiff_t newLoopSize = newLoopEndPosition - newLoopStartPosition;
    DEBUG_ASSERT(newLoopSize > 0);
    mixxx::audio::FramePos adjustedPosition = currentPosition;
    while (adjustedPosition > newLoopEndPosition) {
        adjustedPosition -= newLoopSize;
        VERIFY_OR_DEBUG_ASSERT(adjustedPosition > newLoopStartPosition) {
            // I'm not even sure this is possible.  The new loop would have to be bigger than the
            // old loop, and the playhead was somehow outside the old loop.
            qWarning() << "SHOULDN'T HAPPEN: seekInsideAdjustedLoop couldn't find a new position --"
                       << " seeking to in point";
            adjustedPosition = newLoopStartPosition;
            break;
        }
    }
    while (adjustedPosition < newLoopStartPosition) {
        adjustedPosition += newLoopSize;
        VERIFY_OR_DEBUG_ASSERT(adjustedPosition < newLoopEndPosition) {
            qWarning() << "SHOULDN'T HAPPEN: seekInsideAdjustedLoop couldn't find a new position --"
                       << " seeking to in point";
            adjustedPosition = newLoopStartPosition;
            break;
        }
    }
    if (adjustedPosition != currentPosition) {
        return adjustedPosition;
    } else {
        return mixxx::audio::kInvalidFramePos;
    }
}

BeatJumpControl::BeatJumpControl(const QString& group, double size)
        : m_dBeatJumpSize(size) {
    m_pJumpForward = new ControlPushButton(
            keyForControl(group, "beatjump_%1_forward", size));
    connect(m_pJumpForward, &ControlObject::valueChanged,
            this, &BeatJumpControl::slotJumpForward,
            Qt::DirectConnection);
    m_pJumpBackward = new ControlPushButton(
            keyForControl(group, "beatjump_%1_backward", size));
    connect(m_pJumpBackward, &ControlObject::valueChanged,
            this, &BeatJumpControl::slotJumpBackward,
            Qt::DirectConnection);
}

BeatJumpControl::~BeatJumpControl() {
    delete m_pJumpForward;
    delete m_pJumpBackward;
}

void BeatJumpControl::slotJumpBackward(double pressed) {
    if (pressed > 0) {
        emit beatJump(-m_dBeatJumpSize);
    }
}

void BeatJumpControl::slotJumpForward(double pressed) {
    if (pressed > 0) {
        emit beatJump(m_dBeatJumpSize);
    }
}

LoopMoveControl::LoopMoveControl(const QString& group, double size)
        : m_dLoopMoveSize(size) {
    m_pMoveForward = new ControlPushButton(
            keyForControl(group, "loop_move_%1_forward", size));
    connect(m_pMoveForward, &ControlObject::valueChanged,
            this, &LoopMoveControl::slotMoveForward,
            Qt::DirectConnection);
    m_pMoveBackward = new ControlPushButton(
            keyForControl(group, "loop_move_%1_backward", size));
    connect(m_pMoveBackward, &ControlObject::valueChanged,
            this, &LoopMoveControl::slotMoveBackward,
            Qt::DirectConnection);
}

LoopMoveControl::~LoopMoveControl() {
    delete m_pMoveForward;
    delete m_pMoveBackward;
}

void LoopMoveControl::slotMoveBackward(double v) {
    if (v > 0) {
        emit loopMove(-m_dLoopMoveSize);
    }
}

void LoopMoveControl::slotMoveForward(double v) {
    if (v > 0) {
        emit loopMove(m_dLoopMoveSize);
    }
}

BeatLoopingControl::BeatLoopingControl(const QString& group, double size)
        : m_dBeatLoopSize(size),
          m_bActive(false) {
    // This is the original beatloop control which is now deprecated. Its value
    // is the state of the beatloop control (1 for enabled, 0 for disabled).
    m_pLegacy = new ControlPushButton(
            keyForControl(group, "beatloop_%1", size));
    m_pLegacy->setButtonMode(ControlPushButton::TOGGLE);
    connect(m_pLegacy, &ControlObject::valueChanged,
            this, &BeatLoopingControl::slotLegacy,
            Qt::DirectConnection);
    // A push-button which activates the beatloop.
    m_pActivate = new ControlPushButton(
            keyForControl(group, "beatloop_%1_activate", size));
    connect(m_pActivate, &ControlObject::valueChanged,
            this, &BeatLoopingControl::slotActivate,
            Qt::DirectConnection);
    // A push-button which toggles the beatloop as active or inactive.
    m_pToggle = new ControlPushButton(
            keyForControl(group, "beatloop_%1_toggle", size));
    connect(m_pToggle, &ControlObject::valueChanged,
            this, &BeatLoopingControl::slotToggle,
            Qt::DirectConnection);

    // A push-button which activates rolling beatloops
    m_pActivateRoll = new ControlPushButton(
            keyForControl(group, "beatlooproll_%1_activate", size));
    connect(m_pActivateRoll, &ControlObject::valueChanged,
            this, &BeatLoopingControl::slotActivateRoll,
            Qt::DirectConnection);

    // An indicator control which is 1 if the beatloop is enabled and 0 if not.
    m_pEnabled = new ControlObject(
            keyForControl(group, "beatloop_%1_enabled", size));
    m_pEnabled->setReadOnly();
}

BeatLoopingControl::~BeatLoopingControl() {
    delete m_pActivate;
    delete m_pToggle;
    delete m_pEnabled;
    delete m_pLegacy;
    delete m_pActivateRoll;
}

void BeatLoopingControl::deactivate() {
    if (m_bActive) {
        m_bActive = false;
        m_pEnabled->forceSet(0);
        m_pLegacy->set(0);
    }
}

void BeatLoopingControl::activate() {
    if (!m_bActive) {
        m_bActive = true;
        m_pEnabled->forceSet(1);
        m_pLegacy->set(1);
    }
}

void BeatLoopingControl::slotLegacy(double v) {
    //qDebug() << "slotLegacy" << m_dBeatLoopSize << "v" << v;
    if (v > 0) {
        emit activateBeatLoop(this);
    } else {
        emit deactivateBeatLoop(this);
    }
}

void BeatLoopingControl::slotActivate(double value) {
    //qDebug() << "slotActivate" << m_dBeatLoopSize << "value" << value;
    if (value == 0) {
        return;
    }
    emit activateBeatLoop(this);
}

void BeatLoopingControl::slotActivateRoll(double v) {
    //qDebug() << "slotActivateRoll" << m_dBeatLoopSize << "v" << v;
    if (v > 0) {
        emit activateBeatLoopRoll(this);
    } else {
        emit deactivateBeatLoopRoll(this);
    }
}

void BeatLoopingControl::slotToggle(double value) {
    //qDebug() << "slotToggle" << m_dBeatLoopSize << "value" << value;
    if (value == 0) {
        return;
    }
    if (m_bActive) {
        emit deactivateBeatLoop(this);
    } else {
        emit activateBeatLoop(this);
    }
}
