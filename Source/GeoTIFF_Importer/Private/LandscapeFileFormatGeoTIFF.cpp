// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "LandscapeFileFormatGeoTIFF.h"

#include <memory>
#include <algorithm>

#include "geotiff.h"
#include "geo_tiffp.h"
#include "xtiffio.h"

#undef LOCTEXT_NAMESPACE
#define LOCTEXT_NAMESPACE "LandscapeEditor.NewLandscape"

FLandscapeHeightmapFileFormat_GeoTIFF::FLandscapeHeightmapFileFormat_GeoTIFF() {
	FileTypeInfo.Description = LOCTEXT("FileFormatGeoTIFF_HeightmapDesc", "Heightmap .tif files");
	FileTypeInfo.Extensions.Add(".tif");
	FileTypeInfo.bSupportsExport = true;
}

FLandscapeHeightmapFileFormat_GeoTIFF::~FLandscapeHeightmapFileFormat_GeoTIFF() {

}

FLandscapeHeightmapInfo FLandscapeHeightmapFileFormat_GeoTIFF::Validate(const TCHAR* HeightmapFilename) const {
	FLandscapeHeightmapInfo Result;

	// tmp
	//int readCount;

	// FileOpen
	const auto tmpTiff = XTIFFOpen(TCHAR_TO_ANSI(HeightmapFilename), "r");
	if (tmpTiff == nullptr) {
		Result.ResultCode = ELandscapeImportResult::Error;
		Result.ErrorMessage = LOCTEXT("Import_HeightmapFileReadError", "Error reading heightmap file");
		return Result;
	}

	std::shared_ptr<TIFF> tiff(tmpTiff, XTIFFClose);
	// Rresolution
	int width, height;
	if (TIFFGetField(tiff.get(), TIFFTAG_IMAGEWIDTH, &width) == 0) {
		Result.ResultCode = ELandscapeImportResult::Error;
		Result.ErrorMessage = LOCTEXT("Import_HeightmapFileReadError", "The TIFF file is not defined IMAGE_WIDTH.");
		return Result;

	}
	if (TIFFGetField(tiff.get(), TIFFTAG_IMAGELENGTH, &height) == 0) {
		Result.ResultCode = ELandscapeImportResult::Error;
		Result.ErrorMessage = LOCTEXT("Import_HeightmapFileReadError", "The TIFF file is not defined IMAGE_LENGTH.");
		return Result;
	}

	// Photometric
	uint16 photometric;
	if (TIFFGetField(tiff.get(), TIFFTAG_PHOTOMETRIC, &photometric) == 0) {
		Result.ResultCode = ELandscapeImportResult::Error;
		Result.ErrorMessage = LOCTEXT("Import_HeightmapFileReadError", "The TIFF file is not defined PHOTOMETRIC.");
		return Result;

	}
	if (photometric != 1) {
		Result.ResultCode = ELandscapeImportResult::Error;
		Result.ErrorMessage = LOCTEXT("Import_HeightmapFileReadError", "The TIFF file is not GrayScale.");
		return Result;

	}

	// Return
	FLandscapeFileResolution ImportResolution;
	ImportResolution.Width = width;
	ImportResolution.Height = height;
	Result.PossibleResolutions.Add(ImportResolution);
	return Result;
}

FLandscapeHeightmapImportData FLandscapeHeightmapFileFormat_GeoTIFF::Import(const TCHAR* HeightmapFilename, FLandscapeFileResolution ExpectedResolution) const {
	FLandscapeHeightmapImportData Result;

	// FileOpen
	std::shared_ptr<TIFF> tiff(XTIFFOpen(TCHAR_TO_ANSI(HeightmapFilename), "r"), XTIFFClose);
	if (tiff.get() == nullptr) {
		Result.ResultCode = ELandscapeImportResult::Error;
		Result.ErrorMessage = LOCTEXT("Import_HeightmapFileReadError", "Error reading heightmap file");
		return Result;
	}

	// Rresolution
	uint32 width, height;
	if (TIFFGetField(tiff.get(), TIFFTAG_IMAGEWIDTH, &width) == 0 || width != ExpectedResolution.Width) {
		Result.ResultCode = ELandscapeImportResult::Error;
		Result.ErrorMessage = LOCTEXT("Import_HeightmapFileReadError", "The TIFF file is not defined IMAGE_WIDTH.");
		return Result;

	}
	if (TIFFGetField(tiff.get(), TIFFTAG_IMAGELENGTH, &height) == 0 || height != ExpectedResolution.Height) {
		Result.ResultCode = ELandscapeImportResult::Error;
		Result.ErrorMessage = LOCTEXT("Import_HeightmapFileReadError", "The TIFF file is not defined IMAGE_LENGTH.");
		return Result;
	}

	// Photometric
	uint16 photometric;
	if (TIFFGetField(tiff.get(), TIFFTAG_PHOTOMETRIC, &photometric) == 0) {
		Result.ResultCode = ELandscapeImportResult::Error;
		Result.ErrorMessage = LOCTEXT("Import_HeightmapFileReadError", "The TIFF file is not defined PHOTOMETRIC.");
		return Result;

	}

	// BitsPerSample
	uint16 bitspersample;
	if (TIFFGetField(tiff.get(), TIFFTAG_BITSPERSAMPLE, &bitspersample) == 0) {
		Result.ResultCode = ELandscapeImportResult::Error;
		Result.ErrorMessage = LOCTEXT("Import_HeightmapFileReadError", "The TIFF file is not defined BITS_PER_SAMPLE.");
		return Result;

	}

	// Strip
	uint32 RowsPerStrip;
	if (TIFFGetField(tiff.get(), TIFFTAG_ROWSPERSTRIP, &RowsPerStrip) == 0) {
		Result.ResultCode = ELandscapeImportResult::Error;
		Result.ErrorMessage = LOCTEXT("Import_HeightmapFileReadError", "The TIFF file is not defined ROWS_PER_STRIP.");
		return Result;

	}

	// SampleFormat
	uint32 SampleFormat;
	if (TIFFGetField(tiff.get(), TIFFTAG_SAMPLEFORMAT, &SampleFormat) == 0) {
		Result.ResultCode = ELandscapeImportResult::Error;
		Result.ErrorMessage = LOCTEXT("Import_HeightmapFileReadError", "The TIFF file is not defined SAMPLE_FORMAT.");
		return Result;
	}

	// data
	TArray<uint16> data;
	const int byteSize = bitspersample / 8;
	//const int byteSize = sizeof(uint16) / sizeof(uint8);
	const uint32 ScanlineSize = TIFFScanlineSize(tiff.get()) / byteSize;
	TArray<float> buf;
	buf.SetNum(ScanlineSize);
	data.Empty(width * height);
	for (uint32 i = 0; i < height; i++) {
		// ReadLine
		TIFFReadScanline(tiff.get(), buf.GetData(), i);
		for (uint32 j = 0; j < ScanlineSize; j++) {
			data.Emplace(buf[j] < 32788u ? buf[j] + 32788u : std::numeric_limits<uint16>::max());
		}
	}
	Result.Data.Empty(ExpectedResolution.Width * ExpectedResolution.Height);
	Result.Data.AddUninitialized(ExpectedResolution.Width * ExpectedResolution.Height);
	FMemory::Memcpy(Result.Data.GetData(), data.GetData(), ExpectedResolution.Width * ExpectedResolution.Height * sizeof(uint16));

	return Result;
}

#undef LOCTEXT_NAMESPACE
