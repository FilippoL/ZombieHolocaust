/*
    Beginning DirectX 11 Game Programming
    By Allen Sherrod and Wendy Jones

    ObjModel - Used to represent an OBJ model.
*/


#include<fstream>
#include<vector>
#include<string>
#include"objLoader.h"
#include"TokenStream.h"


ObjModel::ObjModel( )
{
    _vertices = 0;
    _normals = 0;
    _texCoords = 0;
    _totalVerts = 0;
}

      
ObjModel::~ObjModel( )
{
    Release( );
}


void ObjModel::Release( )
{
    _totalVerts = 0;

    if( _vertices != 0 ) delete[] _vertices;
    if( _normals != 0 ) delete[] _normals;
    if( _texCoords != 0 ) delete[] _texCoords;

    _vertices = 0;
    _normals = 0;
    _texCoords = 0;
}


bool ObjModel::LoadOBJ(const char *fileName )
{
   //------------------------------------------------------------------------------------------------------------- OPEN FILE
    std::ifstream fileStream;	int fileSize = 0;

    fileStream.open( fileName, std::ifstream::in );
   
	if (fileStream.is_open() == false)	{return false;}

	//------------------------------------------------------------------------------------------------------------- LOAD FILE
	fileStream.seekg(0, std::ios::end);
	fileSize = (int)fileStream.tellg();
	fileStream.seekg(0, std::ios::beg);

	if (fileSize <= 0)					{ return false; }
	
	//------------------------------------------------------------------------------------------------------------- CREATE BUFFER
	char *buffer = new char[fileSize];

	if (buffer == 0)					{ return false; }
	
	memset(buffer, '\0', fileSize);

	//------------------------------------------------------------------------------------------------------------- READ FROM BUFFER
    TokenStream tokenStream, lineStream, faceStream;
    std::string tempLine, token;

	fileStream.read(buffer, fileSize);
	tokenStream.SetTokenStream(buffer);

    delete[] buffer;

    tokenStream.ResetStream( );

    std::vector<float> verts, norms, texC;
    std::vector<int> faces;

	char lineDelimiters[2] = { '\n', ' ' };

	while (tokenStream.MoveToNextLine(&tempLine))
    {
		lineStream.SetTokenStream((char*)tempLine.c_str());
        tokenStream.GetNextToken( 0, 0, 0 );

		if (!lineStream.GetNextToken(&token, lineDelimiters, 2)) { continue; }

		//------------------------------------------------------------------------------------------------------------- GET _vertices
        if( strcmp( token.c_str( ), "v" ) == 0 )
        {
			lineStream.GetNextToken(&token, lineDelimiters, 2);
			verts.push_back((float)atof(token.c_str()));

			lineStream.GetNextToken(&token, lineDelimiters, 2);
			verts.push_back((float)atof(token.c_str()));

			lineStream.GetNextToken(&token, lineDelimiters, 2);
			verts.push_back((float)atof(token.c_str()));
        }

		//------------------------------------------------------------------------------------------------------------- GET _normals
        else if( strcmp( token.c_str( ), "vn" ) == 0 )
        {
			lineStream.GetNextToken(&token, lineDelimiters, 2);
			norms.push_back((float)atof(token.c_str()));

			lineStream.GetNextToken(&token, lineDelimiters, 2);
			norms.push_back((float)atof(token.c_str()));

			lineStream.GetNextToken(&token, lineDelimiters, 2);
			norms.push_back((float)atof(token.c_str()));
        }

		//------------------------------------------------------------------------------------------------------------- GET TEXTURE _vertices
        else if( strcmp( token.c_str( ), "vt" ) == 0 )
        {
			lineStream.GetNextToken(&token, lineDelimiters, 2);
			texC.push_back((float)atof(token.c_str()));

			lineStream.GetNextToken(&token, lineDelimiters, 2);
			texC.push_back((float)atof(token.c_str()));
        }

		//------------------------------------------------------------------------------------------------------------- GET FACES INDEX
		else if (strcmp(token.c_str(), "f") == 0)
        {
			char faceTokens[3] = { '\n', ' ', '/' };
            std::string faceIndex;

			faceStream.SetTokenStream((char*)tempLine.c_str());
			faceStream.GetNextToken(0, 0, 0);

            for( int i = 0; i < 3; i++ )
            {
				faceStream.GetNextToken(&faceIndex, faceTokens, 3);
				faces.push_back((int)atoi(faceIndex.c_str()));

				faceStream.GetNextToken(&faceIndex, faceTokens, 3);
				faces.push_back((int)atoi(faceIndex.c_str()));

				faceStream.GetNextToken(&faceIndex, faceTokens, 3);
				faces.push_back((int)atoi(faceIndex.c_str()));
            }
        }
		else if (strcmp(token.c_str(), "#") == 0)
        {
			int a = 0;
            int b = a;
        }

		token[0] = '\0';
    }

	//------------------------------------------------------------------------------------------------------------- "UNROLL" OBJ DATA
	int vIndex = 0, nIndex = 0, tIndex = 0;
	int numFaces = (int)faces.size() / 9;

    _totalVerts = numFaces * 3;

    _vertices = new float[_totalVerts * 3];

	if ((int)norms.size() != 0)
    {
		_normals = new float[_totalVerts * 3];
    }

	if ((int)texC.size() != 0)
    {
		_texCoords = new float[_totalVerts * 2];
    }

	for (int f = 0; f < (int)faces.size(); f += 3)
    {
		_vertices[vIndex + 0] = verts[(faces[f + 0] - 1) * 3 + 0];
		_vertices[vIndex + 1] = verts[(faces[f + 0] - 1) * 3 + 1];
		_vertices[vIndex + 2] = verts[(faces[f + 0] - 1) * 3 + 2];
        vIndex += 3;

        if(_texCoords)
        {
			_texCoords[tIndex + 0] = texC[(faces[f + 1] - 1) * 2 + 0];
			_texCoords[tIndex + 1] = texC[(faces[f + 1] - 1) * 2 + 1];
            tIndex += 2;
        }

        if(_normals)
        {
			_normals[nIndex + 0] = norms[(faces[f + 2] - 1) * 3 + 0];
			_normals[nIndex + 1] = norms[(faces[f + 2] - 1) * 3 + 1];
			_normals[nIndex + 2] = norms[(faces[f + 2] - 1) * 3 + 2];
            nIndex += 3;
        }
    }

    verts.clear( );
    norms.clear( );
    texC.clear( );
    faces.clear( );

    return true;
}