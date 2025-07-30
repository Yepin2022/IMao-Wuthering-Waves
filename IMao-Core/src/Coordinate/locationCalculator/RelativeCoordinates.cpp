#include "RelativeCoordinates.h"
#include "MapCoordinate.h"

Coordinate RelativeCoordinates::GetRelativeCoordinates(const Coordinate& coordinate, const Coordinate& originCoordinate) {
	Coordinate relativeCoordinates(coordinate.x - originCoordinate.x, coordinate.y - originCoordinate.y);
	return relativeCoordinates;
}

Coordinate RelativeCoordinates::ImgMapCoordToRC(const Coordinate& imgMapCoordinate,int SceneId) {
	if (SceneId == 1) {
		return ImgMapCoordToRWOC(imgMapCoordinate);
	}

	if (SceneId == 2) {
		return ImgMapCoordToRTOC(imgMapCoordinate);
	}

	if (SceneId == 3) {
		return ImgMapCoordToRFOC(imgMapCoordinate);
	}

	return Coordinate(0, 0);
}


Coordinate RelativeCoordinates::IdentifyCoordToRC(const Coordinate& identifyCoordinate, int SceneId) {
	if (SceneId == 1) {
		return WorldCoordToRWOC(identifyCoordinate);
	}

	if (SceneId == 2) {
		return TethysCoordToRTOC(identifyCoordinate);
	}

	if (SceneId == 3) {
		return FabricatoriumCoordToRFOC(identifyCoordinate);
	}

	return Coordinate(0, 0);
}

//RWOC:Relative coordinates to the iamge map world origin.
Coordinate RelativeCoordinates::ImgMapCoordToRWOC(const Coordinate& imgMapCoordinate) {
	Coordinate relativeCoordinates(imgMapCoordinate.x - WorldOriginCoordinates::x, WorldOriginCoordinates::y- imgMapCoordinate.y);
	return relativeCoordinates;
}

Coordinate RelativeCoordinates::WorldCoordToRWOC(const Coordinate& worldCoordinate) {
	Coordinate mapCoord = MapCoordinate::PlayerWorldCoordToImgMapCoord(worldCoordinate);
	Coordinate relativeCoordinates = ImgMapCoordToRWOC(mapCoord);
	return relativeCoordinates;
}

//RTOC:Relative coordinates to the image map Tethys origin.
Coordinate RelativeCoordinates::ImgMapCoordToRTOC(const Coordinate& imgMapCoordinate) {
	Coordinate relativeCoordinates(imgMapCoordinate.x - TethysOriginCoordinates::x, TethysOriginCoordinates::y - imgMapCoordinate.y);
	return relativeCoordinates;
}

Coordinate RelativeCoordinates::TethysCoordToRTOC(const Coordinate& TethysCoordinate) {
	Coordinate mapCoord = MapCoordinate::PlayerTethysCoordToImgMapCoord(TethysCoordinate);
	Coordinate relativeCoordinates = ImgMapCoordToRTOC(mapCoord);
	return relativeCoordinates;
}

//RTOC:Relative coordinates to the image map Fabricatorium origin.
Coordinate RelativeCoordinates::ImgMapCoordToRFOC(const Coordinate& imgMapCoordinate) {
	Coordinate relativeCoordinates(imgMapCoordinate.x - FabricatoriumOriginCoordinates::x, FabricatoriumOriginCoordinates::y - imgMapCoordinate.y);
	return relativeCoordinates;
}


Coordinate RelativeCoordinates::FabricatoriumCoordToRFOC(const Coordinate& TethysCoordinate) {
	Coordinate mapCoord = MapCoordinate::PlayerFabricatoriumCoordToImgMapCoord(TethysCoordinate);
	Coordinate relativeCoordinates = ImgMapCoordToRFOC(mapCoord);
	return relativeCoordinates;
}