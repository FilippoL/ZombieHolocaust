/*
    Beginning DirectX 11 Game Programming
    By Allen Sherrod and Wendy Jones

    3D Models Demo - Demonstrates loading a model from an .OBJ file.
*/


#ifndef _MODELS_DEMO_H_
#define _MODELS_DEMO_H_
#define ZOMBIE_NUMBER 4

#define LIVES 20

#define ROAD_HEIGHT 2000
#define ROAD_SWAP_AFTER 300

#define DESERT_HEIGHT 2000
#define DESERT_SWAP_AFTER 300

#include "Dx11DemoBase.h"
#include "ArcCamera.h"
#include "FirstPersonCamera.h"
#include "ModelManager.h"
#include "Enemy.h"
#include "Ground.h"
#include "Animation.h"
#include "Barrell.h"
#include "Car.h"
#include "LifeBar.h"
#include "Enviroment.h"
#include "HighScore.h"
#include "Wall.h"
#include "AudioComponent.h"

#include <vector>
#include <XInput.h>

enum GameStates {PLAY_INTRO, START_MENU, RUN, PAUSED, HIGHSCORE, GAMEOVERSTATE, CREDITS};

enum PauseMenuSelection {RETURN, BACK_TO_MENU, QUIT};

enum MainMenuSelection { PLAY , HG, CRED,  RESET_HG, DIFF, LOOP_MOVIE, EXIT };

enum Difficulty {BEGINNER, MEDIUM, MASTER};

enum _AUDIO { SPLAT, CRASH, MOAN, BACKGROUND_SONG};



#define PAUSE_MENU_ITEMS 3

#define MAIN_MENU_ITEMS 7

#define LEVELS 3

inline PauseMenuSelection operator++(PauseMenuSelection &eDOW, int)
{
   const PauseMenuSelection ePrev = eDOW;
   const int i = static_cast<int>(eDOW);
   eDOW = static_cast<PauseMenuSelection>((i + 1) % PAUSE_MENU_ITEMS);
   return ePrev;
}
inline PauseMenuSelection operator--(PauseMenuSelection &eDOW, int)
{
   const PauseMenuSelection ePrev = eDOW;
   const int i = static_cast<int>(eDOW);
   if(i > 0)
   {
		eDOW = static_cast<PauseMenuSelection>(i - 1);
   }
   else
   {
	   eDOW = static_cast<PauseMenuSelection>(PAUSE_MENU_ITEMS - 1);
   }
   return ePrev;
}

inline MainMenuSelection operator++(MainMenuSelection &eDOW, int)
{
   const MainMenuSelection ePrev = eDOW;
   const int i = static_cast<int>(eDOW);
   eDOW = static_cast<MainMenuSelection>((i + 1) % MAIN_MENU_ITEMS);
   return ePrev;
}
inline MainMenuSelection operator--(MainMenuSelection &eDOW, int)
{
   const MainMenuSelection ePrev = eDOW;
   const int i = static_cast<int>(eDOW);
   if(i > 0)
   {
		eDOW = static_cast<MainMenuSelection>(i - 1);
   }
   else
   {
	   eDOW = static_cast<MainMenuSelection>(MAIN_MENU_ITEMS - 1);
   }
   return ePrev;
}

inline Difficulty operator++(Difficulty &eDOW, int)
{
   const Difficulty ePrev = eDOW;
   const int i = static_cast<int>(eDOW);
   eDOW = static_cast<Difficulty>((i + 1) % LEVELS);
   return ePrev;
}
inline Difficulty operator--(Difficulty &eDOW, int)
{
   const Difficulty ePrev = eDOW;
   const int i = static_cast<int>(eDOW);
   if(i > 0)
   {
		eDOW = static_cast<Difficulty>(i - 1);
   }
   else
   {
	   eDOW = static_cast<Difficulty>(LEVELS - 1);
   }
   return ePrev;
}



class ModelsDemo : public Dx11DemoBase
{
    public:
        ModelsDemo( );
        virtual ~ModelsDemo( );

        bool LoadContent( );
        void UnloadContent( );

		void TurnZBufferOn();
		void TurnZBufferOff();

		void TurnOnAlphaBlending();
		void TurnOffAlphaBlending();

		bool DrawString( char* message, float startX, float startY );
        void Update( float dt );
		void Reset();
        void Render( );

		void SetGameState(GameStates state);
		GameStates GetGameState();

		float score;
		float _score_counter;
		float _damage_counter;
    private:
        ID3D11VertexShader* textureMapVS_;
        ID3D11PixelShader* textureMapPS_;
		ID3D11VertexShader* textTextureMapVS_;
        ID3D11PixelShader* textTextureMapPS_;

        ID3D11InputLayout* inputLayout_;
		ID3D11InputLayout* textInputLayout_;
        ID3D11Buffer* vertexBuffer_;
		ID3D11Buffer* vertexBuffer2_;
		ID3D11Buffer* textVertexBuffer_;
		ID3D11Buffer* vertexBufferTerrain_;
        int totalVerts_;
		int totalVerts2_;

        ID3D11ShaderResourceView* colorMap_;
		ID3D11ShaderResourceView* textColorMap_;
        ID3D11SamplerState* colorMapSampler_;
		ID3D11SamplerState* textColorMapSampler_;

        ID3D11Buffer* viewCB_;
        ID3D11Buffer* projCB_;
        ID3D11Buffer* worldCB_;
        ID3D11Buffer* camPosCB_;
        XMMATRIX projMatrix_;
		Difficulty _diff;
		TextureManager * TextTexture = new TextureManager;
	
        ArcCamera camera_;
		//FirstPersonCamera camera_;

        XINPUT_STATE controller1State_;
        XINPUT_STATE prevController1State_;

		GameStates gameState_;
		PauseMenuSelection pauseMenuSelection;
		MainMenuSelection mainmenuselec;
		bool displayFPS;


		Mesh* _dead_zombie = new Mesh;
		Enemy Enemies [ZOMBIE_NUMBER];
		Enemy Second_Enemies [ZOMBIE_NUMBER];

		Enviroment _enviroment;
		Car _car;
		Ground Desert [2];
		Barrell _barrell;
		HighScore _hg;
		Ground Road [2];
		Wall _wall;
		AudioComponent _background_music;
		AudioComponent _splat;
		AudioComponent _moan;
		AudioComponent _crash;


		LifeBar _lifes;


		float _game_speed;
		float zoom = 0.0;

		//////////////time globals///////////////////
public:
		//double countsPerSecond_ = 0.0;
		//__int64 CounterStart_ = 0;

		//int frameCount_ = 0;
		int fps_;

		//__int64 frameTimeOld_ = 0;
		double frameTime_;
};

#endif