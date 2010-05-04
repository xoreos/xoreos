--------------------------------------------------------------
--                                                          --
-- NWN mdl Import/Export utility for GMax, v0.3b             --
-- by Wayland (wreid@spectrumanalytic.com)                  --
--                                                          --
-- (c) Wayland Reid                                         --
-- Based on Zaddix's original WOK file importer maxscript   --                                                                 --
-- Based on code by (c) Copyright Zaddix, 2002              --
--                                                          --
-- Use this code in any way you see fit.                    --
-- If you use any/all of the code in your own scripts,      --
-- just credit me and Zaddix somewhere.                     --
--                                                          --
--------------------------------------------------------------

global lastDanglyName = undefined
global AnimRootNodeName = undefined
global LastAnimNumber = undefined
global LastAnimEventNumber = undefined

-------------
-- Structs --
-------------
struct Tokenizer
(
	tokens = #(),
              
	fn SetString str =
	(
		str = trimright(trimleft(str))
		tokens = filterString str "\t ,"
	),
	               
	fn ReadToken =
	(
		if tokens.count > 0 then
		(
			local tok = tokens[1]
			deleteItem tokens 1
			tok
		)
		else
		(
			undefined
		)
	),
               
	fn PeekToken =
	(
		if tokens.count > 0 then tokens[1] else undefined
	),

	fn ReadPoint3 =
	(
		local x = ReadToken() as float;
		local y = ReadToken() as float;
		local z = ReadToken() as float;
		(point3 x y z)
	),

	fn ReadFloat =
	(
		(ReadToken() as float)
	),

	fn ReadInteger =
	(
		(ReadToken() as integer)
	),

	fn ReadString =
	(
		(ReadToken())
	),

	fn ReadColor =
	(
		local r = 255 * (ReadToken() as float);
		local g = 255 * (ReadToken() as float);
		local b = 255 * (ReadToken() as float);
		(color r g b)
	),

	fn ReadRemainingTokens = 
	(
		local str = ""
		while (PeekToken() != undefined) do str += ReadToken() + " "
		substring str 1 (str.count-1)
	)
)

-------------
--- round ---
-------------

fn round x=
(
	local correction = 0 as integer
	local tenth = ((mod x 1) * 10) as integer
	if (tenth >= 5) then correction = 1 as integer
	return (((x as integer) + correction) as integer)
)

----------------------------
--- Check if file exists ---
----------------------------

fn existFile fname = (getfiles fname).count != 0 


-------------------------------
--- Get Next Non-Blank line ---
-------------------------------

fn getNextNonBlankLine fileHandle=
(
	local line = ""
	while ((line == "") and (not eof fileHandle)) do
	(
		line = trimright (readDelimitedString fileHandle "\n")
	)
	return line
)

-----------------------------------
-- UpdateMaterialEditor function --
-----------------------------------
  fn UpdateMaterialEditor MatLib=
  (
    if MatLib.count > 0 then
    (
      local curLibIndex = 1
      for matIndex in 1 to 24 do
      (
        curMat = meditMaterials[matIndex]
        if findString curMat.name " - Default" != 0 then
        (
          -- Put the model material into the Material Editor
          setMeditMaterial matIndex MatLib[curLibIndex]
          -- Set the sample object type for this material to a square
          setMTLMeditObjType MatLib[curLibIndex] 3
          curLibIndex = curLibIndex + 1
          if curLibIndex > MatLib.count then
          (
            exit
          ) -- end curLibIndex > MatLib.count check
        ) -- end current material slot in editor is a standard material check
      ) -- end materials in editor loop
    ) -- end MatLib.count > 0 check
  ) -- end UpdateMaterialEditor function
    
-----------------------------------
--- Create NWN Surface Material ---
-----------------------------------

fn createNWNWalkMaterial =
(
    local SurfaceMatName = #()
    local SurfaceMat = #()
    local matName = "NWN_WalkMaterial";

    local newMaterial = sceneMaterials[matName]
    if (newMaterial == undefined) then
	(
	    append SurfaceMatName "Dirt"
	    append SurfaceMat brown               -- Dirt             
	    append SurfaceMatName "Obscuring"
	    append SurfaceMat white               -- Obscuring    
	    append SurfaceMatName "Grass"
	    append SurfaceMat (color 0 128 0)     -- Grass        
	    append SurfaceMatName "Stone"
	    append SurfaceMat gray                -- Stone        
	    append SurfaceMatName "Wood"
	    append SurfaceMat brown               -- Wood         
	    append SurfaceMatName "Water"
	    append SurfaceMat (color 0 0 192)     -- Water        
	    append SurfaceMatName "Nonwalk"
	    append SurfaceMat (color 180 0 255)   -- Nonwalk      
	    append SurfaceMatName "Transparent"
	    append SurfaceMat black               -- Transparent  
	    append SurfaceMatName "Carpet"
	    append SurfaceMat (color 198 200 0)   -- Carpet       
	    append SurfaceMatName "Metal"
	    append SurfaceMat black               -- Metal        
	    append SurfaceMatName "Puddles"
	    append SurfaceMat (color 0 128 128)   -- Puddles      
	    append SurfaceMatName "Swamp"
	    append SurfaceMat (color 0 64 0)      -- Swamp        
	    append SurfaceMatName "Mud"
	    append SurfaceMat (color 92 52 25)    -- Mud          
	    append SurfaceMatName "Leaves"
	    append SurfaceMat (color 0 192 0)     -- Leaves       
	    append SurfaceMatName "Lava"
	    append SurfaceMat (color 192 0 0)     -- Lava         
	    append SurfaceMatName "BottomlessPit"
	    append SurfaceMat (color 200 99 0)    -- BottomlessPit
	    append SurfaceMatName "DeepWater"
	    append SurfaceMat (color 0 0 96)      -- DeepWater 
	    append SurfaceMatName "Door"
	    append SurfaceMat yellow              -- Door 
  
 	   newMaterial = multimaterial name:matName numsubs:SurfaceMatName.count
  
 	   for i in 1 to SurfaceMatName.count do
 	   (
 	     newMaterial.names[i] = SurfaceMatName[i]
 	     newMaterial.materialList[i].name = SurfaceMatName[i]
 	     newMaterial.materialList[i].diffuse = SurfaceMat[i]  
 	   )
	)
	return newMaterial
)


fn addMDLSkin node buf=
(
	setCommandPanelTaskMode mode:#modify
	modPanel.setCurrentObject node.skin
	
	local weightStream = buf as stringstream;
	local v = 0;
	while (not eof weightStream) do
	(
		v += 1; -- increment vertex
		local tok = Tokenizer()
		local weightLine = readDelimitedString weightStream "\n" 
		tok.SetString(weightLine)
		while (tok.PeekToken() != undefined) do
		(
			local i;
			local b = 0; -- bone index
			local boneName = tok.ReadString();
			local weight = tok.ReadFloat();
			for i = 1 to (skinops.GetNumberBones node.skin) do
			(
				if ((skinops.GetBoneName node.skin i 0) == boneName) then b = i
			)
			if (b == 0) then 
			(
				local boneNode = getNodeByName boneName
				skinops.AddBone node.skin boneNode 0 
				b = skinops.GetNumberBones node.skin
			)
			local gmax_bug = node.numVerts
			skinops.SetVertexWeights (node.skin) v b weight
		)
	)
)

-------------------------
-- ImportFile function --
-------------------------
		 
fn ImportNWNmdl pFile importAnims showWarnings=
(             
	-- Model heirarchy defs
	local MDL_DEF = 1
	local MDL_GEOM = 2
	local MDL_ANIM = 3
	local MDL_NOT_IN_MDL = 4

	local currentPath = getFilenamePath pFile

	-- Open up the file and check to see if its a binary file
	local binMdlStream = fopen pFile "rb"
	local isNotBinary = ReadByte binMdlStream
	fclose binMdlStream

	if (isNotBinary == 0) then
	(
		MessageBox("Binary MDL Files not supported.  Convert model to ASCII");
		return false
	)

	-- Open up the file
	local mdlStream = openFile pFile mode:"r"

	local numAnims = 0
	local animGap = 60
	local lastFrame = 0
	local lineNum = 0
	local mdl_pos = MDL_DEF
	local objProps = ""
	local quitEarly = false
	local in_aabb = false

	try (
		-- Go through the whole mdl file
		while not (eof mdlStream) and not quitEarly do
		(
			local tok = Tokenizer() 
			local line = getNextNonBlankLine mdlStream
			tok.SetString (line)
			lineNum += 1
			 
			-- Read the data ID tag
			local idToken = tok.ReadToken()
			if (idToken[1] == "#") then idToken = "#"
			if (idToken == "donemodel") then
			(
				mdl_pos = MDL_NOT_IN_MDL
				quitEarly = true 
			)
			case mdl_pos of (
				MDL_DEF: 
				(
					case idToken of 
					(
						"#": () -- ignore comments
						"newmodel": newModelName = tok.ReadString()
						"setsupermodel": 
						(
							tok.ReadToken();
							objProps += "setsupermodel=" + tok.ReadRemainingTokens() + "\r\n"
						)
						"beginmodelgeom":	mdl_pos = MDL_GEOM
						default: objProps += idToken + "=" + tok.ReadRemainingTokens() + "\r\n"
					)
				)
			
				MDL_GEOM:
				(
					if (idToken == "aabb") then
					(
						in_aabb = true
						idToken = tok.ReadToken()

					)
					if (not (isKindOf idToken float) and
					    not (isKindOf idToken integer)) then in_aabb = false
					if (in_aabb) then
					(		
						local x = idToken as float
						local y = tok.ReadFloat()
						local z = tok.ReadFloat()
						local bl = point3 x y z
						local tr = tok.ReadPoint3()
						local leaf = (tok.ReadInteger() != -1)
						bl *= 100
						tr *= 100
						local box_pos = (bl+tr)/2
						box_pos.z = bl.z
						box_pos += objPos
						if (leaf) then
						( 
							-- local b = box pos:box_pos length:(tr.y-bl.y) width:(tr.x-bl.x) height:(tr.z-bl.z)
						)
					)
					case idToken of 
					(
						"#": () -- ignore comments
						"node": -- Node Struct
						(
							objType = tok.ReadToken()
							objName = tok.ReadToken()
							objWireColor = color 255 255 255
							objAmbient = color 0 0 0
							objDiffuse = color 0 0 255
							objSpecular = color 0 0 0
							objSelfIllum = color 0 0 0
							objAlpha = 100
							objShine = 0
							objPos = [0, 0, 0]
							objRot = quat 0 0 0 0
							objScale = [1, 1, 1]
							objMaterial = undefined
							textureName = ""
					 
							numVerts = 0
							verts = #()
							numFaces = 0
							faces = #()
							numTVerts = 0
							TVerts = #()
							TVfaces = #()
							smoothGroups = #()
							surfacemats = #()
							numConstraints = 0
							danglyConstraints = #()
							numWeights = 0
							weightBuffer = ""
						)
					 
						"parent": parent = tok.ReadToken()
						"position":	
						(
							objPos = tok.ReadPoint3()
							objPos *= 100
						)
						"orientation": 
						(
							local axis = tok.ReadPoint3()
							local angle = RadToDeg (tok.ReadFloat())
							objRot = quat angle axis
						)
						"scale":	
						(
							objScale = [1,1,1] * tok.ReadFloat()
						)
					 	"wirecolor": objWireColor = tok.ReadColor()
						"ambient": objAmbient = tok.ReadColor()
						"diffuse": objDiffuse = tok.ReadColor()
						"specular": objSpecular = tok.ReadColor()
 						"selfillumcolor": objSelfIllum = tok.ReadColor()
 						"alpha": objAlpha = 100 * tok.ReadFloat()
 
						"bitmap": 
						(
							textureName = tok.ReadToken()
							if (textureName == "NULL") then textureName = ""
						)
						"verts": -- Verticies Struct
						(
							numVerts = tok.ReadInteger()
							for i = 1 to numVerts do
							(
								tok.SetString (getNextNonBlankLine mdlStream)
								lineNum += 1
								local p3 = tok.ReadPoint3()
								p3 *= 100
						 		append verts p3
							)
						)
						"constraints": -- Verticies Struct
						(
							numConstraints = tok.ReadInteger()
							for i = 1 to numConstraints do
							(
								tok.SetString (getNextNonBlankLine mdlStream)
								lineNum += 1
								local p1 = tok.ReadFloat()
						 		append danglyConstraints p1
							)
						)
						"weights": -- Weights for skin modifier
						(
							numWeights = tok.ReadInteger()
							for i = 1 to numWeights do
							(
								weightBuffer += (getNextNonBlankLine mdlStream)
								weightBuffer += "\n"
							)
						)
						"faces": -- Faces Struct
						(
							local p1
							local p3
							numFaces = tok.ReadInteger()
							for i = 1 to numFaces do
							(
								tok.SetString (getNextNonBlankLine mdlStream)
								lineNum += 1
							 
								-- Get the current Face
								-- mdl file is zero-based index, max expects 
								-- one-based index into vert array
								p3 = (tok.ReadPoint3() + [1,1,1])
								append faces p3
							 
								-- Get Smoothing Group
								p1 = tok.ReadInteger()
								append smoothGroups p1
						
								-- Get the current TVface
								p3 = (tok.ReadPoint3() + [1,1,1])
								append TVfaces p3
							 
								-- Get Surface Material
								p1 = tok.ReadInteger()
								append surfacemats p1
							)
						)
						 
						"tverts": -- Verticies Struct
						(
							numTVerts = tok.ReadInteger()
							for i = 1 to numTVerts do
							(
								tok.SetString (getNextNonBlankLine mdlStream)
								lineNum += 1
								local p3 = tok.ReadPoint3()
								append TVerts p3
							)
						)
								 
						"endnode": -- Done parsing mesh, so create the object
						(
							in_aabb = false
							local newObj
							case of
							(
								((objType == "trimesh") or 
								 (objType == "danglymesh") or 
								 (objType == "skin") or
								 (objType == "aabb")):
								(
									-- Make material, if necessary
									local matName = objName
									if ((textureName != "") and 
									    (textureName != "NULL") and
									    (textureName != undefined)) then matName = textureName
									objMaterial = sceneMaterials[matName]
									if (objType == "aabb") then
									(
										objMaterial = createNWNWalkMaterial()
										objProps = ""
									)
									if (objMaterial == undefined) then
									(
										objMaterial = standardmaterial name:matName
										objMaterial.shaderType = 1 -- Blinn
										objMaterial.ambient = objAmbient
										objMaterial.diffuse = objDiffuse
										objMaterial.specular = objSpecular
										objMaterial.glossiness = objShine
										objMaterial.selfillumcolor = objSelfIllum
										objMaterial.opacity = objAlpha
										if (textureName != "") then
										(
											local tgaFilename = currentPath + textureName + ".tga"
											local bmpFilename = currentPath + textureName + ".bmp"
											if ((existFile tgaFilename) or (existFile bmpFilename)) then
											(
												local bmp
												if (existFile tgaFilename) then
													bmp = openBitMap(tgaFilename)
												else
													bmp = openBitMap(bmpFilename)
												local texMap = BitmapTexture bitmap:bmp name:textureName
												objMaterial.mapAmounts[2] = 100
												objMaterial.maps[2] = texmap
												objMaterial.mapEnables[2] = true;
												showTextureMap objMaterial texmap true

											) 
											else
											(
												format "Missing texture file: %\r\n" textureName
												if (showWarnings) then messagebox ("Missing texture file: " + textureName)
											)
										)
									)
											 
									-- Make an editable mesh
									local newObj = mesh name:objName vertices:verts faces:faces wirecolor:objWireColor material:objMaterial materialIDS:surfacemats
										 
									-- Assign smoothing groups
									for i = 1 to smoothGroups.count do
										setFaceSmoothGroup newObj i smoothGroups[i]
												 
									-- Add texture, if we have them
									if numTVerts > 0 do
									(
										setNumTVerts newObj numTVerts
										for i = 1 to numTVerts do setTVert newObj i TVerts[i]
										buildTVFaces newObj
										for i = 1 to numFaces do setTVFace newObj i TVfaces[i]
									)
								
									if numConstraints > 0 then
									(
										for i = 1 to numConstraints do
										(
											local c = (color danglyConstraints[i] danglyConstraints[i] danglyConstraints[i])
											meshop.SetVertColor newObj 0 i c
									 	)
									)

									-- Update the mesh
									update newObj
									
								) --End trimesh case

								(objType == "emitter"):
								(
									newObj = Cone_Angle name:objName
									objRot += quat 180 (point3 1 0 0) -- NWN Emitters starts in +z, GMax cone manipulator starts in -z				
									newObj.Angle = 30;
									newObj.Distance = 100;
								)

								(objType == "light"):
								(
									newObj = omniLight radius:1000 name:objName
									objProps = ""
								)

								default:
								(
									newObj = dummy name:objName
									newObj.boxsize = [5,5,5]
									if (newModelName == objName) then
									(
										newObj.boxsize = [100,100,5]
										objProps += "mdl_root=true\r\n" 
									)
								)
							) --End objType case

							local itsParent = for p in objects where p.name == parent collect p
							if itsParent != undefined and itsParent.count > 0 then
							(           
								newObj.parent = itsParent[1]
								newObj.scale.controller = bezier_scale()
								newObj.position.controller = bezier_position()
								newObj.rotation.controller = TCB_rotation()
								in coordsys parent
								(
									with animate on at time 0 newObj.scale = objScale
									with animate on at time 0 newObj.rotation = objRot
									with animate on at time 0 newObj.position = objPos
								)
							)
							setUserPropBuffer newObj objProps
							setUserProp newObj "node_type" objType 
							objProps = ""
							
							-- Skin modifier must be created after proper positioning.
							if ((objType == "skin") and (numWeights == numVerts)) then
							(
								-- Modifier must be applied here, rather than inside
								-- the addMDLSkin finction where it should be.
								-- I think its a gmax maxscript bug.
								addmodifier newObj (skin())
								addMDLSkin newObj weightBuffer
							)
								
						) -- End endnode case

						"endmodelgeom": -- End of model geometry
						(
							mdl_pos = MDL_ANIM
							if (not importAnims) then quitEarly = true;
						)

						default: 
						(
							objProps += idToken + "=" + tok.ReadRemainingTokens() + "\r\n"
						)
					) -- end iktoken case statement
				) -- End MDL_GEOM case
		
				MDL_ANIM:
				(
					case idToken of
					(
						"#": () -- ignore comments

						"newanim": -- New animation
						(
							animName = tok.ReadToken()
							animRoot = getNodeByName(tok.ReadToken())
							animLength = 0
							firstFrame = lastFrame+animGap
							numAnims += 1
							numEvents = 0
							anim_property = "anim_name_" + (numAnims as string)
							setUserProp animRoot anim_property animName
							anim_property = "anim_firstframe_" + (numAnims as string)
							setUserProp animRoot anim_property firstFrame
						)
						"length": -- animtion length
						(
							animLength = round((tok.ReadToken() as float) * frameRate) as integer
							lastFrame = firstFrame + animLength
							anim_property = "anim_lastframe_" + (numAnims as string)
							setUserProp animRoot anim_property lastFrame
						)
						"transtime": -- animtion length
						(
							animTrans = (tok.ReadToken() as float)
							anim_property = "anim_transtime_" + (numAnims as string)
							setUserProp animRoot anim_property animTrans
						)
						"event": -- animtion event
						(
							numEvents += 1
							local anim_property = "anim_eventframe_" + (numAnims as string) + "_" + (numEvents as string)
							local event_frame = round((tok.ReadFloat()*frameRate) + firstFrame)
							setUserProp animRoot anim_property event_frame
									
							anim_property = "anim_eventname_" + (numAnims as string) + "_" + (numEvents as string)
							local event_name = tok.ReadString()
							setUserProp animRoot anim_property event_name
						)
						"node":
						(
							objType = tok.ReadString()
							objName = tok.ReadString()
							
							local animObj = getNodeByName(objName)
							if (animObj != undefined) then
							(
								in coordsys parent
								(
									local zeroPos = at time 0 animObj.pos
									local zeroRot = at time 0 animObj.rotation
									local keytime = 0
									keytime = (firstFrame - 1)
									with animate on at time keytime animObj.pos = zeroPos
									with animate on at time keytime animObj.rotation = zeroRot
									newkey = getKeyIndex animObj.pos.controller keytime
									animObj.pos.keys[newkey].inTangentType = #step 
									animObj.pos.keys[newkey].outTangentType = #step 
									newkey = getKeyIndex animObj.rotation.controller keytime
									animObj.rotation.keys[newkey].continuity = 0 
	
									keytime = (lastFrame + 1)
									with animate on at time keytime animObj.pos = zeroPos
									with animate on at time keytime animObj.rotation = zeroRot
									newkey = getKeyIndex animObj.pos.controller keytime
									animObj.pos.keys[newkey].inTangentType = #step 
									animObj.pos.keys[newkey].outTangentType = #step 
									newkey = getKeyIndex animObj.rotation.controller keytime
									animObj.rotation.keys[newkey].continuity = 0 
								)
							)
						)
						"positionkey":
						(
							local newkey
							local animObj = getNodeByName(objName)
							if (animObj != undefined) then
							(
								local keyCount = undefined;
								if (tok.PeekToken() != undefined) then keyCount = tok.ReadInteger()
								local originalObjPos
								in coordsys parent 
								(
									at time 0 originalObjPos = animObj.pos
									tok.SetString (getNextNonBlankLine mdlStream)
									while ((tok.PeekToken() != "endlist") and (keyCount != 0)) do
									(
										lineNum += 1
										local keytime = round(firstFrame + (frameRate*tok.ReadFloat()))
										local p3 = tok.ReadPoint3()
										p3 *= 100
										with animate on at time keytime animObj.pos = p3
									 	newkey = getKeyIndex animObj.pos.controller keytime
										animObj.pos.keys[newkey].inTangentType = #linear 
										animObj.pos.keys[newkey].outTangentType = #linear 
										tok.SetString (getNextNonBlankLine mdlStream)
										if (keyCount != undefined) then keyCount -= 1
									)
								)
							)
						)
						"orientationkey":
						(
							local keytime
							local axis
							local angle
							local objRot
							local newKey
							local originalObjRot
							local keyCount = undefined
							if (tok.PeekToken() != undefined) then keyCount = tok.ReadInteger()
							local animObj = getNodeByName(objName)
							if (animObj != undefined) then
							(
								in coordsys parent 
								(
									tok.SetString (getNextNonBlankLine mdlStream)
									while ((tok.PeekToken() != "endlist") and (keyCount != 0)) do
									(
										lineNum += 1
										keytime = round(firstFrame + (frameRate*tok.ReadFloat()))
										axis = tok.ReadPoint3()
										angle = RadToDeg (tok.ReadFloat())
										objRot = quat angle axis
										local existingPosKey = getKeyIndex animObj.pos.controller keytime
										local existingPos
										if (existingPosKey > 0) then existingPos = animObj.pos.keys[existingPosKey].value
										with animate on at time keytime animObj.rotation = objRot
										newkey = getKeyIndex animObj.rotation.controller keytime
										animObj.rotation.keys[newkey].continuity = 0 
										if (existingPosKey == 0) then
										(
											local newPosKey = getKeyIndex animObj.pos.controller keytime
											if (newPosKey != 0) then deleteKey animObj.pos.controller newPosKey
										) 
										else
										(
											animObj.pos.keys[existingPosKey].value = existingPos
										)
										
										tok.SetString (getNextNonBlankLine mdlStream)
										if (keyCount != undefined) then keyCount -= 1
									)
								)
							)
						)
						"doneanim": ()
						default: ()
					) -- end idtoken case
				) -- End MDL_ANIM case 
			) -- end mdl_pos case statement
		) -- end while not eof
		lastFrame += animGap
		if (importAnims) then animationRange = (interval 0 lastFrame)
	)
	catch
	(
		--messageBox ("NWN importer Script Error reading mdl file at line:" + lineNum as string) 
		close mdlStream
		throw
	)
	close mdlStream
)


-----------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------
---
---   Export routines
---
-----------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------


------------------------
--- WriteTrimeshNode ---
------------------------

fn WriteTrimeshNode node=
(
	local isAABB = ((getUserProp node "node_type" == "aabb") or 
			    (getUserProp node "node_type" == "pwk_aabb"))

	local isSkin = (node.modifiers["skin"] != undefined)

	if (isAABB) then
	(
		format "    diffuse 0.8000 0.8000 0.8000\r\n"
		format "    ambient 0.2000 0.2000 0.2000\r\n"
		format "    specular 0.0000 0.0000 0.0000\r\n"
		format "    render 0\r\n"
		format "    bitmap NULL\r\n"
	)
	if ((node.material != undefined) and (not isAABB)) then
	(
		local r = node.material.diffuse.r / 255.0
		local g = node.material.diffuse.g / 255.0
		local b = node.material.diffuse.b / 255.0
		format "    diffuse % % %\r\n" r g b

		r = node.material.ambient.r / 255.0
		g = node.material.ambient.g / 255.0
		b = node.material.ambient.b / 255.0
		format "    ambient % % %\r\n" r g b

		r = node.material.specular.r / 255.0
		g = node.material.specular.g / 255.0
		b = node.material.specular.b / 255.0
		format "    specular % % %\r\n" r g b

		r = node.material.selfillumcolor.r / 255.0
		g = node.material.selfillumcolor.g / 255.0
		b = node.material.selfillumcolor.b / 255.0
		format "    selfillumcolor % % %\r\n" r g b

		local a = node.material.opacity / 100.0
		format "    alpha %\r\n" a

		if ((node.material.mapEnables[2]) and
			(getUserProp node "nobitmap" != 1)) then
		(
			local fullpath = node.material.maps[2].filename
			local path_parts = filterString fullpath "\\"
			local filename = path_parts[path_parts.count]
			if ((findString filename ".") >= 1) then 
				filename = substring filename 1 ((findString filename ".")-1)
			format "    bitmap %\r\n" filename
		)
	)
		
	format "    verts %\r\n" node.mesh.numverts
	for i = 1 to node.mesh.numverts do 
	(
		in coordsys parent 
		(
			v = getvert node.mesh i
			v *= 0.01
		)
		format "        % % %\r\n" v.x v.y v.z
	)

	format "    faces %\r\n" node.mesh.numfaces
	for i = 1 to node.mesh.numfaces do
	(
		v = getFace node.mesh i
		smooth = getFaceSmoothGroup node.mesh i
		matid = getFaceMatID node.mesh i

		local x = v.x as Integer 
		local y = v.y as Integer
		local z = v.z as Integer
		x -= 1 -- mdl file is zero-based index, max expects one-based index into vert array
		y -= 1 --
		z -= 1 --
		format "        % % % % " x y z smooth

		if (node.mesh.numtverts > 0) then
		(
			tv = getTVFace node.mesh i
			x = tv.x as Integer
			y = tv.y as Integer
			z = tv.z as Integer
			x -= 1 -- mdl file is zero-based index, max expects one-based index into vert array
			y -= 1 --
			z -= 1 --
		) 
		else 
		(
			x = 0
			y = 0
			z = 0
		)
		format "% % % %\r\n" x y z matid
	)
	
	if (node.mesh.numtverts > 0) then
	(
		format "    tverts %\r\n" node.mesh.numtverts
		for i = 1 to node.mesh.numtverts do
		(
			v = gettvert node.mesh i
			format "        % % 0\r\n" v.x v.y
		)
	)

	if (getuserprop node "node_type" == "danglymesh") then
	(
		format "    danglymesh 1\r\n"
		format "# Fake period, tightness and displacement parameters\r\n"
		format "    period 6.000000\r\n"
		format "    tightness 3.000000\r\n"
		format "    displacement 0.025000\r\n"
		format "    constraints %\r\n" node.numverts
		for i = 1 to node.numverts do
		(
			local c = GetVertColor node i
			format "        %\r\n" c.r
		)
	)
	
	if (isSkin) then
	(
		setCommandPanelTaskMode mode:#modify
		modPanel.setCurrentObject node.skin

		format "    weights %\r\n" node.numVerts
		local v
		
		local gmax_bug = node.numVerts
		for v = 1 to (skinops.GetNumberVertices node.skin) do
		(
			local b
			format "        "
			for b = 1 to (skinops.getVertexWeightCount node.skin v) do
			(
				local bone_id = skinops.getVertexWeightBoneID node.skin v b
				local bone_name = skinops.getBoneName node.skin bone_id 0
				local weight = skinops.getVertexWeight node.skin v b
				format "% % " bone_name weight
			)
			if ((skinops.getVertexWeightCount node.skin v) == 0) then
			(
				local bone_name = skinops.getBoneName node.skin 1 0
				format "% % " bone_name 0
			)
			format "\r\n" 
		)
	)
)


-------------------------
--- BuildAABBTreeNode ---
-------------------------

fn BuildAABBTreeNode node facemids facelist level=
(
	if (facelist.count == 0) then return 0
	if (level > 10) then 
	(
		MessageBox("AABB Generation: Maximum recursion level reached.  Check for duplicate verticies and/or faces.")
		return 0
	)
	local bot_left = point3 100000.0 100000.0 100000.0
	local top_right = point3 -100000.0 -100000.0 -100000.0
	local midpoint_average = point3 0 0 0

	for i=1 to facelist.count do
	(
		local v3 = getface node.mesh facelist[i]
		local p1 = getvert node.mesh v3.x
		local p2 = getvert node.mesh v3.y
		local p3 = getvert node.mesh v3.z

		if (p1.x < bot_left.x) then bot_left.x = p1.x
		if (p1.y < bot_left.y) then bot_left.y = p1.y
		if (p1.z < bot_left.z) then bot_left.z = p1.z

		if (p2.x < bot_left.x) then bot_left.x = p2.x
		if (p2.y < bot_left.y) then bot_left.y = p2.y
		if (p2.z < bot_left.z) then bot_left.z = p2.z

		if (p3.x < bot_left.x) then bot_left.x = p3.x
		if (p3.y < bot_left.y) then bot_left.y = p3.y
		if (p3.z < bot_left.z) then bot_left.z = p3.z

		if (p1.x > top_right.x) then top_right.x = p1.x
		if (p1.y > top_right.y) then top_right.y = p1.y
		if (p1.z > top_right.z) then top_right.z = p1.z

		if (p2.x > top_right.x) then top_right.x = p2.x
		if (p2.y > top_right.y) then top_right.y = p2.y
		if (p2.z > top_right.z) then top_right.z = p2.z

		if (p3.x > top_right.x) then top_right.x = p3.x
		if (p3.y > top_right.y) then top_right.y = p3.y
		if (p3.z > top_right.z) then top_right.z = p3.z

		midpoint_average += facemids[facelist[i]]
	)

	midpoint_average /= facelist.count
	
	local box_pos = (bot_left+top_right)/2

--	format "DEBUG:            % % \r\n" bot_left top_right
	box_pos.z = bot_left.z
	in coordsys parent ( box_pos += node.pos)

	local bl = bot_left / 100
	local tr = top_right / 100
	format "        % % % % % %" bl.x bl.y bl.z tr.x tr.y tr.z
	if (facelist.count == 1) then
	(
-- 	display box
--		local b = box pos:box_pos length:(top_right.y-bot_left.y) width:(top_right.x-bot_left.x) height:(top_right.z-bot_left.z)
--		sphere pos:box_pos radius:10

		format " %\r\n" (facelist[1]-1)
	)
	else
	(
		format " -1\r\n"
		local bb_size = top_right - bot_left
-- 		Axis 1=x 2=y 3=z
		local axis = 1 
		if (bb_size.y > bb_size.x) then axis = 2
		if (bb_size.z > bb_size.y) then axis = 3

		-- Check exception case, where all points are coplanar with the axis-aligned split plane
		local change_axis = true
		for i=1 to facelist.count do
		(
			p1 = facemids[facelist[i]]
			case axis of
			(
				1: change_axis = change_axis and (p1.x == midpoint_average.x)
				2: change_axis = change_axis and (p1.y == midpoint_average.y)	
				3: change_axis = change_axis and (p1.z == midpoint_average.z)	
			)
--			format "DEBUG:            % % \r\n" p1 midpoint_average
		)
		if (change_axis) then
		(
			axis += 1
			if (axis > 3) then axis = 1
		)

		local leftside = true
		local good_split = false
		local leftlist = #()
		local rightlist = #()
		while (not good_split) do
		(
			leftlist = #()
			rightlist = #()
			for i=1 to facelist.count do
			(
				p1 = facemids[facelist[i]]
--				format "DEBUG:            % % \r\n" p1 axis
				case axis of
				(
					1: leftside = (p1.x < midpoint_average.x)
					2: leftside = (p1.y < midpoint_average.y)	
					3: leftside = (p1.z < midpoint_average.z)	
				)
				if (leftside) then
					append leftlist facelist[i]
				else
					append rightlist facelist[i]
			)
			if ((leftlist.count > 0) and (rightlist.count >0)) then
			(
				good_split = true
			)
			else
			(
				axis += 1
				if (axis > 3) then axis = 1
			)
		)
	
		BuildAABBTreeNode node facemids leftlist (level+1)
		BuildAABBTreeNode node facemids rightlist (level+1)
	)
)


---------------------
--- WriteAABBTree ---
---------------------

fn WriteAABBTree node=
(
	local facemids =#()
	local facelist = #()
	if ((mod node.mesh.numfaces 2) == 1) then
	(
		MessageBox("Odd number of faces in AABB tree.  Expect problems.");
	)
	for i=1 to node.mesh.numfaces do
	(
		v3 = getface node.mesh i
		p1 = getvert node.mesh v3.x
		p2 = getvert node.mesh v3.y
		p3 = getvert node.mesh v3.z
		mid = (p1+p2+p3) / 3
		append faceMids mid
		append facelist i
	)

	format "    aabb " -- no newline here, NWN chokes if the data doesn't start on the same line
	BuildAABBTreeNode node faceMids facelist 0
)


------------------------------
--- WriteNWNRootProperties ---
------------------------------

fn WriteNWNRootProperties node=
(
	local dep = getUserProp node "filedependancy"
	if (dep == undefined) then dep = "Unknown"
	format "filedependancy %\r\n" dep
	format "newmodel %\r\n" node.name
	format "setsupermodel % %\r\n" node.name (getUserProp node "setsupermodel")
	format "classification %\r\n" (getUserProp node "classification")
	format "setanimationscale %\r\n" (getUserProp node "setanimationscale")
	format "beginmodelgeom %\r\n" node.name
)
	
-------------------------
--- DumpNWNProperties ---
-------------------------

fn DumpNWNProperties node=
(
	local line
	local val
	local buffer = (getUserPropBuffer(node) + "\r\n") as stringstream
	while (not eof buffer) do
	(
		line = trimright (readline buffer)
		if (line != "") then
		(
			val = filterstring line "="
			val[1] = trimright(trimleft(val[1]))
			if ((val[1] != "node_type") and
			    (val[1] != "mdl_root") and
			    (val[1] != "number_animations") and
			    (val[1] != "StartFrame") and
			    (val[1] != "EndFrame") and 
			    (val[1] != "danglymesh") and
			    (val[1] != "nobitmap") and
			    ((findString val[1] "anim") != 1)) then 
				format "    % %\r\n" val[1] val[2]
		)
	)
)


--------------------
--- ExportNWNmdl ---
--------------------

fn ExportNWNmdl node only_nodetype children=
(
	local isMesh = false
	local isEmitter = false
	
	if (getUserProp node "node_type" == undefined) then
	(
		setUserProp node "node_type" "dummy"
		if (canConvertTo node Editable_Mesh) then setUserProp node "node_type" "trimesh"
		if (classOf node == Cone_Angle) then setUserProp node "node_type" "emitter" 
		if (classOf(node.baseobject) == OmniLight) then setUserProp node "node_type" "light"
	)
	if (node.modifiers["skin"] != undefined) then setUserProp node "node_type" "skin"

	local node_type = getuserprop node "node_type"

	if ((node_type == "trimesh") or 
	    (node_type == "danglymesh") or
	    (node_type == "skin") or
	    (node_type == "aabb") or 
	    (node_type == "pwk_aabb")) then isMesh = true
	if (node_type == "emitter") then isEmitter = true

	if ((node_type == only_nodetype) or ((only_nodetype == "") and
							 (node_type != "pwk_aabb") and
							 (node_type != "pwk_use_dummy"))) then
	(
		local node_name = node.name
		local parent_name
		if (node_type == "pwk_use_dummy") then
		(
			node_type = "dummy"
			node_name = (substring node.parent.name 5 30) + "_pwk_" + node.name
			parent_name = node.parent.name
			parent_name += "_pwk"
		)
		if (node_type == "pwk_aabb") then
		(
			node_type = "trimesh"
			node_name = node.parent.name
			node_name += "_wg"
			parent_name = node.parent.name
			parent_name += "_pwk"
		)
		format "node % %\r\n" node_type node_name
		if (node.parent != undefined) then
		(
			if (parent_name == undefined) then parent_name = node.parent.name
			format "    parent %\r\n" parent_name
			in coordsys parent
			(
				local objScale = at time 0 node.scale;
				local objPos = at time 0 node.pos;
				local objRot = at time 0 node.rotation;
				if (isEmitter) then objRot += quat -180 (point3 1 0 0)
				local rotVector = normalize (point3 -objRot.x -objRot.y -objRot.z)
				local angle = -degToRad(objRot.angle)
				objPos *= 0.01		
		
				format "    position % % %\r\n" objPos.x objPos.y objPos.z
				format "    orientation % % % %\r\n" rotVector.x rotVector.y rotVector.z angle
				format "    scale %\r\n" objScale.x
			)
			DumpNWNProperties node
		)
		else
		(
			format "    parent NULL\r\n"
		)
	
		if (isMesh) then WriteTrimeshNode(node)
		if (node_type == "aabb") then WriteAABBTree node
		if (node_type == "light") then
		(
			format "#   Fake light data\r\n"
			format "    ambientonly 0\r\n"
			format "    shadow 0\r\n"
			format "    isdynamic 0\r\n"
			format "    affectdynamic 1\r\n"
			format "    lightpriority 5\r\n"
			format "    fadingLight 1\r\n"
			format "    flareradius 0\r\n"
			format "    radius 14\r\n"
			format "    multiplier 1\r\n"
			format "    color 0 0 0	\r\n"		
		)
		format("endnode\r\n");
	)

    local skinnodes = #()
	if (children) then
	(
		for c in node.children do 
		(
			if (c.modifiers["skin"] == undefined) then
				ExportNWNmdl c only_nodetype children
			else
				append skinnodes c
		)
	)
	
	for c in skinnodes do ExportNWNmdl c only_nodetype children
)	 

-----------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------
---
---   Animation export routines
---
-----------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------

fn ExportAnimKeys node firstframe lastframe=
(
	if (getUserProp node "node_type" == undefined) then
	(
		setUserProp node "node_type" "dummy"
		if (canConvertTo node Editable_Mesh) then setUserProp node "node_type" "trimesh"
		if (classOf node == Cone_Angle) then setUserProp node "node_type" "emitter" 
		if (classOf(node.baseobject) == OmniLight) then setUserProp node "node_type" "light"
	)
	local node_type = getuserprop node "node_type"

	format "    node % %\r\n" node_type node.name
	if (node.parent != undefined) then
		format "        parent %\r\n" node.parent.name
	else
		format "        parent NULL\r\n" 

	local t
	local keycount = 0;
	for t = firstframe to lastframe do
	(
		if ((getKeyIndex node.pos.controller t) > 0) then
		(
			if (keycount == 0) then format "        positionkey\r\n"
			keycount += 1;
			at time t in coordsys parent objPos = node.pos * 0.01
			key_time = ((t-firstframe) as float) / (frameRate as float);
			format "            % % % %\r\n" key_time objPos.x objPos.y objPos.z
		)
	)
	if (keycount > 0) then format "        endlist\r\n"

	keycount = 0;
	for t = firstframe to lastframe do
	(
		if ((getKeyIndex node.rotation.controller t) > 0) then
		(
			if (keycount == 0) then format "        orientationkey\r\n"
			keycount += 1;
			local objRot
			at time t in coordsys parent objRot = node.rotation;
			local rotVector = normalize (point3 -objRot.x -objRot.y -objRot.z)
			local angle = -degToRad(objRot.angle)
			key_time = ((t-firstframe) as float) / (frameRate as float);
			format "            % % % % %\r\n" key_time rotVector.x rotVector.y rotVector.z angle
		)
	)
	if (keycount > 0) then format "        endlist\r\n"

	format("    endnode\r\n")

	for c in node.children do ExportAnimKeys c firstframe lastframe
)

fn ExportAnimEvents node anim_number firstframe=
(
	local user_props = getUserPropBuffer node as stringstream
	local event_prefix = "anim_eventname_" + (anim_number as string)
	while not eof user_props do
	(
		local prop = readDelimitedString user_props "\n"
		if ((findString prop (event_prefix + "_")) == 1) then
		(
			local event_name = trimleft (trimright (substring prop ((findstring prop "=")+1) -1))
			if (event_name != "") then
			(
				local event_number = trimright(substring prop ((event_prefix.count)+2) ((findstring prop "=")-(event_prefix.count)-2))
				local event_prop = "anim_eventframe_" + (anim_number as string) + "_" + (event_number as string)
				event_time = (getUserProp node event_prop) as float
				event_time = ((event_time - firstframe) as float) / (frameRate as float)
				format "    event % %\r\n" event_time event_name
			)
		)
	)
)

fn ExportAnimByNumber node anim_number=
(
	local name = getUserProp node ("anim_name_" + (anim_number as string))
	local firstframe = getUserProp node ("anim_firstframe_" + (anim_number as string))
	local lastframe = getUserProp node ("anim_lastframe_" + (anim_number as string))
	local transtime = getUserProp node ("anim_transtime_" + (anim_number as string))
	local anim_length = ((lastframe - firstframe) as float) / (frameRate as float);
	format "\r\n"
	format "newanim % %\r\n" name node.name
	format "    length %\r\n" anim_length
	format "    transtime %\r\n" transtime
	format "    animroot %\r\n" node.name
	ExportAnimEvents node anim_number firstframe
	ExportAnimKeys node firstframe lastframe
	format "doneanim % %\r\n" name node.name
	format "\r\n"
)

fn ExportAnims node export_anim_name=
(
	if (node == undefined) then return 
	local all_anims
	all_anims = #()
	local user_props = getUserPropBuffer node as stringstream
	while not eof user_props do
	(
		local prop = readDelimitedString user_props "\n"
		if ((findString prop "anim_name") == 1) then
		(
			local anim_name = trimleft (trimright (substring prop ((findstring prop "=")+1) -1))
			if (anim_name != "") then
			(
				local anim_number = substring prop 11 ((findstring prop "=")-11)
				anim_number = (trimright anim_number) as integer
				if ((export_anim_name == "<All>") or (export_anim_name == anim_name)) then 
					append all_anims anim_number
			)
		)
	)
	for anim_number in all_anims do
	(
		ExportAnimByNumber node anim_number
	)
)

-------------------------
--- ShowDanglyConstraints ---
-------------------------

fn ShowDanglyConstraints node onoff =
(
	if (node != undefined) then
	(
		if ((classof(node) == Editable_Mesh) and ((GetUserProp node "node_type") == "danglymesh")) then
			node.showVertexColors = onoff
		if (onoff) then
			lastDanglyName = node.name
		else
			lastDanglyName = undefined
	)
)

-----------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------
---
---   Rollout definitions
---
-----------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------

rollout ImportRollout "Import MDL" width:160 height:192
(
	groupBox igrp4 "Filename" pos:[8,8] width:144 height:72
	editText import_filename_edit "" pos:[16,24] width:128 height:16
	button import_browse_button "Browse" pos:[72,48] width:70 height:24
	groupBox igrp5 "Options" pos:[8,80] width:144 height:64
	checkbox anims_checkbox "Import animations" pos:[16,104] width:128 height:16
	checkbox warn_checkbox "Show warnings" pos:[16,120] width:128 height:16
	button import_button "Import" pos:[32,152] width:104 height:24

	on import_browse_button pressed do 
	(
		local filename = getOpenFileName caption:"Import mdl" types:"NWN mdl (*.mdl)|*.mdl|NWN ASCII Model (*.ascii)|*.ascii|All Files (*.*)|*.*|"
		if (filename == undefined) then filename = ""
		import_filename_edit.text = filename
	)

	on import_button pressed do
	(
		--Open up a dialog box
		-- local mdlFileName = "c:\\windows\\desktop\\nwn\\" + import_filename_edit.text
		local mdlFileName = import_filename_edit.text
  
		if (not existFile mdlFileName) then
		(
			MessageBox ("Model not imported.  " + mdlFileName + " does not exist.") title:"File does not exist"
		) 
		else
		(
			clearListener()
			format "Importing %\r\n" mdlFileName
						  
			ImportNWNmdl mdlFileName anims_checkbox.checked warn_checkbox.checked
			  
			-- Redraw gmax viewports
			clearSelection()
			max tool zoomextents all
			max views redraw
	
			format "Finished.\n\r\n"
		)
	)
)

rollout ExportRollout "Export" width:160 height:104 rolledUp:true
(
	button export_mdl_button "MDL" pos:[8,8] width:64 height:24
	button export_node_button "Node" pos:[88,8] width:64 height:24
	button export_wok_button "WOK" pos:[8,40] width:64 height:24
	button export_pwk_button "PWK" pos:[8,72] width:64 height:24
	dropDownList anim_combo "Animations to Export" pos:[8,112] width:144 height:40
	button export_anim_button "Anim" pos:[88,40] width:64 height:24

on export_mdl_button pressed do
(
local node = selection[1]
clearListener()
format "# Exported from Wayland's NWN Maxscript v0.3b at %\r\n" localtime
format "# mdl file\r\n"
format "#\r\n"
if ((getUserProp node "mdl_root") == true) then
(
WriteNWNRootProperties node
)
local anim_name = anim_combo.text
ExportNWNmdl node "" true
if ((getUserProp node "mdl_root") == true) then 
(
format "endmodelgeom %\r\n" node.name
if (anim_name != "<None>") then ExportAnims node anim_name
format "donemodel %\r\n" node.name
)
clearSelection()
select node
)
on export_mdl_button pressed do
(
local node = selection[1]
clearListener()
format "# Exported from Wayland's NWN Maxscript v0.3b at %\r\n" localtime
format "# mdl file\r\n"
format "#\r\n"
if ((getUserProp node "mdl_root") == true) then
(
WriteNWNRootProperties node
)
local anim_name = anim_combo.text
ExportNWNmdl node "" true
if ((getUserProp node "mdl_root") == true) then 
(
format "endmodelgeom %\r\n" node.name
if (anim_name != "<None>") then ExportAnims node anim_name
format "donemodel %\r\n" node.name
)
clearSelection()
select node
)
on export_mdl_button pressed do
(
local node = selection[1]
clearListener()
format "# Exported from Wayland's NWN Maxscript v0.3b at %\r\n" localtime
format "# mdl file\r\n"
format "#\r\n"
if ((getUserProp node "mdl_root") == true) then
(
WriteNWNRootProperties node
)
local anim_name = anim_combo.text
ExportNWNmdl node "" true
if ((getUserProp node "mdl_root") == true) then 
(
format "endmodelgeom %\r\n" node.name
if (anim_name != "<None>") then ExportAnims node anim_name
format "donemodel %\r\n" node.name
)
clearSelection()
select node
)


	on export_node_button pressed do
	(
		local node = selection[1]
		clearListener()
		format "# Exported from Wayland's NWN Maxscript v0.3b at %\r\n" localtime
		format "# mdl file\r\n"
		format "#\r\n"
		ExportNWNmdl node "" false
		clearSelection()
		select node
	)

	on export_anim_button pressed do
	(
		local node = selection[1]
		clearListener()
		format "# Exported from Wayland's NWN Maxscript v0.3b at %\r\n" localtime
		format "# mdl file\r\n"
		format "#\r\n"
		local anim_name = anim_combo.text
		if (anim_name != "<None>") then ExportAnims node anim_name
		clearSelection()
		select node
	)

	on export_wok_button pressed do
	(
		clearListener()
		format "# Exported from Wayland's NWN Maxscript v0.3b at %\r\n" localtime
		format "# wok file\r\n"
		format "#\r\n"
		if ((getUserProp selection[1] "mdl_root") == true) then
		(
			format "beginwalkmeshgeom %\r\n" selection[1].name
			ExportNWNmdl selection[1] "aabb" true
			format "endwalkmeshgeom %\r\n" selection[1].name
		)
		else
		(
			format "beginwalkmeshgeom %\r\n" selection[1].parent.name
			ExportNWNmdl selection[1] "aabb" true
			format "endwalkmeshgeom %\r\n" selection[1].parent.name
		)
	)

	on export_pwk_button pressed do
	(
		local node = selection[1]
		if ((node.name.count < 7) or (node.name[4] != "_")) then
			MessageBox("You should follow the placeable naming convention of \"xxx_xxx\" or \"xxx_xxxx\"")
		clearListener()
		format "# Exported from Wayland's NWN Maxscript v0.3b at %\r\n" localtime
		format "# pwk file\r\n"
		format "#\r\n"
		ExportNWNmdl node "pwk_aabb" true
		ExportNWNmdl node "pwk_use_dummy" true
	)

	on ExportRollout open do callbacks.addscript #selectionSetChanged "selChanged()" id:#waysNWNtool
	on ExportRollout close do callbacks.removeScripts #selectionSetChanged id:#waysNWNtool
)

rollout RootPropRollout "Model Properties" width:160 height:216 rolledUp:true
(
	button make_root_button "Create \"Model Base\"" pos:[8,8] width:144 height:24
	groupBox rgrp3 "Model Properties" pos:[8,40] width:144 height:160
	editText supermodel_edit "" pos:[56,56] width:88 height:16
	label rlbl2 "Super" pos:[16,56] width:32 height:16
	radioButtons classification_radio "" pos:[56,80] width:69 height:96 labels:#("Character", "Tile", "Door", "FX", "Other") columns:1
	label rlbl3 "Class" pos:[16,80] width:32 height:16
	label rlbl4 "Animation Scale" pos:[16,166] width:80 height:15
	spinner anim_scale_spinner "" pos:[96,166] width:48 height:16 range:[0,100,0] scale:0.01

	on make_root_button pressed do 
	(
			if (selection.count == 0) then
			(
				local new_dummy = dummy name:"unnamed model" boxSize:[100,100,5] pos:[0,0,0]
				select new_dummy
			)
			selection[1].boxSize = [100,100,5]
			setUserProp selection[1] "mdl_root" "true"
			setUserProp selection[1] "setsupermodel" "NULL"
			setUserProp selection[1] "classification" "Character"
			setUserProp selection[1] "setanimationscale" "1.0"
			--
			-- Work around until I can figure out how to get around this forward reference restriction
			-- UpdateRootPropertiesUI selection[1]
			local sm = getuserprop selection[1] "setsupermodel"
			local mdl_class = getuserprop selection[1] "classification"
			local anim_scale = getuserprop selection[1] "setanimationscale"
			if (sm == undefined) then sm = "NULL"
			if (mdl_class == undefined) then mdl_class = 1
			if (anim_scale == undefined) then anim_scale = 1
			RootPropRollout.make_root_button.enabled = false
			RootPropRollout.supermodel_edit.enabled = true
			RootPropRollout.supermodel_edit.text = sm
			RootPropRollout.classification_radio.enabled = true
			case (mdl_class) of
			(
				"Character": RootPropRollout.classification_radio.state = 1
				"Tile": RootPropRollout.classification_radio.state = 2
				"Door": RootPropRollout.classification_radio.state = 3
				"FX": RootPropRollout.classification_radio.state = 4
				"Other": RootPropRollout.classification_radio.state = 5
			)
			RootPropRollout.anim_scale_spinner.enabled = true
			RootPropRollout.anim_scale_spinner.value = anim_scale
			select selection[1]
	)
	on supermodel_edit entered newText do setUserProp selection[1] "setsupermodel" newText
	on classification_radio changed newState do
	(
		case newState of
		(
			1: setUserProp selection[1] "classification" "Character"
			2: setUserProp selection[1] "classification" "Tile"
			3: setUserProp selection[1] "classification" "Door"
			4: setUserProp selection[1] "classification" "FX"
			5: setUserProp selection[1] "classification" "Other"
		)
	)
	on anim_scale_spinner changed newVal do
	(
		setUserProp selection[1] "setanimationscale" newVal
	)
)

rollout GeometryPropRollout "Node Properties" width:160 height:136 rolledUp:true
(
	radioButtons geom_type_radio "" pos:[24,32] width:98 height:64 enabled:true labels:#("Trimesh", "Danglymesh", "AABB", "PWK walkmesh")
	checkButton show_dangle_checkbutton "Danglymesh Constraints" pos:[8,440] width:144 height:24
	groupBox grp1 "Node Type" pos:[8,8] width:144 height:128
	groupBox grp2 "Render Options" pos:[8,144] width:144 height:96
	checkbox render_checkbox "Render" pos:[24,168] width:96 height:16
	checkbox shadow_checkbox "Shadow" pos:[24,184] width:96 height:16
	checkbox beaming_checkbox "Beaming" pos:[24,200] width:104 height:16
	groupBox grp3 "Tile Fade" pos:[8,248] width:144 height:96
	radioButtons tile_fade_radio "" pos:[24,272] width:109 height:64 labels:#("Don't Fade", "Fade", "Horizontal Blocker", "Vertical Blocker")
	groupBox grp4 "Alpha Channel Options" pos:[8,352] width:144 height:80
	radioButtons alpha_radio "" pos:[24,376] width:112 height:48 labels:#("Don't Use", "Transparency Map", "Reflection Map")
	radioButtons dummy_type_radio "" pos:[24,96] width:104 height:32 labels:#("Dummy", "PWK use dummy")
	checkbox no_bitmap_checkbox "Don't export bitmap" pos:[24,216] width:120 height:16

	on geom_type_radio changed newState do
	(
		local node = selection[1]
		case newState of 
		(
			1:
			(
				setUserProp node "node_type" "trimesh"
				node.material = sceneMaterials[node.name]
			)

			2:
			(
				if (getUserProp node "danglymesh" != "1") then
				(
					for i = 1 to node.mesh.numverts do
					(
						meshop.setVertColor node.mesh 0 i (color 0 0 0)
					)
					update node.mesh
				)
				setUserProp node "node_type" "danglymesh"
				setUserProp node "danglymesh" "1"
				node.material = sceneMaterials[node.name]
			)

			3:
			(
				setUserProp node "node_type" "aabb"
				node.material = createNWNWalkMaterial()
			)
			4:
			(
				setUserProp node "node_type" "pwk_aabb"
				node.material = createNWNWalkMaterial()
			)
		)
	)
		
	on dummy_type_radio changed newState do
	(
		local node = selection[1]
		node.boxsize = [5, 5, 5]
		case newState of 
		(
			1:
			(
				setUserProp node "node_type" "dummy"
				node.material = sceneMaterials[node.name]
			)

			2:
			(
				SetUserProp node "node_type" "pwk_use_dummy"

				local use_count = 1
				for i in objects do
				(
					if (((findString i.name "use") == 1) and (i != node)) then use_count += 1
				)
				node.name = "use"
				if (use_count < 10) then node.name += "0"
				node.name += use_count as string
			)
		)
	)

	on render_checkbox changed curSetting do
	(
		local node = selection[1]
		if (curSetting) then
			setUserProp node "render" "1"
		else
			setUserProp node "render" "0"
	)

	on shadow_checkbox changed curSetting do
	(
		local node = selection[1]
		if (curSetting) then
			setUserProp node "shadow" "1"
		else
			setUserProp node "shadow" "0"
	)

	on beaming_checkbox changed curSetting do
	(
		local node = selection[1]
		if (curSetting) then
			setUserProp node "beaming" "1"
		else
			setUserProp node "beaming" "0"
	)

	on no_bitmap_checkbox changed curSetting do
	(
		local node = selection[1]
		if (curSetting) then
			setUserProp node "nobitmap" "1"
		else
			setUserProp node "nobitmap" "0"
	)
	
	on tile_fade_radio changed newState do
	(
		local node = selection[1]
		case newState of
		(
			1: setUserProp node "tilefade" "0"
			2: setUserProp node "tilefade" "1"
			3: setUserProp node "tilefade" "2"
			4: setUserProp node "tilefade" "4"
		)
	)
	
	on alpha_radio changed newState do
	(
		local node = selection[1]
		case newState of
		(
			1: 
			(
				setUserProp node "shininess" "0"
				setUserProp node "transparencyhint" "0"
			)
			2: 
			(
				setUserProp node "shininess" "0"
				setUserProp node "transparencyhint" "1"
			)
			3: 
			(
				setUserProp node "shininess" "1"
				setUserProp node "transparencyhint" "0"
			)
		)
	)
		
	on show_dangle_checkbutton changed curSetting do
	(
		ShowDanglyConstraints selection[1] curSetting
	)
)

fn FindAnimByName lookup_name=
(
	if ((lookup_name == undefined) or (AnimRootNodeName == undefined)) then return undefined
	local node = getNodeByName(AnimRootNodeName)
	if node == undefined then return undefined

	local user_props = getUserPropBuffer(selection[1]) as stringstream
	while not eof user_props do
	(
		local prop = readDelimitedString user_props "\n"
		if ((findString prop "anim_name") == 1) then
		(
			local anim_name = trimleft (trimright (substring prop ((findstring prop "=")+1) -1))
			if (anim_name == lookup_name) then
			(
				local anim_number = substring prop 11 ((findstring prop "=")-11)
				anim_number = trimright anim_number
				return (anim_number as integer)
			)
		)
	)
	return undefined
)

fn FindFreeAnimNumber=
(
	local i = 0
	local node = getNodeByName(AnimRootNodeName)
	while (true) do
	(
		i += 1
		local prop_name = "anim_name_" + (i as string)
		local anim_name = getUserProp node prop_name
		if ((anim_name == undefined) or (anim_name == "")) then return i
	)
)

fn FindFreeEventNumber=
(
	local i = 0
	local node = getNodeByName(AnimRootNodeName)
	while (true) do
	(
		i += 1
		local prop_name = "anim_eventname_" + (LastAnimNumber as string) + "_" + (i as string)
		local event_name = getUserProp node prop_name
		if ((event_name == undefined) or (event_name == "")) then return i
	)
)

rollout AnimationPropRollout "Animations" width:160 height:488 rolledUp:true
(
	listBox anim_list "" pos:[8,8] width:144 height:9
	groupBox grp1 "Properties" pos:[8,160] width:144 height:128
	button add_anim_button "Add" pos:[8,136] width:40 height:20
	button delete_anim_button "Delete" pos:[56,136] width:48 height:20
	button zoom_to_anim_button "Zoom" pos:[112,136] width:40 height:20
	editText anim_name "" pos:[56,184] width:88 height:16
	spinner first_frame "" pos:[80,208] width:64 height:16 range:[0,10000,0] type:#integer scale:1
	spinner last_frame "" pos:[80,232] width:64 height:16 range:[0,10000,0] type:#integer scale:1
	label lbl1 "First Frame:" pos:[16,208] width:56 height:16
	label lbl2 "Last Frame:" pos:[16,232] width:56 height:16
	label lbl3 "Name:" pos:[16,184] width:32 height:16
	spinner transition "" pos:[80,256] width:64 height:16 range:[0,10,0.01] type:#float scale:1
	label lbl4 "Transition" pos:[16,256] width:48 height:16
	groupBox grp2 "Events" pos:[8,296] width:144 height:192
	listBox event_list "" pos:[16,312] width:128 height:6
	editText event_name "" pos:[64,432] width:80 height:16
	spinner event_frame "" pos:[64,456] width:80 height:16 range:[0,10000,0]
	label lbl5 "Name:" pos:[16,432] width:40 height:16
	label lbl6 "Frame:" pos:[16,456] width:40 height:16
	button event_add_button "Add" pos:[32,400] width:40 height:20
	button event_delete_button "Delete" pos:[80,400] width:48 height:20

	fn LoadAnimEvents anim_number=
	(
		event_list.items = #()
		LastAnimEventNumber = undefined
		local node = getNodeByName(AnimRootNodeName)
		
		local user_props = getUserPropBuffer(node) as stringstream
		local event_name_array = #()
		while not eof user_props do
		(
			local prop = readDelimitedString user_props "\n"
			local event_prefix = "anim_eventname_" + (anim_number as string)
			if ((findString prop event_prefix) == 1) then
			(
				local prop_tokens = filterString prop " ="
				local prop_name_tokens = filterString prop_tokens[1] "_"
				local event_number = (prop_name_tokens[4] as integer);
				local prop_name
				prop_name = "anim_eventname_" + (anim_number as string) + "_" + (event_number as string);
				local event_name = getUserProp node prop_name
				prop_name = "anim_eventframe_" + (anim_number as string) + "_" + (event_number as string);
				local event_frame = getUserProp node prop_name
				local event_info = (event_number as string) + ". " + event_name
				append event_name_array event_info
			)
		)
		AnimationPropRollout.event_list.items = event_name_array
		AnimationPropRollout.event_list.selection = 0		
	)
	
	fn LoadSelectedAnimation=
	(
		LastAnimNumber = FindAnimByName(AnimationPropRollout.anim_list.selected)
		if (LastAnimNumber != undefined) then
		(
			local node = getNodeByName(AnimRootNodeName)
			local prop_name
			prop_name = "anim_name_" + (LastAnimNumber as string)
			AnimationPropRollout.anim_name.text = getUserProp node prop_name 
			prop_name = "anim_firstframe_" + (LastAnimNumber as string)
			AnimationPropRollout.first_frame.value = getUserProp node prop_name 
			prop_name = "anim_lastframe_" + (LastAnimNumber as string)
			AnimationPropRollout.last_frame.value = getUserProp node prop_name
			prop_name = "anim_transtime_" + (LastAnimNumber as string)
			AnimationPropRollout.transition.value = getUserProp node prop_name
			LoadAnimEvents LastAnimNumber
		) 
		else
		(
			AnimationPropRollout.anim_name.text = "" 
			AnimationPropRollout.first_frame.value = 0 
			AnimationPropRollout.last_frame.value = 0
			AnimationPropRollout.transition.value = 0
			AnimationPropRollout.event_list.items = #()
			AnimationPropRollout.event_name.text = ""
			AnimationPropRollout.event_frame.value = 0
		)
	)

	fn ZoomToAnim=
	(
		local firstframe = AnimationPropRollout.first_frame.value
		local lastframe = AnimationPropRollout.last_frame.value
		if (firstframe < lastframe) then 
		(
			animationRange = (interval firstframe lastframe)
			sliderTime = firstframe
		)
	)
		
	fn ChangeAnimProperty property new_val=
	(
		if ((LastAnimNumber != undefined) and (AnimRootNodeName != undefined)) then
		(
			local node = getNodeByName(AnimRootNodeName)
			local prop_name = "anim_" + property + "_" + (LastAnimNumber as string)
			setUserProp node prop_name new_val
		)
	)

	on anim_list selected list_idx do LoadSelectedAnimation()
	on anim_list doubleClicked list_idx do ZoomToAnim()

	on add_anim_button pressed do
	(
		local node = getNodeByName(AnimRootNodeName)
		if (node != undefined) then
		(
			local anim_number = FindFreeAnimNumber()
			local anim_name = "untitled" + (anim_number as string)
			local prop_name
			prop_name = "anim_name_" + (anim_number as string)
			setUserProp node prop_name anim_name
			prop_name = "anim_firstframe_" + (anim_number as string)
			setUserProp node prop_name 0
			prop_name = "anim_lastframe_" + (anim_number as string)
			setUserProp node prop_name 1
			prop_name = "anim_transtime_" + (anim_number as string)
			setUserProp node prop_name 0.25

			local newlist = AnimationPropRollout.anim_list.items 
			append newlist anim_name
			AnimationPropRollout.anim_list.items = newlist
			AnimationPropRollout.anim_list.selection = newlist.count
			LoadSelectedAnimation()
		)
	)
	
    on delete_anim_button pressed do
    (
	    local sel = AnimationPropRollout.anim_list.selection
		if (sel > 0) then
		(
			ChangeAnimProperty "name" ""
			AnimationPropRollout.anim_list.selection = 0
			LoadSelectedAnimation()
			
			local newlist = AnimationPropRollout.anim_list.items 
			deleteItem newlist sel
			AnimationPropRollout.anim_list.items = newlist
		)
    )
    
	on zoom_to_anim_button pressed do ZoomToAnim()

	on anim_name changed new_val do 
	(
		ChangeAnimProperty "name" new_val
		if (AnimationPropRollout.anim_list.selection > 0) then
			AnimationPropRollout.anim_list.selected = new_val
	)
	
	on first_frame changed new_val do ChangeAnimProperty "firstframe" new_val
	on last_frame changed new_val do ChangeAnimProperty "lastframe" new_val
	on transition changed new_val do ChangeAnimProperty "transition" new_val

	fn LoadSelectedAnimEvent=
	(
		if (AnimationPropRollout.event_list.selection > 0) then
		(
			LastAnimEventNumber = readDelimitedString (AnimationPropRollout.event_list.selected as stringstream) "."
			local node = getNodeByName(AnimRootNodeName)
			prop_name = "anim_eventname_" + (LastAnimNumber as string) + "_" + (LastAnimEventNumber as string)
			AnimationPropRollout.event_name.text = getUserProp node prop_name 
			prop_name = "anim_eventframe_" + (LastAnimNumber as string) + "_" + (LastAnimEventNumber as string)
			AnimationPropRollout.event_frame.value = getUserProp node prop_name 
		)
		else
		(
			LastAnimEventNumber = undefined
			AnimationPropRollout.event_name.text = ""
			AnimationPropRollout.event_frame.value = 0 
		)
	)

	fn ChangeAnimEventProperty property new_val=
	(
		if ((LastAnimNumber != undefined) and (LastAnimEventNumber != undefined)) then
		(
			local node = getNodeByName(AnimRootNodeName)
			local prop_name = "anim_" + property + "_" + (LastAnimNumber as string) + "_" + (LastAnimEventNumber as string)
			setUserProp node prop_name new_val
		)
	)
	
	on add_event_button pressed do
	(
		local node = getNodeByName(AnimRootNodeName)
		if ((node != undefined) and (LastAnimNumber != undefined)) then
		(
			local event_number = FindFreeEventNumber()
			local event_info = (event_number as string) + ". untitled"
			local prop_name
			prop_name = "anim_eventname_" + (LastAnimNumber as string) + "_" + (event_number as string)
			setUserProp node prop_name "untitled"
			prop_name = "anim_eventframe_" + (LastAnimNumber as string) + "_" + (event_number as string)
			setUserProp node prop_name 0

			local newlist = AnimationPropRollout.event_list.items 
			append newlist event_info
			AnimationPropRollout.event_list.items = newlist
			AnimationPropRollout.event_list.selection = newlist.count
			LoadSelectedAnimEvent()
		)
	)
	
	on delete_event_button pressed do
    (
	    local sel = AnimationPropRollout.event_list.selection
		if (sel > 0) then
		(
			ChangeAnimProperty "eventname" ""
			AnimationPropRollout.event_list.selection = 0
			LoadSelectedAnimEvent()
			
			local newlist = AnimationPropRollout.event_list.items 
			deleteItem newlist sel
			AnimationPropRollout.event_list.items = newlist
		)
    )
    
	on event_list selected list_idx do LoadSelectedAnimEvent()
	on event_name changed new_val do 
	(
		ChangeAnimEventProperty "eventname" new_val
		if (AnimationPropRollout.event_list.selection > 0) then
		(
			local event_info = (LastAnimEventNumber as string) + ". " + new_val
			AnimationPropRollout.event_list.selected = event_info
		)
	)

	on event_frame changed new_val do ChangeAnimEventProperty "eventframe" new_val
)


rollout AboutRollout "About" width:160 height:248 rolledUp:true
(
	label lbl1 "NWN mdl import/export" pos:[8,8] width:144 height:16
	label lbl2 "By Wayland Reid" pos:[8,24] width:136 height:16
	listBox lbx1 "Special thanks to" pos:[8,48] width:144 height:8 items:#("Zaddix", "Mr X", "Hlubocky", "scooterpb", "Polyhedral")
	label lbl4 "And anyone else whose posted there problems and solutions to the Bioware NWN forums." pos:[8,184] width:144 height:56
)

-----------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------
---
---   Acutal window creation
---
-----------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------

-- create the rollout window 
if wayland_NWN_mdl_tool != undefined do
(
	closerolloutfloater wayland_NWN_mdl_tool
)
		
wayland_NWN_mdl_tool = newRolloutFloater "NWN MDL Tool" 189 480 0 100
addRollout ImportRollout	wayland_NWN_mdl_tool
addRollout ExportRollout	wayland_NWN_mdl_tool
addRollout RootPropRollout	wayland_NWN_mdl_tool
addRollout GeometryPropRollout	wayland_NWN_mdl_tool
addRollout AnimationPropRollout	wayland_NWN_mdl_tool
addRollout AboutRollout		wayland_NWN_mdl_tool rolledUp:true

if IsSceneRedrawDisabled()==true do enablesceneredraw()

-----------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------
---
---   GUI updaters
---
-----------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------

fn UpdateRootPropertiesUI node=
(
	if ((node != undefined) and (GetUserProp selection[1] "mdl_root" == true)) then 
	(
		RootPropRollout.make_root_button.enabled = false
		local sm = getuserprop node "setsupermodel"
		local mdl_class = getuserprop node "classification"
		local anim_scale = getuserprop node "setanimationscale"
		if (sm == undefined) then sm = "NULL"
		if (mdl_class == undefined) then mdl_class = 1
		if (anim_scale == undefined) then anim_scale = 1
		RootPropRollout.supermodel_edit.enabled = true
		RootPropRollout.supermodel_edit.text = sm
		RootPropRollout.classification_radio.enabled = true
		case (mdl_class) of
		(
			"Character": RootPropRollout.classification_radio.state = 1
			"Tile": RootPropRollout.classification_radio.state = 2
			"Door": RootPropRollout.classification_radio.state = 3
			"FX": RootPropRollout.classification_radio.state = 4
			"Other": RootPropRollout.classification_radio.state = 5
		)
		RootPropRollout.anim_scale_spinner.enabled = true
		RootPropRollout.anim_scale_spinner.value = anim_scale
	)
	else
	(
		if ((node == undefined) or ((node.parent == undefined) and (IsKindOf node Dummy))) then 
			RootPropRollout.make_root_button.enabled = true
		else
			RootPropRollout.make_root_button.enabled = false
		RootPropRollout.supermodel_edit.enabled = false
		RootPropRollout.classification_radio.enabled = false
		RootPropRollout.anim_scale_spinner.enabled = false
	)

)

fn UpdateGeomUI node=
(
	if (lastDanglyName != undefined) then ShowDanglyConstraints (getNodeByName(lastDanglyName)) false
	if ((node != undefined) and (canConvertTo node Editable_Mesh)) then
	(
		ShowDanglyConstraints node GeometryPropRollout.show_dangle_checkbutton.checked
		GeometryPropRollout.geom_type_radio.enabled = true
		GeometryPropRollout.render_checkbox.enabled = true
		GeometryPropRollout.shadow_checkbox.enabled = true
		GeometryPropRollout.beaming_checkbox.enabled = true
		GeometryPropRollout.no_bitmap_checkbox.enabled = true
		GeometryPropRollout.tile_fade_radio.enabled = true
		GeometryPropRollout.alpha_radio.enabled = true
		case (getuserprop node "node_type") of
		(
			"danglymesh": GeometryPropRollout.geom_type_radio.state = 2
			"aabb": GeometryPropRollout.geom_type_radio.state = 3
			"pwk_aabb": GeometryPropRollout.geom_type_radio.state = 4
			default: GeometryPropRollout.geom_type_radio.state = 1
		)
		GeometryPropRollout.render_checkbox.checked = (getuserprop node "render" == 1)
		GeometryPropRollout.shadow_checkbox.checked = (getuserprop node "shadow" == 1)
		GeometryPropRollout.beaming_checkbox.checked = (getuserprop node "beaming" == 1)
		GeometryPropRollout.no_bitmap_checkbox.checked = (getuserprop node "nobitmap" == 1)
		case (getuserprop node "tilefade") of
		(
			1: GeometryPropRollout.tile_fade_radio.state = 2
			2: GeometryPropRollout.tile_fade_radio.state = 3
			4: GeometryPropRollout.tile_fade_radio.state = 4
			default: GeometryPropRollout.tile_fade_radio.state = 1
		) 
		case (getuserprop node "transparencyhint") of
		(
			1: GeometryPropRollout.alpha_radio.state = 2
			0: 
			(
				case (getuserprop node "shininess") of
				(
					1: GeometryPropRollout.alpha_radio.state = 3
					default:  GeometryPropRollout.alpha_radio.state = 1
				)
			)
			default:  GeometryPropRollout.alpha_radio.state = 1
		)
	)
	else
	(
		GeometryPropRollout.geom_type_radio.enabled = false
		GeometryPropRollout.render_checkbox.enabled = false
		GeometryPropRollout.shadow_checkbox.enabled = false
		GeometryPropRollout.beaming_checkbox.enabled = false
		GeometryPropRollout.no_bitmap_checkbox.enabled = false
		GeometryPropRollout.tile_fade_radio.enabled = false
		GeometryPropRollout.alpha_radio.enabled = false
	)

	if ((node != undefined) and (isKindOf node Dummy) and (getUserProp node "mdl_root" != true)) then
	(
		GeometryPropRollout.dummy_type_radio.enabled = true
		case (getuserprop node "node_type") of
		(
			"pwk_use_dummy": GeometryPropRollout.geom_type_radio.state = 2
			default: GeometryPropRollout.geom_type_radio.state = 1
		)
	)
	else
	(
		GeometryPropRollout.dummy_type_radio.enabled = false
	)


)

fn UpdateExportUI node =
(

	ExportRollout.anim_combo.items = #()
	if (node != undefined) then
	(
		ExportRollout.export_node_button.enabled = true
		if (getUserProp node "mdl_root" == true) then
		(
			ExportRollout.export_mdl_button.enabled = true
			ExportRollout.export_wok_button.enabled = true
			ExportRollout.export_pwk_button.enabled = true
			ExportRollout.export_anim_button.enabled = true
			ExportRollout.anim_combo.enabled = true

			local user_props = getUserPropBuffer(selection[1]) as stringstream
			local anim_name_array = #("<None>", "<All>")
			while not eof user_props do
			(
				local prop = readDelimitedString user_props "\n"
				if ((findString prop "anim_name") == 1) then
				(
					local anim_name = trimleft (trimright (substring prop ((findstring prop "=")+1) -1))
					if (anim_name != "") then append anim_name_array anim_name
				)
			)
			ExportRollout.anim_combo.items = anim_name_array	
			ExportRollout.anim_combo.selection = 1
		)
		else
		(
			ExportRollout.export_mdl_button.enabled = false
			ExportRollout.export_wok_button.enabled = false
			ExportRollout.export_pwk_button.enabled = false
			ExportRollout.export_anim_button.enabled = false
			ExportRollout.anim_combo.enabled = false
		)
	)
	else
	(
		ExportRollout.export_mdl_button.enabled = false
		ExportRollout.export_wok_button.enabled = false
		ExportRollout.export_pwk_button.enabled = false
		ExportRollout.export_node_button.enabled = false
		ExportRollout.export_anim_button.enabled = false
		ExportRollout.anim_combo.enabled = false
	)
)

fn UpdateAnimUI node=
(
	if ((node != undefined) and (getUserProp node "mdl_root" == true)) then
	(
		AnimRootNodeName = selection[1].name
		AnimationPropRollout.anim_list.items = #()
		AnimationPropRollout.event_list.items = #()
		LastAnimNumber = undefined
		LastAnimEventNumber = undefined
		local user_props = getUserPropBuffer(selection[1]) as stringstream
		local anim_name_array = #()
		while not eof user_props do
		(
			local prop = readDelimitedString user_props "\n"
			if ((findString prop "anim_name") == 1) then
			(
				local anim_name = trimleft (trimright (substring prop ((findstring prop "=")+1) -1))
				if (anim_name != "") then append anim_name_array anim_name
			)
		)
		AnimationPropRollout.anim_list.items = anim_name_array	
		AnimationPropRollout.anim_list.selection = 0

		AnimationPropRollout.anim_list.enabled = true
		AnimationPropRollout.add_anim_button.enabled = true
		AnimationPropRollout.delete_anim_button.enabled = true
		AnimationPropRollout.zoom_to_anim_button.enabled = true
		AnimationPropRollout.anim_name.enabled = true
		AnimationPropRollout.first_frame.enabled = true
		AnimationPropRollout.last_frame.enabled = true
		AnimationPropRollout.transition.enabled = true
		AnimationPropRollout.event_list.enabled = true
		AnimationPropRollout.event_name.enabled = true
		AnimationPropRollout.event_frame.enabled = true
		AnimationPropRollout.event_add_button.enabled = true
		AnimationPropRollout.event_delete_button.enabled = true
	)
	else
	(
		AnimRootNodeName = undefined
		AnimationPropRollout.anim_list.items = #()
		AnimationPropRollout.event_list.items = #()

		AnimationPropRollout.anim_list.enabled = false
		AnimationPropRollout.add_anim_button.enabled = false
		AnimationPropRollout.delete_anim_button.enabled = false
		AnimationPropRollout.zoom_to_anim_button.enabled = false
		AnimationPropRollout.anim_name.enabled = false
		AnimationPropRollout.first_frame.enabled = false
		AnimationPropRollout.last_frame.enabled = false
		AnimationPropRollout.transition.enabled = false
		AnimationPropRollout.event_list.enabled = false
		AnimationPropRollout.event_name.enabled = false
		AnimationPropRollout.event_frame.enabled = false
		AnimationPropRollout.event_add_button.enabled = false
		AnimationPropRollout.event_delete_button.enabled = false
	)
)

------------------
--- selChanged ---
------------------

fn selChanged =
(
	UpdateRootPropertiesUI selection[1]
	UpdateGeomUI selection[1]
	UpdateExportUI selection[1]
	UpdateAnimUI selection[1]
)

-- Call this function once to set all the UI elements properly
selChanged()
