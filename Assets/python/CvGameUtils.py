## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
##
## Implementaion of miscellaneous game functions

import CvUtil
from CvPythonExtensions import *
import CvEventInterface
import CustomFunctions
import ScenarioFunctions

import PyHelpers
PyPlayer = PyHelpers.PyPlayer

# globals
cf = CustomFunctions.CustomFunctions()
gc = CyGlobalContext()
sf = ScenarioFunctions.ScenarioFunctions()



import CvModName

class CvGameUtils:
	"Miscellaneous game functions"
	def __init__(self):
		pass

	def isVictoryTest(self):
		return CyGame().getElapsedGameTurns() > 10

	def isVictory(self, argsList):
		eVictory = argsList[0]
		return True

	def isPlayerResearch(self, argsList):
		ePlayer = argsList[0]
		return True

	def getExtraCost(self, argsList):
		ePlayer = argsList[0]
		return 0

	def createBarbarianCities(self):
		return False

	def createBarbarianUnits(self):
		return False

	def skipResearchPopup(self,argsList):
		ePlayer = argsList[0]
		return False

	def showTechChooserButton(self,argsList):
		ePlayer = argsList[0]
		return True

	def getFirstRecommendedTech(self,argsList):
		ePlayer = argsList[0]
		return TechTypes.NO_TECH

	def getSecondRecommendedTech(self,argsList):
		ePlayer = argsList[0]
		eFirstTech = argsList[1]
		return TechTypes.NO_TECH

	def canRazeCity(self,argsList):
		iRazingPlayer, pCity = argsList
		return True

	def canDeclareWar(self,argsList):
		iAttackingTeam, iDefendingTeam = argsList
		return True

	def skipProductionPopup(self,argsList):
		pCity = argsList[0]
		return False

	def showExamineCityButton(self,argsList):
		pCity = argsList[0]
		return True

	def getRecommendedUnit(self,argsList):
		pCity = argsList[0]
		return UnitTypes.NO_UNIT

	def getRecommendedBuilding(self,argsList):
		pCity = argsList[0]
		return BuildingTypes.NO_BUILDING

	def updateColoredPlots(self):
		return False

	def isActionRecommended(self,argsList):
		pUnit = argsList[0]
		iAction = argsList[1]
		return False

	def unitCannotMoveInto(self,argsList):
		ePlayer = argsList[0]
		iUnitId = argsList[1]
		iPlotX = argsList[2]
		iPlotY = argsList[3]
		return False

	def cannotHandleAction(self,argsList):
		pPlot = argsList[0]
		iAction = argsList[1]
		bTestVisible = argsList[2]
		return False

	def canBuild(self,argsList):
		iX, iY, iBuild, iPlayer = argsList
		return -1	# Returning -1 means ignore; 0 means Build cannot be performed; 1 or greater means it can

	def cannotFoundCity(self,argsList):
		iPlayer, iPlotX, iPlotY = argsList
		return False

	def cannotSelectionListMove(self,argsList):
		pPlot = argsList[0]
		bAlt = argsList[1]
		bShift = argsList[2]
		bCtrl = argsList[3]
		return False

	def cannotSelectionListGameNetMessage(self,argsList):
		eMessage = argsList[0]
		iData2 = argsList[1]
		iData3 = argsList[2]
		iData4 = argsList[3]
		iFlags = argsList[4]
		bAlt = argsList[5]
		bShift = argsList[6]
		return False

	def cannotDoControl(self,argsList):
		eControl = argsList[0]
		return False

	def canResearch(self,argsList):
		ePlayer = argsList[0]
		eTech = argsList[1]
		bTrade = argsList[2]
		return False

	def cannotResearch(self,argsList):
		ePlayer = argsList[0]
		eTech = argsList[1]
		bTrade = argsList[2]
		pPlayer = gc.getPlayer(ePlayer)
		iCiv = pPlayer.getCivilizationType()
		eTeam = gc.getTeam(pPlayer.getTeam())

		bNonReligiousPlayer = False
		if pPlayer.hasTrait(gc.getInfoTypeForString('TRAIT_AGNOSTIC')):
			bNonReligiousPlayer = True
		elif pPlayer.isBarbarian():
			bNonReligiousPlayer = True

		if eTech == gc.getInfoTypeForString('TECH_ORDERS_FROM_HEAVEN'):
			if gc.getGame().isOption(GameOptionTypes.GAMEOPTION_NO_RELIGION_1):
				return True
#			if pPlayer.hasTrait(gc.getInfoTypeForString('TRAIT_AGNOSTIC')):
			if bNonReligiousPlayer:
				return True

		if eTech == gc.getInfoTypeForString('TECH_WAY_OF_THE_EARTHMOTHER'):
			if gc.getGame().isOption(GameOptionTypes.GAMEOPTION_NO_RELIGION_3):
				return True
#			if pPlayer.hasTrait(gc.getInfoTypeForString('TRAIT_AGNOSTIC')):
			if bNonReligiousPlayer:
				return True

		if eTech == gc.getInfoTypeForString('TECH_WAY_OF_THE_FORESTS'):
			if gc.getGame().isOption(GameOptionTypes.GAMEOPTION_NO_RELIGION_0):
				return True
#			if pPlayer.hasTrait(gc.getInfoTypeForString('TRAIT_AGNOSTIC')):
			if bNonReligiousPlayer:
				return True

		if eTech == gc.getInfoTypeForString('TECH_MESSAGE_FROM_THE_DEEP'):
			if gc.getGame().isOption(GameOptionTypes.GAMEOPTION_NO_RELIGION_2):
				return True
#			if pPlayer.hasTrait(gc.getInfoTypeForString('TRAIT_AGNOSTIC')):
			if bNonReligiousPlayer:
				return True

		if eTech == gc.getInfoTypeForString('TECH_CORRUPTION_OF_SPIRIT'):
			if gc.getGame().isOption(GameOptionTypes.GAMEOPTION_NO_RELIGION_4):
				return True
#			if pPlayer.hasTrait(gc.getInfoTypeForString('TRAIT_AGNOSTIC')):
			if bNonReligiousPlayer:
				return True

		if eTech == gc.getInfoTypeForString('TECH_HONOR'):
#			if pPlayer.hasTrait(gc.getInfoTypeForString('TRAIT_AGNOSTIC')):
			if bNonReligiousPlayer:
				return True

		if eTech == gc.getInfoTypeForString('TECH_DECEPTION'):
#			if pPlayer.hasTrait(gc.getInfoTypeForString('TRAIT_AGNOSTIC')):
			if bNonReligiousPlayer:
				return True

		if eTech == gc.getInfoTypeForString('TECH_SEAFARING'):
			if iCiv != gc.getInfoTypeForString('CIVILIZATION_LANUN'):
				return True

		if CyGame().getWBMapScript():
			bBlock = sf.cannotResearch(ePlayer, eTech, bTrade)
			if bBlock:
				return True

		return False

	def canDoCivic(self,argsList):
		ePlayer = argsList[0]
		eCivic = argsList[1]
		return False

	def cannotDoCivic(self,argsList):
		ePlayer = argsList[0]
		eCivic = argsList[1]
		return False

	def canTrain(self,argsList):
		pCity = argsList[0]
		eUnit = argsList[1]
		bContinue = argsList[2]
		bTestVisible = argsList[3]
		bIgnoreCost = argsList[4]
		bIgnoreUpgrades = argsList[5]
		return False

	def cannotTrain(self,argsList):
		pCity = argsList[0]
		eUnit = argsList[1]
		bContinue = argsList[2]
		bTestVisible = argsList[3]
		bIgnoreCost = argsList[4]
		bIgnoreUpgrades = argsList[5]
		ePlayer = pCity.getOwner()
		pPlayer = gc.getPlayer(ePlayer)
		eUnitClass = gc.getUnitInfo(eUnit).getUnitClassType()
		eTeam = gc.getTeam(pPlayer.getTeam())

		if eUnit == gc.getInfoTypeForString('UNIT_BEAST_OF_AGARES'):
			if pCity.getPopulation() <= 5:
				return True

		if pPlayer.getCivics(gc.getInfoTypeForString('CIVICOPTION_CULTURAL_VALUES')) == gc.getInfoTypeForString('CIVIC_CRUSADE'):
			if eUnit == gc.getInfoTypeForString('UNIT_WORKER'):
				return True
			if eUnit == gc.getInfoTypeForString('UNIT_SETTLER'):
				return True
			if eUnit == gc.getInfoTypeForString('UNIT_WORKBOAT'):
				return True

		if eUnit == gc.getInfoTypeForString('UNIT_ACHERON'):
			if gc.getGame().isOption(GameOptionTypes.GAMEOPTION_NO_ACHERON):
				return True

		if eUnit == gc.getInfoTypeForString('UNIT_DUIN'):
			if gc.getGame().isOption(GameOptionTypes.GAMEOPTION_NO_DUIN):
				return True

		if CyGame().getWBMapScript():
			bBlock = sf.cannotTrain(pCity, eUnit, bContinue, bTestVisible, bIgnoreCost, bIgnoreUpgrades)
			if bBlock:
				return True

		return False

	def canConstruct(self,argsList):
		pCity = argsList[0]
		eBuilding = argsList[1]
		bContinue = argsList[2]
		bTestVisible = argsList[3]
		bIgnoreCost = argsList[4]
		return False

	def cannotConstruct(self,argsList):
		pCity = argsList[0]
		eBuilding = argsList[1]
		bContinue = argsList[2]
		bTestVisible = argsList[3]
		bIgnoreCost = argsList[4]
		pPlayer = gc.getPlayer(pCity.getOwner())
		iBuildingClass = gc.getBuildingInfo(eBuilding).getBuildingClassType()
		eTeam = gc.getTeam(pPlayer.getTeam())

		if pPlayer.hasTrait(gc.getInfoTypeForString('TRAIT_AGNOSTIC')):
			if eBuilding == gc.getInfoTypeForString('BUILDING_TEMPLE_OF_LEAVES'):
				return True
			if eBuilding == gc.getInfoTypeForString('BUILDING_TEMPLE_OF_KILMORPH'):
				return True
			if eBuilding == gc.getInfoTypeForString('BUILDING_TEMPLE_OF_THE_EMPYREAN'):
				return True
			if eBuilding == gc.getInfoTypeForString('BUILDING_TEMPLE_OF_THE_OVERLORDS'):
				return True
			if eBuilding == gc.getInfoTypeForString('BUILDING_TEMPLE_OF_THE_VEIL'):
				return True
			if eBuilding == gc.getInfoTypeForString('BUILDING_TEMPLE_OF_THE_ORDER'):
				return True

		if pPlayer.getCivics(gc.getInfoTypeForString('CIVICOPTION_CULTURAL_VALUES')) == gc.getInfoTypeForString('CIVIC_CRUSADE'):
			if eBuilding == gc.getInfoTypeForString('BUILDING_ELDER_COUNCIL'):
				return True
			if eBuilding == gc.getInfoTypeForString('BUILDING_MARKET'):
				return True
			if eBuilding == gc.getInfoTypeForString('BUILDING_MONUMENT'):
				return True
			if eBuilding == gc.getInfoTypeForString('BUILDING_MONEYCHANGER'):
				return True
			if eBuilding == gc.getInfoTypeForString('BUILDING_THEATRE'):
				return True
			if eBuilding == gc.getInfoTypeForString('BUILDING_AQUEDUCT'):
				return True
			if eBuilding == gc.getInfoTypeForString('BUILDING_PUBLIC_BATHS'):
				return True
			if eBuilding == gc.getInfoTypeForString('BUILDING_HERBALIST'):
				return True
			if eBuilding == gc.getInfoTypeForString('BUILDING_CARNIVAL'):
				return True
			if eBuilding == gc.getInfoTypeForString('BUILDING_COURTHOUSE'):
				return True
			if eBuilding == gc.getInfoTypeForString('BUILDING_GAMBLING_HOUSE'):
				return True
			if eBuilding == gc.getInfoTypeForString('BUILDING_GRANARY'):
				return True
			if eBuilding == gc.getInfoTypeForString('BUILDING_SMOKEHOUSE'):
				return True
			if eBuilding == gc.getInfoTypeForString('BUILDING_LIBRARY'):
				return True
			if eBuilding == gc.getInfoTypeForString('BUILDING_HARBOR'):
				return True
			if eBuilding == gc.getInfoTypeForString('BUILDING_ALCHEMY_LAB'):
				return True
			if eBuilding == gc.getInfoTypeForString('BUILDING_BREWERY'):
				return True

		if eBuilding == gc.getInfoTypeForString('BUILDING_SHRINE_OF_THE_CHAMPION'):
			iHero = cf.getHero(pPlayer)
			if iHero == -1:
				return True
			if CyGame().isUnitClassMaxedOut(iHero, 0) == False:
				return True
			if pPlayer.getUnitClassCount(iHero) > 0:
				return True

		if eBuilding == gc.getInfoTypeForString('BUILDING_MERCURIAN_GATE'):
			if gc.getGame().isOption(GameOptionTypes.GAMEOPTION_NO_HYBOREM_OR_BASIUM):
				return True
			if pPlayer.getStateReligion() == gc.getInfoTypeForString('RELIGION_THE_ASHEN_VEIL'):
				return True
			if pCity.isCapital():
				return True

		iAltar1 = gc.getInfoTypeForString('BUILDING_ALTAR_OF_THE_LUONNOTAR')
		iAltar2 = gc.getInfoTypeForString('BUILDING_ALTAR_OF_THE_LUONNOTAR_ANOINTED')
		iAltar3 = gc.getInfoTypeForString('BUILDING_ALTAR_OF_THE_LUONNOTAR_BLESSED')
		iAltar4 = gc.getInfoTypeForString('BUILDING_ALTAR_OF_THE_LUONNOTAR_CONSECRATED')
		iAltar5 = gc.getInfoTypeForString('BUILDING_ALTAR_OF_THE_LUONNOTAR_DIVINE')
		iAltar6 = gc.getInfoTypeForString('BUILDING_ALTAR_OF_THE_LUONNOTAR_EXALTED')
		iAltar7 = gc.getInfoTypeForString('BUILDING_ALTAR_OF_THE_LUONNOTAR_FINAL')
		if (eBuilding == iAltar1 or eBuilding == iAltar2 or eBuilding == iAltar3 or eBuilding == iAltar4 or eBuilding == iAltar5 or eBuilding == iAltar6 or eBuilding == iAltar7):
			if pPlayer.getAlignment() == gc.getInfoTypeForString('ALIGNMENT_EVIL'):
				return True
			if eBuilding == iAltar1:
				if (pPlayer.countNumBuildings(iAltar2) > 0 or pPlayer.countNumBuildings(iAltar3) > 0 or pPlayer.countNumBuildings(iAltar4) > 0 or pPlayer.countNumBuildings(iAltar5) > 0 or pPlayer.countNumBuildings(iAltar6) > 0 or pPlayer.countNumBuildings(iAltar7) > 0):
					return True
			if eBuilding == iAltar2:
				if (pPlayer.countNumBuildings(iAltar3) > 0 or pPlayer.countNumBuildings(iAltar4) > 0 or pPlayer.countNumBuildings(iAltar5) > 0 or pPlayer.countNumBuildings(iAltar6) > 0 or pPlayer.countNumBuildings(iAltar7) > 0):
					return True
			if eBuilding == iAltar3:
				if (pPlayer.countNumBuildings(iAltar4) > 0 or pPlayer.countNumBuildings(iAltar5) > 0 or pPlayer.countNumBuildings(iAltar6) > 0 or pPlayer.countNumBuildings(iAltar7) > 0):
					return True
			if eBuilding == iAltar4:
				if (pPlayer.countNumBuildings(iAltar5) > 0 or pPlayer.countNumBuildings(iAltar6) > 0 or pPlayer.countNumBuildings(iAltar7) > 0):
					return True
			if eBuilding == iAltar5:
				if (pPlayer.countNumBuildings(iAltar6) > 0 or pPlayer.countNumBuildings(iAltar7) > 0):
					return True
			if eBuilding == iAltar6:
				if pPlayer.countNumBuildings(iAltar7) > 0:
					return True

### Start AI restrictions ###
		if pPlayer.isHuman() == False:
			if eBuilding == gc.getInfoTypeForString('BUILDING_PROPHECY_OF_RAGNAROK'):
				if pPlayer.getAlignment() != gc.getInfoTypeForString('ALIGNMENT_EVIL'):
					return True

			if eBuilding == gc.getInfoTypeForString('BUILDING_MERCURIAN_GATE'):
				if pCity.isHolyCity():
					return True
				if pCity.getAltarLevel() > 0:
					return True
				if pPlayer.getAlignment() == gc.getInfoTypeForString('ALIGNMENT_EVIL'):
					return True
### End AI restrictions ###

		if eBuilding == gc.getInfoTypeForString('BUILDING_SMUGGLERS_PORT'):
			if pPlayer.isSmugglingRing() == False:
				return True

		return False

	def canCreate(self,argsList):
		pCity = argsList[0]
		eProject = argsList[1]
		bContinue = argsList[2]
		bTestVisible = argsList[3]
		return False

	def cannotCreate(self,argsList):
		pCity = argsList[0]
		eProject = argsList[1]
		bContinue = argsList[2]
		bTestVisible = argsList[3]
		pPlayer = gc.getPlayer(pCity.getOwner())
		eTeam = gc.getTeam(pPlayer.getTeam())

		if eProject == gc.getInfoTypeForString('PROJECT_PURGE_THE_UNFAITHFUL'):
			if pPlayer.isHuman() == False:
				return True
			if pPlayer.getStateReligion() == -1:
				return True

		if eProject == gc.getInfoTypeForString('PROJECT_BIRTHRIGHT_REGAINED'):
			if not pPlayer.isFeatAccomplished(FeatTypes.FEAT_GLOBAL_SPELL):
				return True

		if eProject == gc.getInfoTypeForString('PROJECT_STIR_FROM_SLUMBER'):
			if pPlayer.getPlayersKilled() == 0:
				return True

		if eProject == gc.getInfoTypeForString('PROJECT_ASCENSION'):
			if pPlayer.getCivilizationType() != gc.getInfoTypeForString('CIVILIZATION_ILLIANS'):
				return True

		if eProject == gc.getInfoTypeForString('PROJECT_SAMHAIN'):
			if pPlayer.isHuman() == False:
				if pPlayer.getNumCities() <= 3:
					return True


		return False

	def canMaintain(self,argsList):
		pCity = argsList[0]
		eProcess = argsList[1]
		bContinue = argsList[2]
		return False

	def cannotMaintain(self,argsList):
		pCity = argsList[0]
		eProcess = argsList[1]
		bContinue = argsList[2]
		return False

	def AI_chooseTech(self,argsList):
		ePlayer = argsList[0]
		bFree = argsList[1]
		pPlayer = gc.getPlayer(ePlayer)

		return TechTypes.NO_TECH

	def AI_chooseProduction(self,argsList):
		pCity = argsList[0]
		ePlayer = pCity.getOwner()
		pPlayer = gc.getPlayer(ePlayer)
		pPlot = pCity.plot()

		## AI catches for buildings and projects that have python-only effects
		if not pPlayer.isHuman():
			## Illians - make sure we build our best projects
			if pPlayer.getCivilizationType() == gc.getInfoTypeForString('CIVILIZATION_ILLIANS'):
				if pCity.canConstruct(gc.getInfoTypeForString('BUILDING_TEMPLE_OF_THE_HAND'), True, False, False):
					iBadTileCount = 0
					for iiX in range(pCity.getX()-1, pCity.getX()+2, 1):
						for iiY in range(pCity.getY()-1, pCity.getY()+2, 1):
							pNearbyPlot = CyMap().plot(iiX,iiY)
							if (not pNearbyPlot.isWater()):
								if (pNearbyPlot.getYield(YieldTypes.YIELD_FOOD) < 2):
									iBadTileCount += 1
					if (iBadTileCount >= 4):
						pCity.pushOrder(OrderTypes.ORDER_CONSTRUCT,gc.getInfoTypeForString('BUILDING_TEMPLE_OF_THE_HAND'),-1, False, False, False, False)
						return 1
			
				if pCity.findYieldRateRank(YieldTypes.YIELD_PRODUCTION) < 3:
					if pCity.canCreate(gc.getInfoTypeForString('PROJECT_THE_WHITE_HAND'), True, True):
						pCity.pushOrder(OrderTypes.ORDER_CREATE,gc.getInfoTypeForString('PROJECT_THE_WHITE_HAND'),-1, False, False, False, False)
						return 1
					if pCity.canCreate(gc.getInfoTypeForString('PROJECT_ASCENSION'), True, True):
						pCity.pushOrder(OrderTypes.ORDER_CREATE,gc.getInfoTypeForString('PROJECT_ASCENSION'),-1, False, False, False, False)
						return 1
			## Clan should build Warrens
			if pPlayer.getCivilizationType() == gc.getInfoTypeForString('CIVILIZATION_CLAN_OF_EMBERS'):
				if (pCity.getCultureLevel() > 1) and (pCity.getPopulation() > 3):
					if pCity.canConstruct(gc.getInfoTypeForString('BUILDING_WARRENS'), True, False, False):
						pCity.pushOrder(OrderTypes.ORDER_CONSTRUCT,gc.getInfoTypeForString('BUILDING_WARRENS'),-1, False, False, False, False)
						return 1
			## Amurites should build Cave of Ancestors
			if pPlayer.getCivilizationType() == gc.getInfoTypeForString('CIVILIZATION_AMURITES'):
				if (pCity.getNumBuilding(gc.getInfoTypeForString('BUILDING_MAGE_GUILD')) > 0):
					if pCity.canConstruct(gc.getInfoTypeForString('BUILDING_CAVE_OF_ANCESTORS'), True, False, False):
						pCity.pushOrder(OrderTypes.ORDER_CONSTRUCT,gc.getInfoTypeForString('BUILDING_CAVE_OF_ANCESTORS'),-1, False, False, False, False)
						return 1
			## Demons should build Demon Altars
			#if gc.getCivilizationInfo(pPlayer.getCivilizationType()).getDefaultRace == gc.getInfoTypeForString('PROMOTION_DEMON'):
			if pPlayer.getCivilizationType() == gc.getInfoTypeForString('CIVILIZATION_INFERNAL'):
				if pCity.canConstruct(gc.getInfoTypeForString('BUILDING_DEMONS_ALTAR'), True, False, False):
						pCity.pushOrder(OrderTypes.ORDER_CONSTRUCT,gc.getInfoTypeForString('BUILDING_DEMONS_ALTAR'),-1, False, False, False, False)
						return 1

			if pCity.canTrain(gc.getInfoTypeForString('UNIT_HAWK'), True, False):
				if pPlot.countNumAirUnits(pPlayer.getTeam()) == 0:
					pCity.pushOrder(OrderTypes.ORDER_TRAIN, gc.getInfoTypeForString('UNIT_HAWK'), -1, False, False, False, False)
					return 1

		return False

	def AI_unitUpdate(self,argsList):
		pUnit = argsList[0]
		pPlot = pUnit.plot()
		iUnitType = pUnit.getUnitType()
		iPlayer = pUnit.getOwner()

		if iUnitType == gc.getInfoTypeForString('UNIT_GIANT_SPIDER'):
			iX = pUnit.getX()
			iY = pUnit.getY()
			for iiX in range(iX-1, iX+2, 1):
				for iiY in range(iY-1, iY+2, 1):
					pLoopPlot = CyMap().plot(iiX,iiY)
					for i in range(pLoopPlot.getNumUnits()):
						if pLoopPlot.getUnit(i).getOwner() != iPlayer:
							return 0
			pUnit.getGroup().pushMission(MissionTypes.MISSION_SKIP, 0, 0, 0, False, False, MissionAITypes.NO_MISSIONAI, pUnit.plot(), pUnit)
			return 1

		elif iUnitType == gc.getInfoTypeForString('UNIT_ACHERON'):
			if pPlot.isVisibleEnemyUnit(iPlayer):
				pUnit.cast(gc.getInfoTypeForString('SPELL_BREATH_FIRE'))

		# iImprovement = pPlot.getImprovementType()
		# if iImprovement != -1:
			# if (iImprovement == gc.getInfoTypeForString('IMPROVEMENT_BARROW') or iImprovement == gc.getInfoTypeForString('IMPROVEMENT_RUINS') or iImprovement == gc.getInfoTypeForString('IMPROVEMENT_HELLFIRE')):
				# if not pUnit.isAnimal():
					# if pPlot.getNumUnits() - pPlot.getNumAnimalUnits() == 1:
						# pUnit.getGroup().pushMission(MissionTypes.MISSION_SKIP, 0, 0, 0, False, False, MissionAITypes.NO_MISSIONAI, pUnit.plot(), pUnit)
						# return 1
			# if (iImprovement == gc.getInfoTypeForString('IMPROVEMENT_BEAR_DEN') or iImprovement == gc.getInfoTypeForString('IMPROVEMENT_LION_DEN')):
				# if pUnit.isAnimal():
					# if pPlot.getNumAnimalUnits() == 1:
						# pUnit.getGroup().pushMission(MissionTypes.MISSION_SKIP, 0, 0, 0, False, False, MissionAITypes.NO_MISSIONAI, pUnit.plot(), pUnit)
						# return 1
			# if iImprovement == gc.getInfoTypeForString('IMPROVEMENT_GOBLIN_FORT'):
				# if pUnit.getUnitCombatType() == gc.getInfoTypeForString('UNITCOMBAT_ARCHER'):
					# pUnit.getGroup().pushMission(MissionTypes.MISSION_SKIP, 0, 0, 0, False, False, MissionAITypes.NO_MISSIONAI, pUnit.plot(), pUnit)
					# return 1
				# if not pUnit.isAnimal():
					# if pPlot.getNumUnits() - pPlot.getNumAnimalUnits() <= 2:
						# pUnit.getGroup().pushMission(MissionTypes.MISSION_SKIP, 0, 0, 0, False, False, MissionAITypes.NO_MISSIONAI, pUnit.plot(), pUnit)
						# return 1

		return False

	def AI_doWar(self,argsList):
		eTeam = argsList[0]
		return False

	def AI_doDiplo(self,argsList):
		ePlayer = argsList[0]
		return False

	def calculateScore(self,argsList):
		ePlayer = argsList[0]
		bFinal = argsList[1]
		bVictory = argsList[2]

		iPopulationScore = CvUtil.getScoreComponent(gc.getPlayer(ePlayer).getPopScore(), CyGame().getInitPopulation(), CyGame().getMaxPopulation(), gc.getDefineINT("SCORE_POPULATION_FACTOR"), True, bFinal, bVictory)
		iLandScore = CvUtil.getScoreComponent(gc.getPlayer(ePlayer).getLandScore(), CyGame().getInitLand(), CyGame().getMaxLand(), gc.getDefineINT("SCORE_LAND_FACTOR"), True, bFinal, bVictory)
		iTechScore = CvUtil.getScoreComponent(gc.getPlayer(ePlayer).getTechScore(), CyGame().getInitTech(), CyGame().getMaxTech(), gc.getDefineINT("SCORE_TECH_FACTOR"), True, bFinal, bVictory)
		iWondersScore = CvUtil.getScoreComponent(gc.getPlayer(ePlayer).getWondersScore(), CyGame().getInitWonders(), CyGame().getMaxWonders(), gc.getDefineINT("SCORE_WONDER_FACTOR"), False, bFinal, bVictory)
		return int(iPopulationScore + iLandScore + iWondersScore + iTechScore)

	def doHolyCity(self):
		return False

	def doHolyCityTech(self,argsList):
		eTeam = argsList[0]
		ePlayer = argsList[1]
		eTech = argsList[2]
		bFirst = argsList[3]
		return False

	def doGold(self,argsList):
		ePlayer = argsList[0]
		return False

	def doResearch(self,argsList):
		ePlayer = argsList[0]
		return False

	def doGoody(self,argsList):
		ePlayer = argsList[0]
		pPlot = argsList[1]
		pUnit = argsList[2]
		return False

	def doGrowth(self,argsList):
		pCity = argsList[0]
		return False

	def doProduction(self,argsList):
		pCity = argsList[0]
		return False

	def doCulture(self,argsList):
		pCity = argsList[0]

		pPlayer = gc.getPlayer(pCity.getOwner())
		if pPlayer.isBarbarian():
			if pCity.getNumRealBuilding(gc.getInfoTypeForString('BUILDING_THE_DRAGONS_HORDE')) == 0:
				return 1
		return False

	def doPlotCulture(self,argsList):
		pCity = argsList[0]
		bUpdate = argsList[1]
		ePlayer = argsList[2]
		iCultureRate = argsList[3]
		return False

	def doReligion(self,argsList):
		pCity = argsList[0]
		return False

	def cannotSpreadReligion(self,argsList):
		iOwner, iUnitID, iReligion, iX, iY = argsList[0]
		return False

	def doGreatPeople(self,argsList):
		pCity = argsList[0]
		return False

	def doMeltdown(self,argsList):
		pCity = argsList[0]
		return False

	def doReviveActivePlayer(self,argsList):
		"allows you to perform an action after an AIAutoPlay"
		iPlayer = argsList[0]
		return False

	def doPillageGold(self, argsList):
		"controls the gold result of pillaging"
		pPlot = argsList[0]
		pUnit = argsList[1]

		iPillageGold = 0
		iPillageGold += CyGame().getSorenRandNum(gc.getImprovementInfo(pPlot.getImprovementType()).getPillageGold(), "Pillage Gold 1")
		iPillageGold += CyGame().getSorenRandNum(gc.getImprovementInfo(pPlot.getImprovementType()).getPillageGold(), "Pillage Gold 2")
		iPillageGold += (pUnit.getPillageChange() * iPillageGold) / 100
		return iPillageGold

	def doCityCaptureGold(self, argsList):
		"controls the gold result of capturing a city"
		pOldCity = argsList[0]
		iCaptureGold = 0
		iCaptureGold += gc.getDefineINT("BASE_CAPTURE_GOLD")
		iCaptureGold += (pOldCity.getPopulation() * gc.getDefineINT("CAPTURE_GOLD_PER_POPULATION"))
		iCaptureGold += CyGame().getSorenRandNum(gc.getDefineINT("CAPTURE_GOLD_RAND1"), "Capture Gold 1")
		iCaptureGold += CyGame().getSorenRandNum(gc.getDefineINT("CAPTURE_GOLD_RAND2"), "Capture Gold 2")
		if gc.getDefineINT("CAPTURE_GOLD_MAX_TURNS") > 0:
			iCaptureGold *= cyIntRange((CyGame().getGameTurn() - pOldCity.getGameTurnAcquired()), 0, gc.getDefineINT("CAPTURE_GOLD_MAX_TURNS"))
			iCaptureGold /= gc.getDefineINT("CAPTURE_GOLD_MAX_TURNS")
		return iCaptureGold

	def citiesDestroyFeatures(self,argsList):
		iX, iY= argsList
		return True

	def canFoundCitiesOnWater(self,argsList):
		iX, iY= argsList
		return False

	def doCombat(self,argsList):
		pSelectionGroup, pDestPlot = argsList
		return False

	def getConscriptUnitType(self, argsList):
		iPlayer = argsList[0]
		iConscriptUnitType = -1 #return this with the value of the UNIT TYPE you want to be conscripted, -1 uses default system
		return iConscriptUnitType

	def getCityFoundValue(self, argsList):
		iPlayer, iPlotX, iPlotY = argsList
		iFoundValue = -1 # Any value besides -1 will be used
		return iFoundValue

	def canPickPlot(self, argsList):
		pPlot = argsList[0]
		return True

	def getUnitCostMod(self, argsList):
		iPlayer, iUnit = argsList
		iCostMod = -1 # Any value > 0 will be used
		return iCostMod

	def getBuildingCostMod(self, argsList):
		iPlayer, iCityID, iBuilding = argsList
		pPlayer = gc.getPlayer(iPlayer)
		pCity = pPlayer.getCity(iCityID)
		iCostMod = -1 # Any value > 0 will be used
		if iBuilding == gc.getInfoTypeForString('BUILDING_GAMBLING_HOUSE'):
			if pPlayer.isGamblingRing():
				iCostMod = 25
		return iCostMod

	def canUpgradeAnywhere(self, argsList):
		pUnit = argsList
		bCanUpgradeAnywhere = 0
		return bCanUpgradeAnywhere

	def getWidgetHelp(self, argsList):
		eWidgetType, iData1, iData2, bOption = argsList
## Religion Screen ##
		if eWidgetType == WidgetTypes.WIDGET_HELP_RELIGION:
			if iData1 == -1:
				return CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ())
## Platy WorldBuilder ##
		elif eWidgetType == WidgetTypes.WIDGET_PYTHON:
			if iData1 == 1027:
				return CyTranslator().getText("TXT_KEY_WB_PLOT_DATA",())
			elif iData1 == 1028:
				return gc.getGameOptionInfo(iData2).getHelp()
			elif iData1 == 1029:
				if iData2 == 0:
					sText = CyTranslator().getText("TXT_KEY_WB_PYTHON", ())
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onFirstContact"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onChangeWar"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onVassalState"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onCityAcquired"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onCityBuilt"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onCultureExpansion"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onGoldenAge"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onEndGoldenAge"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onGreatPersonBorn"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onPlayerChangeStateReligion"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onReligionFounded"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onReligionSpread"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onReligionRemove"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onCorporationFounded"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onCorporationSpread"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onCorporationRemove"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onUnitCreated"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onUnitLost"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onUnitPromoted"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onBuildingBuilt"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onProjectBuilt"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onTechAcquired"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onImprovementBuilt"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onImprovementDestroyed"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onRouteBuilt"
					sText += "\n" + CyTranslator().getText("[ICON_BULLET]", ()) + "onPlotRevealed"
					return sText
				elif iData2 == 1:
					return CyTranslator().getText("TXT_KEY_WB_PLAYER_DATA",())
				elif iData2 == 2:
					return CyTranslator().getText("TXT_KEY_WB_TEAM_DATA",())
				elif iData2 == 3:
					return CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_TECH",())
				elif iData2 == 4:
					return CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_PROJECT",())
				elif iData2 == 5:
					return CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_UNIT", ()) + " + " + CyTranslator().getText("TXT_KEY_CONCEPT_CITIES", ())
				elif iData2 == 6:
					return CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_PROMOTION",())
				elif iData2 == 7:
					return CyTranslator().getText("TXT_KEY_WB_CITY_DATA2",())
				elif iData2 == 8:
					return CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_BUILDING",())
				elif iData2 == 9:
					return CvModName.getName() + '\nVersion: ' + CvModName.getVersion() +  "\nPlaty Builder\nVersion: 4.10"
				elif iData2 == 10:
					return CyTranslator().getText("TXT_KEY_CONCEPT_EVENTS",())
				elif iData2 == 11:
					return CyTranslator().getText("TXT_KEY_WB_RIVER_PLACEMENT",())
				elif iData2 == 12:
					return CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_IMPROVEMENT",())
				elif iData2 == 13:
					return CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_BONUS",())
				elif iData2 == 14:
					return CyTranslator().getText("TXT_KEY_WB_PLOT_TYPE",())
				elif iData2 == 15:
					return CyTranslator().getText("TXT_KEY_CONCEPT_TERRAIN",())
				elif iData2 == 16:
					return CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_ROUTE",())
				elif iData2 == 17:
					return CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_FEATURE",())
				elif iData2 == 18:
					return CyTranslator().getText("TXT_KEY_MISSION_BUILD_CITY",())
				elif iData2 == 19:
					return CyTranslator().getText("TXT_KEY_WB_ADD_BUILDINGS",())
				elif iData2 == 20:
					return CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_RELIGION",())
				elif iData2 == 21:
					return CyTranslator().getText("TXT_KEY_CONCEPT_CORPORATIONS",())
				elif iData2 == 22:
					return CyTranslator().getText("TXT_KEY_ESPIONAGE_CULTURE",())
				elif iData2 == 23:
					return CyTranslator().getText("TXT_KEY_PITBOSS_GAME_OPTIONS",())
				elif iData2 == 24:
					return CyTranslator().getText("TXT_KEY_WB_SENSIBILITY",())
				elif iData2 == 27:
					return CyTranslator().getText("TXT_KEY_WB_ADD_UNITS",())
				elif iData2 == 28:
					return CyTranslator().getText("TXT_KEY_WB_TERRITORY",())
				elif iData2 == 29:
					return CyTranslator().getText("TXT_KEY_WB_ERASE_ALL_PLOTS",())
				elif iData2 == 30:
					return CyTranslator().getText("TXT_KEY_WB_REPEATABLE",())
				elif iData2 == 31:
					return CyTranslator().getText("TXT_KEY_PEDIA_HIDE_INACTIVE", ())
				elif iData2 == 32:
					return CyTranslator().getText("TXT_KEY_WB_STARTING_PLOT", ())
				elif iData2 == 33:
					return CyTranslator().getText("TXT_KEY_INFO_SCREEN", ())
				elif iData2 == 34:
					return CyTranslator().getText("TXT_KEY_CONCEPT_TRADE", ())
			elif iData1 > 1029 and iData1 < 1040:
				if iData1 %2:
					return "-"
				return "+"
			elif iData1 == 6782:
				return CyGameTextMgr().parseCorporationInfo(iData2, False)
			elif iData1 == 6785:
				return CyGameTextMgr().getProjectHelp(iData2, False, CyCity())
			elif iData1 == 6787:
				return gc.getProcessInfo(iData2).getDescription()
			elif iData1 == 6788:
				if iData2 == -1:
					return CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ())
				return gc.getRouteInfo(iData2).getDescription()
## City Hover Text ##
			elif iData1 > 7199 and iData1 < 7300:
				iPlayer = iData1 - 7200
				pPlayer = gc.getPlayer(iPlayer)
				pCity = pPlayer.getCity(iData2)
				if CyGame().GetWorldBuilderMode():
					sText = "<font=3>"
					if pCity.isCapital():
						sText += CyTranslator().getText("[ICON_STAR]", ())
					elif pCity.isGovernmentCenter():
						sText += CyTranslator().getText("[ICON_SILVER_STAR]", ())
					sText += u"%s: %d<font=2>" %(pCity.getName(), pCity.getPopulation())
					sTemp = ""
					if pCity.isConnectedToCapital(iPlayer):
						sTemp += CyTranslator().getText("[ICON_TRADE]", ())
					for i in xrange(gc.getNumReligionInfos()):
						if pCity.isHolyCityByType(i):
							sTemp += u"%c" %(gc.getReligionInfo(i).getHolyCityChar())
						elif pCity.isHasReligion(i):
							sTemp += u"%c" %(gc.getReligionInfo(i).getChar())

					for i in xrange(gc.getNumCorporationInfos()):
						if pCity.isHeadquartersByType(i):
							sTemp += u"%c" %(gc.getCorporationInfo(i).getHeadquarterChar())
						elif pCity.isHasCorporation(i):
							sTemp += u"%c" %(gc.getCorporationInfo(i).getChar())
					if len(sTemp) > 0:
						sText += "\n" + sTemp

					iMaxDefense = pCity.getTotalDefense(False)
					if iMaxDefense > 0:
						sText += u"\n%s: " %(CyTranslator().getText("[ICON_DEFENSE]", ()))
						iCurrent = pCity.getDefenseModifier(False)
						if iCurrent != iMaxDefense:
							sText += u"%d/" %(iCurrent)
						sText += u"%d%%" %(iMaxDefense)

					sText += u"\n%s: %d/%d" %(CyTranslator().getText("[ICON_FOOD]", ()), pCity.getFood(), pCity.growthThreshold())
					iFoodGrowth = pCity.foodDifference(True)
					if iFoodGrowth != 0:
						sText += u" %+d" %(iFoodGrowth)

					if pCity.isProduction():
						sText += u"\n%s:" %(CyTranslator().getText("[ICON_PRODUCTION]", ()))
						if not pCity.isProductionProcess():
							sText += u" %d/%d" %(pCity.getProduction(), pCity.getProductionNeeded())
							iProduction = pCity.getCurrentProductionDifference(False, True)
							if iProduction != 0:
								sText += u" %+d" %(iProduction)
						sText += u" (%s)" %(pCity.getProductionName())
					
					iGPRate = pCity.getGreatPeopleRate()
					iProgress = pCity.getGreatPeopleProgress()
					if iGPRate > 0 or iProgress > 0:
						sText += u"\n%s: %d/%d %+d" %(CyTranslator().getText("[ICON_GREATPEOPLE]", ()), iProgress, pPlayer.greatPeopleThreshold(False), iGPRate)

					sText += u"\n%s: %d/%d (%s)" %(CyTranslator().getText("[ICON_CULTURE]", ()), pCity.getCulture(iPlayer), pCity.getCultureThreshold(), gc.getCultureLevelInfo(pCity.getCultureLevel()).getDescription())

					lTemp = []
					for i in xrange(CommerceTypes.NUM_COMMERCE_TYPES):
						iAmount = pCity.getCommerceRateTimes100(i)
						if iAmount <= 0: continue
						sTemp = u"%d.%02d%c" %(pCity.getCommerceRate(i), pCity.getCommerceRateTimes100(i)%100, gc.getCommerceInfo(i).getChar())
						lTemp.append(sTemp)
					if len(lTemp) > 0:
						sText += "\n"
						for i in xrange(len(lTemp)):
							sText += lTemp[i]
							if i < len(lTemp) - 1:
								sText += ", "

					iMaintenance = pCity.getMaintenanceTimes100()
					if iMaintenance != 0:
						sText += "\n" + CyTranslator().getText("[COLOR_WARNING_TEXT]", ()) + CyTranslator().getText("INTERFACE_CITY_MAINTENANCE", ()) + " </color>"
						sText += u"-%d.%02d%c" %(iMaintenance/100, iMaintenance%100, gc.getCommerceInfo(CommerceTypes.COMMERCE_GOLD).getChar())

					lBuildings = []
					lWonders = []
					for i in xrange(gc.getNumBuildingInfos()):
						if pCity.isHasBuilding(i):
							Info = gc.getBuildingInfo(i)
							if isLimitedWonderClass(Info.getBuildingClassType()):
								lWonders.append(Info.getDescription())
							else:
								lBuildings.append(Info.getDescription())
					if len(lBuildings) > 0:
						lBuildings.sort()
						sText += "\n" + CyTranslator().getText("[COLOR_BUILDING_TEXT]", ()) + CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_BUILDING", ()) + ": </color>"
						for i in xrange(len(lBuildings)):
							sText += lBuildings[i]
							if i < len(lBuildings) - 1:
								sText += ", "
					if len(lWonders) > 0:
						lWonders.sort()
						sText += "\n" + CyTranslator().getText("[COLOR_SELECTED_TEXT]", ()) + CyTranslator().getText("TXT_KEY_CONCEPT_WONDERS", ()) + ": </color>"
						for i in xrange(len(lWonders)):
							sText += lWonders[i]
							if i < len(lWonders) - 1:
								sText += ", "
					sText += "</font>"
					return sText
## Religion Widget Text##
			elif iData1 == 7869:
				return CyGameTextMgr().parseReligionInfo(iData2, False)
## Building Widget Text##
			elif iData1 == 7870:
				return CyGameTextMgr().getBuildingHelp(iData2, False, False, False, None)
## Tech Widget Text##
			elif iData1 == 7871:
				if iData2 == -1:
					return CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ())
				return CyGameTextMgr().getTechHelp(iData2, False, False, False, False, -1)
## Civilization Widget Text##
			elif iData1 == 7872:
				iCiv = iData2 % 10000
				return CyGameTextMgr().parseCivInfos(iCiv, False)
## Promotion Widget Text##
			elif iData1 == 7873:
				return CyGameTextMgr().getPromotionHelp(iData2, False)
## Feature Widget Text##
			elif iData1 == 7874:
				if iData2 == -1:
					return CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ())
				iFeature = iData2 % 10000
				return CyGameTextMgr().getFeatureHelp(iFeature, False)
## Terrain Widget Text##
			elif iData1 == 7875:
				return CyGameTextMgr().getTerrainHelp(iData2, False)
## Leader Widget Text##
			elif iData1 == 7876:
				iLeader = iData2 % 10000
				return CyGameTextMgr().parseLeaderTraits(iLeader, -1, False, False)
## Improvement Widget Text##
			elif iData1 == 7877:
				if iData2 == -1:
					return CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ())
				return CyGameTextMgr().getImprovementHelp(iData2, False)
## Bonus Widget Text##
			elif iData1 == 7878:
				if iData2 == -1:
					return CyTranslator().getText("TXT_KEY_CULTURELEVEL_NONE", ())
				return CyGameTextMgr().getBonusHelp(iData2, False)
## Specialist Widget Text##
			elif iData1 == 7879:
				return CyGameTextMgr().getSpecialistHelp(iData2, False)
## Yield Text##
			elif iData1 == 7880:
				return gc.getYieldInfo(iData2).getDescription()
## Commerce Text##
			elif iData1 == 7881:
				return gc.getCommerceInfo(iData2).getDescription()
## Corporation Screen ##
			elif iData1 == 8201:
				return CyGameTextMgr().parseCorporationInfo(iData2, False)
## Military Screen ##
			elif iData1 == 8202:
				if iData2 == -1:
					return CyTranslator().getText("TXT_KEY_PEDIA_ALL_UNITS", ())
				return CyGameTextMgr().getUnitHelp(iData2, False, False, False, None)
			elif iData1 > 8299 and iData1 < 8400:
				iPlayer = iData1 - 8300
				pUnit = gc.getPlayer(iPlayer).getUnit(iData2)
				sText = CyGameTextMgr().getSpecificUnitHelp(pUnit, True, False)
				if CyGame().GetWorldBuilderMode():
					sText += "\n" + CyTranslator().getText("TXT_KEY_WB_UNIT", ()) + " ID: " + str(iData2)
					sText += "\n" + CyTranslator().getText("TXT_KEY_WB_GROUP", ()) + " ID: " + str(pUnit.getGroupID())
					sText += "\n" + "X: " + str(pUnit.getX()) + ", Y: " + str(pUnit.getY())
					sText += "\n" + CyTranslator().getText("TXT_KEY_WB_AREA_ID", ()) + ": "  + str(pUnit.plot().getArea())
				return sText
## Civics Screen ##
			elif iData1 == 8205 or iData1 == 8206:
				sText = CyGameTextMgr().parseCivicInfo(iData2, False, True, False)
				if gc.getCivicInfo(iData2).getUpkeep() > -1:
					sText += "\n" + gc.getUpkeepInfo(gc.getCivicInfo(iData2).getUpkeep()).getDescription()
				else:
					sText += "\n" + CyTranslator().getText("TXT_KEY_CIVICS_SCREEN_NO_UPKEEP", ())
				return sText
#Magister Start
			elif iData1 == 9000:
				return CyGameTextMgr().parseTraits(iData2, CivilizationTypes.NO_CIVILIZATION, False )
			elif iData1 == 9001:
				return CyGameTextMgr().getSpellHelp(iData2, False)
#Magister Stop

## Ultrapack ##
		return u""

	def getUpgradePriceOverride(self, argsList):
		iPlayer, iUnitID, iUnitTypeUpgrade = argsList
		return -1	# Any value 0 or above will be used

	def getExperienceNeeded(self, argsList):
		# use this function to set how much experience a unit needs
		iLevel, iOwner = argsList
		iExperienceNeeded = 0
		# regular epic game experience
		iExperienceNeeded = iLevel * iLevel + 1
		iModifier = gc.getPlayer(iOwner).getLevelExperienceModifier()
		if 0 != iModifier:
			iExperienceNeeded += (iExperienceNeeded * iModifier + 99) / 100   # ROUND UP
		return iExperienceNeeded

##--------	Unofficial Bug Fix: Added by Denev 2009/12/31
	def applyBuildEffects(self, argsList):
		pUnit, pCity = argsList
		if pCity.getNumBuilding(gc.getInfoTypeForString('BUILDING_CHANCEL_OF_GUARDIANS')) > 0:
			if not pUnit.noDefensiveBonus() and pUnit.baseCombatStrDefense() > 0:
				if CyGame().getSorenRandNum(100, "Chancel of Guardians") < 20:
					pUnit.setHasPromotion(gc.getInfoTypeForString('PROMOTION_DEFENSIVE'), True)

		if pCity.getNumBuilding(gc.getInfoTypeForString('BUILDING_CAVE_OF_ANCESTORS')) > 0:
			if pUnit.getUnitCombatType() == gc.getInfoTypeForString('UNITCOMBAT_ADEPT'):
				iNumManaKinds = 0
				for iBonus in range(gc.getNumBonusInfos()):
					if pCity.hasBonus(iBonus):
						if gc.getBonusInfo(iBonus).getBonusClassType() == gc.getInfoTypeForString('BONUSCLASS_MANA'):
							iNumManaKinds += 1
				if iNumManaKinds > 0:
					pUnit.changeExperience(iNumManaKinds, -1, False, False, False)

		if pCity.getNumBuilding(gc.getInfoTypeForString('BUILDING_ASYLUM')) > 0:
			if pUnit.isAlive():
				if not isWorldUnitClass(pUnit.getUnitClassType()):
					if CyGame().getSorenRandNum(100, "Asylum") < 10:
						pUnit.setHasPromotion(gc.getInfoTypeForString('PROMOTION_CRAZED'), True)
						pUnit.setHasPromotion(gc.getInfoTypeForString('PROMOTION_ENRAGED'), True)

		if pUnit.getRace() == gc.getInfoTypeForString('PROMOTION_GOLEM'):
			if pCity.getNumBuilding(gc.getInfoTypeForString('BUILDING_BLASTING_WORKSHOP')) > 0:
				pUnit.setHasPromotion(gc.getInfoTypeForString('PROMOTION_FIRE2'), True)
			if pCity.getNumBuilding(gc.getInfoTypeForString('BUILDING_PALLENS_ENGINE')) > 0:
				pUnit.setHasPromotion(gc.getInfoTypeForString('PROMOTION_PERFECT_SIGHT'), True)
			if pCity.getNumBuilding(gc.getInfoTypeForString('BUILDING_ADULARIA_CHAMBER')) > 0:
				pUnit.setHasPromotion(gc.getInfoTypeForString('PROMOTION_HIDDEN'), True)

		elif pUnit.getRace() == gc.getInfoTypeForString('PROMOTION_DWARF'):
			if pCity.getNumBuilding(gc.getInfoTypeForString('BUILDING_BREWERY')) > 0:
				pUnit.changeExperience(2, -1, False, False, False)

		elif pUnit.getRace() == gc.getInfoTypeForString('PROMOTION_DEMON'):
			if pCity.getNumBuilding(gc.getInfoTypeForString('BUILDING_DEMONS_ALTAR')) > 0:
				pUnit.changeExperience(2, -1, False, False, False)
##--------	Unofficial Bug Fix: End Add



# Return 1 if a Mission was pushed
	def AI_MageTurn(self, argsList):
		pUnit = argsList[0]
		pPlot = pUnit.plot()
		pPlayer = gc.getPlayer(pUnit.getOwner())
		eTeam = gc.getTeam(pPlayer.getTeam())
		iCiv = pPlayer.getCivilizationType()
		iX = pUnit.getX()
		iY = pUnit.getY()

		if (pUnit.getUnitAIType() == gc.getInfoTypeForString('UNITAI_TERRAFORMER')):

#-----------------------------------
#TERRAFORMING
#
#SETTING FLAGS
#
#-----------------------------------

			searchdistance=3

#-----------------------------------
#SETTING FLAGS
#
#INIT
#CIV SPECIFIC
#UNIT SPECIFIC
#-----------------------------------

#INIT
			smokeb = false #terraformer tries to put out smoke
			desertb = false #terraformer tries to spring deserts
			plainsb = false #terraformer tries to improve plains
			snowb = false #terraformer tries to scorch snow to tundra
			tundrab = false #terraformer tries to scorch tundra to plains
			marshb = false #terraformer tries to scorch marsh to grassland
			hellterrb = false #terraformer tries to remove hell terrain
			treesb = false #terraformer tries to Create Trees

#CIV SPECIFICS
#			if iCiv == gc.getInfoTypeForString('CIVILIZATION_INFERNAL'):
#				smokeb = false
#			if iCiv == gc.getInfoTypeForString('CIVILIZATION_ILLIANS'):
#				snowb = false
#			if (iCiv == gc.getInfoTypeForString('CIVILIZATION_DOVIELLO') or iCiv == gc.getInfoTypeForString('CIVILIZATION_ILLIANS')):
#				tundrab = false
#			if iCiv == gc.getInfoTypeForString('CIVILIZATION_INFERNAL'):
#				hellterrb = false

#UNIT SPECIFIC
			if (pUnit.getUnitType() == gc.getInfoTypeForString('UNIT_DEVOUT') or pUnit.isHasPromotion(gc.getInfoTypeForString('PROMOTION_LIFE1'))):
				if not iCiv == gc.getInfoTypeForString('CIVILIZATION_INFERNAL'):
					hellterrb = true #terraformer tries to remove hell terrain

			if pUnit.getUnitType() == gc.getInfoTypeForString('UNIT_PRIEST_OF_LEAVES'):
				treesb = true #terraformer tries to Create Trees
				treesimpb = false
				if (iCiv == gc.getInfoTypeForString('CIVILIZATION_LJOSALFAR') or iCiv == gc.getInfoTypeForString('CIVILIZATION_SVARTALFAR')):
					treesimpb = true
				if ((treesimpb == False) and (pPlayer.getStateReligion() != gc.getInfoTypeForString('RELIGION_FELLOWSHIP_OF_LEAVES'))):
					if not pPlayer.isHuman():
						pUnit.setUnitAIType(gc.getInfoTypeForString('UNITAI_MEDIC'))
						return 0
			
			if pUnit.isHasPromotion(gc.getInfoTypeForString('PROMOTION_WATER1')):
				smokeb = true
				desertb = true

			if pUnit.isHasPromotion(gc.getInfoTypeForString('PROMOTION_SUN1')):
				tundrab = true
				marshb = true
				if not iCiv == gc.getInfoTypeForString('CIVILIZATION_ILLIANS'):
					snowb = true

			if pUnit.isHasPromotion(gc.getInfoTypeForString('PROMOTION_NATURE3')):
				desertb = true
				plainsb = true
				tundrab = true
				marshb = true
				if not iCiv == gc.getInfoTypeForString('CIVILIZATION_ILLIANS'):
					snowb = true

#TERRAFORMING CURRENT PLOT
			if pPlot.getOwner() == pUnit.getOwner():
				if (desertb and	pPlot.getTerrainType() == gc.getInfoTypeForString('TERRAIN_DESERT')):
					if pUnit.canCast(gc.getInfoTypeForString('SPELL_SPRING'),false):
						pUnit.cast(gc.getInfoTypeForString('SPELL_SPRING'))
					elif pUnit.canCast(gc.getInfoTypeForString('SPELL_VITALIZE'),false):
						pUnit.cast(gc.getInfoTypeForString('SPELL_VITALIZE'))
							
				if smokeb:
					if pPlot.getImprovementType() == gc.getInfoTypeForString('IMPROVEMENT_SMOKE'):
						if pUnit.canCast(gc.getInfoTypeForString('SPELL_SPRING'),false):
							pUnit.cast(gc.getInfoTypeForString('SPELL_SPRING'))

				if (snowb and pPlot.getTerrainType() == gc.getInfoTypeForString('TERRAIN_SNOW')):
					if pUnit.canCast(gc.getInfoTypeForString('SPELL_SCORCH'),false):
						pUnit.cast(gc.getInfoTypeForString('SPELL_SCORCH'))
					elif pUnit.canCast(gc.getInfoTypeForString('SPELL_VITALIZE'),false):
						pUnit.cast(gc.getInfoTypeForString('SPELL_VITALIZE'))

				if (tundrab and pPlot.getTerrainType() == gc.getInfoTypeForString('TERRAIN_TUNDRA')):
					if pUnit.canCast(gc.getInfoTypeForString('SPELL_SCORCH'),false):
						pUnit.cast(gc.getInfoTypeForString('SPELL_SCORCH'))
					elif pUnit.canCast(gc.getInfoTypeForString('SPELL_VITALIZE'),false):
						pUnit.cast(gc.getInfoTypeForString('SPELL_VITALIZE'))

				if (plainsb and pPlot.getTerrainType() == gc.getInfoTypeForString('TERRAIN_PLAINS')):
					if pUnit.canCast(gc.getInfoTypeForString('SPELL_VITALIZE'),false):
						pUnit.cast(gc.getInfoTypeForString('SPELL_VITALIZE'))

				if (marshb and pPlot.getTerrainType() == gc.getInfoTypeForString('TERRAIN_MARSH')):
					if pUnit.canCast(gc.getInfoTypeForString('SPELL_SCORCH'),false):
						pUnit.cast(gc.getInfoTypeForString('SPELL_SCORCH'))
					elif pUnit.canCast(gc.getInfoTypeForString('SPELL_VITALIZE'),false):
						pUnit.cast(gc.getInfoTypeForString('SPELL_VITALIZE'))

				if hellterrb:
					if pUnit.canCast(gc.getInfoTypeForString('SPELL_SANCTIFY'),false):
						pUnit.cast(gc.getInfoTypeForString('SPELL_SANCTIFY'))

				if treesb:
					if pPlot.getFeatureType() == -1:
						if pUnit.canCast(gc.getInfoTypeForString('SPELL_BLOOM'),false):
							if treesimpb or pPlot.getBonusType(-1) == -1:
								pUnit.cast(gc.getInfoTypeForString('SPELL_BLOOM'))

## LOOK FOR WORK
			if not pUnit.canMove():
				return 2
				
			for isearch in range(1,searchdistance,1):
				for iiX in range(iX-isearch, iX+isearch+1, 1):
					for iiY in range(iY-isearch, iY+isearch+1, 1):
						pPlot2 = CyMap().plot(iiX,iiY)
						if not (pPlot2.isNone() or pPlot2.isImpassable() or pPlot2.isVisibleEnemyUnit(pUnit.getOwner())):
							if pPlot2.getOwner()==pUnit.getOwner() and pPlot2 != pUnit.plot():
								if not (pPlot2.getImprovementType() != -1 and (gc.getImprovementInfo(pPlot2.getImprovementType()).isUnique() == true)):
									if smokeb:
										if (pPlot2.getImprovementType() == gc.getInfoTypeForString('IMPROVEMENT_SMOKE')):
											pUnit.getGroup().pushMission(MissionTypes.MISSION_MOVE_TO, iiX, iiY, 0, False, False, MissionAITypes.NO_MISSIONAI, pUnit.plot(), pUnit)
											return 2
									if desertb:
										if (pPlot2.getTerrainType()==gc.getInfoTypeForString('TERRAIN_DESERT') and not pPlot2.getFeatureType() == gc.getInfoTypeForString('FEATURE_FLOOD_PLAINS')):
											pUnit.getGroup().pushMission(MissionTypes.MISSION_MOVE_TO, iiX, iiY, 0, False, False, MissionAITypes.NO_MISSIONAI, pUnit.plot(), pUnit)
											return 2
									if snowb:
										if pPlot2.getTerrainType()==gc.getInfoTypeForString('TERRAIN_SNOW'):
											pUnit.getGroup().pushMission(MissionTypes.MISSION_MOVE_TO, iiX, iiY, 0, False, False, MissionAITypes.NO_MISSIONAI, pUnit.plot(), pUnit)
											return 2
									if tundrab:
										if pPlot2.getTerrainType()==gc.getInfoTypeForString('TERRAIN_TUNDRA'):
											pUnit.getGroup().pushMission(MissionTypes.MISSION_MOVE_TO, iiX, iiY, 0, False, False, MissionAITypes.NO_MISSIONAI, pUnit.plot(), pUnit)
											return 2
									if marshb:
										if pPlot2.getTerrainType()==gc.getInfoTypeForString('TERRAIN_MARSH'):
											pUnit.getGroup().pushMission(MissionTypes.MISSION_MOVE_TO, iiX, iiY, 0, False, False, MissionAITypes.NO_MISSIONAI, pUnit.plot(), pUnit)
											return 2
									if plainsb:
										if pPlot2.getTerrainType()==gc.getInfoTypeForString('TERRAIN_PLAINS'):
											pUnit.getGroup().pushMission(MissionTypes.MISSION_MOVE_TO, iiX, iiY, 0, False, False, MissionAITypes.NO_MISSIONAI, pUnit.plot(), pUnit)
											return 2
									if hellterrb:
										if (pPlot2.getTerrainType() == gc.getInfoTypeForString('TERRAIN_BROKEN_LANDS') or pPlot2.getTerrainType() == gc.getInfoTypeForString('TERRAIN_BURNING_SANDS') or pPlot2.getTerrainType() == gc.getInfoTypeForString('TERRAIN_FIELDS_OF_PERDITION') or pPlot2.getTerrainType() == gc.getInfoTypeForString('TERRAIN_SHALLOWS')):
											pUnit.getGroup().pushMission(MissionTypes.MISSION_MOVE_TO, iiX, iiY, 0, False, False, MissionAITypes.NO_MISSIONAI, pUnit.plot(), pUnit)
											return 2
									if treesb:
										if (pPlot2.getFeatureType() == -1):
											if (pPlot2.getTerrainType() == gc.getInfoTypeForString('TERRAIN_GRASS') or pPlot2.getTerrainType() == gc.getInfoTypeForString('TERRAIN_PLAINS') or pPlot2.getTerrainType() == gc.getInfoTypeForString('TERRAIN_TUNDRA') or pPlot2.getTerrainType() == gc.getInfoTypeForString('TERRAIN_MARSH')):
												if not pPlot2.isCity():
													if (pPlot2.getImprovementType() == -1 or treesimpb):
														pUnit.getGroup().pushMission(MissionTypes.MISSION_MOVE_TO, iiX, iiY, 0, False, False, MissionAITypes.NO_MISSIONAI, pUnit.plot(), pUnit)
														return 2

#Nothing to do, lets move on to another City!
			iBestCount=0
			pBestCity=0
			for icity in range(pPlayer.getNumCities()):
				pCity = pPlayer.getCity(icity)
				if not pCity.isNone():
					iCount=0
					for iI in range(1, 21):
						pPlot2 = pCity.getCityIndexPlot(iI)
						if not (pPlot2.isNone() or pPlot2.isImpassable() or pPlot2.isVisibleEnemyUnit(pUnit.getOwner())):
							if pPlot2.getOwner()==pUnit.getOwner():
								if not (pPlot2.getImprovementType() != -1 and (gc.getImprovementInfo(pPlot2.getImprovementType()).isUnique() == true)):
									if smokeb:
										if (pPlot2.getImprovementType() == gc.getInfoTypeForString('IMPROVEMENT_SMOKE')):
											iCount=iCount+1
									if desertb:
										if (pPlot2.getTerrainType() == gc.getInfoTypeForString('TERRAIN_DESERT') and not pPlot2.getFeatureType() == gc.getInfoTypeForString('FEATURE_FLOOD_PLAINS')):
											iCount=iCount+1
									if snowb:
										if pPlot2.getTerrainType() == gc.getInfoTypeForString('TERRAIN_SNOW'):
											iCount=iCount+1
									if tundrab:
										if pPlot2.getTerrainType() == gc.getInfoTypeForString('TERRAIN_TUNDRA'):
											iCount=iCount+1
									if marshb:
										if pPlot2.getTerrainType() == gc.getInfoTypeForString('TERRAIN_MARSH'):
											iCount=iCount+1
									if hellterrb:
										if (pPlot2.getTerrainType() == gc.getInfoTypeForString('TERRAIN_BROKEN_LANDS') or pPlot2.getTerrainType() == gc.getInfoTypeForString('TERRAIN_BURNING_SANDS') or pPlot2.getTerrainType() == gc.getInfoTypeForString('TERRAIN_FIELDS_OF_PERDITION') or pPlot2.getTerrainType() == gc.getInfoTypeForString('TERRAIN_SHALLOWS')):
											iCount=iCount+1
									if treesb:
										if (pPlot2.getFeatureType() == -1):
											if (pPlot2.getTerrainType() == gc.getInfoTypeForString('TERRAIN_GRASS') or pPlot2.getTerrainType() == gc.getInfoTypeForString('TERRAIN_PLAINS') or pPlot2.getTerrainType() == gc.getInfoTypeForString('TERRAIN_TUNDRA')):
												if not pPlot2.isCity():
													if (pPlot2.getImprovementType() == -1 or treesimpb):
														iCount=iCount+1

					if (iCount>iBestCount):
						pBestCity=pCity
						iBestCount=iCount
			if (pBestCity!=0):
				pCPlot = pBestCity.plot()
				CX = pCPlot.getX()
				CY = pCPlot.getY()
				pUnit.getGroup().pushMission(MissionTypes.MISSION_MOVE_TO, CX, CY, 0, False, False, MissionAITypes.NO_MISSIONAI, pUnit.plot(), pUnit)
				return 1
			return 0


	def AI_Mage_UPGRADE_MANA(self, argsList):
		pUnit = argsList[0]

#-----------------------------------
#UNITAI_MANA_UPGRADE
#Terraformer looks around for mana, changes UNITAI if he doesn't find some
#
# 1) Look for non raw mana and upgrade
# 2) Look for raw mana, decide how to upgrade, and do it!
# 3) Look for mana to dispel, and do it!
#-----------------------------------

		pPlot = pUnit.plot()
		iPlayer = pUnit.getOwner()
		pPlayer = gc.getPlayer(iPlayer)
		eTeam = gc.getTeam(pPlayer.getTeam())
		iCiv =  pPlayer.getCivilizationType()
		iX = pUnit.getX()
		iY = pUnit.getY()

		smokeb = True #Civ likes to put out smoke
		desertb = True #Civ likes to spring deserts
		snowb = True #Civ likes to scorch snow to tundra
		tundrab = False #Civ likes to scorch tundra to plains
		marshb = True #Civ likes to scorch marsh to grassland
		grassb = False #Civ likes to scorch grassland to plains
		hellterrb = True #Civ likes to remove hell terrain

		if iCiv == gc.getInfoTypeForString('CIVILIZATION_INFERNAL'):
			smokeb = False
			hellterrb = False

#		elif iCiv == gc.getInfoTypeForString('CIVILIZATION_INFERNAL')):
#			desertb = False

		elif iCiv == gc.getInfoTypeForString('CIVILIZATION_ILLIANS'):
			snowb = False

		elif iCiv == gc.getInfoTypeForString('CIVILIZATION_SHEAIM'):
			hellterrb = False


#Look for Mana to Dispel
		searchdistance=15

		if pUnit.isHasPromotion(gc.getInfoTypeForString('PROMOTION_METAMAGIC2')):
			for isearch in range(1,searchdistance+1,1):
				for iiY in range(iY-isearch, iY+isearch, 1):
					for iiX in range(iX-isearch, iX+isearch, 1):
						pPlot2 = CyMap().plot(iiX,iiY)
						if not (pPlot2.isNone() or pPlot2.isImpassable() or pPlot2.isVisibleEnemyUnit(iPlayer)):
							if pPlot2.getOwner() == iPlayer:

								if pPlot2.getBonusType(-1) != -1:
									iBonus = pPlot2.getBonusType(TeamTypes.NO_TEAM)
									if gc.getBonusInfo(iBonus).getBonusClassType() == gc.getInfoTypeForString('BONUSCLASS_MANA'):
										bDispel = True

										if pPlayer.getArcaneTowerVictoryFlag() == 0:
											if CyGame().getSorenRandNum(50, "Don't have to Dispel all the Time"):
												bDispel = False
										if pPlayer.getArcaneTowerVictoryFlag() == 1:
											if iBonus == gc.getInfoTypeForString('BONUS_MANA_BODY'):
												if pPlayer.getNumAvailableBonuses(gc.getInfoTypeForString('BONUS_MANA_BODY'))==1:
													bDispel = False
											elif iBonus == gc.getInfoTypeForString('BONUS_MANA_LIFE'):
												if pPlayer.getNumAvailableBonuses(gc.getInfoTypeForString('BONUS_MANA_LIFE'))==1:
													bDispel = False
											elif iBonus == gc.getInfoTypeForString('BONUS_MANA_ENCHANTMENT'):
												if pPlayer.getNumAvailableBonuses(gc.getInfoTypeForString('BONUS_MANA_ENCHANTMENT'))==1:
													bDispel = False
											elif iBonus == gc.getInfoTypeForString('BONUS_MANA_NATURE'):
												if pPlayer.getNumAvailableBonuses(gc.getInfoTypeForString('BONUS_MANA_NATURE'))==1:
													bDispel = False

										if pPlayer.getArcaneTowerVictoryFlag() == 2:
											if iBonus == gc.getInfoTypeForString('BONUS_MANA_LAW'):
												if pPlayer.getNumAvailableBonuses(gc.getInfoTypeForString('BONUS_MANA_LAW'))==1:
													bDispel = False
											elif iBonus == gc.getInfoTypeForString('BONUS_MANA_SUN'):
												if pPlayer.getNumAvailableBonuses(gc.getInfoTypeForString('BONUS_MANA_SUN'))==1:
													bDispel = False
											elif iBonus == gc.getInfoTypeForString('BONUS_MANA_SPIRIT'):
												if pPlayer.getNumAvailableBonuses(gc.getInfoTypeForString('BONUS_MANA_SPIRIT'))==1:
													bDispel = False
											elif iBonus == gc.getInfoTypeForString('BONUS_MANA_MIND'):
												if pPlayer.getNumAvailableBonuses(gc.getInfoTypeForString('BONUS_MANA_MIND'))==1:
													bDispel = False

										if pPlayer.getArcaneTowerVictoryFlag() == 3:
											if iBonus == gc.getInfoTypeForString('BONUS_MANA_CHAOS'):
												if pPlayer.getNumAvailableBonuses(gc.getInfoTypeForString('BONUS_MANA_CHAOS'))==1:
													bDispel = False
											elif iBonus == gc.getInfoTypeForString('BONUS_MANA_DEATH'):
												if pPlayer.getNumAvailableBonuses(gc.getInfoTypeForString('BONUS_MANA_DEATH'))==1:
													bDispel = False
											elif iBonus == gc.getInfoTypeForString('BONUS_MANA_ENTROPY'):
												if pPlayer.getNumAvailableBonuses(gc.getInfoTypeForString('BONUS_MANA_ENTROPY'))==1:
													bDispel = False
											elif iBonus == gc.getInfoTypeForString('BONUS_MANA_SHADOW'):
												if pPlayer.getNumAvailableBonuses(gc.getInfoTypeForString('BONUS_MANA_SHADOW'))==1:
													bDispel = False

										if pPlayer.getArcaneTowerVictoryFlag() == 4:
											if iBonus == gc.getInfoTypeForString('BONUS_MANA_EARTH'):
												if pPlayer.getNumAvailableBonuses(gc.getInfoTypeForString('BONUS_MANA_EARTH'))==1:
													bDispel = False
											elif iBonus == gc.getInfoTypeForString('BONUS_MANA_FIRE'):
												if pPlayer.getNumAvailableBonuses(gc.getInfoTypeForString('BONUS_MANA_FIRE'))==1:
													bDispel = False
											elif iBonus == gc.getInfoTypeForString('BONUS_MANA_AIR'):
												if pPlayer.getNumAvailableBonuses(gc.getInfoTypeForString('BONUS_MANA_AIR'))==1:
													bDispel = False
											elif iBonus == gc.getInfoTypeForString('BONUS_MANA_WATER'):
												if pPlayer.getNumAvailableBonuses(gc.getInfoTypeForString('BONUS_MANA_WATER'))==1:
													bDispel = False

										if bDispel:
											if not pUnit.at(iiX, iiY):
#												CyInterface().addImmediateMessage('Searching for stuff to Dispel', "AS2D_NEW_ERA")
												pUnit.getGroup().pushMission(MissionTypes.MISSION_MOVE_TO, iiX, iiY, 0, False, False, MissionAITypes.NO_MISSIONAI, pUnit.plot(), pUnit)
												return 1
											if pUnit.canCast(gc.getInfoTypeForString('SPELL_DISPEL_MAGIC'),False):
												pUnit.cast(gc.getInfoTypeForString('SPELL_DISPEL_MAGIC'))
												return 1

#Dispel more if we seek Tower Victory Condition
			if pPlayer.getArcaneTowerVictoryFlag()>0:
				iBestCount=0
				pBestCity=0
				for icity in range(pPlayer.getNumCities()):
					pCity = pPlayer.getCity(icity)
					if not pCity.isNone():
						iCount=0
						for iI in range(1, 21):
							pPlot2 = pCity.getCityIndexPlot(iI)
							if not (pPlot2.isNone() or pPlot2.isImpassable() or pPlot2.isVisibleEnemyUnit(iPlayer)):
								if pPlot2.getOwner()==iPlayer:
									if pPlot2.getBonusType(-1) != -1:
										iBonus = pPlot2.getBonusType(TeamTypes.NO_TEAM)
										if gc.getBonusInfo(iBonus).getBonusClassType() == gc.getInfoTypeForString('BONUSCLASS_MANA'):
											iCount=iCount+1
										if gc.getBonusInfo(iBonus).getBonusClassType() == gc.getInfoTypeForString('BONUSCLASS_RAWMANA'):
											iCount=iCount+1

						if iCount > iBestCount:
							pBestCity=pCity
							iBestCount=iCount
				if pBestCity != 0:
					pCPlot = pBestCity.plot()
					CX = pCPlot.getX()
					CY = pCPlot.getY()
					pUnit.getGroup().pushMission(MissionTypes.MISSION_MOVE_TO, CX, CY, 0, False, False, MissionAITypes.NO_MISSIONAI, pUnit.plot(), pUnit)
					return 1

#found no mana, return 2 so UNITAI is reset

		return 2

#returns the current flag for Tower Victory
	def AI_TowerMastery(self, argsList):
		ePlayer = argsList[0]
		flag = argsList[1]

		pPlayer = gc.getPlayer(ePlayer)
		eTeam = gc.getTeam(pPlayer.getTeam())

#		CyInterface().addImmediateMessage('This is AI_TowerMastery ', "AS2D_NEW_ERA")
#		CyInterface().addImmediateMessage('Flag is '+str(pPlayer.getArcaneTowerVictoryFlag()), "AS2D_NEW_ERA")

		if flag == 0:
#			if eTeam.isHasTech(gc.getInfoTypeForString('TECH_SORCERY')) == False :
#				return 0
#			if pPlayer.getNumAvailableBonuses(gc.getInfoTypeForString('BONUS_MANA_METAMAGIC'))==0:
#				return 0

			possiblemana=0
			for i in range (CyMap().numPlots()):
				pPlot = CyMap().plotByIndex(i)
				if pPlot.getOwner()==ePlayer:
					if pPlot.getBonusType(-1) != -1:
						iBonus = pPlot.getBonusType(TeamTypes.NO_TEAM)
						if gc.getBonusInfo(iBonus).getBonusClassType() == gc.getInfoTypeForString('BONUSCLASS_MANA'):
							possiblemana=possiblemana+1
						elif gc.getBonusInfo(iBonus).getBonusClassType() == gc.getInfoTypeForString('BONUSCLASS_RAWMANA'):
							possiblemana=possiblemana+1

			if possiblemana<4:
				return 0

			if pPlayer.isHasTech(gc.getInfoTypeForString('TECH_ALTERATION')):
				if pPlayer.getBuildingClassCount(gc.getInfoTypeForString('BUILDINGCLASS_TOWER_OF_ALTERATION'))==0:
					return 1

			if pPlayer.isHasTech(gc.getInfoTypeForString('TECH_DIVINATION')):
				if pPlayer.getBuildingClassCount(gc.getInfoTypeForString('BUILDINGCLASS_TOWER_OF_DIVINATION'))==0:
					return 2

			if pPlayer.isHasTech(gc.getInfoTypeForString('TECH_NECROMANCY')):
				if pPlayer.getBuildingClassCount(gc.getInfoTypeForString('BUILDINGCLASS_TOWER_OF_NECROMANCY'))==0:
					if not pPlayer.isCivic(CvUtil.findInfoTypeNum(gc.getCivicInfo,gc.getNumCivicInfos(),'CIVIC_OVERCOUNCIL')):
						return 3

			if pPlayer.isHasTech(gc.getInfoTypeForString('TECH_ELEMENTALISM')):
				if pPlayer.getBuildingClassCount(gc.getInfoTypeForString('BUILDINGCLASS_TOWER_OF_THE_ELEMENTS'))==0:
					return 4

		if flag==1:
			if pPlayer.getBuildingClassCount(gc.getInfoTypeForString('BUILDINGCLASS_TOWER_OF_ALTERATION'))>0:
				return 0
			else:
				return 1

		if flag==2:
			if pPlayer.getBuildingClassCount(gc.getInfoTypeForString('BUILDINGCLASS_TOWER_OF_DIVINATION'))>0:
				return 0
			else:
				return 2

		if flag==3:
			if pPlayer.getBuildingClassCount(gc.getInfoTypeForString('BUILDINGCLASS_TOWER_OF_NECROMANCY'))>0:
				return 0
			else:
				return 3

		if flag==4:
			if pPlayer.getBuildingClassCount(gc.getInfoTypeForString('BUILDINGCLASS_TOWER_OF_THE_ELEMENTS'))>0:
				return 0
			else:
				return 4

		return 0
