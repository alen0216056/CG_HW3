#include "scene.h"

scene::scene(char* scene_str, int texture_type, int id[])
{
	type = texture_type;
	texture_id[0] = id[0];
	texture_id[1] = id[1];

	char* ptr = strtok(scene_str, " ");
	if (strcmp(ptr, "model") == 0)
	{
		ptr = strtok(NULL, " ");
		strcpy(name, ptr);
		ptr = strtok(NULL, " ");
		for (int i = 0; i < 3; i++)
		{
			s[i] = atof(ptr);
			ptr = strtok(NULL, " ");
		}
		for (int i = 0; i < 4; i++)
		{
			r[i] = atof(ptr);
			ptr = strtok(NULL, " ");
		}
		for (int i = 0; i < 3; i++)
		{
			t[i] = atof(ptr);
			ptr = strtok(NULL, " ");
		}
	}
}

scene::~scene()
{
}

scenes::scenes(const char* file_name)
{
	glewInit();
	FreeImage_Initialise();
	glGenTextures(30, texture_obj);

	selected = -1;
	x = 0.0;
	y = 0.0;
	int cur_type, cur_texture[2], all_texture;
	cur_type = cur_texture[0] = cur_texture[1] = all_texture = 0;

	char buffer[1000];
	FILE* f_scene;
	f_scene = fopen(file_name, "r");
	if (!f_scene)
	{
		cout << "can not open the file\n";
		return;
	}

	while (fgets(buffer, 1000, f_scene) != NULL)
	{
		if (strstr(buffer, "model") != NULL)
		{
			scene* tmp = new scene(buffer, cur_type, cur_texture);
			scene_list.push_back(*tmp);
		}
		else
		{
			if (strstr(buffer, "no-texture") != NULL)
			{
				cur_type = 0;
				cur_texture[0] = 0;
				cur_texture[1] = 0;
			}
			else if (strstr(buffer, "single-texture") != NULL)
			{
				char* ptr = strtok(buffer, " \n");
				ptr = strtok(NULL, " \n");
				all_texture += 1;
				LoadTexture(ptr, all_texture);
				
				cur_type = 1;
				cur_texture[0] = all_texture;
				cur_texture[1] = all_texture;
			}
			else if (strstr(buffer, "multi-texture") !=NULL )
			{
				char* ptr = strtok(buffer, " \n");
				ptr = strtok(NULL, " \n");
				all_texture += 1;
				LoadTexture(ptr, all_texture);

				ptr = strtok(NULL, " \n");
				all_texture += 1;
				LoadTexture(ptr, all_texture);
				
				cur_type = 2;
				cur_texture[0] = all_texture - 1;
				cur_texture[1] = all_texture;
			}
			else
			{
				all_texture += 1;
				LoadCubeTexture(buffer, all_texture);
				
				cur_type = 3;
				cur_texture[0] = all_texture;
				cur_texture[1] = all_texture;
			}
		}
	}

	FreeImage_DeInitialise();
}

scenes::~scenes()
{
}

void scenes::LoadTexture(char* pFilename, int texture_count){
	FIBITMAP* pImage = FreeImage_Load(FreeImage_GetFileType(pFilename, 0), pFilename);
	FIBITMAP *p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
	int iWidth = FreeImage_GetWidth(p32BitsImage);
	int iHeight = FreeImage_GetHeight(p32BitsImage);

	glBindTexture(GL_TEXTURE_2D, texture_obj[texture_count]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_obj[texture_count]);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	FreeImage_Unload(p32BitsImage);
	FreeImage_Unload(pImage);
}

void scenes::LoadCubeTexture(char* pFilename, int texture_count){
	FIBITMAP* pImage;
	FIBITMAP *p32BitsImage;
	int iWidth;
	int iHeight;

	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_obj[texture_count]);

	char* ptr = strtok(pFilename, " \n");
	ptr = strtok(NULL, " \n");
	pImage = FreeImage_Load(FreeImage_GetFileType(ptr, 0), ptr);
	p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
	iWidth = FreeImage_GetWidth(p32BitsImage);
	iHeight = FreeImage_GetHeight(p32BitsImage);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));

	ptr = strtok(NULL, " \n");
	pImage = FreeImage_Load(FreeImage_GetFileType(ptr, 0), ptr);
	p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
	iWidth = FreeImage_GetWidth(p32BitsImage);
	iHeight = FreeImage_GetHeight(p32BitsImage);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));

	ptr = strtok(NULL, " \n");
	pImage = FreeImage_Load(FreeImage_GetFileType(ptr, 0), ptr);
	p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
	iWidth = FreeImage_GetWidth(p32BitsImage);
	iHeight = FreeImage_GetHeight(p32BitsImage);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));

	ptr = strtok(NULL, " \n");
	pImage = FreeImage_Load(FreeImage_GetFileType(ptr, 0), ptr);
	p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
	iWidth = FreeImage_GetWidth(p32BitsImage);
	iHeight = FreeImage_GetHeight(p32BitsImage);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));

	ptr = strtok(NULL, " \n");
	pImage = FreeImage_Load(FreeImage_GetFileType(ptr, 0), ptr);
	p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
	iWidth = FreeImage_GetWidth(p32BitsImage);
	iHeight = FreeImage_GetHeight(p32BitsImage);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));

	ptr = strtok(NULL, " \n");
	pImage = FreeImage_Load(FreeImage_GetFileType(ptr, 0), ptr);
	p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
	iWidth = FreeImage_GetWidth(p32BitsImage);
	iHeight = FreeImage_GetHeight(p32BitsImage);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_obj[texture_count]);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	FreeImage_Unload(p32BitsImage);
	FreeImage_Unload(pImage);
}

void scenes::object(const char* file_name, int scene_index)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(scene_list[scene_index].t[0], scene_list[scene_index].t[1], scene_list[scene_index].t[2]);
	glRotatef(scene_list[scene_index].r[0], scene_list[scene_index].r[1], scene_list[scene_index].r[2], scene_list[scene_index].r[3]);
	glScalef(scene_list[scene_index].s[0], scene_list[scene_index].s[1], scene_list[scene_index].s[2]);

	mesh* obj = new mesh(file_name);
	int lastMaterial = -1;
	for (size_t i = 0; i < obj->fTotal; ++i)
	{
		// set material property if this face used different material
		if (lastMaterial != obj->faceList[i].m)
		{
			lastMaterial = (int)obj->faceList[i].m;
			glMaterialfv(GL_FRONT, GL_AMBIENT, obj->mList[lastMaterial].Ka);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, obj->mList[lastMaterial].Kd);
			glMaterialfv(GL_FRONT, GL_SPECULAR, obj->mList[lastMaterial].Ks);
			glMaterialfv(GL_FRONT, GL_SHININESS, &obj->mList[lastMaterial].Ns);

			//you can obtain the texture name by object->mList[lastMaterial].map_Kd
			//load them once in the main function before mainloop
			//bind them in display function here
		}

		glBegin(GL_TRIANGLES);
		for (size_t j = 0; j<3; ++j)
		{
			glMultiTexCoord2fv(GL_TEXTURE0, obj->tList[obj->faceList[i][j].t].ptr);
			glMultiTexCoord2fv(GL_TEXTURE1, obj->tList[obj->faceList[i][j].t].ptr);
			glNormal3fv(obj->nList[obj->faceList[i][j].n].ptr);
			glVertex3fv(obj->vList[obj->faceList[i][j].v].ptr);
			//cout << obj->vList[obj->faceList[i][j].v].ptr[0] << obj->vList[obj->faceList[i][j].v].ptr[1] << obj->vList[obj->faceList[i][j].v].ptr[2] << endl;
		}
		glEnd();
	}

	glPopMatrix();
}

void scenes::shadow(const char* file_name, int scene_index, float* light_pos)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(scene_list[scene_index].t[0], scene_list[scene_index].t[1], scene_list[scene_index].t[2]);
	glRotatef(scene_list[scene_index].r[0], scene_list[scene_index].r[1], scene_list[scene_index].r[2], scene_list[scene_index].r[3]);
	glScalef(scene_list[scene_index].s[0], scene_list[scene_index].s[1], scene_list[scene_index].s[2]);

	mesh* obj = new mesh(file_name);
	int lastMaterial = -1;
	for (size_t i = 0; i < obj->fTotal; ++i)
	{
		// set material property if this face used different material
		if (lastMaterial != obj->faceList[i].m)
		{
			lastMaterial = (int)obj->faceList[i].m;
			glMaterialfv(GL_FRONT, GL_AMBIENT, obj->mList[lastMaterial].Ka);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, obj->mList[lastMaterial].Kd);
			glMaterialfv(GL_FRONT, GL_SPECULAR, obj->mList[lastMaterial].Ks);
			glMaterialfv(GL_FRONT, GL_SHININESS, &obj->mList[lastMaterial].Ns);

			//you can obtain the texture name by object->mList[lastMaterial].map_Kd
			//load them once in the main function before mainloop
			//bind them in display function here
		}

		float vector[4][3];
		vector[0][0] = obj->vList[obj->faceList[i][0].v].ptr[0] - obj->vList[obj->faceList[i][1].v].ptr[0];
		vector[0][1] = obj->vList[obj->faceList[i][0].v].ptr[1] - obj->vList[obj->faceList[i][1].v].ptr[1];
		vector[0][2] = obj->vList[obj->faceList[i][0].v].ptr[2] - obj->vList[obj->faceList[i][1].v].ptr[2];

		vector[1][0] = obj->vList[obj->faceList[i][0].v].ptr[0] - obj->vList[obj->faceList[i][2].v].ptr[0];
		vector[1][1] = obj->vList[obj->faceList[i][0].v].ptr[1] - obj->vList[obj->faceList[i][2].v].ptr[1];
		vector[1][2] = obj->vList[obj->faceList[i][0].v].ptr[2] - obj->vList[obj->faceList[i][2].v].ptr[2];
		
		vector[2][0] = vector[0][1] * vector[1][2] - vector[0][2] * vector[1][1];
		vector[2][1] = vector[0][2] * vector[1][0] - vector[0][0] * vector[1][2];
		vector[2][2] = vector[0][0] * vector[1][1] - vector[0][1] * vector[1][0];

		vector[3][0] = obj->vList[obj->faceList[i][0].v].ptr[0] - light_pos[0];
		vector[3][1] = obj->vList[obj->faceList[i][0].v].ptr[1] - light_pos[1];
		vector[3][2] = obj->vList[obj->faceList[i][0].v].ptr[2] - light_pos[2];
		
		if (vector[2][0] * vector[3][0] + vector[2][1] * vector[3][1] + vector[2][2] * vector[3][2] < 0)
		{
			glBegin(GL_POLYGON);
			glNormal3fv(obj->nList[obj->faceList[i][1].n].ptr);
			glVertex3fv(obj->vList[obj->faceList[i][1].v].ptr);
			glNormal3fv(obj->nList[obj->faceList[i][0].n].ptr);
			glVertex3fv(obj->vList[obj->faceList[i][0].v].ptr);
			glNormal3fv(obj->nList[obj->faceList[i][0].n].ptr);
			glVertex3f(obj->vList[obj->faceList[i][0].v].ptr[0] + 5 * (obj->vList[obj->faceList[i][0].v].ptr[0] - light_pos[0]), obj->vList[obj->faceList[i][0].v].ptr[1] + 5 * (obj->vList[obj->faceList[i][0].v].ptr[1] - light_pos[1]), obj->vList[obj->faceList[i][0].v].ptr[2] + 5 * (obj->vList[obj->faceList[i][0].v].ptr[2] - light_pos[2]));
			glNormal3fv(obj->nList[obj->faceList[i][1].n].ptr);
			glVertex3f(obj->vList[obj->faceList[i][1].v].ptr[0] + 5 * (obj->vList[obj->faceList[i][1].v].ptr[0] - light_pos[0]), obj->vList[obj->faceList[i][1].v].ptr[1] + 5 * (obj->vList[obj->faceList[i][1].v].ptr[1] - light_pos[1]), obj->vList[obj->faceList[i][1].v].ptr[2] + 5 * (obj->vList[obj->faceList[i][1].v].ptr[2] - light_pos[2]));
			glEnd();

			glBegin(GL_POLYGON);
			glNormal3fv(obj->nList[obj->faceList[i][2].n].ptr);
			glVertex3fv(obj->vList[obj->faceList[i][2].v].ptr);
			glNormal3fv(obj->nList[obj->faceList[i][1].n].ptr);
			glVertex3fv(obj->vList[obj->faceList[i][1].v].ptr);
			glNormal3fv(obj->nList[obj->faceList[i][1].n].ptr);
			glVertex3f(obj->vList[obj->faceList[i][1].v].ptr[0] + 5 * (obj->vList[obj->faceList[i][1].v].ptr[0] - light_pos[0]), obj->vList[obj->faceList[i][1].v].ptr[1] + 5 * (obj->vList[obj->faceList[i][1].v].ptr[1] - light_pos[1]), obj->vList[obj->faceList[i][1].v].ptr[2] + 5 * (obj->vList[obj->faceList[i][1].v].ptr[2] - light_pos[2]));
			glNormal3fv(obj->nList[obj->faceList[i][2].n].ptr);
			glVertex3f(obj->vList[obj->faceList[i][2].v].ptr[0] + 5 * (obj->vList[obj->faceList[i][2].v].ptr[0] - light_pos[0]), obj->vList[obj->faceList[i][2].v].ptr[1] + 5 * (obj->vList[obj->faceList[i][2].v].ptr[1] - light_pos[1]), obj->vList[obj->faceList[i][2].v].ptr[2] + 5 * (obj->vList[obj->faceList[i][2].v].ptr[2] - light_pos[2]));
			glEnd();

			glBegin(GL_POLYGON);
			glNormal3fv(obj->nList[obj->faceList[i][0].n].ptr);
			glVertex3fv(obj->vList[obj->faceList[i][0].v].ptr);
			glNormal3fv(obj->nList[obj->faceList[i][2].n].ptr);
			glVertex3fv(obj->vList[obj->faceList[i][2].v].ptr);
			glNormal3fv(obj->nList[obj->faceList[i][2].n].ptr);
			glVertex3f(obj->vList[obj->faceList[i][2].v].ptr[0] + 5 * (obj->vList[obj->faceList[i][2].v].ptr[0] - light_pos[0]), obj->vList[obj->faceList[i][2].v].ptr[1] + 5 * (obj->vList[obj->faceList[i][2].v].ptr[1] - light_pos[1]), obj->vList[obj->faceList[i][2].v].ptr[2] + 5 * (obj->vList[obj->faceList[i][2].v].ptr[2] - light_pos[2]));
			glNormal3fv(obj->nList[obj->faceList[i][0].n].ptr);
			glVertex3f(obj->vList[obj->faceList[i][0].v].ptr[0] + 5 * (obj->vList[obj->faceList[i][0].v].ptr[0] - light_pos[0]), obj->vList[obj->faceList[i][0].v].ptr[1] + 5 * (obj->vList[obj->faceList[i][0].v].ptr[1] - light_pos[1]), obj->vList[obj->faceList[i][0].v].ptr[2] + 5 * (obj->vList[obj->faceList[i][0].v].ptr[2] - light_pos[2]));
			glEnd();
		}
	}
	glPopMatrix();
}

void scenes::shadow_setting(float* light_pos)
{
	for (int i = 0; i < scene_list.size(); i++)
	{
		shadow(scene_list[i].name, i, light_pos);
	}
}

void scenes::scene_setting()
{
	for (int i = 0; i < scene_list.size(); i++)
	{
		switch (scene_list[i].type)
		{
			case 0:
				object(scene_list[i].name, i);
				break;
			case 1:
				glEnable(GL_ALPHA_TEST);
				glAlphaFunc(GL_GREATER, 0.5f);

				glActiveTexture(GL_TEXTURE0);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, texture_obj[scene_list[i].texture_id[0]]);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				
				object(scene_list[i].name, i);

				glActiveTexture(GL_TEXTURE0);
				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
				break;
			case 2:
				glActiveTexture(GL_TEXTURE0);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, texture_obj[scene_list[i].texture_id[0]]);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);

				glActiveTexture(GL_TEXTURE1);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, texture_obj[scene_list[i].texture_id[1]]);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);

				object(scene_list[i].name, i);

				glActiveTexture(GL_TEXTURE1);
				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);

				glActiveTexture(GL_TEXTURE0);
				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
				break;
			case 3:
				glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
				glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
				glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);
				glEnable(GL_TEXTURE_GEN_R);

				glEnable(GL_TEXTURE_CUBE_MAP);
				glBindTexture(GL_TEXTURE_CUBE_MAP, texture_obj[scene_list[i].texture_id[0]]);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

				object(scene_list[i].name, i);

				glDisable(GL_TEXTURE_CUBE_MAP);
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
				glDisable(GL_TEXTURE_GEN_R);
				glDisable(GL_TEXTURE_GEN_T);
				glDisable(GL_TEXTURE_GEN_S);
				break;
			default:
				object(scene_list[i].name, i);
				break;
		}
	}
}

void scenes::select(unsigned int i)
{
	if (i<scene_list.size())
		selected = i;
}

void scenes::set_x_y(int a, int b)
{
	x = a;
	y = b;
}

void scenes::move(int a, int b, int w, int h)
{
	if (selected >= 0)
	{
		scene_list[selected].t[0] += (a - x) / double(w);
		scene_list[selected].t[1] -= (b - y) / double(h);
	}
}
