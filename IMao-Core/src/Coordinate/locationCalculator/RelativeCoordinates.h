#pragma once
#include "../CoordinateStruct.h"
class RelativeCoordinates
{
public:
	static Coordinate ImgMapCoordToROC_World(const Coordinate& imgMapCoordinate);
	static Coordinate GetRelativeCoordinates(const Coordinate& coordinate,const Coordinate& originCoordinate);
	static Coordinate ImgMapCoordToROC(const Coordinate& imgMapCoordinate, int SceneId);
	static Coordinate IdentifyCoordToROC(const Coordinate& identifyCoordinate, int SceneId);
	static Coordinate WorldCoordToROC_World(const Coordinate& worldCoordinate);
	static Coordinate ImgMapCoordToROC_Tethys(const Coordinate& imgMapCoordinate);
	static Coordinate TethysCoordToROC_Tethys(const Coordinate& TethysCoordinate);
	static Coordinate ImgMapCoordToROC_Fabricatorium(const Coordinate& imgMapCoordinate);
	static Coordinate FabricatoriumCoordToROC_Fabricatorium(const Coordinate& TethysCoordinate);
	static Coordinate ImgMapCoordToROC_Avinoleum(const Coordinate& imgMapCoordinate);
	static Coordinate AvinoleumCoordToROC_Avinoleum(const Coordinate& AvinoleumCoordinate);
	static Coordinate ImgMapCoordToROC_Lahai(const Coordinate& imgMapCoordinate);
	static Coordinate LahaiCoordToROC_Lahai(const Coordinate& LahaiCoordinate);
};

