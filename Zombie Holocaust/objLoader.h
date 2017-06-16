/*
    Beginning DirectX 11 Game Programming
    By Allen Sherrod and Wendy Jones

    ObjModel - Used to represent an OBJ model.
*/


#ifndef _OBJ_LOADER_H_
#define _OBJ_LOADER_H_


class ObjModel
{
   public:
      ObjModel( );      
      ~ObjModel( );

      void Release( );
      bool LoadOBJ(const char *fileName );


		float *GetVertices()		{ return _vertices; }
		float *GetNormals()		{ return _normals; }
		float *GetTexCoords()	{ return _texCoords; }
		int    GetTotalVerts()	{ return _totalVerts; }


   private:
	  
	   float *_vertices;
      float *_normals;
      float *_texCoords;
      int _totalVerts;
};

#endif