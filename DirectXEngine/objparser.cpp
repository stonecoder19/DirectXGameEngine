#include <iostream>
#include <fstream>

using namespace std;

typedef struct
{
	float x, y, z;
}VertexType;

typedef struct
{
	int vIndex1, vIndex2, vIndex3;
	int tIndex1, tIndex2, tIndex3;
	int nIndex1, nIndex2, nIndex3;
}FaceType;


void GetModelFilename(char*);
bool ReadFileCounts(char*, int&, int&, int&, int&);
bool LoadDataStructures(char*, int, int, int, int);


/*int main()
{
	bool result;
	char filename[256];
	int vertexCount, textureCount, normalCount, faceCount;
	char garbage;

	//Read in the number of vertices, tex coords, normals, and faces so that the data structures can be initialized
	result = ReadFileCounts(filename, vertexCount, textureCount, normalCount, faceCount);
	if (!result)
	{
		return -1;
	}

	//Display the counts to the screen
	cout << endl;
	cout << "Vertices: " << vertexCount << endl;
	cout << "UVs: " << textureCount << endl;
	cout << "Normal: " << normalCount << endl;
	cout << "Faces: " << faceCount << endl;


	//now read data from file into data structures
	result = LoadDataStructures(filename, vertexCount, textureCount, normalCount, faceCount);
	if (!result)
	{
		return -1;
	}

	//notify the user the model has been converted
	cout << "\nFile has been converted." << endl;
	cout << "\nDo you wish to exit(y/n)?";
	cin >> garbage;

	return 0;

}*/


void GetModelFilename(char* filename)
{
	bool done = false;
	ifstream fin;

	while (!done)
	{
		cout << "Enter filename";

		cin >> filename;

		fin.open(filename);

		if (fin.good())
		{
			done = true;
		}
		else
		{
			fin.clear();
			cout << endl;
			cout << "File:" << filename << "could not be opened." << endl << endl;
		}
	}

	return;
}

bool ReadFileCounts(char* filename, int& vertexCount, int& textureCount, int& normalCount, int& faceCount)
{
	ifstream fin;
	char input;

	//Initialize the counts
	vertexCount = 0;
	textureCount = 0;
	normalCount = 0;
	faceCount = 0;

	//open the file
	fin.open(filename);

	//check if it was succesful in opening the file
	if (fin.fail() == true)
	{
		return false;
	}

	//read from the file and continue to read unti the end of file
	fin.get(input);
	while (!fin.eof())
	{
		//if the line starts with v then count either vertex, texture or normal
		if (input == 'v')
		{
			fin.get(input);
			if (input == ' ') { vertexCount++; }
			if (input == 't') { textureCount++; };
			if (input == 'n') { normalCount++; };

			
		}
		//if the line starts with 'f' then increment the face count
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ') { faceCount++; }

		}

		//read in the remainde of the line
		while (input != '\n')
		{
			fin.get(input);
		}

		//start reading beginning of next line
		fin.get(input);


	}

	//close the file
	fin.close();

	return true;
}

bool LoadDataStructures(char* filename, int vertexCount, int textureCount, int normalCount, int faceCount)
{
	VertexType *vertices, *texcoords, *normals;
	FaceType *faces;
	ifstream fin;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex, vIndex, tIndex, nIndex;
	char input, input2;
	ofstream fout;


	//initialize the four data structures
	vertices = new VertexType[vertexCount];
	if (!vertices)
	{
		return false;
	}

	texcoords = new VertexType[textureCount];
	if (!texcoords)
	{
		return false;
	}

	normals = new VertexType[normalCount];
	if (!normals)
	{
		return false;
	}

	faces = new FaceType[faceCount];
	if (!faces)
	{
		return false;
	}

	//initialize the indexes
	vertexIndex = 0;
	texcoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;

	//open the file
	fin.open(filename);

	//check if it was succesfull
	if (fin.fail() == true)
	{
		return false;
	}

	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);

			//read in the vertices
			if (input == ' ')
			{
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;

				//invert the z vertex to chanage to left hand system
				vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
				vertexIndex++;

			}

			//read in the texture uv coordinates
			if (input == 't')
			{
				fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;

				//invert v texture coordinates to left hand system
				texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
				texcoordIndex++;
			}

			//read in the normals
			if (input == 'n')
			{
				fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;

				//invert the z normal to change to left hand system
				normals[normalIndex].z = normals[normalIndex].z * -1.0f;
				normalIndex++;
			}
		}

		//read in the faces
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
			{
				//read face data in backwards to convert it to left hand system
				fin >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >> faces[faceIndex].nIndex3
					>> faces[faceIndex].vIndex2 >> input2 >> faces[faceIndex].tIndex2 >> faces[faceIndex].nIndex2
					>> faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> faces[faceIndex].nIndex1;
				faceIndex++;
			}
		}

		//read in remainder of line
		while (input != '\n')
		{
			fin.get(input);
		}

		//start reading beginning of next line
		fin.get(input);
	}

	//close the file
	fin.close();

	//open the output file
	fout.open("model.txt");

	fout << "Vertex Count: " << (faceCount * 3) << endl;
	fout << endl;
	fout << "Data:" << endl;
	fout << endl;

	for (int i = 0;i < faceIndex;i++)
	{
		vIndex = faces[i].vIndex1 - 1;
		tIndex = faces[i].tIndex1 - 1;
		nIndex = faces[i].nIndex1 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

		vIndex = faces[i].vIndex2 - 1;
		tIndex = faces[i].tIndex2 - 1;
		nIndex = faces[i].nIndex2 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].z << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

		vIndex = faces[i].vIndex3 - 1;
		tIndex = faces[i].tIndex3 - 1;
		nIndex = faces[i].nIndex3 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

	}

	fout.close();

	if (vertices)
	{
		delete[] vertices;
		vertices = 0;
	}

	if (texcoords)
	{
		delete[] texcoords;
		texcoords = 0;
	}

	if (normals)
	{
		delete[] normals;
		normals = 0;
	}

	if (faces)
	{
		delete[] faces;
		faces = 0;
	}

	return true;	
}