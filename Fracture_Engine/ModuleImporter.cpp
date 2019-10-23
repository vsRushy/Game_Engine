#include "GL/glew.h"

#include "Application.h"
#include "ModuleImporter.h"
#include "ModuleSceneIntro.h"

#include "GameObject.h"

#include "Component_Transform.h"
#include "Component_Mesh.h"
#include "Component_Material.h"

#include "Mesh.h"
#include "Texture.h"

#include "MathGeoLib.h"

ModuleImporter::ModuleImporter(bool start_enabled) : Module(start_enabled)
{

}

ModuleImporter::~ModuleImporter()
{}

// -----------------------------------------------------------------
bool ModuleImporter::Init()
{
	bool ret = true;

	texture_root_path = "Assets/Textures/";

	/* Assimp log stream */
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	/* Initializate DevIL */
	ilInit();
	iluInit();
	ilEnable(IL_CONV_PAL);
	ilutEnable(ILUT_OPENGL_CONV);
	ilutInit();

	ilutRenderer(ILUT_OPENGL);
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	return ret;
}

// -----------------------------------------------------------------
bool ModuleImporter::CleanUp()
{
	aiDetachAllLogStreams();

	return true;
}

// -----------------------------------------------------------------
update_status ModuleImporter::Update(float dt)
{
	return UPDATE_CONTINUE;
}

void ModuleImporter::LoadModel(const char* path)
{
	const aiScene* scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
		aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcessPreset_TargetRealtime_MaxQuality);
	
	if (scene != nullptr && scene->HasMeshes())
	{
		g_o = App->scene_intro->CreateEmptyGameObject(
			App->file_system->GetFileNameFromPath(path).data(),
			App->scene_intro->root_game_object
		);
		
		LOG(LOG_INFORMATION, "New scene with %d mesh(es)", scene->mNumMeshes);
		
		LoadSceneNode(scene, scene->mRootNode);
		
		aiVector3D position;
		aiQuaternion ai_rotation;
		aiVector3D scale;

		scene->mRootNode->mTransformation.Decompose(scale, ai_rotation, position);
		Quat q_rotation(ai_rotation.x, ai_rotation.y, ai_rotation.z, ai_rotation.w);

		float3 e_rotation = q_rotation.ToEulerXYZ();
		
		g_o->component_transform->Set(math::float3(position.x, position.y, position.z), 
			q_rotation, math::float3(scale.x, scale.y, scale.z));
		
		aiReleaseImport(scene);
	}
	else
		LOG(LOG_ERROR, "Error loading scene %s", path);
}


void ModuleImporter::LoadSceneNode(const aiScene* scene, aiNode* node)
{
	for (uint i = 0; i < node->mNumMeshes; i++)
	{
		GameObject* go = App->scene_intro->CreateEmptyGameObject(node->mName.C_Str(), g_o);
	
		aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
		go->CreateComponentMesh(LoadMesh(ai_mesh));

		aiMaterial* material = scene->mMaterials[ai_mesh->mMaterialIndex];
		aiString file_path_tex;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &file_path_tex);

		go->CreateComponentMaterial();
		ComponentMaterial* tmp_cm = go->GetComponentMaterial();
		std::string full_tex_path = texture_root_path;
		full_tex_path.append(file_path_tex.C_Str());
		tmp_cm->SetTexture(LoadTexture(full_tex_path.c_str()));

		aiVector3D position;
		aiQuaternion ai_rotation;
		aiVector3D scale;

		node->mTransformation.Decompose(scale, ai_rotation, position);
		Quat q_rotation(ai_rotation.x, ai_rotation.y, ai_rotation.z, ai_rotation.w);

		go->component_transform->Set(math::float3(position.x, position.y, position.z),
			q_rotation,
			math::float3(scale.x, scale.y, scale.z));
	}

	for (uint i = 0; i < node->mNumChildren; i++)
	{
		LoadSceneNode(scene, node->mChildren[i]);
	}
}

Texture* ModuleImporter::LoadTexture(const char* path)
{
	Texture* texture = nullptr;
	
	ILuint devil_id = 0;
	ilGenImages(1, &devil_id);
	ilBindImage(devil_id);

	LOG(LOG_INFORMATION, "Loading texture with path %s", path);
	if ((bool)ilLoadImage(path))
	{
		ILinfo img_info;
		iluGetImageInfo(&img_info);

		iluFlipImage();

		texture = new Texture();

		if (ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE)) 
		{
			texture->data = (unsigned char*)ilGetData();
			texture->width = ilGetInteger(IL_IMAGE_WIDTH);
			texture->height = ilGetInteger(IL_IMAGE_HEIGHT);
			texture->name = path;

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			glGenTextures(1, &texture->id);
			glBindTexture(GL_TEXTURE_2D, texture->id);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_FORMAT), texture->width, texture->height,
				0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, texture->data);
		}
		else
			LOG(LOG_ERROR, "Image could not be converted. Error code: %s", iluErrorString(ilGetError()));

		App->scene_intro->textures.insert({"", texture});
	}
	else
	{
		LOG(LOG_ERROR, "Error loading the texture %s. Error code: %s", path, iluErrorString(ilGetError()));
	}

	ilDeleteImages(1, &devil_id);

	return texture;
}

Mesh* ModuleImporter::LoadMesh(aiMesh* ai_mesh)
{
	Mesh* m = new Mesh();

	m->LoadVertices(ai_mesh);
	m->LoadFaces(ai_mesh);
	m->LoadNormals(ai_mesh);
	m->LoadUVs(ai_mesh);

	m->CreateBuffers();

	return m;
}
