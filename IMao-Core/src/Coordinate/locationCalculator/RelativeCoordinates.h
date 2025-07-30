#pragma once
#include "../CoordinateStruct.h"
class RelativeCoordinates
{
public:
	static Coordinate ImgMapCoordToRWOC(const Coordinate& imgMapCoordinate);
	static Coordinate GetRelativeCoordinates(const Coordinate& coordinate,const Coordinate& originCoordinate);
	static Coordinate ImgMapCoordToRC(const Coordinate& imgMapCoordinate, int SceneId);
	static Coordinate IdentifyCoordToRC(const Coordinate& identifyCoordinate, int SceneId);
	static Coordinate WorldCoordToRWOC(const Coordinate& worldCoordinate);
	static Coordinate ImgMapCoordToRTOC(const Coordinate& imgMapCoordinate);
	static Coordinate TethysCoordToRTOC(const Coordinate& TethysCoordinate);
	static Coordinate ImgMapCoordToRFOC(const Coordinate& imgMapCoordinate);
	static Coordinate FabricatoriumCoordToRFOC(const Coordinate& TethysCoordinate);
};

