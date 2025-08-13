#include "RelativeCoordinates.h"
#include "MapCoordinate.h"

Coordinate RelativeCoordinates::GetRelativeCoordinates(const Coordinate& coordinate, const Coordinate& originCoordinate) {
	Coordinate relativeCoordinates(coordinate.x - originCoordinate.x, coordinate.y - originCoordinate.y);
	return relativeCoordinates;
}

Coordinate RelativeCoordinates::ImgMapCoordToROC(const Coordinate& imgMapCoordinate,int SceneId) {
	if (SceneId == 1) {
		return ImgMapCoordToROC_World(imgMapCoordinate);
	}

	if (SceneId == 2) {
		return ImgMapCoordToROC_Tethys(imgMapCoordinate);
	}

	if (SceneId == 3) {
		return ImgMapCoordToROC_Fabricatorium(imgMapCoordinate);
	}

	if (SceneId == 4) {
		return ImgMapCoordToROC_Avinoleum(imgMapCoordinate);
	}

	return Coordinate(0, 0);
}

Coordinate RelativeCoordinates::IdentifyCoordToROC(const Coordinate& identifyCoordinate, int SceneId) {
	if (SceneId == 1) {
		return WorldCoordToROC_World(identifyCoordinate);
	}

	if (SceneId == 2) {
		return TethysCoordToROC_Tethys(identifyCoordinate);
	}

	if (SceneId == 3) {
		return FabricatoriumCoordToROC_Fabricatorium(identifyCoordinate);
	}

	if (SceneId == 4) {
		return AvinoleumCoordToROC_Avinoleum(identifyCoordinate);
	}

	return Coordinate(0, 0);
}

//ROC:Relative coordinates to the iamge map xxx origin.
Coordinate RelativeCoordinates::ImgMapCoordToROC_World(const Coordinate& imgMapCoordinate) {
	Coordinate relativeCoordinates(imgMapCoordinate.x - WorldOriginCoordinates::x, WorldOriginCoordinates::y- imgMapCoordinate.y);
	return relativeCoordinates;
}

Coordinate RelativeCoordinates::WorldCoordToROC_World(const Coordinate& worldCoordinate) {
	Coordinate mapCoord = MapCoordinate::PlayerWorldCoordToImgMapCoord(worldCoordinate);
	Coordinate relativeCoordinates = ImgMapCoordToROC_World(mapCoord);
	return relativeCoordinates;
}



Coordinate RelativeCoordinates::ImgMapCoordToROC_Tethys(const Coordinate& imgMapCoordinate) {
	Coordinate relativeCoordinates(imgMapCoordinate.x - TethysOriginCoordinates::x, TethysOriginCoordinates::y - imgMapCoordinate.y);
	return relativeCoordinates;
}

Coordinate RelativeCoordinates::TethysCoordToROC_Tethys(const Coordinate& TethysCoordinate) {
	Coordinate mapCoord = MapCoordinate::PlayerTethysCoordToImgMapCoord(TethysCoordinate);
	Coordinate relativeCoordinates = ImgMapCoordToROC_Tethys(mapCoord);
	return relativeCoordinates;
}



Coordinate RelativeCoordinates::ImgMapCoordToROC_Fabricatorium(const Coordinate& imgMapCoordinate) {
	Coordinate relativeCoordinates(imgMapCoordinate.x - FabricatoriumOriginCoordinates::x, FabricatoriumOriginCoordinates::y - imgMapCoordinate.y);
	return relativeCoordinates;
}

Coordinate RelativeCoordinates::FabricatoriumCoordToROC_Fabricatorium(const Coordinate& TethysCoordinate) {
	Coordinate mapCoord = MapCoordinate::PlayerFabricatoriumCoordToImgMapCoord(TethysCoordinate);
	Coordinate relativeCoordinates = ImgMapCoordToROC_Fabricatorium(mapCoord);
	return relativeCoordinates;
}




Coordinate RelativeCoordinates::ImgMapCoordToROC_Avinoleum(const Coordinate& imgMapCoordinate) {
	Coordinate relativeCoordinates(imgMapCoordinate.x - AvinoleumOriginCoordinates::x, AvinoleumOriginCoordinates::y - imgMapCoordinate.y);
	return relativeCoordinates;
}

Coordinate RelativeCoordinates::AvinoleumCoordToROC_Avinoleum(const Coordinate& AvinoleumCoordinate) {
	Coordinate mapCoord = MapCoordinate::PlayerAvinoleumCoordToImgMapCoord(AvinoleumCoordinate);
	Coordinate relativeCoordinates = ImgMapCoordToROC_Avinoleum(mapCoord);
	return relativeCoordinates;
}
