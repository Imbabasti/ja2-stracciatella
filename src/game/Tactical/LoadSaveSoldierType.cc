#include "Debug.h"
#include "LoadSaveData.h"
#include "LoadSaveObjectType.h"
#include "LoadSaveSoldierType.h"
#include "Overhead.h"
#include "Tactical_Save.h"
#include "Types.h"

#include <string_theory/string>

#include <algorithm>
#include <stdexcept>

static UINT32 MercChecksum(SOLDIERTYPE const& s)
{
	UINT32 sum = 1;

	sum += 1 + s.bLife;
	sum *= 1 + s.bLifeMax;
	sum += 1 + s.bAgility;
	sum *= 1 + s.bDexterity;
	sum += 1 + s.bStrength;
	sum *= 1 + s.bMarksmanship;
	sum += 1 + s.bMedical;
	sum *= 1 + s.bMechanical;
	sum += 1 + s.bExplosive;
	sum *= 1 + s.bExpLevel;
	sum += 1 + s.ubProfile;

	CFOR_EACH_SOLDIER_INV_SLOT(i, s)
	{
		sum += i->usItem;
		sum += i->ubNumberOfObjects;
	}

	return sum;
}


void ExtractSoldierType(const BYTE* const data, SOLDIERTYPE* const s, bool stracLinuxFormat, UINT32 uiSavedGameVersion)
{
	UINT16 usPathingData[ MAX_PATH_LIST_SIZE ];
	UINT16 usPathDataSize;
	UINT16 usPathIndex;

	*s = SOLDIERTYPE{};

	DataReader d{data};
	EXTR_U8(d, s->ubID)
	EXTR_SKIP(d, 1)
	EXTR_U8(d, s->ubBodyType)
	EXTR_I8(d, s->bActionPoints)
	EXTR_I8(d, s->bInitialActionPoints)
	EXTR_SKIP(d, 3)
	EXTR_U32(d, s->uiStatusFlags)
	FOR_EACH_SOLDIER_INV_SLOT(i, *s)
	{
		ExtractObject(d, i);
	}
	EXTR_PTR(d, s->pTempObject)
	EXTR_PTR(d, s->pKeyRing)
	EXTR_I8(d, s->bOldLife)
	EXTR_U8(d, s->bInSector)
	EXTR_I8(d, s->bFlashPortraitFrame)
	EXTR_SKIP(d, 1)
	EXTR_I16(d, s->sFractLife)
	EXTR_I8(d, s->bBleeding)
	EXTR_I8(d, s->bBreath)
	EXTR_I8(d, s->bBreathMax)
	EXTR_I8(d, s->bStealthMode)
	EXTR_I16(d, s->sBreathRed)
	EXTR_BOOL(d, s->fDelayedMovement)
	EXTR_SKIP(d, 1);
	EXTR_U8(d, s->ubWaitActionToDo)
	EXTR_SKIP(d, 1)
	EXTR_I8(d, s->ubInsertionDirection)
	EXTR_SKIP(d, 1)
	EXTR_SOLDIER(d, s->opponent)
	EXTR_I8(d, s->bLastRenderVisibleValue)
	EXTR_SKIP(d, 1)
	EXTR_U8(d, s->ubAttackingHand)
	EXTR_SKIP(d, 2)
	EXTR_I16(d, s->sWeightCarriedAtTurnStart)
	if(stracLinuxFormat)
	{
		EXTR_SKIP(d, 2)
		s->name = d.readUTF32(SOLDIERTYPE_NAME_LENGTH);
	}
	else
	{
		s->name = d.readUTF16(SOLDIERTYPE_NAME_LENGTH);
	}
	EXTR_I8(d, s->bVisible)
	EXTR_I8(d, s->bActive)
	EXTR_I8(d, s->bTeam)
	EXTR_U8(d, s->ubGroupID)
	EXTR_BOOL(d, s->fBetweenSectors)
	EXTR_U8(d, s->ubMovementNoiseHeard)
	if(stracLinuxFormat)
	{
		EXTR_SKIP(d, 2)
	}
	EXTR_FLOAT(d, s->dXPos)
	EXTR_FLOAT(d, s->dYPos)
	EXTR_SKIP(d, 8)
	EXTR_I16(d, s->sInitialGridNo)
	EXTR_I16(d, s->sGridNo)
	EXTR_I8(d, s->bDirection)
	EXTR_SKIP(d, 1)
	EXTR_I16(d, s->sHeightAdjustment)
	EXTR_I16(d, s->sDesiredHeight)
	EXTR_I16(d, s->sTempNewGridNo)
	EXTR_SKIP(d, 2)
	EXTR_I8(d, s->bOverTerrainType)
	EXTR_SKIP(d, 1)
	EXTR_I8(d, s->bCollapsed)
	EXTR_I8(d, s->bBreathCollapsed)
	EXTR_U8(d, s->ubDesiredHeight)
	EXTR_SKIP(d, 1)
	EXTR_U16(d, s->usPendingAnimation)
	EXTR_U8(d, s->ubPendingStanceChange)
	EXTR_SKIP(d, 1)
	EXTR_U16(d, s->usAnimState)
	EXTR_BOOL(d, s->fNoAPToFinishMove)
	EXTR_BOOL(d, s->fPausedMove)
	EXTR_BOOL(d, s->fUIdeadMerc)
	EXTR_SKIP(d, 1)
	EXTR_BOOL(d, s->fUICloseMerc)
	EXTR_SKIP(d, 5)
	EXTR_I32(d, s->UpdateCounter)
	EXTR_I32(d, s->DamageCounter)
	EXTR_SKIP_I32(d)
	EXTR_SKIP(d, 4)
	EXTR_I32(d, s->AICounter)
	EXTR_I32(d, s->FadeCounter)
	EXTR_U8(d, s->ubSkillTrait1)
	EXTR_U8(d, s->ubSkillTrait2)
	EXTR_SKIP(d, 6)
	EXTR_I8(d, s->bDexterity)
	EXTR_I8(d, s->bWisdom)
	EXTR_SKIP_I16(d)
	EXTR_SOLDIER(d, s->attacker)
	EXTR_SOLDIER(d, s->previous_attacker)
	EXTR_BOOL(d, s->fTurnInProgress)
	EXTR_BOOL(d, s->fIntendedTarget)
	EXTR_BOOL(d, s->fPauseAllAnimation)
	EXTR_I8(d, s->bExpLevel)
	EXTR_I16(d, s->sInsertionGridNo)
	EXTR_BOOL(d, s->fContinueMoveAfterStanceChange)
	EXTR_SKIP(d, 15)
	EXTR_I8(d, s->bLife)
	EXTR_U8(d, s->bSide)
	EXTR_SKIP(d, 1)
	EXTR_I8(d, s->bNewOppCnt)
	EXTR_SKIP(d, 2)
	EXTR_U16(d, s->usAniCode)
	EXTR_U16(d, s->usAniFrame)
	EXTR_I16(d, s->sAniDelay)
	EXTR_I8(d, s->bAgility)
	EXTR_SKIP(d, 1)
	EXTR_I16(d, s->sDelayedMovementCauseGridNo)
	EXTR_I16(d, s->sReservedMovementGridNo)
	EXTR_I8(d, s->bStrength)
	EXTR_SKIP(d, 1)
	EXTR_I16(d, s->sTargetGridNo)
	EXTR_I8(d, s->bTargetLevel)
	EXTR_I8(d, s->bTargetCubeLevel)
	EXTR_I16(d, s->sLastTarget)
	EXTR_I8(d, s->bTilesMoved)
	EXTR_I8(d, s->bLeadership)
	EXTR_FLOAT(d, s->dNextBleed)
	EXTR_BOOL(d, s->fWarnedAboutBleeding)
	EXTR_BOOL(d, s->fDyingComment)
	EXTR_U8(d, s->ubTilesMovedPerRTBreathUpdate)
	EXTR_SKIP(d, 1)
	EXTR_U16(d, s->usLastMovementAnimPerRTBreathUpdate)
	EXTR_BOOL(d, s->fTurningToShoot)
	EXTR_SKIP(d, 1)
	EXTR_BOOL(d, s->fTurningUntilDone)
	EXTR_BOOL(d, s->fGettingHit)
	EXTR_BOOL(d, s->fInNonintAnim)
	EXTR_BOOL(d, s->fFlashLocator)
	EXTR_I16(d, s->sLocatorFrame)
	EXTR_BOOL(d, s->fShowLocator)
	EXTR_BOOL(d, s->fFlashPortrait)
	EXTR_I8(d, s->bMechanical)
	EXTR_I8(d, s->bLifeMax)
	EXTR_SKIP(d, 6)
	s->HeadPal = d.readUTF8(PaletteRepID_LENGTH, ST::substitute_invalid);
	s->PantsPal = d.readUTF8(PaletteRepID_LENGTH, ST::substitute_invalid);
	s->VestPal = d.readUTF8(PaletteRepID_LENGTH, ST::substitute_invalid);
	s->SkinPal = d.readUTF8(PaletteRepID_LENGTH, ST::substitute_invalid);
	EXTR_SKIP(d, 328)
	EXTR_I8(d, s->bMedical)
	EXTR_BOOL(d, s->fBeginFade)
	EXTR_U8(d, s->ubFadeLevel)
	EXTR_U8(d, s->ubServiceCount)
	EXTR_SOLDIER(d, s->service_partner)
	EXTR_I8(d, s->bMarksmanship)
	EXTR_I8(d, s->bExplosive)
	EXTR_SKIP(d, 1)
	EXTR_PTR(d, s->pThrowParams)
	EXTR_BOOL(d, s->fTurningFromPronePosition)
	EXTR_I8(d, s->bReverse)
	EXTR_SKIP(d, 2)
	EXTR_PTR(d, s->pLevelNode)
	EXTR_SKIP(d, 8)
	EXTR_I8(d, s->bDesiredDirection)
	EXTR_SKIP(d, 1)
	EXTR_I16(d, s->sDestXPos)
	EXTR_I16(d, s->sDestYPos)
	EXTR_SKIP(d, 2)
	EXTR_I16(d, s->sDestination)
	EXTR_I16(d, s->sFinalDestination)
	EXTR_I8(d, s->bLevel)
	EXTR_SKIP(d, 3)

	// pathing info takes up 16 bit in the savegame but 8 bit in the engine
	EXTR_U16A(d, usPathingData, lengthof(usPathingData))
	EXTR_U16(d, usPathDataSize)
	EXTR_U16(d, usPathIndex)
	for (UINT8 i = 0; i < usPathDataSize && i < MAX_PATH_LIST_SIZE; i++) {
		s->ubPathingData[i] = (UINT8)usPathingData[i];
	}
	s->ubPathDataSize = (UINT8)usPathDataSize;
	s->ubPathIndex = (UINT8)usPathIndex;

	EXTR_I16(d, s->sBlackList)
	EXTR_I8(d, s->bAimTime)
	EXTR_I8(d, s->bShownAimTime)
	EXTR_I8(d, s->bPathStored)
	EXTR_I8(d, s->bHasKeys)
	EXTR_SKIP(d, 18)
	EXTR_U8(d, s->ubStrategicInsertionCode)
	EXTR_SKIP(d, 1)
	EXTR_U16(d, s->usStrategicInsertionData)
	EXTR_SKIP(d, 8)
	EXTR_I8(d, s->bMuzFlashCount)
	EXTR_SKIP(d, 1)
	EXTR_I16(d, s->sX)
	EXTR_I16(d, s->sY)
	EXTR_U16(d, s->usOldAniState)
	EXTR_I16(d, s->sOldAniCode)
	EXTR_I8(d, s->bBulletsLeft)
	EXTR_U8(d, s->ubSuppressionPoints)
	EXTR_U32(d, s->uiTimeOfLastRandomAction)
	EXTR_SKIP(d, 2)
	EXTR_I8A(d, s->bOppList, lengthof(s->bOppList))
	EXTR_I8(d, s->bLastAction)
	EXTR_I8(d, s->bAction)
	EXTR_U16(d, s->usActionData)
	EXTR_I8(d, s->bNextAction)
	EXTR_SKIP(d, 1)
	EXTR_U16(d, s->usNextActionData)
	EXTR_I8(d, s->bActionInProgress)
	EXTR_I8(d, s->bAlertStatus)
	EXTR_I8(d, s->bOppCnt)
	EXTR_I8(d, s->bNeutral)
	EXTR_I8(d, s->bNewSituation)
	EXTR_I8(d, s->bNextTargetLevel)
	EXTR_I8(d, s->bOrders)
	EXTR_I8(d, s->bAttitude)
	EXTR_I8(d, s->bUnderFire)
	EXTR_I8(d, s->bShock)
	EXTR_SKIP(d, 1)
	EXTR_I8(d, s->bBypassToGreen)
	EXTR_SKIP(d, 1)
	EXTR_I8(d, s->bDominantDir)
	EXTR_I8(d, s->bPatrolCnt)
	EXTR_I8(d, s->bNextPatrolPnt)
	EXTR_I16A(d, s->usPatrolGrid, lengthof(s->usPatrolGrid))
	EXTR_I16(d, s->sNoiseGridno)
	EXTR_U8(d, s->ubNoiseVolume)
	EXTR_I8(d, s->bLastAttackHit)
	EXTR_SOLDIER(d, s->xrayed_by)
	EXTR_SKIP(d, 1)
	EXTR_FLOAT(d, s->dHeightAdjustment)
	EXTR_I8(d, s->bMorale)
	EXTR_I8(d, s->bTeamMoraleMod)
	EXTR_I8(d, s->bTacticalMoraleMod)
	EXTR_I8(d, s->bStrategicMoraleMod)
	EXTR_I8(d, s->bAIMorale)
	EXTR_U8(d, s->ubPendingAction)
	EXTR_U8(d, s->ubPendingActionAnimCount)
	EXTR_SKIP(d, 1)
	EXTR_U32(d, s->uiPendingActionData1)
	EXTR_I16(d, s->sPendingActionData2)
	EXTR_I8(d, s->bPendingActionData3)
	EXTR_I8(d, s->ubDoorHandleCode)
	EXTR_U32(d, s->uiPendingActionData4)
	EXTR_I8(d, s->bInterruptDuelPts)
	EXTR_I8(d, s->bPassedLastInterrupt)
	EXTR_I8(d, s->bIntStartAPs)
	EXTR_I8(d, s->bMoved)
	EXTR_I8(d, s->bHunting)
	EXTR_SKIP(d, 1)
	EXTR_U8(d, s->ubCaller)
	EXTR_SKIP(d, 1)
	EXTR_I16(d, s->sCallerGridNo)
	EXTR_U8(d, s->bCallPriority)
	EXTR_I8(d, s->bCallActedUpon)
	EXTR_I8(d, s->bFrenzied)
	EXTR_I8(d, s->bNormalSmell)
	EXTR_I8(d, s->bMonsterSmell)
	EXTR_I8(d, s->bMobility)
	EXTR_SKIP(d, 1)
	EXTR_I8(d, s->fAIFlags)
	EXTR_BOOL(d, s->fDontChargeReadyAPs)
	EXTR_SKIP(d, 1)
	EXTR_U16(d, s->usAnimSurface)
	EXTR_U16(d, s->sZLevel)
	EXTR_BOOL(d, s->fPrevInWater)
	EXTR_BOOL(d, s->fGoBackToAimAfterHit)
	EXTR_I16(d, s->sWalkToAttackGridNo)
	EXTR_I16(d, s->sWalkToAttackWalkToCost)
	EXTR_SKIP(d, 7)
	EXTR_BOOL(d, s->fForceShade)
	EXTR_SKIP(d, 2)
	EXTR_PTR(d, s->pForcedShade)
	EXTR_I8(d, s->bDisplayDamageCount)
	EXTR_I8(d, s->fDisplayDamage)
	EXTR_I16(d, s->sDamage)
	EXTR_I16(d, s->sDamageX)
	EXTR_I16(d, s->sDamageY)
	EXTR_SKIP(d, 1)
	EXTR_I8(d, s->bDoBurst)
	EXTR_I16(d, s->usUIMovementMode)
	EXTR_SKIP(d, 1)
	EXTR_BOOL(d, s->fUIMovementFast)
	EXTR_SKIP(d, 2)
	EXTR_I32(d, s->BlinkSelCounter)
	EXTR_I32(d, s->PortraitFlashCounter)
	EXTR_BOOL(d, s->fDeadSoundPlayed)
	EXTR_U8(d, s->ubProfile)
	EXTR_U8(d, s->ubQuoteRecord)
	EXTR_U8(d, s->ubQuoteActionID)
	EXTR_U8(d, s->ubBattleSoundID)
	EXTR_BOOL(d, s->fClosePanel)
	EXTR_BOOL(d, s->fClosePanelToDie)
	EXTR_U8(d, s->ubClosePanelFrame)
	EXTR_BOOL(d, s->fDeadPanel)
	EXTR_U8(d, s->ubDeadPanelFrame)
	EXTR_SKIP(d, 2)
	EXTR_I16(d, s->sPanelFaceX)
	EXTR_I16(d, s->sPanelFaceY)
	EXTR_I8(d, s->bNumHitsThisTurn)
	EXTR_SKIP(d, 1)
	EXTR_U16(d, s->usQuoteSaidFlags)
	EXTR_I8(d, s->fCloseCall)
	EXTR_I8(d, s->bLastSkillCheck)
	EXTR_I8(d, s->ubSkillCheckAttempts)
	EXTR_SKIP(d, 1)
	EXTR_I8(d, s->bStartFallDir)
	EXTR_I8(d, s->fTryingToFall)
	EXTR_U8(d, s->ubPendingDirection)
	EXTR_SKIP(d, 1)
	EXTR_U32(d, s->uiAnimSubFlags)
	EXTR_U8(d, s->bAimShotLocation)
	EXTR_U8(d, s->ubHitLocation)
	EXTR_SKIP(d, 16)
	EXTR_I16A(d, s->sSpreadLocations, lengthof(s->sSpreadLocations))
	EXTR_BOOL(d, s->fDoSpread)
	EXTR_SKIP(d, 1)
	EXTR_I16(d, s->sStartGridNo)
	EXTR_I16(d, s->sEndGridNo)
	EXTR_I16(d, s->sForcastGridno)
	EXTR_I16(d, s->sZLevelOverride)
	EXTR_I8(d, s->bMovedPriorToInterrupt)
	EXTR_SKIP(d, 3)
	EXTR_I32(d, s->iEndofContractTime)
	EXTR_I32(d, s->iStartContractTime)
	EXTR_I32(d, s->iTotalContractLength)
	EXTR_I32(d, s->iNextActionSpecialData)
	EXTR_U8(d, s->ubWhatKindOfMercAmI)
	EXTR_I8(d, s->bAssignment)
	EXTR_SKIP(d, 1)
	EXTR_BOOL(d, s->fForcedToStayAwake)
	EXTR_I8(d, s->bTrainStat)
	EXTR_SKIP(d, 1)
	EXTR_I16(d, s->sSector.x)
	EXTR_I16(d, s->sSector.y)
	EXTR_I8(d, s->sSector.z)
	EXTR_SKIP(d, 1)
	EXTR_I32(d, s->iVehicleId)
	EXTR_PTR(d, s->pMercPath)
	EXTR_U8(d, s->fHitByGasFlags)
	EXTR_SKIP(d, 1)
	EXTR_U16(d, s->usMedicalDeposit)
	EXTR_U16(d, s->usLifeInsurance)
	EXTR_SKIP(d, 26)
	EXTR_I32(d, s->iStartOfInsuranceContract)
	EXTR_U32(d, s->uiLastAssignmentChangeMin)
	EXTR_I32(d, s->iTotalLengthOfInsuranceContract)
	EXTR_U8(d, s->ubSoldierClass)
	EXTR_U8(d, s->ubAPsLostToSuppression)
	EXTR_BOOL(d, s->fChangingStanceDueToSuppression)
	EXTR_SOLDIER(d, s->suppressor)
	EXTR_SKIP(d, 4)
	EXTR_U8(d, s->ubCivilianGroup)
	EXTR_SKIP(d, 3)
	EXTR_U32(d, s->uiChangeLevelTime)
	EXTR_U32(d, s->uiChangeHealthTime)
	EXTR_U32(d, s->uiChangeStrengthTime)
	EXTR_U32(d, s->uiChangeDexterityTime)
	EXTR_U32(d, s->uiChangeAgilityTime)
	EXTR_U32(d, s->uiChangeWisdomTime)
	EXTR_U32(d, s->uiChangeLeadershipTime)
	EXTR_U32(d, s->uiChangeMarksmanshipTime)
	EXTR_U32(d, s->uiChangeExplosivesTime)
	EXTR_U32(d, s->uiChangeMedicalTime)
	EXTR_U32(d, s->uiChangeMechanicalTime)
	EXTR_U32(d, s->uiUniqueSoldierIdValue)
	EXTR_I8(d, s->bBeingAttackedCount)
	EXTR_I8A(d, s->bNewItemCount, lengthof(s->bNewItemCount))
	EXTR_I8A(d, s->bNewItemCycleCount, lengthof(s->bNewItemCycleCount))
	EXTR_BOOL(d, s->fCheckForNewlyAddedItems)
	EXTR_I8(d, s->bEndDoorOpenCode)
	EXTR_U8(d, s->ubScheduleID)
	EXTR_I16(d, s->sEndDoorOpenCodeData)
	EXTR_I32(d, s->NextTileCounter)
	EXTR_BOOL(d, s->fBlockedByAnotherMerc)
	EXTR_I8(d, s->bBlockedByAnotherMercDirection)
	EXTR_U16(d, s->usAttackingWeapon)
	s->bWeaponMode = d.read<WeaponModes>();
	EXTR_SOLDIER(d, s->target)
	EXTR_I8(d, s->bAIScheduleProgress)
	EXTR_SKIP(d, 1)
	EXTR_I16(d, s->sOffWorldGridNo)
	EXTR_SKIP(d, 2)
	EXTR_PTR(d, s->pAniTile)
	EXTR_I8(d, s->bCamo)
	EXTR_SKIP(d, 1)
	EXTR_I16(d, s->sAbsoluteFinalDestination)
	EXTR_U8(d, s->ubHiResDirection)
	EXTR_SKIP(d, 1)
	EXTR_U8(d, s->ubLastFootPrintSound)
	EXTR_I8(d, s->bVehicleID)
	EXTR_I8(d, s->fPastXDest)
	EXTR_I8(d, s->fPastYDest)
	EXTR_I8(d, s->bMovementDirection)
	EXTR_SKIP(d, 1)
	EXTR_I16(d, s->sOldGridNo)
	EXTR_U16(d, s->usDontUpdateNewGridNoOnMoveAnimChange)
	EXTR_I16(d, s->sBoundingBoxWidth)
	EXTR_I16(d, s->sBoundingBoxHeight)
	EXTR_I16(d, s->sBoundingBoxOffsetX)
	EXTR_I16(d, s->sBoundingBoxOffsetY)
	EXTR_U32(d, s->uiTimeSameBattleSndDone)
	EXTR_I8(d, s->bOldBattleSnd)
	EXTR_SKIP(d, 1)
	EXTR_BOOL(d, s->fContractPriceHasIncreased)
	EXTR_SKIP(d, 1)
	EXTR_U32(d, s->uiBurstSoundID)
	EXTR_BOOL(d, s->fFixingSAMSite)
	EXTR_BOOL(d, s->fFixingRobot)
	EXTR_I8(d, s->bSlotItemTakenFrom)
	EXTR_BOOL(d, s->fSignedAnotherContract)
	EXTR_SOLDIER(d, s->auto_bandaging_medic)
	EXTR_BOOL(d, s->fDontChargeTurningAPs)
	EXTR_SOLDIER(d, s->robot_remote_holder)
	EXTR_SKIP(d, 1)
	EXTR_U32(d, s->uiTimeOfLastContractUpdate)
	EXTR_I8(d, s->bTypeOfLastContract)
	EXTR_I8(d, s->bTurnsCollapsed)
	EXTR_I8(d, s->bSleepDrugCounter)
	EXTR_U8(d, s->ubMilitiaKills)
	EXTR_I8A(d, s->bFutureDrugEffect, lengthof(s->bFutureDrugEffect))
	EXTR_I8A(d, s->bDrugEffectRate, lengthof(s->bDrugEffectRate))
	EXTR_I8A(d, s->bDrugEffect, lengthof(s->bDrugEffect))
	EXTR_I8A(d, s->bDrugSideEffectRate, lengthof(s->bDrugSideEffectRate))
	EXTR_I8A(d, s->bDrugSideEffect, lengthof(s->bDrugSideEffect))
	EXTR_SKIP(d, 2)
	EXTR_I8(d, s->bBlindedCounter)
	EXTR_BOOL(d, s->fMercCollapsedFlag)
	EXTR_BOOL(d, s->fDoneAssignmentAndNothingToDoFlag)
	EXTR_BOOL(d, s->fMercAsleep)
	EXTR_BOOL(d, s->fDontChargeAPsForStanceChange)
	EXTR_SKIP(d, 2)
	EXTR_U8(d, s->ubTurnsUntilCanSayHeardNoise)
	EXTR_U16(d, s->usQuoteSaidExtFlags)
	EXTR_U16(d, s->sContPathLocation)
	EXTR_I8(d, s->bGoodContPath)
	EXTR_SKIP(d, 1)
	EXTR_I8(d, s->bNoiseLevel)
	EXTR_I8(d, s->bRegenerationCounter)
	EXTR_I8(d, s->bRegenBoostersUsedToday)
	EXTR_I8(d, s->bNumPelletsHitBy)
	EXTR_I16(d, s->sSkillCheckGridNo)
	EXTR_U8(d, s->ubLastEnemyCycledID)
	EXTR_U8(d, s->ubPrevSectorID)
	EXTR_U8(d, s->ubNumTilesMovesSinceLastForget)
	EXTR_I8(d, s->bTurningIncrement)
	EXTR_U32(d, s->uiBattleSoundID)
	EXTR_BOOL(d, s->fSoldierWasMoving)
	EXTR_BOOL(d, s->fSayAmmoQuotePending)
	EXTR_U16(d, s->usValueGoneUp)
	EXTR_U8(d, s->ubNumLocateCycles)
	EXTR_U8(d, s->ubDelayedMovementFlags)
	EXTR_BOOL(d, s->fMuzzleFlash)
	EXTR_SOLDIER(d, s->CTGTTarget)
	EXTR_I32(d, s->PanelAnimateCounter)
	UINT32 checksum;
	EXTR_U32(d, checksum)
	EXTR_I8(d, s->bCurrentCivQuote)
	EXTR_I8(d, s->bCurrentCivQuoteDelta)
	EXTR_U8(d, s->ubMiscSoldierFlags)
	EXTR_U8(d, s->ubReasonCantFinishMove)
	EXTR_I16(d, s->sLocationOfFadeStart)
	EXTR_U8(d, s->bUseExitGridForReentryDirection)
	EXTR_SKIP(d, 1)
	EXTR_U32(d, s->uiTimeSinceLastSpoke)
	EXTR_U8(d, s->ubContractRenewalQuoteCode)
	EXTR_SKIP(d, 1)
	EXTR_I16(d, s->sPreTraversalGridNo)
	EXTR_U32(d, s->uiXRayActivatedTime)
	EXTR_I8(d, s->bTurningFromUI)
	EXTR_I8(d, s->bPendingActionData5)
	EXTR_I8(d, s->bDelayedStrategicMoraleMod)
	EXTR_U8(d, s->ubDoorOpeningNoise)
	EXTR_SKIP(d, 4)
	EXTR_U8(d, s->ubLeaveHistoryCode)
	EXTR_BOOL(d, s->fDontUnsetLastTargetFromTurn)
	EXTR_I8(d, s->bOverrideMoveSpeed)
	EXTR_BOOL(d, s->fUseMoverrideMoveSpeed)
	EXTR_U32(d, s->uiTimeSoldierWillArrive)
	EXTR_SKIP(d, 1)
	EXTR_BOOL(d, s->fUseLandingZoneForArrival)
	EXTR_BOOL(d, s->fFallClockwise)
	EXTR_I8(d, s->bVehicleUnderRepairID)
	EXTR_I32(d, s->iTimeCanSignElsewhere)
	EXTR_I8(d, s->bHospitalPriceModifier)
	EXTR_SKIP(d, 3)
	EXTR_U32(d, s->uiStartTimeOfInsuranceContract)
	EXTR_BOOL(d, s->fRTInNonintAnim)
	EXTR_BOOL(d, s->fDoingExternalDeath)
	EXTR_I8(d, s->bCorpseQuoteTolerance)
	EXTR_SKIP(d, 1)
	EXTR_I32(d, s->iPositionSndID)
	EXTR_U32(d, s->uiTuringSoundID)
	EXTR_U8(d, s->ubLastDamageReason)
	EXTR_BOOL(d, s->fComplainedThatTired)
	EXTR_I16A(d, s->sLastTwoLocations, lengthof(s->sLastTwoLocations))
	EXTR_SKIP(d, 2)
	EXTR_I32(d, s->uiTimeSinceLastBleedGrunt)
	EXTR_SOLDIER(d, s->next_to_previous_attacker)
	EXTR_SKIP(d, 39)
	if(stracLinuxFormat)
	{
		Assert(d.getConsumed() == 2352);
	}
	else
	{
		Assert(d.getConsumed() == 2328);
	}

	if (checksum != MercChecksum(*s))
	{
		throw std::runtime_error("soldier checksum mismatch");
	}
}


void InjectSoldierType(BYTE* const data, const SOLDIERTYPE* const s)
{
	UINT16 usPathingData[ MAX_PATH_LIST_SIZE ];
	UINT16 usPathDataSize;
	UINT16 usPathIndex;

	DataWriter d{data};
	INJ_U8(d, s->ubID)
	INJ_SKIP(d, 1)
	INJ_U8(d, s->ubBodyType)
	INJ_I8(d, s->bActionPoints)
	INJ_I8(d, s->bInitialActionPoints)
	INJ_SKIP(d, 3)
	INJ_U32(d, s->uiStatusFlags)
	CFOR_EACH_SOLDIER_INV_SLOT(i, *s)
	{
		InjectObject(d, i);
	}
	INJ_PTR(d, s->pTempObject)
	INJ_PTR(d, s->pKeyRing)
	INJ_I8(d, s->bOldLife)
	INJ_U8(d, s->bInSector)
	INJ_I8(d, s->bFlashPortraitFrame)
	INJ_SKIP(d, 1)
	INJ_I16(d, s->sFractLife)
	INJ_I8(d, s->bBleeding)
	INJ_I8(d, s->bBreath)
	INJ_I8(d, s->bBreathMax)
	INJ_I8(d, s->bStealthMode)
	INJ_I16(d, s->sBreathRed)
	INJ_BOOL(d, s->fDelayedMovement)
	INJ_SKIP(d, 1)
	INJ_U8(d, s->ubWaitActionToDo)
	INJ_SKIP(d, 1)
	INJ_I8(d, s->ubInsertionDirection)
	INJ_SKIP(d, 1)
	INJ_SOLDIER(d, s->opponent)
	INJ_I8(d, s->bLastRenderVisibleValue)
	INJ_SKIP(d, 1)
	INJ_U8(d, s->ubAttackingHand)
	INJ_SKIP(d, 2)
	INJ_I16(d, s->sWeightCarriedAtTurnStart)
	d.writeUTF16(s->name, SOLDIERTYPE_NAME_LENGTH);
	INJ_I8(d, s->bVisible)
	INJ_I8(d, s->bActive)
	INJ_I8(d, s->bTeam)
	INJ_U8(d, s->ubGroupID)
	INJ_BOOL(d, s->fBetweenSectors)
	INJ_U8(d, s->ubMovementNoiseHeard)
	INJ_FLOAT(d, s->dXPos)
	INJ_FLOAT(d, s->dYPos)
	INJ_SKIP(d, 8)
	INJ_I16(d, s->sInitialGridNo)
	INJ_I16(d, s->sGridNo)
	INJ_I8(d, s->bDirection)
	INJ_SKIP(d, 1)
	INJ_I16(d, s->sHeightAdjustment)
	INJ_I16(d, s->sDesiredHeight)
	INJ_I16(d, s->sTempNewGridNo)
	INJ_SKIP(d, 2)
	INJ_I8(d, s->bOverTerrainType)
	INJ_SKIP(d, 1)
	INJ_I8(d, s->bCollapsed)
	INJ_I8(d, s->bBreathCollapsed)
	INJ_U8(d, s->ubDesiredHeight)
	INJ_SKIP(d, 1)
	INJ_U16(d, s->usPendingAnimation)
	INJ_U8(d, s->ubPendingStanceChange)
	INJ_SKIP(d, 1)
	INJ_U16(d, s->usAnimState)
	INJ_BOOL(d, s->fNoAPToFinishMove)
	INJ_BOOL(d, s->fPausedMove)
	INJ_BOOL(d, s->fUIdeadMerc)
	INJ_SKIP(d, 1)
	INJ_BOOL(d, s->fUICloseMerc)
	INJ_SKIP(d, 5)
	INJ_I32(d, s->UpdateCounter)
	INJ_I32(d, s->DamageCounter)
	INJ_SKIP_I32(d)
	INJ_SKIP(d, 4)
	INJ_I32(d, s->AICounter)
	INJ_I32(d, s->FadeCounter)
	INJ_U8(d, s->ubSkillTrait1)
	INJ_U8(d, s->ubSkillTrait2)
	INJ_SKIP(d, 6)
	INJ_I8(d, s->bDexterity)
	INJ_I8(d, s->bWisdom)
	INJ_SKIP_I16(d)
	INJ_SOLDIER(d, s->attacker)
	INJ_SOLDIER(d, s->previous_attacker)
	INJ_BOOL(d, s->fTurnInProgress)
	INJ_BOOL(d, s->fIntendedTarget)
	INJ_BOOL(d, s->fPauseAllAnimation)
	INJ_I8(d, s->bExpLevel)
	INJ_I16(d, s->sInsertionGridNo)
	INJ_BOOL(d, s->fContinueMoveAfterStanceChange)
	INJ_SKIP(d, 15)
	INJ_I8(d, s->bLife)
	INJ_U8(d, s->bSide)
	INJ_SKIP(d, 1)
	INJ_I8(d, s->bNewOppCnt)
	INJ_SKIP(d, 2)
	INJ_U16(d, s->usAniCode)
	INJ_U16(d, s->usAniFrame)
	INJ_I16(d, s->sAniDelay)
	INJ_I8(d, s->bAgility)
	INJ_SKIP(d, 1)
	INJ_I16(d, s->sDelayedMovementCauseGridNo)
	INJ_I16(d, s->sReservedMovementGridNo)
	INJ_I8(d, s->bStrength)
	INJ_SKIP(d, 1)
	INJ_I16(d, s->sTargetGridNo)
	INJ_I8(d, s->bTargetLevel)
	INJ_I8(d, s->bTargetCubeLevel)
	INJ_I16(d, s->sLastTarget)
	INJ_I8(d, s->bTilesMoved)
	INJ_I8(d, s->bLeadership)
	INJ_FLOAT(d, s->dNextBleed)
	INJ_BOOL(d, s->fWarnedAboutBleeding)
	INJ_BOOL(d, s->fDyingComment)
	INJ_U8(d, s->ubTilesMovedPerRTBreathUpdate)
	INJ_SKIP(d, 1)
	INJ_U16(d, s->usLastMovementAnimPerRTBreathUpdate)
	INJ_BOOL(d, s->fTurningToShoot)
	INJ_SKIP(d, 1)
	INJ_BOOL(d, s->fTurningUntilDone)
	INJ_BOOL(d, s->fGettingHit)
	INJ_BOOL(d, s->fInNonintAnim)
	INJ_BOOL(d, s->fFlashLocator)
	INJ_I16(d, s->sLocatorFrame)
	INJ_BOOL(d, s->fShowLocator)
	INJ_BOOL(d, s->fFlashPortrait)
	INJ_I8(d, s->bMechanical)
	INJ_I8(d, s->bLifeMax)
	INJ_SKIP(d, 6)
	d.writeUTF8(s->HeadPal, PaletteRepID_LENGTH);
	d.writeUTF8(s->PantsPal, PaletteRepID_LENGTH);
	d.writeUTF8(s->VestPal, PaletteRepID_LENGTH);
	d.writeUTF8(s->SkinPal, PaletteRepID_LENGTH);
	INJ_SKIP(d, 328)
	INJ_I8(d, s->bMedical)
	INJ_BOOL(d, s->fBeginFade)
	INJ_U8(d, s->ubFadeLevel)
	INJ_U8(d, s->ubServiceCount)
	INJ_SOLDIER(d, s->service_partner)
	INJ_I8(d, s->bMarksmanship)
	INJ_I8(d, s->bExplosive)
	INJ_SKIP(d, 1)
	INJ_PTR(d, s->pThrowParams)
	INJ_BOOL(d, s->fTurningFromPronePosition)
	INJ_I8(d, s->bReverse)
	INJ_SKIP(d, 2)
	INJ_PTR(d, s->pLevelNode)
	INJ_SKIP(d, 8)
	INJ_I8(d, s->bDesiredDirection)
	INJ_SKIP(d, 1)
	INJ_I16(d, s->sDestXPos)
	INJ_I16(d, s->sDestYPos)
	INJ_SKIP(d, 2)
	INJ_I16(d, s->sDestination)
	INJ_I16(d, s->sFinalDestination)
	INJ_I8(d, s->bLevel)
	INJ_SKIP(d, 3)

	// pathing info takes up 16 bit in the savegame but 8 bit in the engine
	usPathDataSize = s->ubPathDataSize > MAX_PATH_LIST_SIZE ? (UINT16)MAX_PATH_LIST_SIZE : (UINT16)s->ubPathDataSize;
	usPathIndex = (UINT16)s->ubPathIndex;
	std::fill_n(usPathingData, MAX_PATH_LIST_SIZE, 0);
	for (UINT8 i = 0; i < usPathDataSize; i++) {
		usPathingData[i] = (UINT16)s->ubPathingData[i];
	}
	INJ_U16A(d, usPathingData, lengthof(usPathingData))
	INJ_U16(d, usPathDataSize)
	INJ_U16(d, usPathIndex)

	INJ_I16(d, s->sBlackList)
	INJ_I8(d, s->bAimTime)
	INJ_I8(d, s->bShownAimTime)
	INJ_I8(d, s->bPathStored)
	INJ_I8(d, s->bHasKeys)
	INJ_SKIP(d, 18)
	INJ_U8(d, s->ubStrategicInsertionCode)
	INJ_SKIP(d, 1)
	INJ_U16(d, s->usStrategicInsertionData)
	INJ_SKIP(d, 8)
	INJ_I8(d, s->bMuzFlashCount)
	INJ_SKIP(d, 1)
	INJ_I16(d, s->sX)
	INJ_I16(d, s->sY)
	INJ_U16(d, s->usOldAniState)
	INJ_I16(d, s->sOldAniCode)
	INJ_I8(d, s->bBulletsLeft)
	INJ_U8(d, s->ubSuppressionPoints)
	INJ_U32(d, s->uiTimeOfLastRandomAction)
	INJ_SKIP(d, 2)
	INJ_I8A(d, s->bOppList, lengthof(s->bOppList))
	INJ_I8(d, s->bLastAction)
	INJ_I8(d, s->bAction)
	INJ_U16(d, s->usActionData)
	INJ_I8(d, s->bNextAction)
	INJ_SKIP(d, 1)
	INJ_U16(d, s->usNextActionData)
	INJ_I8(d, s->bActionInProgress)
	INJ_I8(d, s->bAlertStatus)
	INJ_I8(d, s->bOppCnt)
	INJ_I8(d, s->bNeutral)
	INJ_I8(d, s->bNewSituation)
	INJ_I8(d, s->bNextTargetLevel)
	INJ_I8(d, s->bOrders)
	INJ_I8(d, s->bAttitude)
	INJ_I8(d, s->bUnderFire)
	INJ_I8(d, s->bShock)
	INJ_SKIP(d, 1)
	INJ_I8(d, s->bBypassToGreen)
	INJ_SKIP(d, 1)
	INJ_I8(d, s->bDominantDir)
	INJ_I8(d, s->bPatrolCnt)
	INJ_I8(d, s->bNextPatrolPnt)
	INJ_I16A(d, s->usPatrolGrid, lengthof(s->usPatrolGrid))
	INJ_I16(d, s->sNoiseGridno)
	INJ_U8(d, s->ubNoiseVolume)
	INJ_I8(d, s->bLastAttackHit)
	INJ_SOLDIER(d, s->xrayed_by)
	INJ_SKIP(d, 1)
	INJ_FLOAT(d, s->dHeightAdjustment)
	INJ_I8(d, s->bMorale)
	INJ_I8(d, s->bTeamMoraleMod)
	INJ_I8(d, s->bTacticalMoraleMod)
	INJ_I8(d, s->bStrategicMoraleMod)
	INJ_I8(d, s->bAIMorale)
	INJ_U8(d, s->ubPendingAction)
	INJ_U8(d, s->ubPendingActionAnimCount)
	INJ_SKIP(d, 1)
	INJ_U32(d, s->uiPendingActionData1)
	INJ_I16(d, s->sPendingActionData2)
	INJ_I8(d, s->bPendingActionData3)
	INJ_I8(d, s->ubDoorHandleCode)
	INJ_U32(d, s->uiPendingActionData4)
	INJ_I8(d, s->bInterruptDuelPts)
	INJ_I8(d, s->bPassedLastInterrupt)
	INJ_I8(d, s->bIntStartAPs)
	INJ_I8(d, s->bMoved)
	INJ_I8(d, s->bHunting)
	INJ_SKIP(d, 1)
	INJ_U8(d, s->ubCaller)
	INJ_SKIP(d, 1)
	INJ_I16(d, s->sCallerGridNo)
	INJ_U8(d, s->bCallPriority)
	INJ_I8(d, s->bCallActedUpon)
	INJ_I8(d, s->bFrenzied)
	INJ_I8(d, s->bNormalSmell)
	INJ_I8(d, s->bMonsterSmell)
	INJ_I8(d, s->bMobility)
	INJ_SKIP(d, 1)
	INJ_I8(d, s->fAIFlags)
	INJ_BOOL(d, s->fDontChargeReadyAPs)
	INJ_SKIP(d, 1)
	INJ_U16(d, s->usAnimSurface)
	INJ_U16(d, s->sZLevel)
	INJ_BOOL(d, s->fPrevInWater)
	INJ_BOOL(d, s->fGoBackToAimAfterHit)
	INJ_I16(d, s->sWalkToAttackGridNo)
	INJ_I16(d, s->sWalkToAttackWalkToCost)
	INJ_SKIP(d, 7)
	INJ_BOOL(d, s->fForceShade)
	INJ_SKIP(d, 2)
	INJ_PTR(d, s->pForcedShade)
	INJ_I8(d, s->bDisplayDamageCount)
	INJ_I8(d, s->fDisplayDamage)
	INJ_I16(d, s->sDamage)
	INJ_I16(d, s->sDamageX)
	INJ_I16(d, s->sDamageY)
	INJ_SKIP(d, 1)
	INJ_I8(d, s->bDoBurst)
	INJ_I16(d, s->usUIMovementMode)
	INJ_SKIP(d, 1)
	INJ_BOOL(d, s->fUIMovementFast)
	INJ_SKIP(d, 2)
	INJ_I32(d, s->BlinkSelCounter)
	INJ_I32(d, s->PortraitFlashCounter)
	INJ_BOOL(d, s->fDeadSoundPlayed)
	INJ_U8(d, s->ubProfile)
	INJ_U8(d, s->ubQuoteRecord)
	INJ_U8(d, s->ubQuoteActionID)
	INJ_U8(d, s->ubBattleSoundID)
	INJ_BOOL(d, s->fClosePanel)
	INJ_BOOL(d, s->fClosePanelToDie)
	INJ_U8(d, s->ubClosePanelFrame)
	INJ_BOOL(d, s->fDeadPanel)
	INJ_U8(d, s->ubDeadPanelFrame)
	INJ_SKIP(d, 2)
	INJ_I16(d, s->sPanelFaceX)
	INJ_I16(d, s->sPanelFaceY)
	INJ_I8(d, s->bNumHitsThisTurn)
	INJ_SKIP(d, 1)
	INJ_U16(d, s->usQuoteSaidFlags)
	INJ_I8(d, s->fCloseCall)
	INJ_I8(d, s->bLastSkillCheck)
	INJ_I8(d, s->ubSkillCheckAttempts)
	INJ_SKIP(d, 1)
	INJ_I8(d, s->bStartFallDir)
	INJ_I8(d, s->fTryingToFall)
	INJ_U8(d, s->ubPendingDirection)
	INJ_SKIP(d, 1)
	INJ_U32(d, s->uiAnimSubFlags)
	INJ_U8(d, s->bAimShotLocation)
	INJ_U8(d, s->ubHitLocation)
	INJ_SKIP(d, 16)
	INJ_I16A(d, s->sSpreadLocations, lengthof(s->sSpreadLocations))
	INJ_BOOL(d, s->fDoSpread)
	INJ_SKIP(d, 1)
	INJ_I16(d, s->sStartGridNo)
	INJ_I16(d, s->sEndGridNo)
	INJ_I16(d, s->sForcastGridno)
	INJ_I16(d, s->sZLevelOverride)
	INJ_I8(d, s->bMovedPriorToInterrupt)
	INJ_SKIP(d, 3)
	INJ_I32(d, s->iEndofContractTime)
	INJ_I32(d, s->iStartContractTime)
	INJ_I32(d, s->iTotalContractLength)
	INJ_I32(d, s->iNextActionSpecialData)
	INJ_U8(d, s->ubWhatKindOfMercAmI)
	INJ_I8(d, s->bAssignment)
	INJ_SKIP(d, 1)
	INJ_BOOL(d, s->fForcedToStayAwake)
	INJ_I8(d, s->bTrainStat)
	INJ_SKIP(d, 1)
	INJ_I16(d, s->sSector.x)
	INJ_I16(d, s->sSector.y)
	INJ_I8(d, s->sSector.z)
	INJ_SKIP(d, 1)
	INJ_I32(d, s->iVehicleId)
	INJ_PTR(d, s->pMercPath)
	INJ_U8(d, s->fHitByGasFlags)
	INJ_SKIP(d, 1)
	INJ_U16(d, s->usMedicalDeposit)
	INJ_U16(d, s->usLifeInsurance)
	INJ_SKIP(d, 26)
	INJ_I32(d, s->iStartOfInsuranceContract)
	INJ_U32(d, s->uiLastAssignmentChangeMin)
	INJ_I32(d, s->iTotalLengthOfInsuranceContract)
	INJ_U8(d, s->ubSoldierClass)
	INJ_U8(d, s->ubAPsLostToSuppression)
	INJ_BOOL(d, s->fChangingStanceDueToSuppression)
	INJ_SOLDIER(d, s->suppressor)
	INJ_SKIP(d, 4)
	INJ_U8(d, s->ubCivilianGroup)
	INJ_SKIP(d, 3)
	INJ_U32(d, s->uiChangeLevelTime)
	INJ_U32(d, s->uiChangeHealthTime)
	INJ_U32(d, s->uiChangeStrengthTime)
	INJ_U32(d, s->uiChangeDexterityTime)
	INJ_U32(d, s->uiChangeAgilityTime)
	INJ_U32(d, s->uiChangeWisdomTime)
	INJ_U32(d, s->uiChangeLeadershipTime)
	INJ_U32(d, s->uiChangeMarksmanshipTime)
	INJ_U32(d, s->uiChangeExplosivesTime)
	INJ_U32(d, s->uiChangeMedicalTime)
	INJ_U32(d, s->uiChangeMechanicalTime)
	INJ_U32(d, s->uiUniqueSoldierIdValue)
	INJ_I8(d, s->bBeingAttackedCount)
	INJ_I8A(d, s->bNewItemCount, lengthof(s->bNewItemCount))
	INJ_I8A(d, s->bNewItemCycleCount, lengthof(s->bNewItemCycleCount))
	INJ_BOOL(d, s->fCheckForNewlyAddedItems)
	INJ_I8(d, s->bEndDoorOpenCode)
	INJ_U8(d, s->ubScheduleID)
	INJ_I16(d, s->sEndDoorOpenCodeData)
	INJ_I32(d, s->NextTileCounter)
	INJ_BOOL(d, s->fBlockedByAnotherMerc)
	INJ_I8(d, s->bBlockedByAnotherMercDirection)
	INJ_U16(d, s->usAttackingWeapon)
	INJ_I8(d, s->bWeaponMode)
	INJ_SOLDIER(d, s->target)
	INJ_I8(d, s->bAIScheduleProgress)
	INJ_SKIP(d, 1)
	INJ_I16(d, s->sOffWorldGridNo)
	INJ_SKIP(d, 2)
	INJ_PTR(d, s->pAniTile)
	INJ_I8(d, s->bCamo)
	INJ_SKIP(d, 1)
	INJ_I16(d, s->sAbsoluteFinalDestination)
	INJ_U8(d, s->ubHiResDirection)
	INJ_SKIP(d, 1)
	INJ_U8(d, s->ubLastFootPrintSound)
	INJ_I8(d, s->bVehicleID)
	INJ_I8(d, s->fPastXDest)
	INJ_I8(d, s->fPastYDest)
	INJ_I8(d, s->bMovementDirection)
	INJ_SKIP(d, 1)
	INJ_I16(d, s->sOldGridNo)
	INJ_U16(d, s->usDontUpdateNewGridNoOnMoveAnimChange)
	INJ_I16(d, s->sBoundingBoxWidth)
	INJ_I16(d, s->sBoundingBoxHeight)
	INJ_I16(d, s->sBoundingBoxOffsetX)
	INJ_I16(d, s->sBoundingBoxOffsetY)
	INJ_U32(d, s->uiTimeSameBattleSndDone)
	INJ_I8(d, s->bOldBattleSnd)
	INJ_SKIP(d, 1)
	INJ_BOOL(d, s->fContractPriceHasIncreased)
	INJ_SKIP(d, 1)
	INJ_U32(d, s->uiBurstSoundID)
	INJ_BOOL(d, s->fFixingSAMSite)
	INJ_BOOL(d, s->fFixingRobot)
	INJ_I8(d, s->bSlotItemTakenFrom)
	INJ_BOOL(d, s->fSignedAnotherContract)
	INJ_SOLDIER(d, s->auto_bandaging_medic)
	INJ_BOOL(d, s->fDontChargeTurningAPs)
	INJ_SOLDIER(d, s->robot_remote_holder)
	INJ_SKIP(d, 1)
	INJ_U32(d, s->uiTimeOfLastContractUpdate)
	INJ_I8(d, s->bTypeOfLastContract)
	INJ_I8(d, s->bTurnsCollapsed)
	INJ_I8(d, s->bSleepDrugCounter)
	INJ_U8(d, s->ubMilitiaKills)
	INJ_I8A(d, s->bFutureDrugEffect, lengthof(s->bFutureDrugEffect))
	INJ_I8A(d, s->bDrugEffectRate, lengthof(s->bDrugEffectRate))
	INJ_I8A(d, s->bDrugEffect, lengthof(s->bDrugEffect))
	INJ_I8A(d, s->bDrugSideEffectRate, lengthof(s->bDrugSideEffectRate))
	INJ_I8A(d, s->bDrugSideEffect, lengthof(s->bDrugSideEffect))
	INJ_SKIP(d, 2)
	INJ_I8(d, s->bBlindedCounter)
	INJ_BOOL(d, s->fMercCollapsedFlag)
	INJ_BOOL(d, s->fDoneAssignmentAndNothingToDoFlag)
	INJ_BOOL(d, s->fMercAsleep)
	INJ_BOOL(d, s->fDontChargeAPsForStanceChange)
	INJ_SKIP(d, 2)
	INJ_U8(d, s->ubTurnsUntilCanSayHeardNoise)
	INJ_U16(d, s->usQuoteSaidExtFlags)
	INJ_U16(d, s->sContPathLocation)
	INJ_I8(d, s->bGoodContPath)
	INJ_SKIP(d, 1)
	INJ_I8(d, s->bNoiseLevel)
	INJ_I8(d, s->bRegenerationCounter)
	INJ_I8(d, s->bRegenBoostersUsedToday)
	INJ_I8(d, s->bNumPelletsHitBy)
	INJ_I16(d, s->sSkillCheckGridNo)
	INJ_U8(d, s->ubLastEnemyCycledID)
	INJ_U8(d, s->ubPrevSectorID)
	INJ_U8(d, s->ubNumTilesMovesSinceLastForget)
	INJ_I8(d, s->bTurningIncrement)
	INJ_U32(d, s->uiBattleSoundID)
	INJ_BOOL(d, s->fSoldierWasMoving)
	INJ_BOOL(d, s->fSayAmmoQuotePending)
	INJ_U16(d, s->usValueGoneUp)
	INJ_U8(d, s->ubNumLocateCycles)
	INJ_U8(d, s->ubDelayedMovementFlags)
	INJ_BOOL(d, s->fMuzzleFlash)
	INJ_SOLDIER(d, s->CTGTTarget)
	INJ_I32(d, s->PanelAnimateCounter)
	UINT32 const checksum = MercChecksum(*s);
	INJ_U32(d, checksum)
	INJ_I8(d, s->bCurrentCivQuote)
	INJ_I8(d, s->bCurrentCivQuoteDelta)
	INJ_U8(d, s->ubMiscSoldierFlags)
	INJ_U8(d, s->ubReasonCantFinishMove)
	INJ_I16(d, s->sLocationOfFadeStart)
	INJ_U8(d, s->bUseExitGridForReentryDirection)
	INJ_SKIP(d, 1)
	INJ_U32(d, s->uiTimeSinceLastSpoke)
	INJ_U8(d, s->ubContractRenewalQuoteCode)
	INJ_SKIP(d, 1)
	INJ_I16(d, s->sPreTraversalGridNo)
	INJ_U32(d, s->uiXRayActivatedTime)
	INJ_I8(d, s->bTurningFromUI)
	INJ_I8(d, s->bPendingActionData5)
	INJ_I8(d, s->bDelayedStrategicMoraleMod)
	INJ_U8(d, s->ubDoorOpeningNoise)
	INJ_SKIP(d, 4)
	INJ_U8(d, s->ubLeaveHistoryCode)
	INJ_BOOL(d, s->fDontUnsetLastTargetFromTurn)
	INJ_I8(d, s->bOverrideMoveSpeed)
	INJ_BOOL(d, s->fUseMoverrideMoveSpeed)
	INJ_U32(d, s->uiTimeSoldierWillArrive)
	INJ_SKIP(d, 1)
	INJ_BOOL(d, s->fUseLandingZoneForArrival)
	INJ_BOOL(d, s->fFallClockwise)
	INJ_I8(d, s->bVehicleUnderRepairID)
	INJ_I32(d, s->iTimeCanSignElsewhere)
	INJ_I8(d, s->bHospitalPriceModifier)
	INJ_SKIP(d, 3)
	INJ_U32(d, s->uiStartTimeOfInsuranceContract)
	INJ_BOOL(d, s->fRTInNonintAnim)
	INJ_BOOL(d, s->fDoingExternalDeath)
	INJ_I8(d, s->bCorpseQuoteTolerance)
	INJ_SKIP(d, 1)
	INJ_I32(d, s->iPositionSndID)
	INJ_U32(d, s->uiTuringSoundID)
	INJ_U8(d, s->ubLastDamageReason)
	INJ_BOOL(d, s->fComplainedThatTired)
	INJ_I16A(d, s->sLastTwoLocations, lengthof(s->sLastTwoLocations))
	INJ_SKIP(d, 2)
	INJ_I32(d, s->uiTimeSinceLastBleedGrunt)
	INJ_SOLDIER(d, s->next_to_previous_attacker)
	INJ_SKIP(d, 39)
	Assert(d.getConsumed() == 2328);
}
