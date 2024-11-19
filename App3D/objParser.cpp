#include "ObjParser.h"

//
#include <fstream>
#include <sstream>

#include "Collision.h"

using namespace DirectX;

bool ParseMaterial(std::string mtlFileName, ParsedData& data, std::unordered_map<std::string, ShaderResource>& textures, ID3D11Device* device)
{
	std::ifstream ifs("Meshes/" + mtlFileName); //CHANGE THIS BACK LATER, ONLY TEST/ WHEN WE'RE DOING THE TEST LEVEL
	if (!ifs)
	{
		MessageBox(NULL, L"Couldn't open mtl file!", L"Error", MB_OK);
		return false;
	}

	//Read the text file
	std::string lineStr;
	std::string current; //NEW
	int nMaterials = 0;
	while (std::getline(ifs, lineStr))
	{
		std::istringstream lineSS(lineStr);
		std::string lineType;
		lineSS >> lineType;
		if (lineType == "newmtl") //NEW
		{
			if (nMaterials != 0)
			{
				//Load textures (But only if it doesn't already exist)
				if (textures.count(data.materials[current].ambientData) == 0)
				{
					std::string path = "Textures/" + data.materials[current].ambientData;
					textures[data.materials[current].ambientData].Init(device, path.c_str());
				}
				if (textures.count(data.materials[current].diffuseData) == 0)
				{
					std::string path = "Textures/" + data.materials[current].diffuseData;
					textures[data.materials[current].diffuseData].Init(device, path.c_str());
				}
				if (textures.count(data.materials[current].specularData) == 0)
				{
					std::string path = "Textures/" + data.materials[current].specularData;
					textures[data.materials[current].specularData].Init(device, path.c_str());
				}
			}
			nMaterials++;
			lineSS >> current;
			MaterialData mData;

			mData.ambientData = "dark_grey.png";
			mData.diffuseData = "grey.png";
			mData.specularData = "light_grey.png";
			mData.shineData = 1.0f;

			data.materials[current] = mData;
		}
		else if (lineType == "Ka")
		{
			std::string ambientData;
			lineSS >> ambientData;
			data.materials[current].ambientData = ambientData;
		}
		else if (lineType == "Kd")
		{
			std::string diffuseData;
			lineSS >> diffuseData;
			data.materials[current].diffuseData = diffuseData;
		}
		else if (lineType == "Ks")
		{
			std::string specularData;
			lineSS >> specularData;
			data.materials[current].specularData = specularData;
		}
		else if (lineType == "Ns")
		{
			float shineData;
			lineSS >> shineData;
			data.materials[current].shineData = shineData;
		}
	}

	//Load textures (but only once) (I'm a bit of a dumb fuck, this would only load the textures of the LAST MATERIAL IN THE FILE)
	if (textures.count(data.materials[current].ambientData) == 0)
	{
		std::string path = "Textures/" + data.materials[current].ambientData;
		textures[data.materials[current].ambientData].Init(device, path.c_str());
	}
	if (textures.count(data.materials[current].diffuseData) == 0)
	{
		std::string path = "Textures/" + data.materials[current].diffuseData;
		textures[data.materials[current].diffuseData].Init(device, path.c_str());
	}
	if (textures.count(data.materials[current].specularData) == 0)
	{
		std::string path = "Textures/" + data.materials[current].specularData;
		textures[data.materials[current].specularData].Init(device, path.c_str());
	}

	return true;
}

int ParseObj(std::string objFileName, std::vector<Drawable>& vecToFill, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 translate,
	std::unordered_map<std::string, ShaderResource>& textures, ID3D11Device* device, int interact, std::vector<int> interactsWith)
{
	std::ifstream ifs(objFileName);
	if (!ifs)
	{
		MessageBox(NULL, L"Couldn't open obj file!", L"Error", MB_OK);
		return false;
	}

	//Error handling for faces (preferably every .obj file has v, vt and vn, but not all .obj files were made equal)
	bool fileHasTexcoord = false;
	bool fileHasNormal = false;

	//Wrap the mesh in a bounding box by getting the highest and lowest x-, y-, and z-values (use a library for actual infinities xd)
	XMFLOAT3 vMinf3(1000000.0f, 1000000.0f, 1000000.0f);
	XMFLOAT3 vMaxf3(-1000000.0f, -1000000.0f, -1000000.0f);

	XMVECTOR vMin = DirectX::XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = DirectX::XMLoadFloat3(&vMaxf3);

	//Member variables to store positions, texture coordinates and normals
	ParsedData parsed; //data THAT WE'RE PARSING

	//Read the text file
	std::string lineStr;
	while (std::getline(ifs, lineStr))
	{
		std::istringstream lineSS(lineStr);
		std::string lineType;
		lineSS >> lineType;

		/*Material stuff*/
		if (lineType == "mtllib")
		{
			//Get the name of the file containing the material (texture) and parse it
			std::string mtlFileName;
			lineSS >> mtlFileName;

			ParseMaterial(mtlFileName, parsed, textures, device);
		}

		if (lineType == "usemtl") //"From this point onward, use this specified material"
		{
			std::string groupName;
			lineSS >> groupName;

			if (!parsed.indices.empty()) //New submesh
			{
				BufferData::SubMeshData smd;
				smd.startIndex = parsed.currentSubmeshIndex;
				smd.indexCount = parsed.indices.size() - parsed.currentSubmeshIndex;
				smd.ambientSRV = textures[parsed.materials[parsed.currentSubmeshMaterial].ambientData].GetSRV();
				smd.diffuseSRV = textures[parsed.materials[parsed.currentSubmeshMaterial].diffuseData].GetSRV();
				smd.specularSRV = textures[parsed.materials[parsed.currentSubmeshMaterial].specularData].GetSRV();
				smd.shininess = parsed.materials[parsed.currentSubmeshMaterial].shineData;

				parsed.submeshes.push_back(smd);
			}
			parsed.currentSubmeshMaterial = groupName;
			parsed.currentSubmeshIndex = parsed.vertices.size();
		}

		if (lineType == "v") //Vertex (Position)
		{
			float x, y, z;
			lineSS >> x >> y >> z;
			parsed.vPos.push_back({ x,y,z });

			//Check vMin and vMax against the x-, y-, and z-values of the vertex, once we've looped through everything we should have the edges of our box
			XMFLOAT3 vf3(x, y, z);
			XMVECTOR V = DirectX::XMLoadFloat3(&vf3);
			vMin = DirectX::XMVectorMin(vMin, V);
			vMax = DirectX::XMVectorMax(vMax, V);
		}

		if (lineType == "vt") //Vertex Texture Coordinate
		{
			float u, v;
			lineSS >> u >> v;
			parsed.vTex.push_back({ u,v });
			fileHasTexcoord = true;
		}

		if (lineType == "vn") //Vertex Normal
		{
			float nx, ny, nz;
			lineSS >> nx >> ny >> nz;
			parsed.vNor.push_back({ nx, ny, nz });
			fileHasNormal = true;
		}

		if (lineType == "f") //Face
		{
			//Each face references three vertices, with their respective positions, texture coordinates and normals (think triangles)
			std::string refStr;
			while (lineSS >> refStr) //Get line up until the next space, in the case of for example "1/1/1 2/2/1 3/3/1" we get 1/1/1
			{
				std::istringstream ref(refStr);
				std::string vStr, vtStr, vnStr;

				//Split the line at every / (this is the point where we know what kind of obj-file we're reading, as mentioned above)
				std::getline(ref, vStr, '/');
				std::getline(ref, vtStr, '/');
				std::getline(ref, vnStr, '/');

				//Convert to integer (and subtract 1 since obj files start counting from 1 but we want to count from 0)
				int v = atoi(vStr.c_str()) - 1;
				int vt = atoi(vtStr.c_str()) - 1;
				int vn = atoi(vnStr.c_str()) - 1;

				//Error checking in case of negative references (shouldn't be necessary as long as the .obj-file isn't trash but I did have to deal with it one time)
				if (v < 0) v = (int)parsed.vPos.size() + v;
				if (vt < 0) vt = (int)parsed.vTex.size() + vt;
				if (vn < 0) vn = (int)parsed.vNor.size() + vn;

				//If the face is unique, add it to the list of refs so we'll create the vertices later
				if (parsed.refs.count(refStr) == 0)
				{
					//Create the vertex
					Vertex vert;
					vert.position = parsed.vPos[v];
					vert.texcoord = parsed.vTex[vt];
					vert.normal = parsed.vNor[vn];

					uint32_t idx = parsed.vertices.size();
					parsed.vertices.push_back(vert);
					parsed.refs.insert({ refStr, idx });
				}

				//Push back indices regardless of whether or not the face is unique
				parsed.indices.push_back(parsed.refs[refStr]);
			}
		}
	}

	//ONCE THE FILE HAS BEEN READ, PUSH BACK SUBMESH ONE TIME
	BufferData::SubMeshData smd;
	smd.startIndex = parsed.currentSubmeshIndex;
	smd.indexCount = parsed.indices.size() - parsed.currentSubmeshIndex;
	smd.ambientSRV = textures[parsed.materials[parsed.currentSubmeshMaterial].ambientData].GetSRV();
	smd.diffuseSRV = smd.ambientSRV;
	smd.specularSRV = smd.ambientSRV;
	//smd.diffuseSRV = m_textures[parsed.materials[parsed.currentSubmeshMaterial].diffuseData].GetSRV();
	//smd.specularSRV = m_textures[parsed.materials[parsed.currentSubmeshMaterial].specularData].GetSRV();
	smd.shininess = parsed.materials[parsed.currentSubmeshMaterial].shineData;
	parsed.submeshes.push_back(smd);

	BufferData bufferData; //NOW MAKE THE ACTUAL MESH/DRAWABLE BY TAKING DATA FROM THE PARSED DATA STRUCT
	bufferData.vData.size = sizeof(Vertex);
	bufferData.vData.count = (UINT)parsed.vertices.size();
	bufferData.vData.vector = parsed.vertices;

	bufferData.iData.size = sizeof(uint32_t);
	bufferData.iData.count = (UINT)parsed.indices.size();
	bufferData.iData.vector = parsed.indices;

	bufferData.subMeshVector = parsed.submeshes;

	Drawable cube(device, bufferData, scale, rotate, translate, interact, interactsWith);
	cube.CreateBoundingBoxFromPoints(vMin, vMax);

	//New: attempt at adding the self-coded Collider
	/*Collider collider;
	DirectX::XMFLOAT3 vecMin, vecMax;
	DirectX::XMStoreFloat3(&vecMin, vMin);
	DirectX::XMStoreFloat3(&vecMax, vMax);*/

	//Get the corners from the BoundingBox (temp as heck)
	/*DirectX::XMFLOAT3 corners[8];
	cube.GetBoundingBox().GetCorners(corners);
	InitCollider(collider, 8, corners);*/

	vecToFill.push_back(cube);

	//New: Push back the name of the obj-file to a list, which we'll check against in the future whenever we create new drawables (Ignore drawable creation, and attempt instancing)
	//MeshNames::vec.push_back(objFileName);

	return (int)vecToFill.size() - 1;
}

int InitDrawableFromFile(std::string objFileName, std::vector<Drawable>& vecToFill, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 translate,
	std::unordered_map<std::string, ShaderResource>& textures, ID3D11Device* device, int interact, std::vector<int> interactsWith)
{
	bool meshAlreadyExists = false;

	//for (auto name : MeshNames::vec) //O(n) but we don't have that many meshes, might wanna change this to unordered map or something later down the line but right now I'm just learning instancing
	//{
	//	if (name == objFileName)
	//	{
	//		meshAlreadyExists = true;
	//		break;
	//	}
	//}

	//Mesh doesn't already exist, parse the obj file
	if (!meshAlreadyExists)
	{
		return ParseObj(objFileName, vecToFill, scale, rotate, translate, textures, device, interact, interactsWith);
	}

	//Mesh already existed, do instancing stuff (How will this work when we want to modify a drawable during run-time? Can't just check the index of the m_drawables vector anymore)

	return (int)vecToFill.size() - 1; //Return the size - 1 (aka index last element, aka the element we just put in, aka the element we're most probably interested in modifying when we create something)
}


int InitDrawableFromFileInstanced(std::string objFileName, std::string normalMapFileName,	//Path + name of the obj and any potential normalmap
	UINT& nDrawables,																		//Total drawable count (og + instanced)
	std::vector<UINT>& instancedDrawableCounts,												//Vector of instanced drawable counts, meant to split
	std::unordered_map<std::string, InstanceInfo>& instances,								//Map linking objFileNames to the indices of the instanced drawables (And the og)
	std::unordered_map<int, std::vector<DirectX::XMFLOAT4X4>>& transforms,					//Map linking instanced drawable indices to a world transform
	std::vector<Drawable>& vecToFill,														//Vector containing the og drawables
	DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 translate,
	std::unordered_map<std::string, ShaderResource>& textures, ID3D11Device* device,
	int interact, std::vector<int> interactsWith)
{
	bool meshAlreadyExists = (instances.count(objFileName) == 0) ? false : true;

	if (meshAlreadyExists)
	{
		//Get the index of the "real" drawable
		int ogIndex = instances[objFileName].original;

		//Store the world transform and push it back into the map linking it to this drawable
		DirectX::XMFLOAT4X4 transform;
		DirectX::XMMATRIX world = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
			DirectX::XMMatrixRotationX(rotate.x) * DirectX::XMMatrixRotationY(rotate.y) * DirectX::XMMatrixRotationZ(rotate.z) *
			DirectX::XMMatrixTranslation(translate.x, translate.y, translate.z);
		DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixTranspose(world));
		transforms[ogIndex].push_back(transform);

		//Update instanced variables
		instancedDrawableCounts.at(ogIndex)++;											//Increment the instanced drawable count associated with the corresponding original drawable
		instances[objFileName].instanced.push_back(instancedDrawableCounts.at(ogIndex));//Push back the instance to the map linking it to the objFileName
		nDrawables++;																	//Increment the total drawable count
		vecToFill.at(ogIndex).SetInstanceCount(instancedDrawableCounts.at(ogIndex));	//Update the instance count of the og drawable

		return nDrawables - 1;
	}

	//Mesh doesn't already exist, parse the obj-file
	std::ifstream ifs(objFileName);
	if (!ifs)
	{
		MessageBox(NULL, L"Couldn't open obj file!", L"Error", MB_OK);
		return false;
	}

	//Error handling for faces (preferably every .obj file has v, vt and vn, but not all .obj files were made equal)
	bool fileHasTexcoord = false;
	bool fileHasNormal = false;

	//Wrap the mesh in a bounding box by getting the highest and lowest x-, y-, and z-values (use a library for actual infinities xd)
	XMFLOAT3 vMinf3(1000000.0f, 1000000.0f, 1000000.0f);
	XMFLOAT3 vMaxf3(-1000000.0f, -1000000.0f, -1000000.0f);

	XMVECTOR vMin = DirectX::XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = DirectX::XMLoadFloat3(&vMaxf3);

	//Member variables to store positions, texture coordinates and normals
	ParsedData parsed; //data THAT WE'RE PARSING

	//Read the text file
	std::string lineStr;
	while (std::getline(ifs, lineStr))
	{
		std::istringstream lineSS(lineStr);
		std::string lineType;
		lineSS >> lineType;

		/*Material stuff*/
		if (lineType == "mtllib")
		{
			//Get the name of the file containing the material (texture) and parse it
			std::string mtlFileName;
			lineSS >> mtlFileName;

			ParseMaterial(mtlFileName, parsed, textures, device);
		}

		if (lineType == "usemtl") //"From this point onward, use this specified material"
		{
			std::string groupName;
			lineSS >> groupName;

			if (!parsed.indices.empty()) //New submesh
			{
				BufferData::SubMeshData smd;
				smd.startIndex = parsed.currentSubmeshIndex;
				smd.indexCount = parsed.indices.size() - parsed.currentSubmeshIndex;
				smd.ambientSRV = textures[parsed.materials[parsed.currentSubmeshMaterial].ambientData].GetSRV();
				smd.diffuseSRV = textures[parsed.materials[parsed.currentSubmeshMaterial].diffuseData].GetSRV();
				smd.specularSRV = textures[parsed.materials[parsed.currentSubmeshMaterial].specularData].GetSRV();
				smd.shininess = parsed.materials[parsed.currentSubmeshMaterial].shineData;

				parsed.submeshes.push_back(smd);
			}
			parsed.currentSubmeshMaterial = groupName;
			parsed.currentSubmeshIndex = parsed.vertices.size();
		}

		if (lineType == "v") //Vertex (Position)
		{
			float x, y, z;
			lineSS >> x >> y >> z;
			parsed.vPos.push_back({ x,y,z });

			//Check vMin and vMax against the x-, y-, and z-values of the vertex, once we've looped through everything we should have the edges of our box
			XMFLOAT3 vf3(x, y, z);
			XMVECTOR V = DirectX::XMLoadFloat3(&vf3);
			vMin = DirectX::XMVectorMin(vMin, V);
			vMax = DirectX::XMVectorMax(vMax, V);
		}

		if (lineType == "vt") //Vertex Texture Coordinate
		{
			float u, v;
			lineSS >> u >> v;
			parsed.vTex.push_back({ u,v });
			fileHasTexcoord = true;
		}

		if (lineType == "vn") //Vertex Normal
		{
			float nx, ny, nz;
			lineSS >> nx >> ny >> nz;
			parsed.vNor.push_back({ nx, ny, nz });
			fileHasNormal = true;
		}

		if (lineType == "f") //Face
		{
			//Each face references three vertices, with their respective positions, texture coordinates and normals (think triangles)
			std::string refStr;
			while (lineSS >> refStr) //Get line up until the next space, in the case of for example "1/1/1 2/2/1 3/3/1" we get 1/1/1
			{
				std::istringstream ref(refStr);
				std::string vStr, vtStr, vnStr;

				//Split the line at every / (this is the point where we know what kind of obj-file we're reading, as mentioned above)
				std::getline(ref, vStr, '/');
				std::getline(ref, vtStr, '/');
				std::getline(ref, vnStr, '/');

				//Convert to integer (and subtract 1 since obj files start counting from 1 but we want to count from 0)
				int v = atoi(vStr.c_str()) - 1;
				int vt = atoi(vtStr.c_str()) - 1;
				int vn = atoi(vnStr.c_str()) - 1;

				//Error checking in case of negative references (shouldn't be necessary as long as the .obj-file isn't trash but I did have to deal with it one time)
				if (v < 0) v = (int)parsed.vPos.size() + v;
				if (vt < 0) vt = (int)parsed.vTex.size() + vt;
				if (vn < 0) vn = (int)parsed.vNor.size() + vn;

				//If the face is unique, add it to the list of refs so we'll create the vertices later
				if (parsed.refs.count(refStr) == 0)
				{
					//Create the vertex
					Vertex vert;
					vert.position = parsed.vPos[v];
					vert.texcoord = parsed.vTex[vt];
					vert.normal = parsed.vNor[vn];

					uint32_t idx = parsed.vertices.size();
					parsed.vertices.push_back(vert);
					parsed.refs.insert({ refStr, idx });
				}

				//Push back indices regardless of whether or not the face is unique
				parsed.indices.push_back(parsed.refs[refStr]);
			}
		}
	}

	//ONCE THE FILE HAS BEEN READ, PUSH BACK SUBMESH ONE TIME
	BufferData::SubMeshData smd;
	smd.startIndex = parsed.currentSubmeshIndex;
	smd.indexCount = parsed.indices.size() - parsed.currentSubmeshIndex;
	smd.ambientSRV = textures[parsed.materials[parsed.currentSubmeshMaterial].ambientData].GetSRV();
	smd.diffuseSRV = smd.ambientSRV;
	smd.specularSRV = smd.ambientSRV;
	//smd.diffuseSRV = m_textures[parsed.materials[parsed.currentSubmeshMaterial].diffuseData].GetSRV();
	//smd.specularSRV = m_textures[parsed.materials[parsed.currentSubmeshMaterial].specularData].GetSRV();
	smd.shininess = parsed.materials[parsed.currentSubmeshMaterial].shineData;
	parsed.submeshes.push_back(smd);

	BufferData bufferData; //NOW MAKE THE ACTUAL MESH/DRAWABLE BY TAKING DATA FROM THE PARSED DATA STRUCT
	bufferData.vData.size = sizeof(Vertex);
	bufferData.vData.count = (UINT)parsed.vertices.size();
	bufferData.vData.vector = parsed.vertices;

	bufferData.iData.size = sizeof(uint32_t);
	bufferData.iData.count = (UINT)parsed.indices.size();
	bufferData.iData.vector = parsed.indices;

	bufferData.subMeshVector = parsed.submeshes;

	Drawable object(device, bufferData, scale, rotate, translate, interact, interactsWith);
	
	//Anything you want to do to this object needs to be done before it is added to the vecToFill
	object.CreateBoundingBoxFromPoints(vMin, vMax);
	object.SetIndex(vecToFill.size()); //It's the return of storing, because we can align drawable indices with instanced buffer indices
	if (!normalMapFileName.empty())
		object.SetNormalMap(device, normalMapFileName);

	vecToFill.push_back(object);

	//Instanced stuff begins here
	//Get the index of the "real" drawable
	int ogIndex = vecToFill.size() - 1;	

	//Store the world transform
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMMATRIX world = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
		DirectX::XMMatrixRotationX(rotate.x) * DirectX::XMMatrixRotationY(rotate.y) * DirectX::XMMatrixRotationZ(rotate.z) *
		DirectX::XMMatrixTranslation(translate.x, translate.y, translate.z);
	DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixTranspose(world));
	transforms[ogIndex].push_back(transform);

	//Update instance variables
	instancedDrawableCounts.push_back(1);											//Add a new element vector of instanced drawable counts
	instances[objFileName].original = ogIndex;										//Set this drawable as the original, helps us get the ogIndex in future instances
	instances[objFileName].instanced.push_back(instancedDrawableCounts.at(ogIndex));//Push back the instance to the map linking it to the objFileName
	nDrawables++;																	//Increment the total drawable count
	vecToFill.at(ogIndex).SetInstanceCount(instancedDrawableCounts.at(ogIndex));	//Set the instance count

	//Return the index of the last element, aka the element we just put in, aka the element we're most probably interested in modifying when we create something (Useless right now)
	return nDrawables - 1;
}