// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
using System;
using System.IO;

namespace UnrealBuildTool.Rules
{
	public class GeoTIFF_Importer : ModuleRules
	{
		public GeoTIFF_Importer(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
			PublicIncludePaths.AddRange(
				new string[] {
					// ... add public include paths required here ...
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					"GeoTIFF_Importer/Private",
					// ... add other private include paths required here ...
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"UnrealEd",
					"LevelEditor",
					"Slate",
					"SlateCore",
					"MainFrame",
					"InputCore",
					// ... add other public dependencies that you statically link with here ...
					"LandscapeEditor",
					"DesktopPlatform",
				}
				);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					// ... add private dependencies that you statically link with here ...
				}
				);

			DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
					// ... add any modules that your module loads dynamically here ...
				}
				);
			LoadLibs(Target);
		}

		// https://wiki.unrealengine.com/Linking_Static_Libraries_Using_The_Build_System
		private bool LoadLibs(ReadOnlyTargetRules Target)
		{
			bool isLibrarySupported = false;

			if (Target.Platform == UnrealTargetPlatform.Win64)
			{
				isLibrarySupported = true;
				string LibrariesPathTIFF = Path.Combine(ThirdPartyPath, "libtiff", "libs");
				string LibrariesPathGeoTIFF = Path.Combine(ThirdPartyPath, "libgeotiff", "libs");
				PublicAdditionalLibraries.Add(Path.Combine(LibrariesPathTIFF, "tiff.lib"));
				PublicAdditionalLibraries.Add(Path.Combine(LibrariesPathGeoTIFF, "geotiff.lib"));
				PublicAdditionalLibraries.Add(Path.Combine(LibrariesPathGeoTIFF, "xtiff.lib"));

				PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "libtiff", "incs"));
				PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "libgeotiff", "incs"));

				Console.WriteLine("\nGeoTIFF Libraries Loaded\n");
			}

			return isLibrarySupported;
		}

		private string ModulePath {
			get { return ModuleDirectory; }
		}
		private string ThirdPartyPath {
			get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
		}
	}
}
