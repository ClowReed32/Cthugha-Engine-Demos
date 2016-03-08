//Maya ASCII 2014 scene
requires maya "2014";
currentUnit -l centimeter -a degree -t film;
createNode script -n "upAxisScriptNode";
	setAttr ".b" -type "string" "string $currentAxis = `upAxis -q -ax`; if ($currentAxis != \"y\") { upAxis -ax \"y\"; viewSet -home persp; }";
	setAttr ".st" 2;
createNode transform -n "TestFloor";
	addAttr -ln "colladaId" -dt "string";
	setAttr .colladaId -type "string" "TestFloor";
	setAttr ".t" -type "double3" 0.000000 0.000000 0.000000;
	setAttr ".r" -type "double3" -0.000000 0.000000 -0.000000;
createNode blinn -n "TestFloorMaterial";
	setAttr ".dc" 1.000000;
	setAttr ".ec" 0.300000;
	addAttr -ln "colladaEffectId" -dt "string";
	setAttr .colladaEffectId -type "string" "TestFloorMaterial-fx";
	addAttr -ln "colladaMaterialId" -dt "string";
	setAttr .colladaMaterialId -type "string" "TestFloorMaterial";
createNode shadingEngine -n "TestFloorMaterialSG";
createNode materialInfo -n "materialInfo_1";
createNode place2dTexture -n "place2dTexture_1";
createNode lambert -n "lambert1_1";
	setAttr ".dc" 1.000000;
	setAttr ".c" -type "float3" 0.400000 0.400000 0.400000;
	addAttr -ln "colladaEffectId" -dt "string";
	setAttr .colladaEffectId -type "string" "lambert1-fx";
	addAttr -ln "colladaMaterialId" -dt "string";
	setAttr .colladaMaterialId -type "string" "lambert1";
createNode shadingEngine -n "lambert1_1SG";
createNode materialInfo -n "materialInfo_2";
createNode file -n "TestFloorTexture";
	setAttr ".ftn" -type "string" "C:/Users/Clow Reed/Documents/Unreal Projects/MyProject/Content/Exports/T_Brick_Clay_Beveled_D.TGA";
	addAttr -ln "colladaId" -dt "string";
	setAttr .colladaId -type "string" "TestFloorTexture";
createNode mesh -n "TestFloorShape" -p "|TestFloor";
	addAttr -ln "colladaId" -dt "string";
	setAttr .colladaId -type "string" "TestFloorShape";
	setAttr ".vt[0:15]" 9.238797 0.000000 -3.826829 7.071071 0.000000 -7.071064 3.826839 0.000000 -9.238793 0.000000 0.000000 -10.000000 -3.826830 0.000000 -9.238798 -7.071065 0.000000 -7.071071 -9.238794 0.000000 -3.826838 -10.000000 0.000000 0.000000 -9.238796 0.000000 3.826831 -7.071070 0.000000 7.071065 -3.826836 0.000000 9.238793 0.000000 0.000000 9.999999 3.826833 0.000000 9.238794 7.071067 0.000000 7.071068 9.238795 0.000000 3.826834 10.000000 0.000000 0.000000;
	setAttr ".n[0:41]" 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000 0.000000 1.000000 0.000000;
	setAttr ".usz" 1.000000;
	setAttr ".uvst[0].uvsn" -type "string" "TestFloorShape-map1";
	setAttr ".cuvs" -type "string" "TestFloorShape-map1";
	setAttr ".uvst[0].uvsp[0:15]" 3.763683 1.851860 2.997916 2.997913 1.851863 3.763682 0.500002 4.032585 -0.851860 3.763683 -1.997913 2.997916 -2.763682 1.851863 -3.032584 0.500001 -2.763683 -0.851861 -1.997915 -1.997914 -0.851862 -2.763682 0.499999 -3.032584 1.851861 -2.763682 2.997914 -1.997915 3.763682 -0.851862 4.032584 0.500000;
	setAttr ".ed[0:28]" 0 1 0 1 15 0 0 15 0 1 2 0 2 15 0 2 3 0 3 15 0 3 4 0 4 15 0 4 5 0 5 15 0 5 6 0 6 15 0 6 7 0 7 15 0 7 8 0 8 15 0 8 9 0 9 15 0 9 10 0 10 15 0 10 11 0 11 15 0 11 12 0 12 15 0 12 13 0 13 15 0 13 14 0 14 15 0;
	setAttr ".fc[0:13]" -type "polyFaces"  
		f 3 0 1 -3
		mu 0 3 0 1 15 
		f 3 3 4 -2
		mu 0 3 1 2 15 
		f 3 5 6 -5
		mu 0 3 2 3 15 
		f 3 7 8 -7
		mu 0 3 3 4 15 
		f 3 9 10 -9
		mu 0 3 4 5 15 
		f 3 11 12 -11
		mu 0 3 5 6 15 
		f 3 13 14 -13
		mu 0 3 6 7 15 
		f 3 15 16 -15
		mu 0 3 7 8 15 
		f 3 17 18 -17
		mu 0 3 8 9 15 
		f 3 19 20 -19
		mu 0 3 9 10 15 
		f 3 21 22 -21
		mu 0 3 10 11 15 
		f 3 23 24 -23
		mu 0 3 11 12 15 
		f 3 25 26 -25
		mu 0 3 12 13 15 
		f 3 27 28 -27
		mu 0 3 13 14 15;
connectAttr "lambert1_1SG.msg" "materialInfo_2.sg";
connectAttr "lambert1_1SG.pa" ":renderPartition.st" -na;
connectAttr "lambert1_1.oc" "lambert1_1SG.ss";
connectAttr "lambert1_1.msg" "materialInfo_2.m";
connectAttr "TestFloorMaterialSG.msg" "materialInfo_1.sg";
connectAttr "TestFloorMaterialSG.pa" ":renderPartition.st" -na;
connectAttr "TestFloorMaterial.oc" "TestFloorMaterialSG.ss";
connectAttr "TestFloorMaterial.msg" "materialInfo_1.m";
connectAttr "|TestFloor|TestFloorShape.iog" "TestFloorMaterialSG.dsm" -na;
connectAttr "lambert1_1.msg" ":defaultShaderList1.s" -na;
connectAttr "TestFloorMaterial.msg" ":defaultShaderList1.s" -na;
connectAttr "defaultLightSet.msg" "lightLinker1.lnk[0].llnk";
connectAttr "lambert1_1SG.msg" "lightLinker1.lnk[0].olnk";
connectAttr "defaultLightSet.msg" "lightLinker1.slnk[0].sllk";
connectAttr "lambert1_1SG.msg" "lightLinker1.slnk[0].solk";
connectAttr "defaultLightSet.msg" "lightLinker1.lnk[1].llnk";
connectAttr "TestFloorMaterialSG.msg" "lightLinker1.lnk[1].olnk";
connectAttr "defaultLightSet.msg" "lightLinker1.slnk[1].sllk";
connectAttr "TestFloorMaterialSG.msg" "lightLinker1.slnk[1].solk";
connectAttr "defaultLightSet.msg" "lightLinker1.lnk[2].llnk";
connectAttr ":initialShadingGroup.msg" "lightLinker1.lnk[2].olnk";
connectAttr "defaultLightSet.msg" "lightLinker1.slnk[2].sllk";
connectAttr ":initialShadingGroup.msg" "lightLinker1.slnk[2].solk";
connectAttr "lightLinker1.msg" ":lightList1.ln[0]";
connectAttr "place2dTexture_1.o" "TestFloorTexture.uv";
connectAttr "place2dTexture_1.ofs" "TestFloorTexture.fs";
connectAttr "place2dTexture_1.vt1" "TestFloorTexture.vt1";
connectAttr "place2dTexture_1.vt2" "TestFloorTexture.vt2";
connectAttr "place2dTexture_1.vt3" "TestFloorTexture.vt3";
connectAttr "place2dTexture_1.vc1" "TestFloorTexture.vc1";
connectAttr "place2dTexture_1.of" "TestFloorTexture.of";
connectAttr "place2dTexture_1.s" "TestFloorTexture.s";
connectAttr "place2dTexture_1.c" "TestFloorTexture.c";
connectAttr "place2dTexture_1.tf" "TestFloorTexture.tf";
connectAttr "place2dTexture_1.mu" "TestFloorTexture.mu";
connectAttr "place2dTexture_1.mv" "TestFloorTexture.mv";
connectAttr "place2dTexture_1.wu" "TestFloorTexture.wu";
connectAttr "place2dTexture_1.wv" "TestFloorTexture.wv";
connectAttr "place2dTexture_1.n" "TestFloorTexture.n";
connectAttr "place2dTexture_1.r" "TestFloorTexture.ro";
connectAttr "place2dTexture_1.re" "TestFloorTexture.re";
connectAttr "TestFloorTexture.msg" ":defaultTextureList1.tx" -na;
connectAttr "TestFloorTexture.oc" "TestFloorMaterial.c";
connectAttr "TestFloorTexture.msg" "materialInfo_1.t" -na;
