// teamAI.cpp

#include "CvGameCoreDLL.h"
#include "CvTeamAI.h"
#include "CvPlayerAI.h"
#include "CvRandom.h"
#include "CvGlobals.h"
#include "CvGameCoreUtils.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvGameAI.h"
#include "CvInfos.h"
#include "FProfiler.h"
#include "CyArgsList.h"
#include "CvDLLPythonIFaceBase.h"

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      10/02/09                                jdog5000      */
/*                                                                                              */
/* AI logging                                                                                   */
/************************************************************************************************/
#include "BetterBTSAI.h"
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
#include "CvDLLFAStarIFaceBase.h" // K-Mod (currently used in AI_isLandTarget)
#include <numeric> // K-Mod. used in AI_warSpoilsValue
// statics

CvTeamAI* CvTeamAI::m_aTeams = NULL;

void CvTeamAI::initStatics()
{
	m_aTeams = new CvTeamAI[MAX_TEAMS];
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aTeams[iI].m_eID = ((TeamTypes)iI);
	}
}

void CvTeamAI::freeStatics()
{
	SAFE_DELETE_ARRAY(m_aTeams);
}

// inlined for performance reasons
DllExport CvTeamAI& CvTeamAI::getTeamNonInl(TeamTypes eTeam)
{
	return getTeam(eTeam);
}


// Public Functions...

CvTeamAI::CvTeamAI()
{
	m_aiWarPlanStateCounter = new int[MAX_TEAMS];
	m_aiAtWarCounter = new int[MAX_TEAMS];
	m_aiAtPeaceCounter = new int[MAX_TEAMS];
	m_aiHasMetCounter = new int[MAX_TEAMS];
	m_aiOpenBordersCounter = new int[MAX_TEAMS];
	m_aiDefensivePactCounter = new int[MAX_TEAMS];
	m_aiShareWarCounter = new int[MAX_TEAMS];
	m_aiWarSuccess = new int[MAX_TEAMS];
	m_aiEnemyPeacetimeTradeValue = new int[MAX_TEAMS];
	m_aiEnemyPeacetimeGrantValue = new int[MAX_TEAMS];
	m_aeWarPlan = new WarPlanTypes[MAX_TEAMS];


	AI_reset(true);
}


CvTeamAI::~CvTeamAI()
{
	AI_uninit();

	SAFE_DELETE_ARRAY(m_aiWarPlanStateCounter);
	SAFE_DELETE_ARRAY(m_aiAtWarCounter);
	SAFE_DELETE_ARRAY(m_aiAtPeaceCounter);
	SAFE_DELETE_ARRAY(m_aiHasMetCounter);
	SAFE_DELETE_ARRAY(m_aiOpenBordersCounter);
	SAFE_DELETE_ARRAY(m_aiDefensivePactCounter);
	SAFE_DELETE_ARRAY(m_aiShareWarCounter);
	SAFE_DELETE_ARRAY(m_aiWarSuccess);
	SAFE_DELETE_ARRAY(m_aiEnemyPeacetimeTradeValue);
	SAFE_DELETE_ARRAY(m_aiEnemyPeacetimeGrantValue);
	SAFE_DELETE_ARRAY(m_aeWarPlan);
}


void CvTeamAI::AI_init()
{
	AI_reset(false);

	//--------------------------------
	// Init other game data
}


void CvTeamAI::AI_uninit()
{
}


void CvTeamAI::AI_reset(bool bConstructor)
{
	AI_uninit();

	m_eWorstEnemy = NO_TEAM;

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aiWarPlanStateCounter[iI] = 0;
		m_aiAtWarCounter[iI] = 0;
		m_aiAtPeaceCounter[iI] = 0;
		m_aiHasMetCounter[iI] = 0;
		m_aiOpenBordersCounter[iI] = 0;
		m_aiDefensivePactCounter[iI] = 0;
		m_aiShareWarCounter[iI] = 0;
		m_aiWarSuccess[iI] = 0;
		m_aiEnemyPeacetimeTradeValue[iI] = 0;
		m_aiEnemyPeacetimeGrantValue[iI] = 0;
		m_aeWarPlan[iI] = NO_WARPLAN;

		if (!bConstructor && getID() != NO_TEAM)
		{
			TeamTypes eLoopTeam = (TeamTypes) iI;
			CvTeamAI& kLoopTeam = GET_TEAM(eLoopTeam);
			kLoopTeam.m_aiWarPlanStateCounter[getID()] = 0;
			kLoopTeam.m_aiAtWarCounter[getID()] = 0;
			kLoopTeam.m_aiAtPeaceCounter[getID()] = 0;
			kLoopTeam.m_aiHasMetCounter[getID()] = 0;
			kLoopTeam.m_aiOpenBordersCounter[getID()] = 0;
			kLoopTeam.m_aiDefensivePactCounter[getID()] = 0;
			kLoopTeam.m_aiShareWarCounter[getID()] = 0;
			kLoopTeam.m_aiWarSuccess[getID()] = 0;
			kLoopTeam.m_aiEnemyPeacetimeTradeValue[getID()] = 0;
			kLoopTeam.m_aiEnemyPeacetimeGrantValue[getID()] = 0;
			kLoopTeam.m_aeWarPlan[getID()] = NO_WARPLAN;
		}
	}
}


void CvTeamAI::AI_doTurnPre()
{
	AI_doCounter();

	if (isHuman())
	{
		return;
	}

	if (isBarbarian())
	{
		return;
	}

	if (isMinorCiv())
	{
		return;
	}
}


void CvTeamAI::AI_doTurnPost()
{
	AI_updateWorstEnemy();

	AI_updateAreaStragies(false);

	if (isHuman())
	{
		return;
	}

	if (isBarbarian())
	{
		return;
	}

	if (isMinorCiv())
	{
		return;
	}

	AI_doWar();
}


void CvTeamAI::AI_makeAssignWorkDirty()
{
	int iI;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				GET_PLAYER((PlayerTypes)iI).AI_makeAssignWorkDirty();
			}
		}
	}
}

/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						10/6/08				jdog5000	*/
/* 																			*/
/* 	General AI																*/
/********************************************************************************/
// Find plot strength of teammates and potentially vassals
int CvTeamAI::AI_getOurPlotStrength(CvPlot* pPlot, int iRange, bool bDefensiveBonuses, bool bTestMoves, bool bIncludeVassals)
{
	int iI;
	int iPlotStrength = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID() || (bIncludeVassals && GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isVassal(getID())) )
			{
				iPlotStrength += GET_PLAYER((PlayerTypes)iI).AI_getOurPlotStrength(pPlot,iRange,bDefensiveBonuses,bTestMoves);
			}
		}
	}

	return iPlotStrength;
}
/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						END								*/
/********************************************************************************/


void CvTeamAI::AI_updateAreaStragies(bool bTargets)
{
	CvArea* pLoopArea;
	int iLoop;

	if (!(GC.getGameINLINE().isFinalInitialized()))
	{
		return;
	}

	for(pLoopArea = GC.getMapINLINE().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMapINLINE().nextArea(&iLoop))
	{
		pLoopArea->setAreaAIType(getID(), AI_calculateAreaAIType(pLoopArea));
	}

	if (bTargets)
	{
		AI_updateAreaTargets();
	}
}


void CvTeamAI::AI_updateAreaTargets()
{
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.isAlive())
		{
			if (kLoopPlayer.getTeam() == getID())
			{
				kLoopPlayer.AI_updateAreaTargets();
			}
		}
	}
}


int CvTeamAI::AI_countFinancialTrouble() const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				if (GET_PLAYER((PlayerTypes)iI).AI_isFinancialTrouble())
				{
					iCount++;
				}
			}
		}
	}

	return iCount;
}


int CvTeamAI::AI_countMilitaryWeight(CvArea* pArea) const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iCount += GET_PLAYER((PlayerTypes)iI).AI_militaryWeight(pArea);
			}
		}
	}

	return iCount;
}

// K-Mod. return true if is fair enough for the AI to know there is a city here
bool CvTeamAI::AI_deduceCitySite(const CvCity* pCity) const
{
	PROFILE_FUNC();

	if (pCity->isRevealed(getID(), false))
		return true;

	// The rule is this:
	// if we can see more than n plots of the nth culture ring, we can deduce where the city is.

	int iPoints = 0;
	int iLevel = pCity->getCultureLevel();

	for (int iDX = -iLevel; iDX <= iLevel; iDX++)
	{
		for (int iDY = -iLevel; iDY <= iLevel; iDY++)
		{
			int iDist = pCity->cultureDistance(iDX, iDY);
			if (iDist > iLevel)
				continue;

			CvPlot* pLoopPlot = plotXY(pCity->getX_INLINE(), pCity->getY_INLINE(), iDX, iDY);

			if (pLoopPlot && pLoopPlot->getRevealedOwner(getID(), false) == pCity->getOwnerINLINE())
			{
				// if multiple cities have their plot in their range, then that will make it harder to deduce the precise city location.
				iPoints += 1 + std::max(0, iLevel - iDist - pLoopPlot->getNumCultureRangeCities(pCity->getOwnerINLINE())+1);

				if (iPoints > iLevel)
					return true;
			}
		}
	}
	return false;
}
// K-Mod end

bool CvTeamAI::AI_isAnyCapitalAreaAlone() const
{
	int iI;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				if (GET_PLAYER((PlayerTypes)iI).AI_isCapitalAreaAlone())
				{
					return true;
				}
			}
		}
	}

	return false;
}


bool CvTeamAI::AI_isPrimaryArea(CvArea* pArea) const
{
	int iI;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				if (GET_PLAYER((PlayerTypes)iI).AI_isPrimaryArea(pArea))
				{
					return true;
				}
			}
		}
	}

	return false;
}


bool CvTeamAI::AI_hasCitiesInPrimaryArea(TeamTypes eTeam) const
{
	CvArea* pLoopArea;
	int iLoop;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	for(pLoopArea = GC.getMapINLINE().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMapINLINE().nextArea(&iLoop))
	{
		if (AI_isPrimaryArea(pLoopArea))
		{
			if (GET_TEAM(eTeam).countNumCitiesByArea(pLoopArea))
			{
				return true;
			}
		}
	}

	return false;
}

// K-Mod. Return true if this team and eTeam have at least one primary area in common.
bool CvTeamAI::AI_hasSharedPrimaryArea(TeamTypes eTeam) const
{
	FAssert(eTeam != getID());

	const CvTeamAI& kTeam = GET_TEAM(eTeam);

	int iLoop;
	for(CvArea* pLoopArea = GC.getMapINLINE().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMapINLINE().nextArea(&iLoop))
	{
		if (AI_isPrimaryArea(pLoopArea) && kTeam.AI_isPrimaryArea(pLoopArea))
			return true;
	}
	return false;
}
// K-Mod end

AreaAITypes CvTeamAI::AI_calculateAreaAIType(CvArea* pArea, bool bPreparingTotal) const
{
	PROFILE_FUNC();

	// K-Mod. This function originally had "!isWater()" wrapping all of the code.
	// I've changed it to be more readable.
	if (pArea->isWater())
	{
		return AREAAI_NEUTRAL;
	}

	if (isBarbarian())
	{
		if ((pArea->getNumCities() - pArea->getCitiesPerPlayer(BARBARIAN_PLAYER)) == 0)
		{
			return AREAAI_ASSAULT;
		}

		if ((countNumAIUnitsByArea(pArea, UNITAI_ATTACK) + countNumAIUnitsByArea(pArea, UNITAI_ATTACK_CITY) + countNumAIUnitsByArea(pArea, UNITAI_PILLAGE) + countNumAIUnitsByArea(pArea, UNITAI_ATTACK_AIR)) > (((AI_countMilitaryWeight(pArea) * 20) / 100) + 1))
		{
			return AREAAI_OFFENSIVE; // XXX does this ever happen?
		}

		return AREAAI_MASSING;
	}

	bool bRecentAttack = false;
	bool bTargets = false;
	bool bChosenTargets = false;
	bool bDeclaredTargets = false;

	bool bAssault = false;
	bool bPreparingAssault = false;

	// int iOffensiveThreshold = (bPreparingTotal ? 25 : 20); // K-Mod, I don't use this.
	int iAreaCities = countNumCitiesByArea(pArea);
	int iWarSuccessRating = AI_getWarSuccessRating(); // K-Mod

	for (int iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
/********************************************************************************/
/**		REVOLUTION_MOD							6/23/08				jdog5000	*/
/**																				*/
/**		Revolution AI, minor civs												*/
/********************************************************************************/
			// Minor civs will declare war on all, but won't get has met set so that they don't show in score board
			//if (AI_getWarPlan((TeamTypes)iI) != NO_WARPLAN)
			if (AI_getWarPlan((TeamTypes)iI) != NO_WARPLAN && isHasMet((TeamTypes)iI))
			{
/********************************************************************************/
/**		REVOLUTION_MOD							END								*/
/********************************************************************************/
				FAssert(((TeamTypes)iI) != getID());
				FAssert(isHasMet((TeamTypes)iI) || GC.getGameINLINE().isOption(GAMEOPTION_ALWAYS_WAR));

				if (AI_getWarPlan((TeamTypes)iI) == WARPLAN_ATTACKED_RECENT)
				{
					FAssert(isAtWar((TeamTypes)iI));
					bRecentAttack = true;
				}

				if ((GET_TEAM((TeamTypes)iI).countNumCitiesByArea(pArea) > 0) || (GET_TEAM((TeamTypes)iI).countNumUnitsByArea(pArea) > 4))
				{
					bTargets = true;

					if (AI_isChosenWar((TeamTypes)iI))
					{
						bChosenTargets = true;

						if ((isAtWar((TeamTypes)iI)) ? (AI_getAtWarCounter((TeamTypes)iI) < 10) : AI_isSneakAttackReady((TeamTypes)iI))
						{
							bDeclaredTargets = true;
						}
					}
				}
				else
				{
                    bAssault = true;
                    if (AI_isSneakAttackPreparing((TeamTypes)iI))
                    {
                        bPreparingAssault = true;
                    }
				}
			}
		}
	}
    
	// K-Mod - based on idea from BBAI
	if( bTargets )
	{
		if(iAreaCities > 0 && getAtWarCount(true) > 0) 
		{
			int iPower = countPowerByArea(pArea);
			int iEnemyPower = countEnemyPowerByArea(pArea);
			
			iPower *= 100 + iWarSuccessRating + (bChosenTargets ? 100 : 50);
			iEnemyPower *= 100;
			// it would be nice to put some personality modifiers into this. But this is a Team function. :(
			if (iPower < iEnemyPower)
			{
				return AREAAI_DEFENSIVE;
			}
		}
	}
	// K-Mod end

	if (bDeclaredTargets)
	{
		return AREAAI_OFFENSIVE;
	}

	if (bTargets)
	{
		/* BBAI code. -- This code has two major problems.
		* Firstly, it makes offense more likely when we are in more wars.
		* Secondly, it chooses offense based on how many offense units we have -- but offense units are built for offense areas!
		*
		// AI_countMilitaryWeight is based on this team's pop and cities ... if this team is the biggest, it will over estimate needed units
		int iMilitaryWeight = AI_countMilitaryWeight(pArea);
		int iCount = 1;

		for( int iJ = 0; iJ < MAX_CIV_TEAMS; iJ++ )
		{
			if( iJ != getID() && GET_TEAM((TeamTypes)iJ).isAlive() )
			{
				if( !(GET_TEAM((TeamTypes)iJ).isBarbarian() || GET_TEAM((TeamTypes)iJ).isMinorCiv()) )
				{
					if( AI_getWarPlan((TeamTypes)iJ) != NO_WARPLAN )
					{
						iMilitaryWeight += GET_TEAM((TeamTypes)iJ).AI_countMilitaryWeight(pArea);
						iCount++;

						if( GET_TEAM((TeamTypes)iJ).isAVassal() )
						{
							for( int iK = 0; iK < MAX_CIV_TEAMS; iK++ )
							{
								if( iK != getID() && GET_TEAM((TeamTypes)iK).isAlive() )
								{
									if( GET_TEAM((TeamTypes)iJ).isVassal((TeamTypes)iK) )
									{
										iMilitaryWeight += GET_TEAM((TeamTypes)iK).AI_countMilitaryWeight(pArea);
									}
								}
							}
						}
					}
				}
			}
		}

		iMilitaryWeight /= iCount;
		if ((countNumAIUnitsByArea(pArea, UNITAI_ATTACK) + countNumAIUnitsByArea(pArea, UNITAI_ATTACK_CITY) + countNumAIUnitsByArea(pArea, UNITAI_PILLAGE) + countNumAIUnitsByArea(pArea, UNITAI_ATTACK_AIR)) > (((iMilitaryWeight * iOffensiveThreshold) / 100) + 1))
		{
			return AREAAI_OFFENSIVE;
		}
		*/
		// K-Mod. I'm not sure how best to do this yet. Let me just try a rough idea for now.
		// I'm using AI_countMilitaryWeight; but what I really want is "border terriory which needs defending"
		int iOurRelativeStrength = 100 * countPowerByArea(pArea) / (AI_countMilitaryWeight(pArea) + 20);
		iOurRelativeStrength *= 100 + (bDeclaredTargets ? 30 : 0) + (bPreparingTotal ? -20 : 0) + iWarSuccessRating/2;
		iOurRelativeStrength /= 100;
		int iEnemyRelativeStrength = 0;
		bool bEnemyCities = false;

		for (int iJ = 0; iJ < MAX_CIV_TEAMS; iJ++)
		{
			const CvTeamAI& kLoopTeam = GET_TEAM((TeamTypes)iJ);
			if (iJ != getID() && kLoopTeam.isAlive() && AI_getWarPlan((TeamTypes)iJ) != NO_WARPLAN)
			{
				int iPower = 100 * kLoopTeam.countPowerByArea(pArea);
				int iCommitment = (bPreparingTotal ? 30 : 20) + kLoopTeam.AI_countMilitaryWeight(pArea) * ((isAtWar((TeamTypes)iJ) ? 1 : 2) + kLoopTeam.getAtWarCount(true, true)) / 2;
				iPower /= iCommitment;
				iEnemyRelativeStrength += iPower;
				if (kLoopTeam.countNumCitiesByArea(pArea) > 0)
					bEnemyCities = true;
			}
		}
		if (bEnemyCities && iOurRelativeStrength > iEnemyRelativeStrength)
			return AREAAI_OFFENSIVE;
		// K-Mod end
	}

	if (bTargets)
	{
		for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; iPlayer++)
		{
			CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
			
			if (kPlayer.isAlive())
			{
				if (kPlayer.getTeam() == getID())
				{
					if (kPlayer.AI_isDoStrategy(AI_STRATEGY_DAGGER) || kPlayer.AI_isDoStrategy(AI_STRATEGY_FINAL_WAR))
					{
						if (pArea->getCitiesPerPlayer((PlayerTypes)iPlayer) > 0)
						{
							return AREAAI_MASSING;
						}
					}
				}
			}
		}
		if (bRecentAttack)
		{
			int iPower = countPowerByArea(pArea);
			int iEnemyPower = countEnemyPowerByArea(pArea);
			if (iPower > iEnemyPower)
			{
				return AREAAI_MASSING;
			}
			return AREAAI_DEFENSIVE;
		}
	}

	if (iAreaCities > 0)
	{
		if (countEnemyDangerByArea(pArea) > (iAreaCities * 2))
		{
			return AREAAI_DEFENSIVE;
		}
	}

	if (bChosenTargets)
	{
		return AREAAI_MASSING;
	}

	if (bTargets)
	{
		if (iAreaCities > (getNumMembers() * 3))
		{
			if (GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI) || GC.getGameINLINE().isOption(GAMEOPTION_ALWAYS_WAR) || (countPowerByArea(pArea) > ((countEnemyPowerByArea(pArea) * 3) / 2)))
			{
				return AREAAI_MASSING;
			}
		}
		//return AREAAI_DEFENSIVE;
	}
	else
	{
		if (bAssault)
		{
			if (AI_isPrimaryArea(pArea))
			{
                if (bPreparingAssault)
				{
					return AREAAI_ASSAULT_MASSING;
				}
			}
			else if (countNumCitiesByArea(pArea) > 0)
			{
				return AREAAI_ASSAULT_ASSIST;
			}

			return AREAAI_ASSAULT;
		}
	}
	return AREAAI_NEUTRAL;
}

int CvTeamAI::AI_calculateAdjacentLandPlots(TeamTypes eTeam) const
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	int iCount;
	int iI;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	iCount = 0;

	for (iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		if (!(pLoopPlot->isWater()))
		{
			if ((pLoopPlot->getTeam() == eTeam) && pLoopPlot->isAdjacentTeam(getID(), true))
			{
				iCount++;
			}
		}
	}

	return iCount;
}


int CvTeamAI::AI_calculatePlotWarValue(TeamTypes eTeam) const
{
	FAssert(eTeam != getID());

	int iValue = 0;
	if( gTeamLogLevel >= 4 ) logBBAI("     Calculating Plot War Value...");

	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		if (pLoopPlot->getTeam() == eTeam)
		{
			if (!pLoopPlot->isWater() && pLoopPlot->isAdjacentTeam(getID(), true))
			{
				iValue += 4;
			}

			if (pLoopPlot->isRevealed(getID(), false))
			{
				BonusTypes eBonus = pLoopPlot->getBonusType(getID());
				if (NO_BONUS != eBonus)
				{
					if (GC.getBonusInfo(eBonus).getTechReveal() != NO_TECH)
					{
						if (!isHasTech((TechTypes)(GC.getBonusInfo(eBonus).getTechReveal())))
						{
							continue;
						}
					}
					else
					{
						for ( int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; iPlayer++ )
						{
							CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
							if ( kPlayer.getTeam() == getID() && kPlayer.isAlive())
							{
								// add value for bonuses we dont have
								if (kPlayer.countOwnedBonuses(eBonus) == 0)
								{
									int iTempBonusValue = 0;
									iTempBonusValue += (kPlayer.AI_bonusVal(eBonus) / 5);
									iTempBonusValue += (pLoopPlot->getCulture((PlayerTypes)iPlayer) / 5);
									if( gTeamLogLevel >= 4 )  logBBAI("      Coveting their Bonus %S (Value: %d)", GC.getBonusInfo(eBonus).getDescription(), iTempBonusValue);
									iValue += iTempBonusValue;
								}
							}
						}
					}
				}
			}
		}
	}

	return iValue;
}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      07/21/08                                jdog5000      */
/*                                                                                              */
/* War Strategy AI                                                                              */
/************************************************************************************************/
int CvTeamAI::AI_calculateBonusWarValue(TeamTypes eTeam) const
{
	FAssert(eTeam != getID());

	int iValue = 0;

	// Tholal AI - add value for previously owned cities
	for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
	{
		if (GET_PLAYER((PlayerTypes)iJ).getTeam() == eTeam)
		{
			CvCity* pLoopCity;
			int iLoop;
			for (pLoopCity = GET_PLAYER((PlayerTypes)iJ).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iJ).nextCity(&iLoop))
			{
				if (pLoopCity->getOriginalOwner() != iJ)
				{
					for (int iK = 0; iK < MAX_PLAYERS; iK++)
					{
						if (GET_PLAYER((PlayerTypes)iK).getTeam() == getID())
						{
							if (pLoopCity->getOriginalOwner() == iK)
							{
								iValue += pLoopCity->getPopulation() * 5;
							}
						}
					}
				}

			}
		}
	}

	/*
	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		if (pLoopPlot->getTeam() == eTeam)
		{
			BonusTypes eNonObsoleteBonus = pLoopPlot->getNonObsoleteBonusType(getID());
			if (NO_BONUS != eNonObsoleteBonus)
			{
				int iThisValue = 0;
				for( int iJ = 0; iJ < MAX_CIV_PLAYERS; iJ++ )
				{
					if( getID() == GET_PLAYER((PlayerTypes)iJ).getTeam() && GET_PLAYER((PlayerTypes)iJ).isAlive() )
					{
						// 10 seems like a typical value for a health/happiness resource the AI doesn't have
						// Values for strategic resources can be 60 or higher
						iThisValue += GET_PLAYER((PlayerTypes)iJ).AI_bonusVal(eNonObsoleteBonus);
					}
				}
				iThisValue /= getAliveCount();

				if (!pLoopPlot->isWater())
				{
					if( pLoopPlot->isAdjacentTeam(getID(), true))
					{
						iThisValue *= 3;
					}
					else
					{
						CvCity* pWorkingCity = pLoopPlot->getWorkingCity();
						if( pWorkingCity != NULL )
						{
							for( int iJ = 0; iJ < MAX_CIV_PLAYERS; iJ++ )
							{
								if( getID() == GET_PLAYER((PlayerTypes)iJ).getTeam() && GET_PLAYER((PlayerTypes)iJ).isAlive() )
								{
									if( pWorkingCity->AI_playerCloseness((PlayerTypes)iJ ) > 0 )
									{
										iThisValue *= 2;
										break;
									}
								}
							}
						}
					}
				}

				iThisValue = std::max(0, iThisValue - 4);
				iThisValue /= 5;

				iValue += iThisValue;
			}
		}
	}
	*/

	return iValue;
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

int CvTeamAI::AI_calculateCapitalProximity(TeamTypes eTeam) const
{
	CvCity* pOurCapitalCity;
	CvCity* pTheirCapitalCity;
	int iTotalDistance;
	int iCount;
	int iI, iJ;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	iTotalDistance = 0;
	iCount = 0;
	
	int iMinDistance = MAX_INT;
	int iMaxDistance = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				pOurCapitalCity = GET_PLAYER((PlayerTypes)iI).getCapitalCity();

				if (pOurCapitalCity != NULL)
				{
					for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
					{
						if (GET_PLAYER((PlayerTypes)iJ).isAlive())
						{
							if (GET_PLAYER((PlayerTypes)iJ).getTeam() != getID())
							{
								pTheirCapitalCity = GET_PLAYER((PlayerTypes)iJ).getCapitalCity();

								if (pTheirCapitalCity != NULL)
								{
									int iDistance = (plotDistance(pOurCapitalCity->getX_INLINE(), pOurCapitalCity->getY_INLINE(), pTheirCapitalCity->getX_INLINE(), pTheirCapitalCity->getY_INLINE()) * (pOurCapitalCity->area() != pTheirCapitalCity->area() ? 3 : 2));
									if (GET_PLAYER((PlayerTypes)iJ).getTeam() == eTeam)
									{
										iTotalDistance += iDistance;
										iCount++;
									}
									iMinDistance = std::min(iDistance, iMinDistance);
									iMaxDistance = std::max(iDistance, iMaxDistance);
								}
							}
						}
					}
				}
			}
		}
	}
	
	if (iCount > 0)
	{
		FAssert(iMaxDistance > 0);
		return ((GC.getMapINLINE().maxPlotDistance() * (iMaxDistance - ((iTotalDistance / iCount) - iMinDistance))) / iMaxDistance);
	}

	return 0;
}

// K-Mod. Return true if we can deduce the location of at least 'iMiniumum' cities belonging to eTeam.
bool CvTeamAI::AI_haveSeenCities(TeamTypes eTeam, bool bPrimaryAreaOnly, int iMinimum) const
{
	int iCount = 0;
	for (PlayerTypes eLoopPlayer = (PlayerTypes)0; eLoopPlayer < MAX_PLAYERS; eLoopPlayer=(PlayerTypes)(eLoopPlayer+1))
	{
		const CvPlayerAI& kLoopPlayer = GET_PLAYER(eLoopPlayer);
		if (kLoopPlayer.getTeam() == eTeam)
		{
			int iLoop;
			for (CvCity* pLoopCity = kLoopPlayer.firstCity(&iLoop); pLoopCity; pLoopCity = kLoopPlayer.nextCity(&iLoop))
			{
				if (AI_deduceCitySite(pLoopCity))
				{
					if (!bPrimaryAreaOnly || AI_isPrimaryArea(pLoopCity->area()))
					{
						if (++iCount >= iMinimum)
							return true;
					}
				}
			}
		}
	}
	return false;
}
// K-Mod end

bool CvTeamAI::AI_isWarPossible() const
{
	if (getAtWarCount(false) > 0)
	{
		return true;
	}

	if (GC.getGameINLINE().isOption(GAMEOPTION_ALWAYS_WAR))
	{
		return true;
	}

	if (!(GC.getGameINLINE().isOption(GAMEOPTION_ALWAYS_PEACE)) && !(GC.getGameINLINE().isOption(GAMEOPTION_NO_CHANGING_WAR_PEACE)))
	{
		return true;
	}

	return false;
}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      06/12/10                         Fuyu & jdog5000      */
/*                                                                                              */
/* War Strategy AI                                                                              */
/************************************************************************************************/
bool CvTeamAI::AI_isLandTarget(TeamTypes eTeam, bool bNeighborsOnly) const
{
	if (!AI_hasCitiesInPrimaryArea(eTeam))
	{
		return false;
	}

	// If shared capital area is largely unclaimed, then we can reach over land
	int iModifier = 100;

	if( !bNeighborsOnly )
	{
		for( int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; iPlayer++ )
		{
			if( GET_PLAYER((PlayerTypes)iPlayer).getTeam() == getID() && GET_PLAYER((PlayerTypes)iPlayer).getNumCities() > 0 )
			{
				CvCity* pCapital = GET_PLAYER((PlayerTypes)iPlayer).getCapitalCity();
				if( pCapital != NULL )
				{
					if( GET_TEAM(eTeam).AI_isPrimaryArea(pCapital->area()) )
					{
						iModifier *= pCapital->area()->getNumOwnedTiles();
						iModifier /= pCapital->area()->getNumTiles();
					}
				}
			}
		}
	}

	if (AI_calculateAdjacentLandPlots(eTeam) < range((8 * (iModifier - 40))/40, 0, 8))
	{
		return false;
	}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	return true;
}

// this determines if eTeam or any of its allies are land targets of us
bool CvTeamAI::AI_isAllyLandTarget(TeamTypes eTeam) const
{
	for (int iTeam = 0; iTeam < MAX_CIV_TEAMS; iTeam++)
	{
		CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iTeam);
		if (iTeam != getID())
		{
			if (iTeam == eTeam || kLoopTeam.isVassal(eTeam) || GET_TEAM(eTeam).isVassal((TeamTypes)iTeam) || kLoopTeam.isDefensivePact(eTeam))
			{
				if (AI_isLandTarget((TeamTypes)iTeam))
				{
					return true;
				}
			}
		}
	}

	return false;
}


bool CvTeamAI::AI_shareWar(TeamTypes eTeam) const
{
	int iI;

/************************************************************************************************/
/* REVOLUTION_MOD                         10/25/08                                jdog5000      */
/*                                                                                              */
/* For minor civs, StartAsMinors                                                                */
/************************************************************************************************/
	/* original BTS code
	for (iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && !GET_TEAM((TeamTypes)iI).isMinorCiv())
		{
			if ((iI != getID()) && (iI != eTeam))
			{
				if (isAtWar((TeamTypes)iI) && GET_TEAM(eTeam).isAtWar((TeamTypes)iI))
				{
					return true;
				}
			}
		}
	}
	*/
	// No dealing with minor civs
	if( isMinorCiv() || GET_TEAM(eTeam).isMinorCiv() )
	{
		return false;
	}

	// Only accumulate if someone actually declared war, not a left over from StartAsMinors
	for (iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && !GET_TEAM((TeamTypes)iI).isMinorCiv())
		{
			if ((iI != getID()) && (iI != eTeam))
			{
				if (isAtWar((TeamTypes)iI) && GET_TEAM(eTeam).isAtWar((TeamTypes)iI))
				{
					//if( AI_getWarPlan((TeamTypes)iI) != WARPLAN_LIMITED || GET_TEAM(eTeam).AI_getWarPlan((TeamTypes)iI) != WARPLAN_LIMITED || GET_TEAM((TeamTypes)iI).AI_getWarPlan(getID()) != WARPLAN_LIMITED || GET_TEAM((TeamTypes)iI).AI_getWarPlan(eTeam) != WARPLAN_LIMITED )
					{
						return true;
					}
				}
			}
		}
	}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/

	return false;
}


AttitudeTypes CvTeamAI::AI_getAttitude(TeamTypes eTeam, bool bForced) const
{
	int iAttitude;
	int iCount;
	int iI, iJ;

	//FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	// K-Mod
	if (eTeam == getID())
		return ATTITUDE_FRIENDLY;
	// K-Mod end

	iAttitude = 0;
	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive() && iI != iJ)
					{
						TeamTypes eTeamLoop = GET_PLAYER((PlayerTypes)iJ).getTeam();

						//if (eTeamLoop == eTeam || GET_TEAM(eTeamLoop).isVassal(eTeam) || GET_TEAM(eTeam).isVassal(eTeamLoop))
						if (eTeamLoop == eTeam) // K-Mod. Removed attitude averaging between vassals and masters
						{
							//iAttitude += GET_PLAYER((PlayerTypes)iI).AI_getAttitude((PlayerTypes)iJ, bForced);
							iAttitude += GET_PLAYER((PlayerTypes)iI).AI_getAttitudeVal((PlayerTypes)iJ, bForced); // bbai. Average values rather than attitudes directly.
							iCount++;
						}
					}
				}
			}
		}
	}

	if (iCount > 0)
	{
		// return ((AttitudeTypes)(iAttitude / iCount));
		return CvPlayerAI::AI_getAttitudeFromValue(iAttitude/iCount); // bbai / K-Mod
	}

	return ATTITUDE_CAUTIOUS;
}


int CvTeamAI::AI_getAttitudeVal(TeamTypes eTeam, bool bForced) const
{
	int iAttitudeVal;
	int iCount;
	int iI, iJ;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	iAttitudeVal = 0;
	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive())
					{
						if (GET_PLAYER((PlayerTypes)iJ).getTeam() == eTeam)
						{
							iAttitudeVal += GET_PLAYER((PlayerTypes)iI).AI_getAttitudeVal((PlayerTypes)iJ, bForced);
							iCount++;
						}
					}
				}
			}
		}
	}

	if (iCount > 0)
	{
		return (iAttitudeVal / iCount);
	}

	return 0;
}


int CvTeamAI::AI_getMemoryCount(TeamTypes eTeam, MemoryTypes eMemory) const
{
	int iMemoryCount;
	int iCount;
	int iI, iJ;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	iMemoryCount = 0;
	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive())
					{
						if (GET_PLAYER((PlayerTypes)iJ).getTeam() == eTeam)
						{
							iMemoryCount += GET_PLAYER((PlayerTypes)iI).AI_getMemoryCount(((PlayerTypes)iJ), eMemory);
							iCount++;
						}
					}
				}
			}
		}
	}

	if (iCount > 0)
	{
		return (iMemoryCount / iCount);
	}

	return 0;
}


int CvTeamAI::AI_chooseElection(const VoteSelectionData& kVoteSelectionData) const
{
	VoteSourceTypes eVoteSource = kVoteSelectionData.eVoteSource;

	FAssert(!isHuman());
	FAssert(GC.getGameINLINE().getSecretaryGeneral(eVoteSource) == getID());

	int iBestVote = -1;
	int iBestValue = 0;

	for (int iI = 0; iI < (int)kVoteSelectionData.aVoteOptions.size(); ++iI)
	{
		VoteTypes eVote = kVoteSelectionData.aVoteOptions[iI].eVote;
		CvVoteInfo& kVoteInfo = GC.getVoteInfo(eVote);

		FAssert(kVoteInfo.isVoteSourceType(eVoteSource));

		FAssert(GC.getGameINLINE().isChooseElection(eVote));
		bool bValid = true;

		if (!GC.getGameINLINE().isTeamVote(eVote))
		{
			for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
			{
				if (GET_PLAYER((PlayerTypes)iJ).isAlive())
				{
					if (GET_PLAYER((PlayerTypes)iJ).getTeam() == getID())
					{
						PlayerVoteTypes eVote = GET_PLAYER((PlayerTypes)iJ).AI_diploVote(kVoteSelectionData.aVoteOptions[iI], eVoteSource, true);

						if (eVote != PLAYER_VOTE_YES || eVote == GC.getGameINLINE().getVoteOutcome((VoteTypes)iI))
						{
							bValid = false;
							break;
						}
					}
				}
			}
		}

		if (bValid)
		{
			int iValue = (1 + GC.getGameINLINE().getSorenRandNum(10000, "AI Choose Vote"));

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				iBestVote = iI;
			}
		}
	}

	return iBestVote;
}

/// \brief Relative value of starting a war against eTeam.
///
/// This function computes the value of starting a war against eTeam.
/// The returned value should be compared against other possible targets
/// to pick the best target.
int CvTeamAI::AI_startWarVal(TeamTypes eTeam) const
{
	PROFILE_FUNC();

	int iValue;
	CvTeamAI& kTeam = GET_TEAM(eTeam);
	bool bAggressiveAI = GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI);
	
	if( gTeamLogLevel >= 4 ) logBBAI("    Valuing War with Team %d" ,eTeam);

	iValue = AI_calculatePlotWarValue(eTeam);
	if( gTeamLogLevel >= 4 )logBBAI("     Initial Value: %d", iValue);

	iValue += (3 * AI_calculateCapitalProximity(eTeam)) / ((iValue > 0) ? 2 : 3);
	if( gTeamLogLevel >= 4 )logBBAI("     Plus Capital Proximity: %d", iValue);

	int iClosenessValue = AI_teamCloseness(eTeam);

	if (iClosenessValue == 0)
	{
		iValue /= (bAggressiveAI ? 2 : 4);
	}
	else
	{
		iValue += iClosenessValue;
	}
	if( gTeamLogLevel >= 4 ) logBBAI("     After Closeness Modifier: %d", iValue);

	iValue += AI_calculateBonusWarValue(eTeam);
	if( gTeamLogLevel >= 4 ) logBBAI("     After Bonus War Value: %d", iValue);

	// Target other teams close to victory
	if( kTeam.AI_isAnyMemberDoVictoryStrategyLevel3() )
	{
		iValue += 10;

		bool bConq4 = AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_CONQUEST4);

		// Prioritize targets closer to victory
		if( bConq4 || bAggressiveAI )
		{
			iValue *= 3;
			iValue /= 2;
		}

		if( kTeam.AI_isAnyMemberDoVictoryStrategyLevel4() )
		{
			if( kTeam.AI_getLowestVictoryCountdown() >= 0 )
			{
				iValue += 50;
			}

			iValue *= ((bConq4 || bAggressiveAI)? 4 : 2);
		}	
	}

	// This adapted legacy code just makes us more willing to enter a war in a trade deal 
	// as boost applies to all rivals
	if( AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_DOMINATION3) )
	{
		iValue *= (bAggressiveAI ? 3 : 2);
	}

	// boost for declaring war on other religious leaders when pursuing a religious victory
	if (AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_RELIGION3))
	{
		if (kTeam.AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_RELIGION1))
		{
			iValue *= 2;
		}
	}
	
	// assaults on weak opponents
	int iEnemyPowerPercent = kTeam.AI_getEnemyPowerPercent(true);
	if (iEnemyPowerPercent < 65)
	{
		if (bAggressiveAI)
		{
			int iModValue = 0;

			// extra value for targeting neighbors
			if (AI_calculateAdjacentLandPlots(eTeam) > (getNumCities() * 2))
			{
				iModValue += 2;
			}

			iModValue += ((iEnemyPowerPercent < 50) ? 8 : 5);
			iModValue *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIDeclareWarProb();
			iModValue /= 100;

			iValue *= iModValue;
		}
	}

	if (!bAggressiveAI)
	{
		switch (AI_getAttitude(eTeam))
		{
		case ATTITUDE_FURIOUS:
			iValue *= 4;
			break;

		case ATTITUDE_ANNOYED:
			iValue *= 2;
			break;

		case ATTITUDE_CAUTIOUS:
			iValue *= 1;
			break;

		case ATTITUDE_PLEASED:
			iValue /= 5;
			break;

		case ATTITUDE_FRIENDLY:
			iValue /= 10;
			break;

		default:
			FAssert(false);
			break;
		}
	}
	
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      03/21/10                                jdog5000      */
/*                                                                                              */
/* Victory Strategy AI                                                                          */
/************************************************************************************************/
	// avoid wars if pursuing a more peaceful type of victory
	if ( AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_CULTURE4))
	{
		iValue /= 8;
	}
	else if ( AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_SPACE4))
	{
		iValue /= 4;
	}
	else if ( AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_ALTAR3))
	{
		iValue /= 4;
	}
	else if ( AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_TOWERMASTERY3))
	{
		iValue /= 4;
	}
	else if ( AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_TOWERMASTERY2))
	{
		iValue /= 3;
	}
	else if ( AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_CULTURE3))
	{
		iValue /= 3;
	}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	if (AI_calculateAdjacentLandPlots(eTeam) == 0)
	{
		iValue /= 2;
	}

	if (getPower(true) < 100)
	{
		iValue /= 2;
	}

	// MNAI ToDo - devalue based on distance
	return iValue;
}


// XXX this should consider area power...
int CvTeamAI::AI_endWarVal(TeamTypes eTeam) const
{
	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(isAtWar(eTeam), "Current AI Team instance is expected to be at war with eTeam");

	const CvTeam& kWarTeam = GET_TEAM(eTeam); // K-Mod

	int iValue = 100;

	iValue += (getNumCities() * 3);
	iValue += (kWarTeam.getNumCities() * 3);

	iValue += getTotalPopulation();
	iValue += kWarTeam.getTotalPopulation();

	iValue += kWarTeam.AI_getWarSuccess(getID()); //* 30);
	iValue -= AI_getWarSuccess(eTeam);// * 30;

	int iOurPower = std::max(1, getPower(true));
	int iTheirPower = std::max(1, kWarTeam.getDefensivePower());

	// MNAI To Do - increase value for distant opponents
	// MNAI - add value based on the duration of the war
	int iDurationMod = AI_getAtWarCounter(eTeam);// - ((AI_getWarPlan(eTeam) == WARPLAN_TOTAL) ? 40 : 30) * 3);
	iDurationMod *=  GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getVictoryDelayPercent();
	iDurationMod /= 100;
	iValue += iDurationMod;

	iValue += kWarTeam.getWarWeariness(eTeam);

	//todo - check countNumImprovedPlots for enemy cities - if they are all pillaged, its a sign that we're not making headway in the war
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const CvPlayerAI& kLoopPlayer = GET_PLAYER((PlayerTypes)iI); // K-Mod
		if (kLoopPlayer.isAlive() && kLoopPlayer.getTeam() == eTeam)
		{
			if (kLoopPlayer.getCapitalCity() != NULL)
			{
				if (kLoopPlayer.getCapitalCity()->countNumImprovedPlots() < 4)
				{
					iValue += AI_getAtWarCounter(eTeam) * 2;
				}
			}
		}
	}
	// End MNAI

	//iValue *= iTheirPower + 10;
//FfH: Modified by Kael 04/23/2009
//	iValue /= std::max(1, iOurPower + iTheirPower + 10);
	//iValue /= std::max(1, iOurPower + 10);
	if ((iOurPower * 100) > (iTheirPower * 150))
	{
		iValue *= 8;
	    iValue /= 10;
	}
//FfH: End Modify

	WarPlanTypes eWarPlan = AI_getWarPlan(eTeam);

	// if we not human, do we want to continue war for strategic reasons?
	// only check if our power is at least 150% of theirs
	if (!isHuman() && iOurPower > ((150 * iTheirPower) / 100))
	{
		bool bDagger = false;

		bool bAnyFinancialTrouble = false;
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			const CvPlayerAI& kLoopPlayer = GET_PLAYER((PlayerTypes)iI); // K-Mod
			if (kLoopPlayer.isAlive() && kLoopPlayer.getTeam() == getID())
			{
				if (kLoopPlayer.AI_isDoStrategy(AI_STRATEGY_DAGGER))
				{
					bDagger = true;
				}

				if (kLoopPlayer.AI_isFinancialTrouble())
				{
					bAnyFinancialTrouble = true;
				}
			}
		}

		// if dagger, value peace at 90% * power ratio
		if (bDagger)
		{
			iValue *= 9 * iTheirPower;
			iValue /= 10 * iOurPower;
		}
		
	    // for now, we will always do the land mass check for domination
		// if we have more than half the land, then value peace at 90% * land ratio 
		int iLandRatio = getTotalLand(true) * 100 / std::max(1, kWarTeam.getTotalLand(true));
	    if (iLandRatio > 150)
	    {
			iValue *= 9 * 100;
			iValue /= 10 * iLandRatio;
	    }

		// if in financial trouble, warmongers will continue the fight to make more money
		if (bAnyFinancialTrouble)
		{
			switch (eWarPlan)
			{
				case WARPLAN_TOTAL:
					// if we total warmonger, value peace at 70% * power ratio factor
					if (bDagger || AI_maxWarRand() < 100)
					{
						iValue *= 7 * (5 * iTheirPower);
						iValue /= 10 * (iOurPower + (4 * iTheirPower));
					}
					break;

				case WARPLAN_LIMITED:
					// if we limited warmonger, value peace at 70% * power ratio factor
					if (AI_limitedWarRand() < 100)
					{
						iValue *= 7 * (5 * iTheirPower);
						iValue /= 10 * (iOurPower + (4 * iTheirPower));
					}
					break;

				case WARPLAN_DOGPILE:
					// if we dogpile warmonger, value peace at 70% * power ratio factor
					if (AI_dogpileWarRand() < 100)
					{
						iValue *= 7 * (5 * iTheirPower);
						iValue /= 10 * (iOurPower + (4 * iTheirPower));
					}
					break;

			}
		}
	}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      05/19/10                                jdog5000      */
/*                                                                                              */
/* War strategy AI, Victory Strategy AI                                                         */
/************************************************************************************************/	
	if( AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_CULTURE4) || AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_ALTAR3) || AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_TOWERMASTERY3))
	{
		iValue *= 4;
	}
	else if( AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_CULTURE3) || AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_SPACE4) || AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_ALTAR2) || AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_TOWERMASTERY2))
	{
		iValue *= 2;
	}


	if ((!isHuman() && eWarPlan == WARPLAN_TOTAL) ||
		(!kWarTeam.isHuman() && kWarTeam.AI_getWarPlan(getID()) == WARPLAN_TOTAL))
	{
		iValue *= 2;
	}
	else if ((!isHuman() && eWarPlan == WARPLAN_DOGPILE && kWarTeam.getAtWarCount(true) > 1) ||
		     (!kWarTeam.isHuman() && kWarTeam.AI_getWarPlan(getID()) == WARPLAN_DOGPILE && getAtWarCount(true) > 1))
	{
		iValue *= 3;
		iValue /= 2;
	}

	// Do we have a big stack en route?
	int iOurAttackers = 0;
	for( int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; iPlayer++ )
	{
		if( GET_PLAYER((PlayerTypes)iPlayer).getTeam() == getID() )
		{
			iOurAttackers += GET_PLAYER((PlayerTypes)iPlayer).AI_enemyTargetMissions(eTeam);
		}
	}
	int iTheirAttackers = 0;
	CvArea* pLoopArea = NULL;
	int iLoop;
	for(pLoopArea = GC.getMapINLINE().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMapINLINE().nextArea(&iLoop))
	{
		iTheirAttackers += countEnemyDangerByArea(pLoopArea, eTeam);
	}

	int iAttackerRatio = (100 * iOurAttackers) / std::max(1 + GC.getGameINLINE().getCurrentPeriod(), iTheirAttackers);
		
	if( GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI) )
	{
		iValue *= 150;
		iValue /= range(iAttackerRatio, 150, 900);
	}
	else
	{
		iValue *= 200;
		iValue /= range(iAttackerRatio, 200, 600);
	}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	iValue -= (iValue % GC.getDefineINT("DIPLOMACY_VALUE_REMAINDER"));

	if (isHuman())
	{
		return std::max(iValue, GC.getDefineINT("DIPLOMACY_VALUE_REMAINDER"));
	}
	else
	{
		return iValue;
	}
}

/********************************************************************************/
/**		REVOLUTION_MOD							6/9/08				jdog5000	*/
/**																				*/
/**		Revolution AI															*/
/********************************************************************************/
int CvTeamAI::AI_minorKeepWarVal(TeamTypes eTeam) const
{
	int iValue = 0;

	if( (isMinorCiv() && !isRebel()) || getAnyWarPlanCount(true) == 0 )
	{
		if( AI_hasCitiesInPrimaryArea(eTeam) )
		{
			bool bIsGetBetterUnits = false;
			bool bFinancialTrouble = false;
			bool bAggressive = GC.getGame().isOption(GAMEOPTION_AGGRESSIVE_AI);
			int iDaggerCount = 0;
			for( int iJ = 0; iJ < MAX_PLAYERS; iJ++)
			{
				if (GET_PLAYER((PlayerTypes)iJ).getTeam() == getID())
				{
					if (GET_PLAYER((PlayerTypes)iJ).AI_isDoStrategy(AI_STRATEGY_GET_BETTER_UNITS))
					{
						bIsGetBetterUnits = true;
					}
					if (GET_PLAYER((PlayerTypes)iJ).AI_isDoStrategy(AI_STRATEGY_DAGGER))
					{
						iDaggerCount++;
						bAggressive = true;
					}
					if (GET_PLAYER((PlayerTypes)iJ).AI_isFinancialTrouble())
					{
						bFinancialTrouble = true;
						break;
					}
				}
			}

			if( !bFinancialTrouble )
			{
				int iPower = getPower(true);
				if( bAggressive && !bIsGetBetterUnits )
				{
					iPower *= 4;
					iPower /= 3;
				}

				if( GET_TEAM(eTeam).AI_getWarSuccess(getID()) > GC.getDefineINT("WAR_SUCCESS_CITY_CAPTURING") || GC.getGameINLINE().getSorenRandNum(AI_maxWarRand()/100, "Keep war on minor") == 0 )
				{
					if (GET_TEAM(eTeam).getDefensivePower() < ((iPower * AI_maxWarNearbyPowerRatio()) / 100))
					{
						int iNoWarRoll = GC.getGameINLINE().getSorenRandNum(100, "AI No War") - 20;
						iNoWarRoll += (bAggressive ? 10 : 0);
						iNoWarRoll += ((AI_getWarSuccess(eTeam) > GC.getDefineINT("WAR_SUCCESS_CITY_CAPTURING")) ? 10 : 0);
						iNoWarRoll -= (bIsGetBetterUnits ? 15 : 0);
						iNoWarRoll = range(iNoWarRoll, 0, 99);

						if( iNoWarRoll >= AI_noWarAttitudeProb(AI_getAttitude(eTeam)) )
						{
							if( AI_teamCloseness(eTeam) > 0 )
							{
								iValue = AI_startWarVal(eTeam);
							}
						}
					}
				}
			}
		}
	}

	return iValue;
}

int CvTeamAI::AI_getBarbarianCivWarVal(TeamTypes eTeam, int iMaxDistance) const
{
	int iValue = 0;

	if( GET_TEAM(eTeam).isAlive() && !GET_TEAM(eTeam).isMinorCiv() && !GET_TEAM(eTeam).isBarbarian() )
	{
		if( AI_hasCitiesInPrimaryArea(eTeam) )
		{
			int iClosenessValue = AI_teamCloseness(eTeam, iMaxDistance);
			if (iClosenessValue <= 0)
			{
				return 0;
			}
			iValue += iClosenessValue;

			iValue += AI_calculatePlotWarValue(eTeam)/2;

			iValue += (3 * AI_calculateCapitalProximity(eTeam)) / 2;

			iValue += 3*AI_getWarSuccess(eTeam);

			if( GET_TEAM(eTeam).getDefensivePower() > ((3*getPower(true))/2*AI_maxWarNearbyPowerRatio())/100 )
			{
				iValue /= 2;
			}
			else if( GET_TEAM(eTeam).getDefensivePower() < (getPower(true)*AI_maxWarNearbyPowerRatio())/100 )
			{
				iValue *= 2;
			}

			switch (AI_getAttitude(eTeam))
			{
			case ATTITUDE_FURIOUS:
				iValue *= 16;
				break;

			case ATTITUDE_ANNOYED:
				iValue *= 8;
				break;

			case ATTITUDE_CAUTIOUS:
				iValue *= 4;
				break;

			case ATTITUDE_PLEASED:
				iValue *= 1;
				break;

			case ATTITUDE_FRIENDLY:
				iValue *= 0;
				break;

			default:
				FAssert(false);
				break;
			}
		}
	}

	return iValue;
}
/********************************************************************************/
/**		REVOLUTION_MOD							END								*/
/********************************************************************************/


int CvTeamAI::AI_techTradeVal(TechTypes eTech, TeamTypes eTeam) const
{
	FAssert(eTeam != getID());
	int iKnownCount;
	int iPossibleKnownCount;
	int iCost;
	int iValue;
	int iI;

	iCost = std::max(0, (getResearchCost(eTech) - getResearchProgress(eTech)));

	iValue = ((iCost * 3) / 2);

	iKnownCount = 0;
	iPossibleKnownCount = 0;

	for (iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (iI != getID())
			{
				if (isHasMet((TeamTypes)iI))
				{
					if (GET_TEAM((TeamTypes)iI).isHasTech(eTech))
					{
						iKnownCount++;
					}

					iPossibleKnownCount++;
				}
			}
		}
	}

	iValue += (((iCost / 2) * (iPossibleKnownCount - iKnownCount)) / iPossibleKnownCount);

	iValue *= std::max(0, (GC.getTechInfo(eTech).getAITradeModifier() + 100));
	iValue /= 100;

	iValue -= (iValue % GC.getDefineINT("DIPLOMACY_VALUE_REMAINDER"));

	if (isHuman())
	{
		return std::max(iValue, GC.getDefineINT("DIPLOMACY_VALUE_REMAINDER"));
	}
	else
	{
		return iValue;
	}
}


DenialTypes CvTeamAI::AI_techTrade(TechTypes eTech, TeamTypes eTeam) const
{
	PROFILE_FUNC();

	AttitudeTypes eAttitude;
	int iNoTechTradeThreshold;
	int iTechTradeKnownPercent;
	int iKnownCount;
	int iPossibleKnownCount;
	int iI, iJ;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	
	
	if (GC.getGameINLINE().isOption(GAMEOPTION_NO_TECH_BROKERING))
	{
		CvTeam& kTeam = GET_TEAM(eTeam);
		
		if (!kTeam.isHasTech(eTech))
		{
			if (!kTeam.isHuman())
			{
				if (2 * kTeam.getResearchProgress(eTech) > kTeam.getResearchCost(eTech))
				{
					return DENIAL_NO_GAIN;
				}
			}
		}
	}
	
	if (isHuman())
	{
		return NO_DENIAL;
	}

	if (isVassal(eTeam))
	{
		return NO_DENIAL;
	}

	if (isAtWar(eTeam))
	{
		return NO_DENIAL;
	}

	if (AI_getWorstEnemy() == eTeam)
	{
		return DENIAL_WORST_ENEMY;
	}

	eAttitude = AI_getAttitude(eTeam);

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				if (eAttitude <= GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getTechRefuseAttitudeThreshold())
				{
/************************************************************************************************/
/* Afforess	                  Start		 02/19/10                                               */
/* Ruthless AI: Attitude is irrelevant                                                          */
/************************************************************************************************/
					if (GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI))
					{
						if (eAttitude > ATTITUDE_FURIOUS)
							continue;
					}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
					return DENIAL_ATTITUDE;
				}
			}
		}
	}
	
/************************************************************************************************/
/* Afforess	                  Start		 03/19/10                                               */
/* Ruthless AI: Don't Sell Our Military Secrets                                                 */
/************************************************************************************************/
	//if (GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI))
	if (1 < 2)
	{
		//if (GC.getTechInfo(eTech).getFlavorValue(GC.getInfoTypeForString("FLAVOR_MILITARY")) > 3)
		if (GC.getTechInfo(eTech).getFlavorValue(0) > 3)
		{
			//We don't want to spread military techs when we are gearing for war
			//If there is tech brokering, selling the tech to anyone could get it in the hands of our enemy. If there is no brokering, just worry about the current team.
			if (getAnyWarPlanCount(true) > 0 && (!GC.getGameINLINE().isOption(GAMEOPTION_NO_TECH_BROKERING) || AI_getWarPlan(eTeam) != NO_WARPLAN))
			{
				return DENIAL_NO_GAIN;
			}
		}
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	if (eAttitude < ATTITUDE_FRIENDLY)
	{
		if ((GC.getGameINLINE().getTeamRank(getID()) < (GC.getGameINLINE().countCivTeamsEverAlive() / 2)) ||
			  (GC.getGameINLINE().getTeamRank(eTeam) < (GC.getGameINLINE().countCivTeamsEverAlive() / 2)))
		{
			iNoTechTradeThreshold = AI_noTechTradeThreshold();

			iNoTechTradeThreshold *= std::max(0, (GC.getHandicapInfo(GET_TEAM(eTeam).getHandicapType()).getNoTechTradeModifier() + 100));
			iNoTechTradeThreshold /= 100;

			if (AI_getMemoryCount(eTeam, MEMORY_RECEIVED_TECH_FROM_ANY) > iNoTechTradeThreshold)
			{
				return DENIAL_TECH_WHORE;
			}
		}

		iKnownCount = 0;
		iPossibleKnownCount = 0;

		for (iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if ((iI != getID()) && (iI != eTeam))
				{
					if (isHasMet((TeamTypes)iI))
					{
						if (GET_TEAM((TeamTypes)iI).isHasTech(eTech))
						{
							iKnownCount++;
						}

						iPossibleKnownCount++;
					}
				}
			}
		}

		iTechTradeKnownPercent = AI_techTradeKnownPercent();

		iTechTradeKnownPercent *= std::max(0, (GC.getHandicapInfo(GET_TEAM(eTeam).getHandicapType()).getTechTradeKnownModifier() + 100));
		iTechTradeKnownPercent /= 100;
		
		iTechTradeKnownPercent *= AI_getTechMonopolyValue(eTech, eTeam);
		iTechTradeKnownPercent /= 100;

		if ((iPossibleKnownCount > 0) ? (((iKnownCount * 100) / iPossibleKnownCount) < iTechTradeKnownPercent) : (iTechTradeKnownPercent > 0))
		{
			return DENIAL_TECH_MONOPOLY;
		}
	}

	for (iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		if (isTechRequiredForUnit(eTech, ((UnitTypes)iI)))
		{
			if (isWorldUnitClass((UnitClassTypes)(GC.getUnitInfo((UnitTypes)iI).getUnitClassType())))
			{
				if (getUnitClassMaking((UnitClassTypes)(GC.getUnitInfo((UnitTypes)iI).getUnitClassType())) > 0)
				{
					return DENIAL_MYSTERY;
				}
			}
		}
	}

	for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (isTechRequiredForBuilding(eTech, ((BuildingTypes)iI)))
		{
			if (isWorldWonderClass((BuildingClassTypes)(GC.getBuildingInfo((BuildingTypes)iI).getBuildingClassType())))
			{
				if (getBuildingClassMaking((BuildingClassTypes)(GC.getBuildingInfo((BuildingTypes)iI).getBuildingClassType())) > 0)
				{
					return DENIAL_MYSTERY;
				}
			}
		}
	}

	for (iI = 0; iI < GC.getNumProjectInfos(); iI++)
	{
		if (GC.getProjectInfo((ProjectTypes)iI).getTechPrereq() == eTech)
		{
			if (isWorldProject((ProjectTypes)iI))
			{
				if (getProjectMaking((ProjectTypes)iI) > 0)
				{
					return DENIAL_MYSTERY;
				}
			}

			for (iJ = 0; iJ < GC.getNumVictoryInfos(); iJ++)
			{
				if (GC.getGameINLINE().isVictoryValid((VictoryTypes)iJ))
				{
					if (GC.getProjectInfo((ProjectTypes)iI).getVictoryThreshold((VictoryTypes)iJ))
					{
						return DENIAL_VICTORY;
					}
				}
			}
		}
	}

	return NO_DENIAL;
}


int CvTeamAI::AI_mapTradeVal(TeamTypes eTeam) const
{
	CvPlot* pLoopPlot;
	int iValue;
	int iI;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	iValue = 0;

	for (iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		if (!(pLoopPlot->isRevealed(getID(), false)) && pLoopPlot->isRevealed(eTeam, false))
		{
			if (pLoopPlot->isWater())
			{
				iValue++;
			}
			else
			{
				iValue += 5;
			}
		}
	}

	iValue /= 10;
/************************************************************************************************/
/* Afforess	                  Start		 03/19/10                                               */
/* Ruthless AI                                                                                  */
/************************************************************************************************/
	//if (GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI))
	if (1 < 2)
	{
		//Planning war against the team, we need their map!
		if (AI_getWarPlan(eTeam) != NO_WARPLAN)
		{
			iValue *= 15;
		}
		//we should check to see if their map covers the team(s) we are gearing for war with
		else if (getAnyWarPlanCount(true) > 0)
		{
			for (iI = 0; iI < MAX_TEAMS; iI++)
			{
				if (GET_TEAM((TeamTypes)iI).isAlive())
				{
					if (GET_TEAM((TeamTypes)iI).getID() != getID())
					{
						//victim
						if (AI_getWarPlan((TeamTypes)iI) != NO_WARPLAN)
						{
							//if they are friends of the victim, they probably shared maps
							if (GET_TEAM(eTeam).AI_getAttitudeVal((TeamTypes)iI) > 5)
							{
								iValue *= 5;
								break;
							}
						}
					}
				}
			}
		}
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	if (GET_TEAM(eTeam).isVassal(getID()))
	{
		iValue /= 2;
	}

	iValue -= (iValue % GC.getDefineINT("DIPLOMACY_VALUE_REMAINDER"));

	if (isHuman())
	{
		return std::max(iValue, GC.getDefineINT("DIPLOMACY_VALUE_REMAINDER"));
	}
	else
	{
		return iValue;
	}
}


DenialTypes CvTeamAI::AI_mapTrade(TeamTypes eTeam) const
{
	PROFILE_FUNC();

	AttitudeTypes eAttitude;
	int iI;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	if (isHuman())
	{
		return NO_DENIAL;
	}

	if (isVassal(eTeam))
	{
		return NO_DENIAL;
	}

	if (isAtWar(eTeam))
	{
		return NO_DENIAL;
	}

	if (AI_getWorstEnemy() == eTeam)
	{
		return DENIAL_WORST_ENEMY;
	}
/************************************************************************************************/
/* Afforess	                  Start		 03/30/10                                               */
/* Ruthless AI: Selling Maps right before we go to war is stupid                                */
/************************************************************************************************/
	//if (GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI))
	if (1 < 2)
	{
		if (AI_getWarPlan(eTeam) != NO_WARPLAN)
		{
			return DENIAL_MYSTERY;
		}
		//we should check to see if their map covers the team(s) we are gearing for war with
		if (getAnyWarPlanCount(true) > 0)
		{
			for (iI = 0; iI < MAX_TEAMS; iI++)
			{
				if (GET_TEAM((TeamTypes)iI).isAlive())
				{
					if (GET_TEAM((TeamTypes)iI).getID() != getID())
					{
						//victim
						if (AI_getWarPlan((TeamTypes)iI) != NO_WARPLAN)
						{
							//Friends of out victim will sell them their maps, so don't trade our secrets to them
							if (GET_TEAM(eTeam).AI_getAttitudeVal((TeamTypes)iI) > 5)
							{
								return DENIAL_NO_GAIN;
							}
						}
					}
				}
			}
		}
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	eAttitude = AI_getAttitude(eTeam);

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				if (eAttitude <= GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getMapRefuseAttitudeThreshold())
				{
					return DENIAL_ATTITUDE;
				}
			}
		}
	}

	return NO_DENIAL;
}


int CvTeamAI::AI_vassalTradeVal(TeamTypes eTeam) const
{
	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	return AI_surrenderTradeVal(eTeam);
}


DenialTypes CvTeamAI::AI_vassalTrade(TeamTypes eTeam) const
{
	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	CvTeamAI& kMasterTeam = GET_TEAM(eTeam);

	for (int iLoopTeam = 0; iLoopTeam < MAX_TEAMS; iLoopTeam++)
	{
		CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iLoopTeam);
		if (kLoopTeam.isAlive() && iLoopTeam != getID() && iLoopTeam != kMasterTeam.getID())
		{
			if (!kLoopTeam.isAtWar(kMasterTeam.getID()) && kLoopTeam.isAtWar(getID()))
			{
				if (kMasterTeam.isForcePeace((TeamTypes)iLoopTeam) || !kMasterTeam.canChangeWarPeace((TeamTypes)iLoopTeam))
				{
					if (!kLoopTeam.isAVassal())
					{
						return DENIAL_WAR_NOT_POSSIBLE_YOU;
					}
				}

				if (!kMasterTeam.isHuman())
				{
					DenialTypes eWarDenial = kMasterTeam.AI_declareWarTrade((TeamTypes)iLoopTeam, getID(), true);
					if (NO_DENIAL != eWarDenial)
					{
						return DENIAL_WAR_NOT_POSSIBLE_YOU;
					}
				}
			}
			else if (kLoopTeam.isAtWar(kMasterTeam.getID()) && !kLoopTeam.isAtWar(getID()))
			{
				if (!kMasterTeam.canChangeWarPeace((TeamTypes)iLoopTeam))
				{
					if (!kLoopTeam.isAVassal())
					{
						return DENIAL_PEACE_NOT_POSSIBLE_YOU;
					}
				}

				if (!kMasterTeam.isHuman())
				{
					DenialTypes ePeaceDenial = kMasterTeam.AI_makePeaceTrade((TeamTypes)iLoopTeam, getID());
					if (NO_DENIAL != ePeaceDenial)
					{
						return DENIAL_PEACE_NOT_POSSIBLE_YOU;
					}
				}
			}
		}
	}

	return AI_surrenderTrade(eTeam);
}


int CvTeamAI::AI_surrenderTradeVal(TeamTypes eTeam) const
{
	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	return 0;
}


DenialTypes CvTeamAI::AI_surrenderTrade(TeamTypes eTeam, int iPowerMultiplier) const
{
	PROFILE_FUNC();

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	CvTeam& kMasterTeam = GET_TEAM(eTeam);

	for (int iLoopTeam = 0; iLoopTeam < MAX_TEAMS; iLoopTeam++)
	{
		CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iLoopTeam);
		if (kLoopTeam.isAlive() && iLoopTeam != getID() && iLoopTeam != kMasterTeam.getID())
		{
			if (kLoopTeam.isAtWar(kMasterTeam.getID()) && !kLoopTeam.isAtWar(getID()))
			{
				if (isForcePeace((TeamTypes)iLoopTeam) || !canChangeWarPeace((TeamTypes)iLoopTeam))
				{
					if (!kLoopTeam.isAVassal())
					{
						return DENIAL_WAR_NOT_POSSIBLE_US;
					}
				}
			}
			else if (!kLoopTeam.isAtWar(kMasterTeam.getID()) && kLoopTeam.isAtWar(getID()))
			{
				if (!canChangeWarPeace((TeamTypes)iLoopTeam))
				{
					if (!kLoopTeam.isAVassal())
					{
						return DENIAL_PEACE_NOT_POSSIBLE_US;
					}
				}
			}
		}
	}

	if (isHuman())
	{
		return NO_DENIAL;
	}

	int iAttitudeModifier = 0;

	if (!GET_TEAM(eTeam).isParent(getID()))
	{
		int iPersonalityModifier = 0;
		int iMembers = 0;
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
				{
					iPersonalityModifier += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getVassalPowerModifier();
					++iMembers;
				}
			}
		}

/************************************************************************************************/
/* REVOLUTION_MOD                         06/03/09                                jdog5000      */
/*                                                                                              */
/* War Strategy AI                                                                              */
/************************************************************************************************/
/* original BTS code
		int iTotalPower = GC.getGameINLINE().countTotalCivPower();
		int iAveragePower = iTotalPower / std::max(1, GC.getGameINLINE().countCivTeamsAlive());
*/

		int iTotalPower = 0;
		int iNumNonVassals = 0;
		for (int iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			CvTeam& kTeam = GET_TEAM((TeamTypes) iI);
			if ( kTeam.isAlive() )
			{
				if( kTeam.isAVassal() && kTeam.isCapitulated() )
				{
					// Count capitulated vassals as a fractional add to their master's power
					iTotalPower += (2*kTeam.getPower(false))/5;
				}
				else if( !(kTeam.isSingleCityTeam()) && !(kTeam.isMinorCiv()) && !(kTeam.isRebel()) )
				{
					iTotalPower += kTeam.getPower(false);
					iNumNonVassals++;
				}
			}
		}
		int iAveragePower = iTotalPower / std::max(1, iNumNonVassals);
/************************************************************************************************/
/* REVOLUTION_MOD                       END                                                     */
/************************************************************************************************/
		int iMasterPower = kMasterTeam.getPower(false);
		int iOurPower = getPower(true); // K-Mod (this value is used a bunch of times separately)
		int iVassalPower = (iOurPower * (iPowerMultiplier + iPersonalityModifier / std::max(1, iMembers))) / 100;

		if (isAtWar(eTeam))
		{
			int iTheirSuccess = std::max(10, GET_TEAM(eTeam).AI_getWarSuccess(getID()));
			int iOurSuccess = std::max(10, AI_getWarSuccess(eTeam));
			int iOthersBestSuccess = 0;
			for (int iTeam = 0; iTeam < MAX_CIV_TEAMS; ++iTeam)
			{
				if (iTeam != eTeam && iTeam != getID())
				{
					CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iTeam);

					if (kLoopTeam.isAlive() && kLoopTeam.isAtWar(getID()))
					{
						int iSuccess = kLoopTeam.AI_getWarSuccess(getID());
						if (iSuccess > iOthersBestSuccess)
						{
							iOthersBestSuccess = iSuccess;
						}
					}
				}
			}

			// Discourage capitulation to a team that has not done the most damage
			if (iTheirSuccess < iOthersBestSuccess)
			{
				iOurSuccess += iOthersBestSuccess - iTheirSuccess;
			}

			iMasterPower = (2 * iMasterPower * iTheirSuccess) / (iTheirSuccess + iOurSuccess);

			if (AI_getWorstEnemy() == eTeam)
			{
				iMasterPower *= 3;
				iMasterPower /= 4;
			}
		}
		else
		{
			if (!GET_TEAM(eTeam).AI_isLandTarget(getID()))
			{
				iMasterPower /= 2;
			}
		}

		// K-Mod. (condition moved here from lower down; for efficiency.)
		if (3 * iVassalPower > 2 * iMasterPower)
			return DENIAL_POWER_US;
		// K-Mod end

		for (int iLoopTeam = 0; iLoopTeam < MAX_CIV_TEAMS; iLoopTeam++)
		{
			if (iLoopTeam != getID())
			{
				CvTeamAI& kLoopTeam = GET_TEAM((TeamTypes)iLoopTeam);

				if (kLoopTeam.isAlive())
				{
					if (kLoopTeam.AI_isLandTarget(getID()))
					{
						if (iLoopTeam != eTeam)
						{
							int iLoopPower = kLoopTeam.getPower(true); // K-Mod
							if (iLoopPower > iOurPower)
							{
								//if (kLoopTeam.isAtWar(eTeam) && !kLoopTeam.isAtWar(getID()))
								if (kLoopTeam.isAtWar(eTeam) && !kLoopTeam.isAtWar(getID()) && (!isAtWar(eTeam) || iMasterPower < 2 * iLoopPower)) // K-Mod
								{
									return DENIAL_POWER_YOUR_ENEMIES;
								}

								iAveragePower = (2 * iAveragePower * iLoopPower) / std::max(1, iLoopPower + iOurPower);

								//iAttitudeModifier += (3 * kLoopTeam.getPower(true)) / std::max(1, getPower(true)) - 2;
								iAttitudeModifier += (6 * iLoopPower / std::max(1, iOurPower) - 5)/2; // K-Mod. (effectively -2.5 instead of 2)
							}

							if (!kLoopTeam.isAtWar(eTeam) && kLoopTeam.isAtWar(getID()))
							{
								//iAveragePower = (iAveragePower * (getPower(true) + GET_TEAM(eTeam).getPower(false))) / std::max(1, getPower(true));
								iAveragePower = iAveragePower * (iOurPower + iMasterPower) / std::max(1, iOurPower + std::max(iOurPower, iLoopPower)); // K-Mod
							}
						}
					}

					if (!atWar(getID(), eTeam))
					{
						if (kLoopTeam.isAtWar(eTeam) && !kLoopTeam.isAtWar(getID()))
						{
							DenialTypes eDenial = AI_declareWarTrade((TeamTypes)iLoopTeam, eTeam, false);
							if (eDenial != NO_DENIAL)
							{
								return eDenial;
							}
						}
					}
				}
			}
		}

		if (!isVassal(eTeam) && canVassalRevolt(eTeam))
		{
			return DENIAL_POWER_US;
		}

		// if (iVassalPower > iAveragePower || 3 * iVassalPower > 2 * iMasterPower)
		if (5*iVassalPower > 4*iAveragePower) // K-Mod. (second condition already checked)
		{
			return DENIAL_POWER_US;
		}

		for (int i = 0; i < GC.getNumVictoryInfos(); i++)
		{
			bool bPopulationThreat = true;
			if (GC.getGameINLINE().getAdjustedPopulationPercent((VictoryTypes)i) > 0)
			{
				bPopulationThreat = false;

				int iThreshold = GC.getGameINLINE().getTotalPopulation() * GC.getGameINLINE().getAdjustedPopulationPercent((VictoryTypes)i);
				if (400 * getTotalPopulation(!isAVassal()) > 3 * iThreshold)
				{
					return DENIAL_VICTORY;
				}

				if (!atWar(getID(), eTeam))
				{
					if (400 * (getTotalPopulation(isAVassal()) + GET_TEAM(eTeam).getTotalPopulation()) > 3 * iThreshold)
					{
						bPopulationThreat = true;
					}
				}
			}

			bool bLandThreat = true;
			if (GC.getGameINLINE().getAdjustedLandPercent((VictoryTypes)i) > 0)
			{
				bLandThreat = false;

				int iThreshold = GC.getMapINLINE().getLandPlots() * GC.getGameINLINE().getAdjustedLandPercent((VictoryTypes)i);
				if (400 * getTotalLand(!isAVassal()) > 3 * iThreshold)
				{
					return DENIAL_VICTORY;
				}

				if (!atWar(getID(), eTeam))
				{
					if (400 * (getTotalLand(isAVassal()) + GET_TEAM(eTeam).getTotalLand()) > 3 * iThreshold)
					{
						bLandThreat = true;
					}
				}
			}

			if (GC.getGameINLINE().getAdjustedPopulationPercent((VictoryTypes)i) > 0 || GC.getGameINLINE().getAdjustedLandPercent((VictoryTypes)i) > 0)
			{
				if (bLandThreat && bPopulationThreat)
				{
					return DENIAL_POWER_YOU;
				}
			}
		}
	}

	if (!isAtWar(eTeam))
	{
		if (!GET_TEAM(eTeam).isParent(getID()))
		{
			if (AI_getWorstEnemy() == eTeam)
			{
				return DENIAL_WORST_ENEMY;
			}

			if (!AI_hasCitiesInPrimaryArea(eTeam) && AI_calculateAdjacentLandPlots(eTeam) == 0)
			{
				return DENIAL_TOO_FAR;
			}
		}

		AttitudeTypes eAttitude = AI_getAttitude(eTeam, false);

		AttitudeTypes eModifiedAttitude = CvPlayerAI::AI_getAttitudeFromValue(AI_getAttitudeVal(eTeam, false) + iAttitudeModifier);

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
				{
					if (eAttitude <= ATTITUDE_FURIOUS)
					{
						return DENIAL_ATTITUDE;
					}

					if (eModifiedAttitude <= GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getVassalRefuseAttitudeThreshold())
					{
						return DENIAL_ATTITUDE;
					}
				}
			}
		}
	}
	else
	{
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      12/07/09                                jdog5000      */
/*                                                                                              */
/* Diplomacy AI                                                                                 */
/************************************************************************************************/
/* original BTS code
		if (AI_getWarSuccess(eTeam) + 4 * GC.getDefineINT("WAR_SUCCESS_CITY_CAPTURING") > GET_TEAM(eTeam).AI_getWarSuccess(getID()))
		{
			return DENIAL_JOKING;
		}
*/
		// Scale better for small empires, particularly necessary if WAR_SUCCESS_CITY_CAPTURING > 10
		if (AI_getWarSuccess(eTeam) + std::min(getNumCities(), 4) * GC.getWAR_SUCCESS_CITY_CAPTURING() > GET_TEAM(eTeam).AI_getWarSuccess(getID()))
		{
			return DENIAL_JOKING;
		}

		if( !kMasterTeam.isHuman() )
		{
			if( !(GET_TEAM(kMasterTeam.getID()).AI_acceptSurrender(getID())) )
			{
				return DENIAL_JOKING;
			}
		}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	}
	
	return NO_DENIAL;
}

// K-Mod
int CvTeamAI::AI_countMembersWithStrategy(int iStrategy) const
{
	int iCount = 0;
	for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; iPlayer++)
	{
		if (GET_PLAYER((PlayerTypes)iPlayer).getTeam() == getID())
		{
			if (GET_PLAYER((PlayerTypes)iPlayer).isAlive())
			{
				if (GET_PLAYER((PlayerTypes)iPlayer).AI_isDoStrategy(iStrategy))
				{
					iCount++;
				}
			}
		}
	}

	return iCount;
}
// K-Mod end

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      03/20/10                                jdog5000      */
/*                                                                                              */
/* Victory Strategy AI                                                                          */
/************************************************************************************************/
bool CvTeamAI::AI_isAnyMemberDoVictoryStrategy( int iVictoryStrategy ) const
{
	for( int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; iPlayer++ )
	{
		if( GET_PLAYER((PlayerTypes)iPlayer).getTeam() == getID() )
		{
			if( GET_PLAYER((PlayerTypes)iPlayer).isAlive() )
			{
				if( GET_PLAYER((PlayerTypes)iPlayer).AI_isDoVictoryStrategy(iVictoryStrategy) )
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool CvTeamAI::AI_isAnyMemberDoVictoryStrategyLevel4() const
{
	for( int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; iPlayer++ )
	{
		if( GET_PLAYER((PlayerTypes)iPlayer).getTeam() == getID() )
		{
			if( GET_PLAYER((PlayerTypes)iPlayer).isAlive() )
			{
				if( GET_PLAYER((PlayerTypes)iPlayer).AI_isDoVictoryStrategyLevel4() )
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool CvTeamAI::AI_isAnyMemberDoVictoryStrategyLevel3() const
{
	for( int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; iPlayer++ )
	{
		if( GET_PLAYER((PlayerTypes)iPlayer).getTeam() == getID() )
		{
			if( GET_PLAYER((PlayerTypes)iPlayer).isAlive() )
			{
				if( GET_PLAYER((PlayerTypes)iPlayer).AI_isDoVictoryStrategyLevel3() )
				{
					return true;
				}
			}
		}
	}

	return false;
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

// K-Mod. return a rating of our war success between -99 and 99.
// -99 means we losing and have very little hope of surviving. 99 means we are soundly defeating our enemies. Zero is neutral (eg. no wars being fought).
int CvTeamAI::AI_getWarSuccessRating() const
{
	PROFILE_FUNC();
	// (Based on my code for Force Peace diplomacy voting.)

	int iMilitaryUnits = 0;
	for (int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.getTeam() == getID())
		{
			iMilitaryUnits += kLoopPlayer.getNumMilitaryUnits();
		}
	}
	int iSuccessScale = iMilitaryUnits * GC.getDefineINT("WAR_SUCCESS_ATTACKING") / 5;

	int iThisTeamPower = getPower(true);
	int iScore = 0;

	for (int iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		const CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
		if (iI != getID() && isAtWar((TeamTypes)iI) && kLoopTeam.isAlive() && !kLoopTeam.isAVassal())
		{
			int iThisTeamSuccess = AI_getWarSuccess((TeamTypes)iI);
			int iOtherTeamSuccess = kLoopTeam.AI_getWarSuccess(getID());

			int iOtherTeamPower = kLoopTeam.getPower(true);

			iScore += (iThisTeamSuccess+iSuccessScale) * iThisTeamPower;
			iScore -= (iOtherTeamSuccess+iSuccessScale) * iOtherTeamPower;
		}
	}
	iScore = range((100*iScore)/std::max(1, iThisTeamPower*iSuccessScale*5), -99, 99);
	return iScore;
}
// K-Mod end

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      03/20/10                                jdog5000      */
/*                                                                                              */
/* War Strategy AI                                                                              */
/************************************************************************************************/
/// \brief Compute power of enemies as percentage of our power.
///
///
int CvTeamAI::AI_getEnemyPowerPercent( bool bConsiderOthers ) const
{
	int iEnemyPower = 0;

	for( int iI = 0; iI < MAX_CIV_TEAMS; iI++ )
	{
		if( iI != getID() )
		{
			if( GET_TEAM((TeamTypes)iI).isAlive() && isHasMet((TeamTypes)iI) )
			{
				if( isAtWar((TeamTypes)iI) )
				{
					int iTempPower = 220 * GET_TEAM((TeamTypes)iI).getPower(false);
					iTempPower /= (AI_hasCitiesInPrimaryArea((TeamTypes)iI) ? 2 : 3);
					iTempPower /= (GET_TEAM((TeamTypes)iI).isMinorCiv() ? 3 : 1);
					iTempPower /= std::max(1, (bConsiderOthers ? GET_TEAM((TeamTypes)iI).getAtWarCount(true,true) : 1));
					iEnemyPower += iTempPower;
				}
				else if( AI_isChosenWar((TeamTypes)iI) )
				{
					// Haven't declared war yet
					int iTempPower = 240 * GET_TEAM((TeamTypes)iI).getDefensivePower();
					iTempPower /= (AI_hasCitiesInPrimaryArea((TeamTypes)iI) ? 2 : 3);
					iTempPower /= 1 + (bConsiderOthers ? GET_TEAM((TeamTypes)iI).getAtWarCount(true,true) : 0);
					iEnemyPower += iTempPower;
				}
			}
		}
	}

	return (iEnemyPower/std::max(1, (isAVassal() ? getCurrentMasterPower(true) : getPower(true))));
}

/// \brief Sum up air power of enemies plus average of other civs we've met.
///
int CvTeamAI::AI_getRivalAirPower( ) const
{
	// Count enemy air units, not just those visible to us
	int iRivalAirPower = 0;
	int iEnemyAirPower = 0;

	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		CvUnitInfo& kUnitInfo = GC.getUnitInfo((UnitTypes)iI);

		if( kUnitInfo.getDomainType() == DOMAIN_AIR ) 
		{
			if( kUnitInfo.getAirCombat() > 0 )
			{
				for( int iTeam = 0; iTeam < MAX_CIV_TEAMS; iTeam++ )
				{
					if( iTeam != getID() )
					{
						if( GET_TEAM((TeamTypes)iTeam).isAlive() && isHasMet((TeamTypes)iTeam) )
						{
							int iUnitPower = GET_TEAM((TeamTypes)iTeam).getUnitClassCount((UnitClassTypes)kUnitInfo.getUnitClassType());

							if( iUnitPower > 0 )
							{
								iUnitPower *= kUnitInfo.getPowerValue();

								if( AI_getWarPlan((TeamTypes)iTeam) == NO_WARPLAN )
								{
									iRivalAirPower += iUnitPower;
								}
								else
								{
									iEnemyAirPower += iUnitPower;
								}
							}
						}
					}
				}
			}
		}
	}

	return (iEnemyAirPower + (iRivalAirPower / std::max(1,getHasMetCivCount(true))));
}

bool CvTeamAI::AI_acceptSurrender( TeamTypes eSurrenderTeam )
{
	PROFILE_FUNC();

	if( isHuman() )
	{
		return true;
	}

	if( !isAtWar(eSurrenderTeam) )
	{
		return true;
	}

	if( GET_TEAM(eSurrenderTeam).AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_SPACE3) )
	{
		// Capturing capital or Apollo city will stop space
		return false;
	}

	if( GET_TEAM(eSurrenderTeam).AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_CULTURE3) )
	{
		// Capturing top culture cities will stop culture
		return false;
	}

	// Check for whether another team has won enough to cause capitulation
	for( int iI = 0; iI < MAX_CIV_TEAMS; iI++ )
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (iI != getID() && !(GET_TEAM((TeamTypes)iI).isVassal(getID())) )
			{
				if (GET_TEAM(eSurrenderTeam).isAtWar((TeamTypes)iI))
				{
					if (GET_TEAM(eSurrenderTeam).AI_getAtWarCounter((TeamTypes)iI) >= 10)
					{
						if( (GET_TEAM(eSurrenderTeam).AI_getWarSuccess((TeamTypes)iI) + std::min(GET_TEAM(eSurrenderTeam).getNumCities(), 4) * GC.getWAR_SUCCESS_CITY_CAPTURING()) < GET_TEAM((TeamTypes)iI).AI_getWarSuccess(eSurrenderTeam))
						{
							return true;
						}
					}
				}
			}
		}
	}

	int iValuableCities = 0;
	int iCitiesThreatenedByUs = 0;
	int iValuableCitiesThreatenedByUs = 0;
	int iCitiesThreatenedByOthers = 0;

	CvCity* pLoopCity;
	int iLoop;

	for (int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if( GET_PLAYER((PlayerTypes)iI).getTeam() == eSurrenderTeam && GET_PLAYER((PlayerTypes)iI).isAlive() )
		{
			for (pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
			{
				bool bValuable = false;

				if( pLoopCity->isHolyCity() )
				{
					bValuable = true;
				}
				else if( pLoopCity->isHeadquarters() )
				{
					bValuable = true;
				}
				else if( pLoopCity->hasActiveWorldWonder() )
				{
					bValuable = true;
				}
				else if( AI_isPrimaryArea(pLoopCity->area()) && (GET_TEAM(eSurrenderTeam).countNumCitiesByArea(pLoopCity->area()) < 3) )
				{
					bValuable = true;
				}
				else if( pLoopCity->isCapital() && (GET_TEAM(eSurrenderTeam).getNumCities() > GET_TEAM(eSurrenderTeam).getNumMembers() || countNumCitiesByArea(pLoopCity->area()) > 0) )
				{
					bValuable = true;
				}
				else
				{
					// Valuable terrain bonuses
					CvPlot* pLoopPlot = NULL;
					for (int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
					{
						pLoopPlot = plotCity(pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE(), iJ);

						if (pLoopPlot != NULL)
						{
							BonusTypes eBonus = pLoopPlot->getNonObsoleteBonusType(getID());
							if ( eBonus != NO_BONUS)
							{
								if(GET_PLAYER(getLeaderID()).AI_bonusVal(eBonus) > 15)
								{
									bValuable = true;
									break;
								}
							}
						}
					}
				}

				int iOwnerPower = GET_PLAYER((PlayerTypes)iI).AI_getOurPlotStrength(pLoopCity->plot(), 2, true, false);
				int iOurPower = AI_getOurPlotStrength(pLoopCity->plot(), 2, false, false, true);
				int iOtherPower = GET_PLAYER((PlayerTypes)iI).AI_getEnemyPlotStrength(pLoopCity->plot(), 2, false, false) - iOurPower;

				if( iOtherPower > iOwnerPower )
				{
					iCitiesThreatenedByOthers++;
				}

				if (iOurPower > iOwnerPower)
				{
					iCitiesThreatenedByUs++;
					if( bValuable )
					{
						iValuableCities++;
						iValuableCitiesThreatenedByUs++;
						continue;
					}
				}

				if( bValuable && pLoopCity->getHighestPopulation() < 5 )
				{
					bValuable = false;
				}

				if( bValuable )
				{
					if( AI_isPrimaryArea(pLoopCity->area()) )
					{
						iValuableCities++;
					}
					else
					{
						for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
						{
							if (GET_PLAYER((PlayerTypes)iJ).isAlive())
							{
								if (GET_PLAYER((PlayerTypes)iJ).getTeam() == getID())
								{
									if( pLoopCity->AI_playerCloseness((PlayerTypes)iJ) > 5 )
									{
										iValuableCities++;
										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if( iValuableCitiesThreatenedByUs > 0 )
	{
		// Press for capture of valuable city
		return false;
	}

	if( iCitiesThreatenedByOthers > (1 + iCitiesThreatenedByUs/2) )
	{
		// Keep others from capturing spoils, but let it go if surrender civ is too small
		// to care about
		if( 6*(iValuableCities + GET_TEAM(eSurrenderTeam).getNumCities()) > getNumCities() )
		{
			return true;
		}
	}

	// If we're low on the totem poll, accept so enemies don't drag anyone else into war with us
	// Top rank is 0, second is 1, etc.
	int iTeamRank = GC.getGameINLINE().getTeamRank(getID());
	if( iTeamRank > (1 + GC.getGameINLINE().countCivTeamsAlive()/3) )
	{
		return true;
	}

	int iOurWarSuccessRatio = AI_getWarSuccessRating();
	if( iOurWarSuccessRatio < -30 )
	{
		// We're doing badly overall, need to be done with this war and gain an ally
		return true;
	}

	int iWarCount = 0;
	for (int iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && !(GET_TEAM((TeamTypes)iI).isMinorCiv()))
		{
			if ((TeamTypes)iI != eSurrenderTeam && !(GET_TEAM((TeamTypes)iI).isVassal(eSurrenderTeam)))
			{
				if (isAtWar((TeamTypes)iI))
				{
					if( GET_TEAM((TeamTypes)iI).AI_getWarSuccess(getID()) > 5*GC.getDefineINT("WAR_SUCCESS_ATTACKING") )
					{
						iWarCount++;
					}
				}
			}
		}
	}

	if( iWarCount > 0 && iOurWarSuccessRatio < 50 )
	{
		// Accept if we have other wars to fight
		return true;
	}

	// War weariness
	int iWearinessThreshold = (GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI) ? 300 : 240);
	iWearinessThreshold += 10*iValuableCities + 20*iCitiesThreatenedByUs;

	for (int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID() )
			{
				int iWarWearinessPercentAnger = (getWarWeariness(eSurrenderTeam) * std::max(0, 100 + GET_TEAM(eSurrenderTeam).getEnemyWarWearinessModifier())) / 10000;
				iWarWearinessPercentAnger = GET_PLAYER((PlayerTypes)iI).getModifiedWarWearinessPercentAnger(iWarWearinessPercentAnger);

				// Significant war weariness from eSurrenderTeam, 1000 = 100%
				if( iWarWearinessPercentAnger > 50 )
				{
					if( GET_PLAYER((PlayerTypes)iI).getWarWearinessPercentAnger() > iWearinessThreshold )
					{
						return true;
					}
				}
			}
		}
	}

	if( (iValuableCities + iCitiesThreatenedByUs) >= (AI_maxWarRand()/100) )
	{
		// Continue conquest
		return false;
	}

	if( GET_TEAM(eSurrenderTeam).getNumCities() < (getNumCities()/4 - (AI_maxWarRand()/100)) )
	{
		// Too small to bother leaving alive
		return false;
	}
	
	return true;
}

void CvTeamAI::AI_getWarRands( int &iMaxWarRand, int &iLimitedWarRand, int &iDogpileWarRand ) const
{
	iMaxWarRand = AI_maxWarRand();
	iLimitedWarRand = AI_limitedWarRand();
	iDogpileWarRand = AI_dogpileWarRand();

	bool bCult4 = false;
	bool bSpace4 = false;
	bool bCult3 = false;
	bool bFinalWar = false;

	for (int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (GET_PLAYER((PlayerTypes)iI).AI_isDoStrategy(AI_STRATEGY_FINAL_WAR))
				{
					bFinalWar = true;
				}

				if( GET_PLAYER((PlayerTypes)iI).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE4))
				{
					bCult4 = true;
				}
				if( GET_PLAYER((PlayerTypes)iI).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3))
				{
					bCult3 = true;
				}
				if(GET_PLAYER((PlayerTypes)iI).AI_isDoVictoryStrategy(AI_VICTORY_SPACE4))
				{
					bSpace4 = true;
				}
			}
		}
	}

	if( bCult4 )
	{
		iMaxWarRand *= 4;
		iLimitedWarRand *= 3;
		iDogpileWarRand *= 2;
	}
	else if( bSpace4 )
	{
		iMaxWarRand *= 3;

		iLimitedWarRand *= 2;

		iDogpileWarRand *= 3;
		iDogpileWarRand /= 2;
	}
	else if( bCult3 )
	{
		iMaxWarRand *= 2;

		iLimitedWarRand *= 3;
		iLimitedWarRand /= 2;

		iDogpileWarRand *= 3;
		iDogpileWarRand /= 2;
	}

	int iNumMembers = getNumMembers();
	int iNumVassals = getVassalCount();
	
	iMaxWarRand *= (2 + iNumMembers);
	iMaxWarRand /= (2 + iNumMembers + iNumVassals);
	
	if (bFinalWar)
	{
	    iMaxWarRand /= 4;
	}

	iLimitedWarRand *= (2 + iNumMembers);
	iLimitedWarRand /= (2 + iNumMembers + iNumVassals);
	
	iDogpileWarRand *= (2 + iNumMembers);
	iDogpileWarRand /= (2 + iNumMembers + iNumVassals);
}


void CvTeamAI::AI_getWarThresholds( int &iTotalWarThreshold, int &iLimitedWarThreshold, int &iDogpileWarThreshold ) const
{
	iTotalWarThreshold = 0;
	iLimitedWarThreshold = 0;
	iDogpileWarThreshold = 0;

	//int iHighUnitSpendingPercent = 0;
	int iHighUnitSpending = 0; // K-Mod
	bool bConq2 = false;
	bool bDom3 = false;
	bool bAggressive = GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI);
	for (int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				/* int iUnitSpendingPercent = (GET_PLAYER((PlayerTypes)iI).calculateUnitCost() * 100) / std::max(1, GET_PLAYER((PlayerTypes)iI).calculatePreInflatedCosts());
				iHighUnitSpendingPercent += (std::max(0, iUnitSpendingPercent - 7) / 2); */
				int iUnitSpendingPerMil = GET_PLAYER((PlayerTypes)iI).AI_unitCostPerMil(); // K-Mod
				iHighUnitSpending += (std::max(0, iUnitSpendingPerMil - 16) / 6); // K-Mod

				if( GET_PLAYER((PlayerTypes)iI).AI_isDoStrategy(AI_STRATEGY_DAGGER))
				{
					bAggressive = true;
				}
				if( GET_PLAYER((PlayerTypes)iI).AI_isDoVictoryStrategy(AI_VICTORY_CONQUEST4))
				{
					bAggressive = true;
				}
				if( GET_PLAYER((PlayerTypes)iI).AI_isDoVictoryStrategy(AI_VICTORY_DOMINATION4))
				{
					bAggressive = true;
				}
				if( GET_PLAYER((PlayerTypes)iI).AI_isDoVictoryStrategy(AI_VICTORY_CONQUEST2))
				{
					bConq2 = true;
				}
				if(GET_PLAYER((PlayerTypes)iI).AI_isDoVictoryStrategy(AI_VICTORY_DOMINATION3))
				{
					bDom3 = true;
				}
			}
		}
	}

	iHighUnitSpending /= std::max(1, getNumMembers());

	iTotalWarThreshold = iHighUnitSpending * (bAggressive ? 3 : 2);
	if( bDom3 )
	{
		iTotalWarThreshold *= 3;

		iDogpileWarThreshold += 5;
	}
	else if( bConq2 )
	{
		iTotalWarThreshold *= 2;

		iDogpileWarThreshold += 2;
	}
	iTotalWarThreshold /= 3;
	iTotalWarThreshold += bAggressive ? 1 : 0;

	if( bAggressive && GC.getGameINLINE().getCurrentPeriod() < 3 )
	{
		iLimitedWarThreshold += 2;
	}
}

// Returns odds of player declaring total war times 100
int CvTeamAI::AI_getTotalWarOddsTimes100( ) const
{
	int iTotalWarRand;
	int iLimitedWarRand;
	int iDogpileWarRand;
	AI_getWarRands( iTotalWarRand, iLimitedWarRand, iDogpileWarRand );

	int iTotalWarThreshold;
	int iLimitedWarThreshold;
	int iDogpileWarThreshold;
	AI_getWarThresholds( iTotalWarThreshold, iLimitedWarThreshold, iDogpileWarThreshold );

	return ((100 * 100 * iTotalWarThreshold) / std::max(1, iTotalWarRand));
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

int CvTeamAI::AI_makePeaceTradeVal(TeamTypes ePeaceTeam, TeamTypes eTeam) const
{
	int iModifier;
	int iValue;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(ePeaceTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(GET_TEAM(ePeaceTeam).isAlive(), "GET_TEAM(ePeaceTeam).isAlive is expected to be true");
	FAssertMsg(atWar(ePeaceTeam, eTeam), "eTeam should be at war with ePeaceTeam");

	iValue = (50 + GC.getGameINLINE().getGameTurn());
	iValue += ((GET_TEAM(eTeam).getNumCities() + GET_TEAM(ePeaceTeam).getNumCities()) * 8);

	iModifier = 0;

	switch ((GET_TEAM(eTeam).AI_getAttitude(ePeaceTeam) + GET_TEAM(ePeaceTeam).AI_getAttitude(eTeam)) / 2)
	{
	case ATTITUDE_FURIOUS:
		iModifier += 400;
		break;

	case ATTITUDE_ANNOYED:
		iModifier += 200;
		break;

	case ATTITUDE_CAUTIOUS:
		iModifier += 100;
		break;

	case ATTITUDE_PLEASED:
		iModifier += 50;
		break;

	case ATTITUDE_FRIENDLY:
		break;

	default:
		FAssert(false);
		break;
	}

	iValue *= std::max(0, (iModifier + 100));
	iValue /= 100;

	iValue *= 40;
	iValue /= (GET_TEAM(eTeam).AI_getAtWarCounter(ePeaceTeam) + 10);

	iValue -= (iValue % GC.getDefineINT("DIPLOMACY_VALUE_REMAINDER"));

	if (isHuman())
	{
		return std::max(iValue, GC.getDefineINT("DIPLOMACY_VALUE_REMAINDER"));
	}
	else
	{
		return iValue;
	}
}


DenialTypes CvTeamAI::AI_makePeaceTrade(TeamTypes ePeaceTeam, TeamTypes eTeam) const
{
	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(ePeaceTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(GET_TEAM(ePeaceTeam).isAlive(), "GET_TEAM(ePeaceTeam).isAlive is expected to be true");
	FAssertMsg(isAtWar(ePeaceTeam), "should be at war with ePeaceTeam");
/************************************************************************************************/
/* Afforess	                  Start		 07/29/10                                               */
/*                                                                                              */
/* Advanced Diplomacy                                                                           */
/************************************************************************************************/
	if (GC.getGameINLINE().isOption(GAMEOPTION_ADVANCED_TACTICS))
	{
		if (isAtWar(eTeam))
		{
			return NO_DENIAL;
		}
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	if (GET_TEAM(ePeaceTeam).isHuman())
	{
		return DENIAL_CONTACT_THEM;
	}

	if (GET_TEAM(ePeaceTeam).isAVassal())
	{
		return DENIAL_VASSAL;
	}

	if (isHuman())
	{
		return NO_DENIAL;
	}

	if (!canChangeWarPeace(ePeaceTeam))
	{
		return DENIAL_VASSAL;
	}

	if (AI_endWarVal(ePeaceTeam) > (GET_TEAM(ePeaceTeam).AI_endWarVal(getID()) * 2))
	{
		return DENIAL_CONTACT_THEM;
	}

    int iLandRatio = ((getTotalLand(true) * 100) / std::max(20, GET_TEAM(eTeam).getTotalLand(true)));
    if (iLandRatio > 250)
    {
		return DENIAL_VICTORY;
	}

	return NO_DENIAL;
}


int CvTeamAI::AI_declareWarTradeVal(TeamTypes eWarTeam, TeamTypes eTeam) const
{
	PROFILE_FUNC();

	int iModifier;
	int iValue;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(eWarTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(GET_TEAM(eWarTeam).isAlive(), "GET_TEAM(eWarTeam).isAlive is expected to be true");
	FAssertMsg(!atWar(eWarTeam, eTeam), "eTeam should be at peace with eWarTeam");

	iValue = 0;
	iValue += (GET_TEAM(eWarTeam).getNumCities() * 10);
	iValue += (GET_TEAM(eWarTeam).getTotalPopulation(true) * 2);

	iModifier = 0;

	switch (GET_TEAM(eTeam).AI_getAttitude(eWarTeam))
	{
	case ATTITUDE_FURIOUS:
		break;

	case ATTITUDE_ANNOYED:
		iModifier += 25;
		break;

	case ATTITUDE_CAUTIOUS:
		iModifier += 50;
		break;

	case ATTITUDE_PLEASED:
		iModifier += 150;
		break;

	case ATTITUDE_FRIENDLY:
		iModifier += 400;
		break;

	default:
		FAssert(false);
		break;
	}

	iValue *= std::max(0, (iModifier + 100));
	iValue /= 100;

	int iTheirPower = GET_TEAM(eTeam).getPower(true);
	int iWarTeamPower = GET_TEAM(eWarTeam).getPower(true);

	iValue *= 50 + ((100 * iWarTeamPower) / (iTheirPower + iWarTeamPower + 1));
	iValue /= 100;

	if (!(GET_TEAM(eTeam).AI_isAllyLandTarget(eWarTeam)))
	{
		iValue *= 2;
	}

	if (!isAtWar(eWarTeam))
	{
		iValue *= 3;
	}
	else
	{
		iValue *= 150;
		iValue /= 100 + ((50 * std::min(100, (100 * AI_getWarSuccess(eWarTeam)) / (8 + getTotalPopulation(false)))) / 100);
	}

	iValue += (GET_TEAM(eTeam).getNumCities() * 20);
	iValue += (GET_TEAM(eTeam).getTotalPopulation(true) * 15);

	if (isAtWar(eWarTeam))
	{
		switch (GET_TEAM(eTeam).AI_getAttitude(getID()))
		{
		case ATTITUDE_FURIOUS:
		case ATTITUDE_ANNOYED:
		case ATTITUDE_CAUTIOUS:
			iValue *= 100;
			break;

		case ATTITUDE_PLEASED:
			iValue *= std::max(75, 100 - getAtWarCount(true) * 10);
			break;

		case ATTITUDE_FRIENDLY:
			iValue *= std::max(50, 100 - getAtWarCount(true) * 20);
			break;

		default:
			FAssert(false);
			break;
		}
		iValue /= 100;
	}

	iValue += GET_TEAM(eWarTeam).getNumNukeUnits() * 250;//Don't want to get nuked
	iValue += GET_TEAM(eTeam).getNumNukeUnits() * 150;//Don't want to use nukes on another's behalf

	if (GET_TEAM(eWarTeam).getAtWarCount(false) == 0)
	{
		iValue *= 2;

		for (int iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (iI != getID() && iI != eWarTeam && iI != eTeam)
				{
					if (GET_TEAM(eWarTeam).isDefensivePact((TeamTypes)iI))
					{
						iValue += (GET_TEAM((TeamTypes)iI).getNumCities() * 30);
						iValue += (GET_TEAM((TeamTypes)iI).getTotalPopulation(true) * 20);
					}
				}
			}
		}
	}

	iValue *= 60 + (140 * GC.getGameINLINE().getGameTurn()) / std::max(1, GC.getGameINLINE().getEstimateEndTurn());
	iValue /= 100;

	iValue -= (iValue % GC.getDefineINT("DIPLOMACY_VALUE_REMAINDER"));

	if (isHuman())
	{
		return std::max(iValue, GC.getDefineINT("DIPLOMACY_VALUE_REMAINDER"));
	}
	else
	{
		return iValue;
	}
}


DenialTypes CvTeamAI::AI_declareWarTrade(TeamTypes eWarTeam, TeamTypes eTeam, bool bConsiderPower) const
{
	PROFILE_FUNC();

	AttitudeTypes eAttitude;
	AttitudeTypes eAttitudeThem;
	bool bLandTarget;
	int iI;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(eWarTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(GET_TEAM(eWarTeam).isAlive(), "GET_TEAM(eWarTeam).isAlive is expected to be true");
	FAssertMsg(!isAtWar(eWarTeam), "should be at peace with eWarTeam");

	if (GET_TEAM(eWarTeam).isVassal(eTeam) || GET_TEAM(eWarTeam).isDefensivePact(eTeam))
	{
		return DENIAL_JOKING;
	}
/************************************************************************************************/
/* Afforess	                  Start		 04/06/10                                               */
/*  Ruthless AI: Refusing war when we are planning it anyway is silly                           */ 
/************************************************************************************************/
	if (AI_isChosenWar(eWarTeam))// && GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI))
	{
		return NO_DENIAL;
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	if (isHuman())
	{
		return NO_DENIAL;
	}

	if (!canDeclareWar(eWarTeam))
	{
		return DENIAL_VASSAL;
	}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      12/06/09                                jdog5000      */
/*                                                                                              */
/* Diplomacy                                                                                    */
/************************************************************************************************/
/* original BTS code
	if (getAnyWarPlanCount(true) > 0)
	{
		return DENIAL_TOO_MANY_WARS;
	}
*/
	if(!GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI))
	{
		if (getAnyWarPlanCount(true) > 0)
		{
			return DENIAL_TOO_MANY_WARS;
		}
	}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	if (bConsiderPower)
	{
		bLandTarget = AI_isAllyLandTarget(eWarTeam);

		if ((GET_TEAM(eWarTeam).getDefensivePower() / ((bLandTarget) ? 2 : 1)) >
			(getPower(true) + ((atWar(eWarTeam, eTeam)) ? GET_TEAM(eTeam).getPower(true) : 0)))
		{
			if (bLandTarget)
			{
				return DENIAL_POWER_THEM;
			}
			else
			{
				return DENIAL_NO_GAIN;
			}
		}
	}
/************************************************************************************************/
/* Afforess	                  Start		 04/06/10                                               */
/* Ruthless AI: Backstab enemies                                                                */
/************************************************************************************************/
	if ((AI_getMemoryCount(eWarTeam, MEMORY_DECLARED_WAR) > 0))// && GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI))
	{
		return NO_DENIAL;
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	eAttitude = AI_getAttitude(eTeam);

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
/************************************************************************************************/
/* Afforess	                  Start		 03/7/10                                                */
/* Ruthless AI: Attitude Doesn't Matter                                                         */
/************************************************************************************************/
				if (GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI) && eAttitude > ATTITUDE_FURIOUS)
				{
					continue;
				}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
				if (eAttitude <= GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getDeclareWarRefuseAttitudeThreshold())
				{
					return DENIAL_ATTITUDE;
				}
			}
		}
	}

	eAttitudeThem = AI_getAttitude(eWarTeam);

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				if (eAttitudeThem > GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getDeclareWarThemRefuseAttitudeThreshold())
				{
					return DENIAL_ATTITUDE_THEM;
				}
			}
		}
	}
	
	if (!atWar(eWarTeam, eTeam))
	{
		if (GET_TEAM(eWarTeam).getNumNukeUnits() > 0)
		{
			return DENIAL_JOKING;
		}
	}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      12/06/09                                jdog5000      */
/*                                                                                              */
/* Diplomacy                                                                                    */
/************************************************************************************************/
	if (!GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI))
	{
		if (getAnyWarPlanCount(true) > 0)
		{
			return DENIAL_TOO_MANY_WARS;
		}	
	}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	return NO_DENIAL;
}


int CvTeamAI::AI_openBordersTradeVal(TeamTypes eTeam) const
{
/************************************************************************************************/
/* Afforess	                  Start		 03/19/10                                               */
/* Ruthless AI                                                                                  */
/************************************************************************************************/
//Normal Firaxis calculation
	int iValue;
	iValue = (getNumCities() + GET_TEAM(eTeam).getNumCities());
	
	if (GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI))
	{
		//if we are planning war, but not against them
		if (AI_getWarPlan(eTeam) == NO_WARPLAN && getAnyWarPlanCount(true) > 0)
		{
			iValue *= 2;
		}
	}

	if (AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_RELIGION1))
	{
		iValue *= 2;
	}

	return iValue;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
}


DenialTypes CvTeamAI::AI_openBordersTrade(TeamTypes eTeam) const
{
	PROFILE_FUNC();

	AttitudeTypes eAttitude;
	int iI;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	if (isHuman())
	{
		return NO_DENIAL;
	}

	if (isVassal(eTeam))
	{
		return NO_DENIAL;
	}

	if (AI_shareWar(eTeam))
	{
		return NO_DENIAL;
	}

/************************************************************************************************/
/* Afforess	                  Start		 03/30/10                                               */
/* Ruthless AI: Get Open Borders with Nearby Allies, reject them with enemies                   */
/************************************************************************************************/
	//if (GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI))
	if (1 < 2)
	{
		bool bWarplans = getAnyWarPlanCount(true) > 0;
		if (AI_getWarPlan(eTeam) == NO_WARPLAN && bWarplans)
		{
			for (iI = 0; iI < MAX_TEAMS; iI++)
			{
				if (GET_TEAM((TeamTypes)iI).isAlive())
				{
					if (GET_TEAM((TeamTypes)iI).getID() != getID())
					{
						//victim
						if (AI_getWarPlan((TeamTypes)iI) != NO_WARPLAN)
						{
							//Share borders with our enemy
							if (GET_TEAM(eTeam).AI_teamCloseness((TeamTypes)iI) > 0)
							{
								return NO_DENIAL;
							}
						}
					}
				}
			}
		}
		//We are going to attack eTeam soon
		else if (bWarplans)
		{
			return DENIAL_MYSTERY;
		}
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	

	if (AI_getMemoryCount(eTeam, MEMORY_CANCELLED_OPEN_BORDERS) > 0)
	{
		return DENIAL_RECENT_CANCEL;
	}

	if (AI_getWorstEnemy() == eTeam)
	{
		return DENIAL_WORST_ENEMY;
	}

	eAttitude = AI_getAttitude(eTeam);

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				if (eAttitude <= GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getOpenBordersRefuseAttitudeThreshold())
				{
					return DENIAL_ATTITUDE;
				}
			}
		}
	}

	return NO_DENIAL;
}


int CvTeamAI::AI_defensivePactTradeVal(TeamTypes eTeam) const
{
	return ((getNumCities() + GET_TEAM(eTeam).getNumCities()) * 3);
}


DenialTypes CvTeamAI::AI_defensivePactTrade(TeamTypes eTeam) const
{
	PROFILE_FUNC();

	AttitudeTypes eAttitude;
	int iI;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	if (isHuman())
	{
		return NO_DENIAL;
	}

	if (GC.getGameINLINE().countCivTeamsAlive() == 2)
	{
		return DENIAL_NO_GAIN;
	}

	if (AI_getWorstEnemy() == eTeam)
	{
		return DENIAL_WORST_ENEMY;
	}

	eAttitude = AI_getAttitude(eTeam);

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				if (eAttitude <= GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getDefensivePactRefuseAttitudeThreshold())
				{
					return DENIAL_ATTITUDE;
				}
			}
		}
	}

	return NO_DENIAL;
}


DenialTypes CvTeamAI::AI_permanentAllianceTrade(TeamTypes eTeam) const
{
	PROFILE_FUNC();

	AttitudeTypes eAttitude;
	int iI;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	if (isHuman())
	{
		return NO_DENIAL;
	}

	if (AI_getWorstEnemy() == eTeam)
	{
		return DENIAL_WORST_ENEMY;
	}

	if ((getPower(true) + GET_TEAM(eTeam).getPower(true)) > (GC.getGameINLINE().countTotalCivPower() / 2))
	{
		if (getPower(true) > GET_TEAM(eTeam).getPower(true))
		{
			return DENIAL_POWER_US;
		}
		else
		{
			return DENIAL_POWER_YOU;
		}
	}

	if ((AI_getDefensivePactCounter(eTeam) + AI_getShareWarCounter(eTeam)) < 40)
	{
		return DENIAL_NOT_ALLIED;
	}

	eAttitude = AI_getAttitude(eTeam);

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				if (eAttitude <= GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getPermanentAllianceRefuseAttitudeThreshold())
				{
					return DENIAL_ATTITUDE;
				}
			}
		}
	}

	return NO_DENIAL;
}


TeamTypes CvTeamAI::AI_getWorstEnemy() const
{
	return m_eWorstEnemy;
}


void CvTeamAI::AI_updateWorstEnemy()
{
	PROFILE_FUNC();

	TeamTypes eBestTeam = NO_TEAM;
	int iBestValue = MAX_INT;

	for (int iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		TeamTypes eLoopTeam = (TeamTypes) iI;
		CvTeam& kLoopTeam = GET_TEAM(eLoopTeam);
		if (kLoopTeam.isAlive())
		{
			if (iI != getID() && !kLoopTeam.isVassal(getID()))
			{
				if (isHasMet(eLoopTeam))
				{
					if (AI_getAttitude(eLoopTeam) < ATTITUDE_CAUTIOUS)
					{
						int iValue = AI_getAttitudeVal(eLoopTeam);

						if (iValue < iBestValue)
						{
							iBestValue = iValue;
							eBestTeam = eLoopTeam;
						}
					}
				}
			}
		}
	}

	m_eWorstEnemy = eBestTeam;
}


int CvTeamAI::AI_getWarPlanStateCounter(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiWarPlanStateCounter[eIndex];
}


void CvTeamAI::AI_setWarPlanStateCounter(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiWarPlanStateCounter[eIndex] = iNewValue;
	FAssert(AI_getWarPlanStateCounter(eIndex) >= 0);
}


void CvTeamAI::AI_changeWarPlanStateCounter(TeamTypes eIndex, int iChange)
{
	AI_setWarPlanStateCounter(eIndex, (AI_getWarPlanStateCounter(eIndex) + iChange));
}


int CvTeamAI::AI_getAtWarCounter(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiAtWarCounter[eIndex];
}


void CvTeamAI::AI_setAtWarCounter(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiAtWarCounter[eIndex] = iNewValue;
	FAssert(AI_getAtWarCounter(eIndex) >= 0);
}


void CvTeamAI::AI_changeAtWarCounter(TeamTypes eIndex, int iChange)
{
	AI_setAtWarCounter(eIndex, (AI_getAtWarCounter(eIndex) + iChange));
}


int CvTeamAI::AI_getAtPeaceCounter(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiAtPeaceCounter[eIndex];
}


void CvTeamAI::AI_setAtPeaceCounter(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiAtPeaceCounter[eIndex] = iNewValue;
	FAssert(AI_getAtPeaceCounter(eIndex) >= 0);
}


void CvTeamAI::AI_changeAtPeaceCounter(TeamTypes eIndex, int iChange)
{
	AI_setAtPeaceCounter(eIndex, (AI_getAtPeaceCounter(eIndex) + iChange));
}


int CvTeamAI::AI_getHasMetCounter(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiHasMetCounter[eIndex];
}


void CvTeamAI::AI_setHasMetCounter(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiHasMetCounter[eIndex] = iNewValue;
	FAssert(AI_getHasMetCounter(eIndex) >= 0);
}


void CvTeamAI::AI_changeHasMetCounter(TeamTypes eIndex, int iChange)
{
	AI_setHasMetCounter(eIndex, (AI_getHasMetCounter(eIndex) + iChange));
}


int CvTeamAI::AI_getOpenBordersCounter(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiOpenBordersCounter[eIndex];
}


void CvTeamAI::AI_setOpenBordersCounter(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiOpenBordersCounter[eIndex] = iNewValue;
	FAssert(AI_getOpenBordersCounter(eIndex) >= 0);
}


void CvTeamAI::AI_changeOpenBordersCounter(TeamTypes eIndex, int iChange)
{
	AI_setOpenBordersCounter(eIndex, (AI_getOpenBordersCounter(eIndex) + iChange));
}


int CvTeamAI::AI_getDefensivePactCounter(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiDefensivePactCounter[eIndex];
}


void CvTeamAI::AI_setDefensivePactCounter(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiDefensivePactCounter[eIndex] = iNewValue;
	FAssert(AI_getDefensivePactCounter(eIndex) >= 0);
}


void CvTeamAI::AI_changeDefensivePactCounter(TeamTypes eIndex, int iChange)
{
	AI_setDefensivePactCounter(eIndex, (AI_getDefensivePactCounter(eIndex) + iChange));
}


int CvTeamAI::AI_getShareWarCounter(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiShareWarCounter[eIndex];
}


void CvTeamAI::AI_setShareWarCounter(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiShareWarCounter[eIndex] = iNewValue;
	FAssert(AI_getShareWarCounter(eIndex) >= 0);
}


void CvTeamAI::AI_changeShareWarCounter(TeamTypes eIndex, int iChange)
{
	AI_setShareWarCounter(eIndex, (AI_getShareWarCounter(eIndex) + iChange));
}


int CvTeamAI::AI_getWarSuccess(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiWarSuccess[eIndex];
}


void CvTeamAI::AI_setWarSuccess(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      09/03/09                       poyuzhe & jdog5000     */
/*                                                                                              */
/* Efficiency                                                                                   */
/************************************************************************************************/
	// From Sanguo Mod Performance, ie the CAR Mod
	// Attitude cache
	if (m_aiWarSuccess[eIndex] != iNewValue)
	{
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if( GET_PLAYER((PlayerTypes)iI).isAlive() )
			{
				if( GET_PLAYER((PlayerTypes)iI).getTeam() == getID() || GET_PLAYER((PlayerTypes)iI).getTeam() == eIndex )
				{
					for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
					{
						if( GET_PLAYER((PlayerTypes)iJ).isAlive() && GET_PLAYER((PlayerTypes)iJ).getTeam() != GET_PLAYER((PlayerTypes)iI).getTeam() )
						{
							if( GET_PLAYER((PlayerTypes)iJ).getTeam() == getID() || GET_PLAYER((PlayerTypes)iJ).getTeam() == eIndex )
							{
								GET_PLAYER((PlayerTypes)iJ).AI_invalidateAttitudeCache((PlayerTypes)iI);
								GET_PLAYER((PlayerTypes)iI).AI_invalidateAttitudeCache((PlayerTypes)iJ);
							}
						}
					}
				}
			}
		}
	}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	m_aiWarSuccess[eIndex] = iNewValue;
	FAssert(AI_getWarSuccess(eIndex) >= 0);
}


void CvTeamAI::AI_changeWarSuccess(TeamTypes eIndex, int iChange)
{
/************************************************************************************************/
/* REVOLUTION_MOD                         05/30/08                                jdog5000      */
/*                                                                                              */
/* Revolution AI                                                                                */
/************************************************************************************************/
	// Rebels celebrate successes more
	if( isRebelAgainst(eIndex) )
	{
		iChange *= 3;
		iChange /= 2;
	}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
	AI_setWarSuccess(eIndex, (AI_getWarSuccess(eIndex) + iChange));
}


int CvTeamAI::AI_getEnemyPeacetimeTradeValue(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiEnemyPeacetimeTradeValue[eIndex];
}


void CvTeamAI::AI_setEnemyPeacetimeTradeValue(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiEnemyPeacetimeTradeValue[eIndex] = iNewValue;
	FAssert(AI_getEnemyPeacetimeTradeValue(eIndex) >= 0);
}


void CvTeamAI::AI_changeEnemyPeacetimeTradeValue(TeamTypes eIndex, int iChange)
{
	AI_setEnemyPeacetimeTradeValue(eIndex, (AI_getEnemyPeacetimeTradeValue(eIndex) + iChange));
}


int CvTeamAI::AI_getEnemyPeacetimeGrantValue(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiEnemyPeacetimeGrantValue[eIndex];
}


void CvTeamAI::AI_setEnemyPeacetimeGrantValue(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiEnemyPeacetimeGrantValue[eIndex] = iNewValue;
	FAssert(AI_getEnemyPeacetimeGrantValue(eIndex) >= 0);
}


void CvTeamAI::AI_changeEnemyPeacetimeGrantValue(TeamTypes eIndex, int iChange)
{
	AI_setEnemyPeacetimeGrantValue(eIndex, (AI_getEnemyPeacetimeGrantValue(eIndex) + iChange));
}


WarPlanTypes CvTeamAI::AI_getWarPlan(TeamTypes eIndex) const
{
	FAssert(eIndex >= 0);
	FAssert(eIndex < MAX_TEAMS);
	FAssert(eIndex != getID() || m_aeWarPlan[eIndex] == NO_WARPLAN);
	return m_aeWarPlan[eIndex];
}


bool CvTeamAI::AI_isChosenWar(TeamTypes eIndex) const
{
	switch (AI_getWarPlan(eIndex))
	{
	case WARPLAN_ATTACKED_RECENT:
	case WARPLAN_ATTACKED:
		return false;
		break;
	case WARPLAN_PREPARING_LIMITED:
	case WARPLAN_PREPARING_TOTAL:
	case WARPLAN_LIMITED:
	case WARPLAN_TOTAL:
	case WARPLAN_DOGPILE:
		return true;
		break;
	}

	return false;
}


bool CvTeamAI::AI_isSneakAttackPreparing(TeamTypes eIndex) const
{
	return ((AI_getWarPlan(eIndex) == WARPLAN_PREPARING_LIMITED) || (AI_getWarPlan(eIndex) == WARPLAN_PREPARING_TOTAL));
}


bool CvTeamAI::AI_isSneakAttackReady(TeamTypes eIndex) const
{
	return (AI_isChosenWar(eIndex) && !(AI_isSneakAttackPreparing(eIndex)));
}


void CvTeamAI::AI_setWarPlan(TeamTypes eIndex, WarPlanTypes eNewValue, bool bWar)
{
	int iI;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (AI_getWarPlan(eIndex) != eNewValue)
	{
		if (bWar || !isAtWar(eIndex))
		{
			m_aeWarPlan[eIndex] = eNewValue;

			AI_setWarPlanStateCounter(eIndex, 0);

			AI_updateAreaStragies();

			for (iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
					{
						if (!(GET_PLAYER((PlayerTypes)iI).isHuman()))
						{
							GET_PLAYER((PlayerTypes)iI).AI_makeProductionDirty();
						}
					}
				}
			}
		}
	}
}

//if this number is over 0 the teams are "close"
//this may be expensive to run, kinda O(N^2)...
int CvTeamAI::AI_teamCloseness(TeamTypes eIndex, int iMaxDistance) const
{
	PROFILE_FUNC();
	int iI, iJ;
	
	if (iMaxDistance == -1)
	{
		iMaxDistance = DEFAULT_PLAYER_CLOSENESS;
	}
	
	FAssert(eIndex != getID());
	int iValue = 0;
	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive())
					{
						if (GET_PLAYER((PlayerTypes)iJ).getTeam() == eIndex)
						{
							iValue += GET_PLAYER((PlayerTypes)iI).AI_playerCloseness((PlayerTypes)iJ, iMaxDistance);
						}
					}
				}
			}
		}
	}

	return iValue;
}


void CvTeamAI::read(FDataStreamBase* pStream)
{
	CvTeam::read(pStream);

	uint uiFlag=0;
	pStream->Read(&uiFlag);	// flags for expansion

	pStream->Read(MAX_TEAMS, m_aiWarPlanStateCounter);
	pStream->Read(MAX_TEAMS, m_aiAtWarCounter);
	pStream->Read(MAX_TEAMS, m_aiAtPeaceCounter);
	pStream->Read(MAX_TEAMS, m_aiHasMetCounter);
	pStream->Read(MAX_TEAMS, m_aiOpenBordersCounter);
	pStream->Read(MAX_TEAMS, m_aiDefensivePactCounter);
	pStream->Read(MAX_TEAMS, m_aiShareWarCounter);
	pStream->Read(MAX_TEAMS, m_aiWarSuccess);
	pStream->Read(MAX_TEAMS, m_aiEnemyPeacetimeTradeValue);
	pStream->Read(MAX_TEAMS, m_aiEnemyPeacetimeGrantValue);

	pStream->Read(MAX_TEAMS, (int*)m_aeWarPlan);
	pStream->Read((int*)&m_eWorstEnemy);
}


void CvTeamAI::write(FDataStreamBase* pStream)
{
	CvTeam::write(pStream);

	uint uiFlag=0;
	pStream->Write(uiFlag);		// flag for expansion

	pStream->Write(MAX_TEAMS, m_aiWarPlanStateCounter);
	pStream->Write(MAX_TEAMS, m_aiAtWarCounter);
	pStream->Write(MAX_TEAMS, m_aiAtPeaceCounter);
	pStream->Write(MAX_TEAMS, m_aiHasMetCounter);
	pStream->Write(MAX_TEAMS, m_aiOpenBordersCounter);
	pStream->Write(MAX_TEAMS, m_aiDefensivePactCounter);
	pStream->Write(MAX_TEAMS, m_aiShareWarCounter);
	pStream->Write(MAX_TEAMS, m_aiWarSuccess);
	pStream->Write(MAX_TEAMS, m_aiEnemyPeacetimeTradeValue);
	pStream->Write(MAX_TEAMS, m_aiEnemyPeacetimeGrantValue);

	pStream->Write(MAX_TEAMS, (int*)m_aeWarPlan);
	pStream->Write(m_eWorstEnemy);
}

// Protected Functions...

int CvTeamAI::AI_noTechTradeThreshold() const
{
	int iRand;
	int iCount;
	int iI;

	iRand = 0;
	iCount = 0;
	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getNoTechTradeThreshold();
				iCount++;
			}
		}
	}

	if (iCount > 0)
	{
		iRand /= iCount;
	}
	
/************************************************************************************************/
/* Afforess	                  Start		 02/19/10                                               */
/* Ruthless AI: Trade More Techs                                                                */
/************************************************************************************************/
	//if (GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI))
		iRand *= 3;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	return iRand;
}


int CvTeamAI::AI_techTradeKnownPercent() const
{
	int iRand;
	int iCount;
	int iI;

	iRand = 0;
	iCount = 0;
	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getTechTradeKnownPercent();
				iCount++;
			}
		}
	}

	if (iCount > 0)
	{
		iRand /= iCount;
	}
/************************************************************************************************/
/* Afforess	                  Start		 02/19/10                                               */
/* Ruthless AI: Trade More Techs, even techs that others haven't discovered                     */
/************************************************************************************************/
	//if (GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI))
		iRand /= 3;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	return iRand;
}


int CvTeamAI::AI_maxWarRand() const
{
	int iRand;
	int iCount;
	int iI;

	iRand = 0;
	iCount = 0;
	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getMaxWarRand();
				iCount++;
			}
		}
	}

	if (iCount > 0)
	{
		iRand /= iCount;
	}

	return iRand;
}


int CvTeamAI::AI_maxWarNearbyPowerRatio() const
{
	int iRand;
	int iCount;
	int iI;

	iRand = 0;
	iCount = 0;
	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getMaxWarNearbyPowerRatio();
				iCount++;
			}
		}
	}

	if (iCount > 1)
	{
		iRand /= iCount;
	}
/************************************************************************************************/
/* Afforess	                  Start		 02/19/10                                               */
/* Ruthless AI: Attack Weaker, Closer targets                                                   */
/************************************************************************************************/
	//if (GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI))
	if (1 < 2)
	{
		iRand /= 2;
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/	
	

	return iRand;
}


int CvTeamAI::AI_maxWarDistantPowerRatio() const
{
	int iRand;
	int iCount;
	int iI;

	iRand = 0;
	iCount = 0;
	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getMaxWarDistantPowerRatio();
				iCount++;
			}
		}
	}

	if (iCount > 1)
	{
		iRand /= iCount;
	}
/************************************************************************************************/
/* Afforess	                  Start		 02/19/10                                               */
/* Ruthless AI: Avoid Far Away targets                                                          */
/************************************************************************************************/
	//if (GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI))
		iRand /= 3;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	return iRand;
}


int CvTeamAI::AI_maxWarMinAdjacentLandPercent() const
{
	int iRand;
	int iCount;
	int iI;

	iRand = 0;
	iCount = 0;
	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getMaxWarMinAdjacentLandPercent();
				iCount++;
			}
		}
	}

	if (iCount > 0)
	{
		iRand /= iCount;
	}

	return iRand;
}


int CvTeamAI::AI_limitedWarRand() const
{
	int iRand;
	int iCount;
	int iI;

	iRand = 0;
	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getLimitedWarRand();
				iCount++;
			}
		}
	}

	if (iCount > 0)
	{
		iRand /= iCount;
	}

	return iRand;
}


int CvTeamAI::AI_limitedWarPowerRatio() const
{
	int iRand;
	int iCount;
	int iI;

	iRand = 0;
	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getLimitedWarPowerRatio();
				iCount++;
			}
		}
	}

	if (iCount > 0)
	{
		iRand /= iCount;
	}

	return iRand;
}


int CvTeamAI::AI_dogpileWarRand() const
{
	int iRand;
	int iCount;
	int iI;

	iRand = 0;
	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getDogpileWarRand();
				iCount++;
			}
		}
	}

	if (iCount > 0)
	{
		iRand /= iCount;
	}

	return iRand;
}


int CvTeamAI::AI_makePeaceRand() const
{
	int iRand;
	int iCount;
	int iI;

	iRand = 0;
	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getMakePeaceRand();
				iCount++;
			}
		}
	}

	if (iCount > 0)
	{
		iRand /= iCount;
	}

	return iRand;
}


int CvTeamAI::AI_noWarAttitudeProb(AttitudeTypes eAttitude) const
{
	int iProb;
	int iCount;
	int iI;

	iProb = 0;
	iCount = 0;

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      03/20/10                                jdog5000      */
/*                                                                                              */
/* War Strategy AI                                                                              */
/************************************************************************************************/
	int iVictoryStrategyAdjust = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iProb += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getNoWarAttitudeProb(eAttitude);
				iCount++;

				// In final stages of miltaristic victory, AI may turn on its friends!
				if( GET_PLAYER((PlayerTypes)iI).AI_isDoVictoryStrategy(AI_VICTORY_CONQUEST4) )
				{
					iVictoryStrategyAdjust += 30;
				}
				else if( GET_PLAYER((PlayerTypes)iI).AI_isDoVictoryStrategy(AI_VICTORY_DOMINATION4) )
				{
					iVictoryStrategyAdjust += 20;
				}
			}
		}
	}

	if (iCount > 1)
	{
		iProb /= iCount;
		iVictoryStrategyAdjust /= iCount;
	}
	int iFinalCount = iProb - iVictoryStrategyAdjust;
	if (iFinalCount > 90)
	{
		iFinalCount = 90;
	}

	iProb = std::max( 0, iFinalCount);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

/************************************************************************************************/
/* Afforess	                  Start		 02/19/10                                               */
/* Ruthless AI: Friends are just enemies we haven't made yet.                                   */
/************************************************************************************************/
	if (GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI))
		iProb /= 5;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	return iProb;
}


void CvTeamAI::AI_doCounter()
{
	int iI;

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (iI != getID())
			{
				AI_changeWarPlanStateCounter(((TeamTypes)iI), 1);

				if (isAtWar((TeamTypes)iI))
				{
					AI_changeAtWarCounter(((TeamTypes)iI), 1);
				}
				else
				{
					AI_changeAtPeaceCounter(((TeamTypes)iI), 1);
				}

				if (isHasMet((TeamTypes)iI))
				{
					AI_changeHasMetCounter(((TeamTypes)iI), 1);
				}

				if (isOpenBorders((TeamTypes)iI))
				{
					AI_changeOpenBordersCounter(((TeamTypes)iI), 1);
				}

				if (isDefensivePact((TeamTypes)iI))
				{
					AI_changeDefensivePactCounter(((TeamTypes)iI), 1);
				}
				else
				{
					if (AI_getDefensivePactCounter((TeamTypes)iI) > 0)
					{
						AI_changeDefensivePactCounter(((TeamTypes)iI), -1);
					}
				}

				if (isHasMet((TeamTypes)iI))
				{
					if (AI_shareWar((TeamTypes)iI))
					{
						AI_changeShareWarCounter(((TeamTypes)iI), 1);
					}
				}
			}
		}
	}
}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      03/26/10                                jdog5000      */
/*                                                                                              */
/* War Strategy AI                                                                              */
/************************************************************************************************/
// Block AI from declaring war on a distant vassal if it shares an area with the master
bool CvTeamAI::AI_isOkayVassalTarget( TeamTypes eTeam )
{
	if( GET_TEAM(eTeam).isAVassal() )
	{
		if( !(AI_hasCitiesInPrimaryArea(eTeam)) || AI_calculateAdjacentLandPlots(eTeam) == 0 )
		{
			for( int iI = 0; iI < MAX_CIV_TEAMS; iI++ )
			{
				if( GET_TEAM(eTeam).isVassal((TeamTypes)iI) )
				{
					if( AI_hasCitiesInPrimaryArea((TeamTypes)iI) && AI_calculateAdjacentLandPlots((TeamTypes)iI) > 0)
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/


/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      04/25/10                                jdog5000      */
/*                                                                                              */
/* War Strategy, AI logging                                                                     */
/************************************************************************************************/			
/// \brief Make war decisions, mainly for starting or switching war plans.
///
///
// This function has been tweaked throughout by BBAI and K-Mod, some changes marked others not.
// (K-Mod has made several structural changes.)
void CvTeamAI::AI_doWar()
{
	PROFILE_FUNC();

	/* FAssert(!isHuman());
	FAssert(!isBarbarian());
	FAssert(!isMinorCiv());

	if (isAVassal())
	{
		return;
	} */ // disabled by K-Mod. All civs still need to do some basic updates.

	// allow python to handle it
	// allow python to handle it
	CyArgsList argsList;
	argsList.add(getID());
	long lResult=0;
	gDLL->getPythonIFace()->callFunction(PYGameModule, "AI_doWar", argsList.makeFunctionArgs(), &lResult);
	if (lResult == 1)
	{
		return;
	}

	int iEnemyPowerPercent = AI_getEnemyPowerPercent();

	// K-Mod note: This first section also used for vassals, and for human players.
	for (int iI = 0; iI < MAX_CIV_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && isHasMet((TeamTypes)iI))
		{
			if (AI_getWarPlan((TeamTypes)iI) != NO_WARPLAN)
			{
				int iTimeModifier = 100;

				int iAbandonTimeModifier = 100;
				iAbandonTimeModifier *= 50 + GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
				iAbandonTimeModifier /= 150;
				if (!isAtWar((TeamTypes)iI)) // K-Mod. time / abandon modifiers are only relevant for war preparations. We don't need them if we are already at war.
				{
					int iThreshold = (80*AI_maxWarNearbyPowerRatio())/100;

					if( iEnemyPowerPercent < iThreshold )
					{
						iTimeModifier *= iEnemyPowerPercent;
						iTimeModifier /= iThreshold;
					}
					// K-Mod
					// intercontinental wars need more prep time
					if (!AI_hasCitiesInPrimaryArea((TeamTypes)iI))
					{
						iTimeModifier *= 5;
						iTimeModifier /= 4;
						iAbandonTimeModifier *= 5;
						iAbandonTimeModifier /= 4;
						// maybe in the future I'll count the number of local cities and the number of overseas cities
						// and use it to make a more appropriate modifier... but not now.
					}
					else
					{
						//with crush strategy, use just 2/3 of the prep time.
						int iCrushMembers = AI_countMembersWithStrategy(AI_STRATEGY_CRUSH);
						iTimeModifier *= 3 * (getNumMembers()-iCrushMembers) + 2 * iCrushMembers;
						iTimeModifier /= 3;
					}
					// K-Mod end

					iTimeModifier *= 50 + GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
					iTimeModifier /= 150;

					FAssert(iTimeModifier >= 0);
				}

				bool bEnemyVictoryLevel4 = GET_TEAM((TeamTypes)iI).AI_isAnyMemberDoVictoryStrategyLevel4();

				if (AI_getWarPlan((TeamTypes)iI) == WARPLAN_ATTACKED_RECENT)
				{
					FAssert(isAtWar((TeamTypes)iI));

					if (AI_getAtWarCounter((TeamTypes)iI) > ((GET_TEAM((TeamTypes)iI).AI_isLandTarget(getID())) ? 9 : 3))
					{
						if( gTeamLogLevel >= 1 )
						{
							logBBAI("      Team %d (%S) switching WARPLANS against team %d (%S) from ATTACKED_RECENT to ATTACKED with enemy power percent %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), iI, GET_PLAYER(GET_TEAM((TeamTypes)iI).getLeaderID()).getCivilizationDescription(0), iEnemyPowerPercent );
						}
						AI_setWarPlan(((TeamTypes)iI), WARPLAN_ATTACKED);
					}
				}
				else if (AI_getWarPlan((TeamTypes)iI) == WARPLAN_PREPARING_LIMITED)
				{
					FAssert(canEventuallyDeclareWar((TeamTypes)iI));

					if (AI_getWarPlanStateCounter((TeamTypes)iI) > ((5 * iTimeModifier) / (bEnemyVictoryLevel4 ? 400 : 100)))
					{
						if (AI_startWarVal((TeamTypes)iI) > 0) // K-Mod. Last chance to change our mind if circumstances have changed
						{
							if( gTeamLogLevel >= 1 )
							{
								logBBAI("      Team %d (%S) switching WARPLANS against team %d (%S) from PREPARING_LIMITED to LIMITED after %d turns with enemy power percent %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), iI, GET_PLAYER(GET_TEAM((TeamTypes)iI).getLeaderID()).getCivilizationDescription(0), AI_getWarPlanStateCounter((TeamTypes)iI), iEnemyPowerPercent );
							}
							AI_setWarPlan(((TeamTypes)iI), WARPLAN_LIMITED);
						}
						// K-Mod
						else
						{
							if (gTeamLogLevel >= 1)
							{
								logBBAI("      Team %d (%S) abandoning WARPLAN_LIMITED against team %d (%S) after %d turns with enemy power percent %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), iI, GET_PLAYER(GET_TEAM((TeamTypes)iI).getLeaderID()).getCivilizationDescription(0), AI_getWarPlanStateCounter((TeamTypes)iI), iEnemyPowerPercent );
							}
						}
						// K-Mod end
					}
				}
				else if (AI_getWarPlan((TeamTypes)iI) == WARPLAN_LIMITED || AI_getWarPlan((TeamTypes)iI) == WARPLAN_DOGPILE)
				{
					if( !isAtWar((TeamTypes)iI) )
					{
						FAssert(canEventuallyDeclareWar((TeamTypes)iI));

						bool bActive = false;
						for( int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; iPlayer++ )
						{
							if( GET_PLAYER((PlayerTypes)iPlayer).getTeam() == getID() )
							{
								if( GET_PLAYER((PlayerTypes)iPlayer).AI_enemyTargetMissions((TeamTypes)iI) > 0 )
								{
									bActive = true;
									break;
								}
							}
						}

						if( !bActive )
						{
							if (AI_getWarPlanStateCounter((TeamTypes)iI) > ((15 * iAbandonTimeModifier) / (100)))
							{
								if( gTeamLogLevel >= 1 )
								{
									logBBAI("      Team %d (%S) abandoning WARPLAN_LIMITED or WARPLAN_DOGPILE against team %d (%S) after %d turns with enemy power percent %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), iI, GET_PLAYER(GET_TEAM((TeamTypes)iI).getLeaderID()).getCivilizationDescription(0), AI_getWarPlanStateCounter((TeamTypes)iI), iEnemyPowerPercent );
								}
								AI_setWarPlan(((TeamTypes)iI), NO_WARPLAN);
							}
						}

						if( AI_getWarPlan((TeamTypes)iI) == WARPLAN_DOGPILE )
						{
							if( GET_TEAM((TeamTypes)iI).getAtWarCount(true) == 0 )
							{
								if( gTeamLogLevel >= 1 )
								{
									logBBAI("      Team %d (%S) abandoning WARPLAN_DOGPILE against team %d (%S) after %d turns because enemy has no war with enemy power percent %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), iI, GET_PLAYER(GET_TEAM((TeamTypes)iI).getLeaderID()).getCivilizationDescription(0), AI_getWarPlanStateCounter((TeamTypes)iI), iEnemyPowerPercent );
								}
								AI_setWarPlan(((TeamTypes)iI), NO_WARPLAN);
							}
						}
					}
				}
				else if (AI_getWarPlan((TeamTypes)iI) == WARPLAN_PREPARING_TOTAL)
				{
					FAssert(canEventuallyDeclareWar((TeamTypes)iI));

					if (AI_getWarPlanStateCounter((TeamTypes)iI) > ((10 * iTimeModifier) / (bEnemyVictoryLevel4 ? 400 : 100)))
					{
						bool bAreaValid = false;
						bool bShareValid = false;

						int iLoop;
						for(CvArea* pLoopArea = GC.getMapINLINE().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMapINLINE().nextArea(&iLoop))
						{
							if (AI_isPrimaryArea(pLoopArea))
							{
								if (GET_TEAM((TeamTypes)iI).countNumCitiesByArea(pLoopArea) > 0)
								{
									bShareValid = true;

									AreaAITypes eAreaAI = AI_calculateAreaAIType(pLoopArea, true);

									/* BBAI code
									if ( eAreaAI == AREAAI_DEFENSIVE)
									{
										bAreaValid = false;
									}
									else if( eAreaAI == AREAAI_OFFENSIVE )
									{
										bAreaValid = true;
									} */
									// K-Mod. Doing it that way means the order the areas are checked is somehow important...
									if (eAreaAI == AREAAI_OFFENSIVE)
									{
										bAreaValid = true; // require at least one offense area
									}
									else if (eAreaAI == AREAAI_DEFENSIVE)
									{
										bAreaValid = false;
										break; // false if there are _any_ defence areas
									}
									// K-Mod end
								}
							}
						}

						if (((bAreaValid && iEnemyPowerPercent < 140) || (!bShareValid && iEnemyPowerPercent < 110) || GET_TEAM((TeamTypes)iI).AI_getLowestVictoryCountdown() >= 0) &&
							AI_startWarVal((TeamTypes)iI) > 0) // K-Mod. Last chance to change our mind if circumstances have changed
						{
							if( gTeamLogLevel >= 1 )
							{
								logBBAI("      Team %d (%S) switching WARPLANS against team %d (%S) from PREPARING_TOTAL to TOTAL after %d turns with enemy power percent %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), iI, GET_PLAYER(GET_TEAM((TeamTypes)iI).getLeaderID()).getCivilizationDescription(0), AI_getWarPlanStateCounter((TeamTypes)iI), iEnemyPowerPercent );
							}
							AI_setWarPlan(((TeamTypes)iI), WARPLAN_TOTAL);
						}
						else if (AI_getWarPlanStateCounter((TeamTypes)iI) > ((20 * iAbandonTimeModifier) / 100))
						{
							if( gTeamLogLevel >= 1 )
							{
								logBBAI("      Team %d (%S) abandoning WARPLAN_TOTAL_PREPARING against team %d (%S) after %d turns with enemy power percent %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), iI, GET_PLAYER(GET_TEAM((TeamTypes)iI).getLeaderID()).getCivilizationDescription(0), AI_getWarPlanStateCounter((TeamTypes)iI), iEnemyPowerPercent );
							}
							AI_setWarPlan(((TeamTypes)iI), NO_WARPLAN);
						}
					}
				}
				else if (AI_getWarPlan((TeamTypes)iI) == WARPLAN_TOTAL)
				{
					if( !isAtWar((TeamTypes)iI) )
					{
						FAssert(canEventuallyDeclareWar((TeamTypes)iI));

						bool bActive = false;
						for( int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; iPlayer++ )
						{
							if( GET_PLAYER((PlayerTypes)iPlayer).getTeam() == getID() )
							{
								if( GET_PLAYER((PlayerTypes)iPlayer).AI_enemyTargetMissions((TeamTypes)iI) > 0 )
								{
									bActive = true;
									break;
								}
							}
						}

						if( !bActive )
						{
							if (AI_getWarPlanStateCounter((TeamTypes)iI) > ((25 * iAbandonTimeModifier) / (100)))
							{
								if( gTeamLogLevel >= 1 )
								{
									logBBAI("      Team %d (%S) abandoning WARPLAN_TOTAL against team %d (%S) after %d turns with enemy power percent %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), iI, GET_PLAYER(GET_TEAM((TeamTypes)iI).getLeaderID()).getCivilizationDescription(0), AI_getWarPlanStateCounter((TeamTypes)iI), iEnemyPowerPercent );
								}
								AI_setWarPlan(((TeamTypes)iI), NO_WARPLAN);
							}
						}
					}
				}
			}
		}
	}

	// K-Mod. This is the end of the basics updates.
	// The rest of the stuff is related to making peace deals, and planning future wars.
	if (isHuman() || isBarbarian() || isMinorCiv() || isAVassal())
		return;
	// K-Mod end

	for (iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				GET_PLAYER((PlayerTypes)iI).AI_doPeace();
			}
		}
	}
	
	int iNumMembers = getNumMembers();
	/* original bts code
	int iHighUnitSpendingPercent = 0;
	int iLowUnitSpendingPercent = 0;
	
	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				int iUnitSpendingPercent = (GET_PLAYER((PlayerTypes)iI).calculateUnitCost() * 100) / std::max(1, GET_PLAYER((PlayerTypes)iI).calculatePreInflatedCosts());
				iHighUnitSpendingPercent += (std::max(0, iUnitSpendingPercent - 7) / 2);
				iLowUnitSpendingPercent += iUnitSpendingPercent;
			}			
		}
	}
	
	iHighUnitSpendingPercent /= iNumMembers;
	iLowUnitSpendingPercent /= iNumMembers; */ // K-Mod, this simply wasn't being used anywhere.

	// K-Mod. Gather some data...
	bool bAtWar = false;
	bool bTotalWarPlan = false;
	bool bAnyWarPlan = false;
	bool bLocalWarPlan = false;
	for (int i = 0; i < MAX_CIV_TEAMS; i++)
	{
		if (GET_TEAM((TeamTypes)i).isAlive() && !GET_TEAM((TeamTypes)i).isMinorCiv())
		{
			bAtWar = bAtWar || isAtWar((TeamTypes)i);

			switch (AI_getWarPlan((TeamTypes)i))
			{
			case NO_WARPLAN:
				break;
			case WARPLAN_PREPARING_TOTAL:
			case WARPLAN_TOTAL:
				bTotalWarPlan = true;
			default: // all other warplans
				bLocalWarPlan = bLocalWarPlan || AI_isLandTarget((TeamTypes)i);
				bAnyWarPlan = true;
				break;
			}
		}
	}
	// K-Mod end

	// if at war, check for making peace
	//if (getAtWarCount(true) > 0) // XXX
	if (bAtWar) // K-Mod
	{
		if (GC.getGameINLINE().getSorenRandNum(AI_makePeaceRand(), "AI Make Peace") == 0)
		{
			for (int iI = 0; iI < MAX_CIV_TEAMS; iI++)
			{
				if (GET_TEAM((TeamTypes)iI).isAlive())
				{
					if (iI != getID())
					{
						if (!(GET_TEAM((TeamTypes)iI).isHuman()))
						{
							if (canContact((TeamTypes)iI))
							{
								FAssert(!(GET_TEAM((TeamTypes)iI).isMinorCiv()));

								if (isAtWar((TeamTypes)iI))
								{
									if (AI_isChosenWar((TeamTypes)iI))
									{
										if( AI_getAtWarCounter((TeamTypes)iI) > std::max(10, (14 * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getVictoryDelayPercent())/100) )
										{
											// If nothing is happening in war
											if( AI_getWarSuccess((TeamTypes)iI) + GET_TEAM((TeamTypes)iI).AI_getWarSuccess(getID()) < 2*GC.getDefineINT("WAR_SUCCESS_ATTACKING") )
											{
												if( (GC.getGameINLINE().getSorenRandNum(8, "AI Make Peace 1") == 0) )
												{
													bool bValid = true;

													for( int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; iPlayer++ )
													{
														if( GET_PLAYER((PlayerTypes)iPlayer).getTeam() == getID() )
														{
															if( GET_PLAYER((PlayerTypes)iPlayer).AI_enemyTargetMissions((TeamTypes)iI) > 0 )
															{
																bValid = false;
																break;
															}
														}

														if( GET_PLAYER((PlayerTypes)iPlayer).getTeam() == iI )
														{
															//MissionAITypes eMissionAI = MISSIONAI_ASSAULT;
															if( GET_PLAYER((PlayerTypes)iPlayer).AI_enemyTargetMissions(getID()) > 0 )
															{
																bValid = false;
																break;
															}
														}
													}

													if( bValid )
													{
														makePeace((TeamTypes)iI);

														if( gTeamLogLevel >= 1 )
														{
															logBBAI("  Team %d (%S) making peace due to time and no fighting", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0) );
														}

														continue;
													}
												}
											}

											// Fought to a long draw
											if (AI_getAtWarCounter((TeamTypes)iI) > ((((AI_getWarPlan((TeamTypes)iI) == WARPLAN_TOTAL) ? 40 : 30) * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getVictoryDelayPercent())/100) )
											{
												int iOurValue = AI_endWarVal((TeamTypes)iI);
												int iTheirValue = GET_TEAM((TeamTypes)iI).AI_endWarVal(getID());
												if ((iOurValue > (iTheirValue / 2)) && (iTheirValue > (iOurValue / 2)))
												{
													if( gTeamLogLevel >= 1 )
													{
														logBBAI("  Team %d (%S) making peace due to time and endWarVal %d vs their %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0) , iOurValue, iTheirValue );
													}
													makePeace((TeamTypes)iI);
													continue;
												}
											}

											// All alone in what we thought was a dogpile
											if (AI_getWarPlan((TeamTypes)iI) == WARPLAN_DOGPILE)
											{
												if (GET_TEAM((TeamTypes)iI).getAtWarCount(true) == 1)
												{
													int iOurValue = AI_endWarVal((TeamTypes)iI);
													int iTheirValue = GET_TEAM((TeamTypes)iI).AI_endWarVal(getID());
													if ((iTheirValue > (iOurValue / 2)))
													{
														if( gTeamLogLevel >= 1 )
														{
															logBBAI("  Team %d (%S) making peace due to being only dog-piler left", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0) );
														}
														makePeace((TeamTypes)iI);
														continue;
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// if no war plans, consider starting one!
	//if (getAnyWarPlanCount(true) == 0 || iEnemyPowerPercent < 45)
	if (!bAnyWarPlan || (iEnemyPowerPercent < 45 && !(bLocalWarPlan && bTotalWarPlan) && AI_getWarSuccessRating() > (bTotalWarPlan ? 40 : 15))) // K-Mod
	{
		bool bAggressive = GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI);

		int iFinancialTroubleCount = 0;
		int iDaggerCount = 0;
		int iGetBetterUnitsCount = 0;
		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
				{
					if ( GET_PLAYER((PlayerTypes)iI).AI_isDoStrategy(AI_STRATEGY_DAGGER)
						|| GET_PLAYER((PlayerTypes)iI).AI_isDoVictoryStrategy(AI_VICTORY_CONQUEST3)
						|| GET_PLAYER((PlayerTypes)iI).AI_isDoVictoryStrategy(AI_VICTORY_DOMINATION4) )
					{
						iDaggerCount++;
						bAggressive = true;
					}

					if (GET_PLAYER((PlayerTypes)iI).AI_isDoStrategy(AI_STRATEGY_GET_BETTER_UNITS))
					{
						iGetBetterUnitsCount++;
					}
					
					if (GET_PLAYER((PlayerTypes)iI).AI_isFinancialTrouble())
					{
						iFinancialTroubleCount++;
					}
				}
			}
		}

	    // if random in this range is 0, we go to war of this type (so lower numbers are higher probablity)
		// average of everyone on our team
		int iTotalWarRand;
	    int iLimitedWarRand;
	    int iDogpileWarRand;
		AI_getWarRands( iTotalWarRand, iLimitedWarRand, iDogpileWarRand );

		int iTotalWarThreshold;
		int iLimitedWarThreshold;
		int iDogpileWarThreshold;
		AI_getWarThresholds( iTotalWarThreshold, iLimitedWarThreshold, iDogpileWarThreshold );
				
		// we oppose war if half the non-dagger teammates in financial trouble
		bool bFinancesOpposeWar = false;
		if ((iFinancialTroubleCount - iDaggerCount) >= std::max(1, getNumMembers() / 2 ))
		{
			// this can be overridden by by the pro-war booleans
			bFinancesOpposeWar = true;
		}

		// if agressive, we may start a war to get money
		bool bFinancesProTotalWar = false;
		bool bFinancesProLimitedWar = false;
		bool bFinancesProDogpileWar = false;
		if (iFinancialTroubleCount > 0)
		{
			// do we like all out wars?
			if (iDaggerCount > 0 || iTotalWarRand < 100)
			{
				bFinancesProTotalWar = true;
			}

			// do we like limited wars?
			if (iLimitedWarRand < 100)
			{
				bFinancesProLimitedWar = true;
			}
			
			// do we like dogpile wars?
			if (iDogpileWarRand < 100)
			{
				bFinancesProDogpileWar = true;
			}
		}
		bool bFinancialProWar = (bFinancesProTotalWar || bFinancesProLimitedWar || bFinancesProDogpileWar);
		
		// overall war check (quite frequently true)
		bool bMakeWarChecks = false;
		if ((iGetBetterUnitsCount - iDaggerCount) * 3 < iNumMembers * 2)
		{
			if (bFinancialProWar || !bFinancesOpposeWar)
			{
				// random overall war chance (at noble+ difficulties this is 100%)
				if (GC.getGameINLINE().getSorenRandNum(100, "AI Declare War 1") < GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIDeclareWarProb())
				{
					bMakeWarChecks = true;
				}
			}
		}
		
		if (bMakeWarChecks)
		{
			int iOurPower = getPower(true);

			if (bAggressive && (getAnyWarPlanCount(true) == 0))
			{
				iOurPower *= 4;
				iOurPower /= 3;
			}

			iOurPower *= (100 - iEnemyPowerPercent);
			iOurPower /= 100;

			if ((bFinancesProTotalWar || !bFinancesOpposeWar) &&
				(GC.getGameINLINE().getSorenRandNum(iTotalWarRand, "AI Maximum War") <= iTotalWarThreshold))
			{
				int iNoWarRoll = GC.getGameINLINE().getSorenRandNum(100, "AI No War");
				iNoWarRoll = range(iNoWarRoll + (bAggressive ? 10 : 0) + (bFinancesProTotalWar ? 10 : 0) - (20*iGetBetterUnitsCount)/iNumMembers, 0, 99);

				int iBestValue = 75; // minimum value before we start warplans
				TeamTypes eBestTeam = NO_TEAM;

				for (int iPass = 0; iPass < 3; iPass++)
				{
					for (int iI = 0; iI < MAX_CIV_TEAMS; iI++)
					{
						if (canEventuallyDeclareWar((TeamTypes)iI) && AI_haveSeenCities((TeamTypes)iI))
						{
							TeamTypes eLoopMasterTeam = GET_TEAM((TeamTypes)iI).getMasterTeam(); // K-Mod (plus all changes which refer to this variable).
							bool bVassal = eLoopMasterTeam != iI;

							if (bVassal && !AI_isOkayVassalTarget((TeamTypes)iI))
								continue;

							if (iNoWarRoll >= AI_noWarAttitudeProb(AI_getAttitude((TeamTypes)iI)) && (!bVassal || iNoWarRoll >= AI_noWarAttitudeProb(AI_getAttitude(eLoopMasterTeam))))
							{
								int iDefensivePower = (GET_TEAM((TeamTypes)iI).getDefensivePower() * 2) / 3;

								if (iDefensivePower < ((iOurPower * ((iPass > 1) ? AI_maxWarDistantPowerRatio() : AI_maxWarNearbyPowerRatio())) / 100))
								{
									// XXX make sure they share an area....

									FAssertMsg(!(GET_TEAM((TeamTypes)iI).isBarbarian()), "Expected to not be declaring war on the barb civ");
									FAssertMsg(iI != getID(), "Expected not to be declaring war on self (DOH!)");

									if ((iPass > 1 && !bLocalWarPlan) || AI_isLandTarget((TeamTypes)iI) || AI_isAnyCapitalAreaAlone() || GET_TEAM((TeamTypes)iI).AI_isAnyMemberDoVictoryStrategyLevel4())
									{
										if ((iPass > 0) || (AI_calculateAdjacentLandPlots((TeamTypes)iI) >= ((getTotalLand() * AI_maxWarMinAdjacentLandPercent()) / 100)) || GET_TEAM((TeamTypes)iI).AI_isAnyMemberDoVictoryStrategyLevel4())
										{
											int iValue = AI_startWarVal((TeamTypes)iI);

											if( iValue > 0 && gTeamLogLevel >= 2 )
											{
												logBBAI("      Team %d (%S) considering starting TOTAL warplan with team %d with value %d on pass %d with %d adjacent plots", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), iI, iValue, iPass, AI_calculateAdjacentLandPlots((TeamTypes)iI) );
												logBBAI("          Our Power: %d  --  Their Defensive Power: %d", iOurPower, iDefensivePower);
												logBBAI("          Current Attitude: %S", GC.getAttitudeInfo(AI_getAttitude((TeamTypes)iI)).getDescription(0));
											}

											if (iValue > iBestValue)
											{
												iBestValue = iValue;
												eBestTeam = ((TeamTypes)iI);
											}
										}
									}
								}
							}
						}
					}

					if (eBestTeam != NO_TEAM)
					{
						if( gTeamLogLevel >= 1 )
						{
							logBBAI("    Team %d (%S) starting TOTAL warplan preparations against team %d on pass %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eBestTeam, iPass );
						}

						AI_setWarPlan(eBestTeam, (iDaggerCount > 0) ? WARPLAN_TOTAL : WARPLAN_PREPARING_TOTAL);
						break;
					}
				}
			}
/************************************************************************************************/
/* UNOFFICIAL_PATCH                       01/02/09                                jdog5000      */
/*                                                                                              */
/* Bugfix                                                                                       */
/************************************************************************************************/
			else if ((bFinancesProLimitedWar || !bFinancesOpposeWar) &&
				(GC.getGameINLINE().getSorenRandNum(iLimitedWarRand, "AI Limited War") <= iLimitedWarThreshold))
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/
			{
				int iNoWarRoll = GC.getGameINLINE().getSorenRandNum(100, "AI No War") - 10;
				iNoWarRoll = range(iNoWarRoll + (bAggressive ? 10 : 0) + (bFinancesProLimitedWar ? 10 : 0), 0, 99);

				int iBestValue = 50;
				TeamTypes eBestTeam = NO_TEAM;

				for (int iI = 0; iI < MAX_CIV_TEAMS; iI++)
				{
					if (canEventuallyDeclareWar((TeamTypes)iI) && AI_haveSeenCities((TeamTypes)iI))
					{
						TeamTypes eLoopMasterTeam = GET_TEAM((TeamTypes)iI).getMasterTeam(); // K-Mod (plus all changes which refer to this variable).
						bool bVassal = eLoopMasterTeam != iI;

						if (bVassal && !AI_isOkayVassalTarget((TeamTypes)iI))
							continue;

						if (iNoWarRoll >= AI_noWarAttitudeProb(AI_getAttitude((TeamTypes)iI)) && (!bVassal || iNoWarRoll >= AI_noWarAttitudeProb(AI_getAttitude(eLoopMasterTeam))))
						{
							// need to make sure we can reach the opponent
							// loop through units -> if assault -> if player can train -> if can reach opponent
							if (AI_isLandTarget((TeamTypes)iI) || (AI_isAnyCapitalAreaAlone() && GET_TEAM((TeamTypes)iI).AI_isAnyCapitalAreaAlone()))
							{
								if (GET_TEAM((TeamTypes)iI).getDefensivePower() < ((iOurPower * AI_limitedWarPowerRatio()) / 100))
								{
									int iValue = AI_startWarVal((TeamTypes)iI);

									if( iValue > 0 && gTeamLogLevel >= 2 )
									{
										logBBAI("      Team %d (%S) considering starting LIMITED warplan with team %d with value %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), iI, iValue );
										//logBBAI("          Our Power: %d  --  Their Defensive Power: %d", iOurPower, iDefensivePower);
										logBBAI("          Current Attitude: %S", GC.getAttitudeInfo(AI_getAttitude((TeamTypes)iI)).getDescription(0));
									}

									if (iValue > iBestValue)
									{
										//FAssert(!AI_shareWar((TeamTypes)iI)); // disabled by K-Mod. (It isn't always true.)
										iBestValue = iValue;
										eBestTeam = ((TeamTypes)iI);
									}
								}
							}
						}
					}
				}

				if (eBestTeam != NO_TEAM)
				{
					if( gTeamLogLevel >= 1 )
					{
						logBBAI("    Team %d (%S) starting LIMITED warplan preparations against team %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eBestTeam );
					}

					AI_setWarPlan(eBestTeam, (iDaggerCount > 0) ? WARPLAN_LIMITED : WARPLAN_PREPARING_LIMITED);
				}
			}
			else if ((bFinancesProDogpileWar || !bFinancesOpposeWar) &&
				(GC.getGameINLINE().getSorenRandNum(iDogpileWarRand, "AI Dogpile War") <= iDogpileWarThreshold))
			{
				int iNoWarRoll = GC.getGameINLINE().getSorenRandNum(100, "AI No War") - 20;
				iNoWarRoll = range(iNoWarRoll + (bAggressive ? 10 : 0) + (bFinancesProDogpileWar ? 10 : 0), 0, 99);

				int iBestValue = 35;
				TeamTypes eBestTeam = NO_TEAM;

				for (int iI = 0; iI < MAX_CIV_TEAMS; iI++)
				{
					if (canDeclareWar((TeamTypes)iI) && AI_haveSeenCities((TeamTypes)iI))
					{
						TeamTypes eLoopMasterTeam = GET_TEAM((TeamTypes)iI).getMasterTeam(); // K-Mod (plus all changes which refer to this variable).
						bool bVassal = eLoopMasterTeam != iI;

						if (bVassal && !AI_isOkayVassalTarget((TeamTypes)iI))
							continue;

						if (iNoWarRoll >= AI_noWarAttitudeProb(AI_getAttitude((TeamTypes)iI)) && (!bVassal || iNoWarRoll >= AI_noWarAttitudeProb(AI_getAttitude(eLoopMasterTeam))))
						{
							if (GET_TEAM((TeamTypes)iI).getAtWarCount(true) > 0)
							{
								if (AI_isLandTarget((TeamTypes)iI) || GET_TEAM((TeamTypes)iI).AI_isAnyMemberDoVictoryStrategyLevel4())
								{
									int iDogpilePower = iOurPower;

									for (int iJ = 0; iJ < MAX_CIV_TEAMS; iJ++)
									{
										if (GET_TEAM((TeamTypes)iJ).isAlive())
										{
											if (iJ != iI)
											{
												if (atWar(((TeamTypes)iJ), ((TeamTypes)iI)))
												{
													iDogpilePower += GET_TEAM((TeamTypes)iJ).getPower(false);
												}
											}
										}
									}

									FAssert(GET_TEAM((TeamTypes)iI).getPower(true) == GET_TEAM((TeamTypes)iI).getDefensivePower() || GET_TEAM((TeamTypes)iI).isAVassal());

									if (((GET_TEAM((TeamTypes)iI).getDefensivePower() * 3) / 2) < iDogpilePower)
									{
										int iValue = AI_startWarVal((TeamTypes)iI);

										if( iValue > 0 && gTeamLogLevel >= 2 )
										{
											logBBAI("      Team %d (%S) considering starting DOGPILE warplan with team %d with value %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), iI, iValue );
											//logBBAI("          Our Power: %d  --  Their Defensive Power: %d", iOurPower, iDefensivePower);
											logBBAI("          Current Attitude: %S", GC.getAttitudeInfo(AI_getAttitude((TeamTypes)iI)).getDescription(0));
										}

										if (iValue > iBestValue)
										{
											//FAssert(!AI_shareWar((TeamTypes)iI)); // disabled by K-Mod. (why is this even here?)
											iBestValue = iValue;
											eBestTeam = ((TeamTypes)iI);
										}
									}
								}
							}
						}
					}
				}

				if (eBestTeam != NO_TEAM)
				{
					if( gTeamLogLevel >= 1 )
					{
						logBBAI("  Team %d (%S) starting DOGPILE warplan preparations with team %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eBestTeam );
					}
					AI_setWarPlan(eBestTeam, WARPLAN_DOGPILE);
				}
			}
		}
	}
}


//returns true if war is veto'd by rolls.
bool CvTeamAI::AI_performNoWarRolls(TeamTypes eTeam)
{

	if (GC.getGameINLINE().getSorenRandNum(100, "AI Declare War 1") > GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIDeclareWarProb())
	{
		return true;
	}

	if (GC.getGameINLINE().getSorenRandNum(100, "AI No War") <= AI_noWarAttitudeProb(AI_getAttitude(eTeam)))
	{
		return true;
	}



	return false;
}

int CvTeamAI::AI_getAttitudeWeight(TeamTypes eTeam) const
{
	int iAttitudeWeight = 0;
	switch (AI_getAttitude(eTeam))
	{
	case ATTITUDE_FURIOUS:
		iAttitudeWeight = -100;
		break;
	case ATTITUDE_ANNOYED:
		iAttitudeWeight = -40;
		break;
	case ATTITUDE_CAUTIOUS:
		iAttitudeWeight = -5;
		break;
	case ATTITUDE_PLEASED:
		iAttitudeWeight = 50;
		break;
	case ATTITUDE_FRIENDLY:
		iAttitudeWeight = 100;
		break;
	}

	return iAttitudeWeight;
}

int CvTeamAI::AI_getLowestVictoryCountdown() const
{
	int iBestVictoryCountdown = MAX_INT;
	for (int iVictory = 0; iVictory < GC.getNumVictoryInfos(); iVictory++)
	{
		 int iCountdown = getVictoryCountdown((VictoryTypes)iVictory);
		 if (iCountdown > 0)
		 {
			iBestVictoryCountdown = std::min(iBestVictoryCountdown, iCountdown);
		 }
	}
	if (MAX_INT == iBestVictoryCountdown)
	{
		iBestVictoryCountdown = -1;
	}
	return iBestVictoryCountdown;
}

int CvTeamAI::AI_getTechMonopolyValue(TechTypes eTech, TeamTypes eTeam) const
{
	int iValue = 0;
	int iI;

	bool bWarPlan = (getAnyWarPlanCount(eTeam) > 0);

	for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		UnitTypes eLoopUnit = ((UnitTypes)GC.getUnitClassInfo((UnitClassTypes)iI).getDefaultUnitIndex());

		if (eLoopUnit != NO_UNIT)
		{
			if (isTechRequiredForUnit((eTech), eLoopUnit))
			{
				if (isWorldUnitClass((UnitClassTypes)iI))
				{
					iValue += 50;
				}

				if (GC.getUnitInfo(eLoopUnit).getPrereqAndTech() == eTech)
				{
					int iNavalValue = 0;

					int iCombatRatio = (GC.getUnitInfo(eLoopUnit).getCombat() * 100) / std::max(1, GC.getGameINLINE().getBestLandUnitCombat());
					if (iCombatRatio > 50)
					{
						iValue += ((bWarPlan ? 100 : 50) * (iCombatRatio - 40)) / 50;
					}

					switch (GC.getUnitInfo(eLoopUnit).getDefaultUnitAIType())
					{
					case UNITAI_UNKNOWN:
					case UNITAI_ANIMAL:
					case UNITAI_SETTLE:
					case UNITAI_WORKER:
					break;

					case UNITAI_ATTACK:
					case UNITAI_ATTACK_CITY:
					case UNITAI_COLLATERAL:
						iValue += bWarPlan ? 50 : 20;
						break;

					case UNITAI_PILLAGE:
					case UNITAI_RESERVE:
					case UNITAI_COUNTER:
					case UNITAI_PARADROP:
					case UNITAI_CITY_DEFENSE:
					case UNITAI_CITY_COUNTER:
					case UNITAI_CITY_SPECIAL:
						iValue += bWarPlan ? 40 : 15;
						break;


					case UNITAI_EXPLORE:
					case UNITAI_MISSIONARY:
						break;

					case UNITAI_PROPHET:
					case UNITAI_ARTIST:
					case UNITAI_SCIENTIST:
					case UNITAI_GENERAL:
					case UNITAI_MERCHANT:
					case UNITAI_ENGINEER:
						break;

					case UNITAI_SPY:
						break;

					case UNITAI_ICBM:
						iValue += bWarPlan ? 80 : 40;
						break;

					case UNITAI_WORKER_SEA:
						break;

					case UNITAI_ATTACK_SEA:
						iNavalValue += 50;
						break;

					case UNITAI_RESERVE_SEA:
					case UNITAI_ESCORT_SEA:
						iNavalValue += 30;
						break;

					case UNITAI_EXPLORE_SEA:
						iValue += GC.getGame().circumnavigationAvailable() ? 100 : 0;
						break;

					case UNITAI_ASSAULT_SEA:
						iNavalValue += 60;
						break;

					case UNITAI_SETTLER_SEA:
					case UNITAI_MISSIONARY_SEA:
					case UNITAI_SPY_SEA:
						break;

					case UNITAI_CARRIER_SEA:
					case UNITAI_MISSILE_CARRIER_SEA:
						iNavalValue += 40;
						break;

					case UNITAI_PIRATE_SEA:
						iNavalValue += 20;
						break;

					case UNITAI_ATTACK_AIR:
					case UNITAI_DEFENSE_AIR:
						iValue += bWarPlan ? 60 : 30;
						break;

					case UNITAI_CARRIER_AIR:
						iNavalValue += 40;
						break;

					case UNITAI_MISSILE_AIR:
						iValue += bWarPlan ? 40 : 20;
						break;

					default:
//						FAssert(false);	//haven't added all new UNITAIs yet, Sephi
						break;
					}

					if (iNavalValue > 0)
					{
						if (AI_isAnyCapitalAreaAlone())
						{
							iValue += iNavalValue / 2;
						}
						if (bWarPlan && !AI_isLandTarget(eTeam))
						{
							iValue += iNavalValue / 2;
						}
					}
				}
			}
		}
	}

	for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (isTechRequiredForBuilding(eTech, ((BuildingTypes)iI)))
		{
			CvBuildingInfo& kLoopBuilding = GC.getBuildingInfo((BuildingTypes)iI);
			if (kLoopBuilding.getReligionType() == NO_RELIGION)
			{
				iValue += 30;
			}
			if (isWorldWonderClass((BuildingClassTypes)kLoopBuilding.getBuildingClassType()))
			{
				if (!(GC.getGameINLINE().isBuildingClassMaxedOut((BuildingClassTypes)kLoopBuilding.getBuildingClassType())))
				{
					iValue += 50;
				}
			}
		}
	}

	for (iI = 0; iI < GC.getNumProjectInfos(); iI++)
	{
		if (GC.getProjectInfo((ProjectTypes)iI).getTechPrereq() == eTech)
		{
			if (isWorldProject((ProjectTypes)iI))
			{
				if (!(GC.getGameINLINE().isProjectMaxedOut((ProjectTypes)iI)))
				{
					iValue += 100;
				}
			}
			else
			{
				iValue += 50;
			}
		}
	}

	return iValue;


}

bool CvTeamAI::AI_isWaterAreaRelevant(CvArea* pArea)
{
	int iTeamCities = 0;
	int iOtherTeamCities = 0;
	
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      01/15/09                                jdog5000      */
/*                                                                                              */
/* City AI                                                                                      */
/************************************************************************************************/
	CvArea* pBiggestArea = GC.getMap().findBiggestArea(true);
	if (pBiggestArea == pArea)
	{
		return true;
	}
	
	// An area is deemed relevant if it has at least 2 cities of our and different teams.
	// Also count lakes which are connected to ocean by a bridge city
	for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; iPlayer++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		
		if ((iTeamCities < 2 && kPlayer.getTeam() == getID()) || (iOtherTeamCities < 2 && kPlayer.getTeam() != getID()))
		{
			int iLoop;
			CvCity* pLoopCity;
			
			for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				if (pLoopCity->plot()->isAdjacentToArea(pArea->getID()))
				{
					if (kPlayer.getTeam() == getID())
					{
						iTeamCities++;
						
						if( pLoopCity->waterArea() == pBiggestArea )
						{
							return true;
						}
					}
					else
					{
						iOtherTeamCities++;
					}
				}				
			}
		}
		if (iTeamCities >= 2 && iOtherTeamCities >= 2)
		{
			return true;
		}
	}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/	

	return false;
}

// Private Functions...


/************************************************************************************************/
/* Afforess	                  Start		 08/6/10                                               */
/*                                                                                              */
/* Advanced Diplomacy                                                                           */
/************************************************************************************************/
DenialTypes CvTeamAI::AI_embassyTrade(TeamTypes eTeam) const
{
	PROFILE_FUNC();

	AttitudeTypes eAttitude;
	
	if (isHuman())
	{
		return NO_DENIAL;
	}

	if (isVassal(eTeam))
	{
		return NO_DENIAL;
	}

	if (AI_shareWar(eTeam))
	{
		return NO_DENIAL;
	}
	
	if (AI_getMemoryCount(eTeam, MEMORY_RECALLED_AMBASSADOR) > 0 && AI_getAttitude(eTeam) < ATTITUDE_PLEASED)
	{
		return DENIAL_RECENT_CANCEL;
	}

	if (AI_getWorstEnemy() == eTeam)
	{
		return DENIAL_WORST_ENEMY;
	}

	eAttitude = AI_getAttitude(eTeam);

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);

		if (kLoopPlayer.isAlive() && GET_TEAM(kLoopPlayer.getTeam()).getID() == getID())
		{
			if (eAttitude <= GC.getLeaderHeadInfo(kLoopPlayer.getPersonalityType()).getEmbassyRefuseAttitudeThreshold())
			{
				return DENIAL_ATTITUDE;
			}
		}
	}

	return NO_DENIAL;
}


DenialTypes CvTeamAI::AI_LimitedBordersTrade(TeamTypes eTeam) const
{
	PROFILE_FUNC();

	return AI_openBordersTrade(eTeam);
}


int CvTeamAI::AI_embassyTradeVal(TeamTypes eTeam) const
{
	int iValue = 0;

	iValue = (getNumCities() + GET_TEAM(eTeam).getNumCities());

	iValue *= 7;
	iValue /= 5;

	return std::max(0, iValue);
}


int CvTeamAI::AI_LimitedBordersTradeVal(TeamTypes eTeam) const
{
	int iValue = 0;

	iValue = (getNumCities() + GET_TEAM(eTeam).getNumCities());

//	iValue *= 2;
//	iValue /= 5;

	return std::max(0, iValue);
}

/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/