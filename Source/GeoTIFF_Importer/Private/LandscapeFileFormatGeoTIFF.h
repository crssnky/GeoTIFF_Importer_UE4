// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/ArrayView.h"
#include "LandscapeFileFormatInterface.h"

class FLandscapeHeightmapFileFormat_GeoTIFF: public ILandscapeHeightmapFileFormat{
private:
	FLandscapeFileTypeInfo FileTypeInfo;

public:
	FLandscapeHeightmapFileFormat_GeoTIFF();
	~FLandscapeHeightmapFileFormat_GeoTIFF();

	virtual const FLandscapeFileTypeInfo& GetInfo() const override{
		return FileTypeInfo;
	}

	virtual FLandscapeHeightmapInfo Validate(const TCHAR* HeightmapFilename)const override;
	virtual FLandscapeHeightmapImportData Import(const TCHAR* HeightmapFilename, FLandscapeFileResolution ExpectedResolution)const override;
};

